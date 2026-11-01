
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include <regex>

#include "httplib.h"
#include "sqlite3.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

sqlite3* db = nullptr;


/* ==========================================
   DATABASE CONNECTION
========================================== */

void connectDatabase() {

    int result = sqlite3_open(
        "../library.db",
        &db
    );

    if(result != SQLITE_OK) {

        throw runtime_error(
            "Database connection failed: " +
            string(sqlite3_errmsg(db))
        );

    }

    cout << "Database connected successfully!" << endl;

    sqlite3_exec(
        db,
        "PRAGMA foreign_keys = ON;",
        nullptr,
        nullptr,
        nullptr
    );

}


/* ==========================================
   DATABASE INITIALIZATION
========================================== */

void initializeDatabase() {

    const char* sql = R"SQL(

    CREATE TABLE IF NOT EXISTS Books (

        book_id INTEGER PRIMARY KEY AUTOINCREMENT,

        title TEXT NOT NULL,

        author TEXT NOT NULL,

        category TEXT NOT NULL,

        isbn TEXT UNIQUE NOT NULL,

        quantity INTEGER NOT NULL
        CHECK(quantity >= 0),

        available INTEGER NOT NULL
        CHECK(available >= 0),

        CHECK(available <= quantity)

    );


    CREATE TABLE IF NOT EXISTS Members (

        member_id INTEGER PRIMARY KEY AUTOINCREMENT,

        name TEXT NOT NULL,

        email TEXT UNIQUE NOT NULL,

        phone TEXT NOT NULL,

        address TEXT

    );


    CREATE TABLE IF NOT EXISTS IssueRecords (

        issue_id INTEGER PRIMARY KEY AUTOINCREMENT,

        book_id INTEGER NOT NULL,

        member_id INTEGER NOT NULL,

        issue_date TEXT NOT NULL DEFAULT CURRENT_DATE,

        due_date TEXT NOT NULL,

        return_date TEXT,

        status TEXT NOT NULL DEFAULT 'Issued',

        FOREIGN KEY(book_id)
        REFERENCES Books(book_id),

        FOREIGN KEY(member_id)
        REFERENCES Members(member_id)

    );

    )SQL";

    char* error = nullptr;

    int result = sqlite3_exec(
        db,
        sql,
        nullptr,
        nullptr,
        &error
    );

    if(result != SQLITE_OK) {

        string message = error ? error : "Unknown error";

        sqlite3_free(error);

        throw runtime_error(message);

    }

}


/* ==========================================
   STATEMENT HANDLING
========================================== */

using Statement = unique_ptr<
    sqlite3_stmt,
    decltype(&sqlite3_finalize)
>;


Statement prepare(const string& sql) {

    sqlite3_stmt* raw = nullptr;

    if(sqlite3_prepare_v2(
        db,
        sql.c_str(),
        -1,
        &raw,
        nullptr
    ) != SQLITE_OK) {

        throw runtime_error(sqlite3_errmsg(db));

    }

    return Statement(raw, sqlite3_finalize);

}


void bindText(
    sqlite3_stmt* stmt,
    int index,
    const string& value
) {

    if(sqlite3_bind_text(
        stmt,
        index,
        value.c_str(),
        -1,
        SQLITE_TRANSIENT
    ) != SQLITE_OK) {

        throw runtime_error(sqlite3_errmsg(db));

    }

}


void bindInt(
    sqlite3_stmt* stmt,
    int index,
    int value
) {

    if(sqlite3_bind_int(
        stmt,
        index,
        value
    ) != SQLITE_OK) {

        throw runtime_error(sqlite3_errmsg(db));

    }

}


void execute(
    sqlite3_stmt* stmt
) {

    if(sqlite3_step(stmt) != SQLITE_DONE) {

        throw runtime_error(sqlite3_errmsg(db));

    }

}


/* ==========================================
   RESPONSE FUNCTIONS
========================================== */

void success(
    const httplib::Request& req,
    httplib::Response& res,
    const json& data
) {

    res.status = 200;

    res.set_content(
        data.dump(),
        "application/json"
    );

}


void failure(
    httplib::Response& res,
    const string& message,
    int status = 400
) {

    res.status = status;

    res.set_content(
        json{{"error", message}}.dump(),
        "application/json"
    );

}


bool validId(int id) {

    return id > 0;

}


bool validDate(const string& date) {

    if(!regex_match(
        date,
        regex(R"(\d{4}-\d{2}-\d{2})")
    )) {

        return false;

    }

    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    if(month < 1 || month > 12 ||
       day < 1 || day > 31 ||
       year < 2000 || year > 2100) {

        return false;

    }

    return true;

}


/* ==========================================
   GET ALL BOOKS
========================================== */

json getBooks() {

    json books = json::array();

    auto stmt = prepare(

        "SELECT book_id, title, author, category, "
        "isbn, quantity, available FROM Books "
        "ORDER BY book_id DESC"

    );

    while(sqlite3_step(stmt.get()) == SQLITE_ROW) {

        json book;

        book["book_id"] =
            sqlite3_column_int(stmt.get(), 0);

        book["title"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 1)
            );

        book["author"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 2)
            );

        book["category"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 3)
            );

        book["isbn"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 4)
            );

        book["quantity"] =
            sqlite3_column_int(stmt.get(), 5);

        book["available"] =
            sqlite3_column_int(stmt.get(), 6);

        books.push_back(book);

    }

    return books;

}


/* ==========================================
   ADD NEW BOOK
========================================== */

void addBook(
    const httplib::Request& req,
    httplib::Response& res
) {

    json data = json::parse(req.body);

    string title =
        data.at("title").get<string>();

    string author =
        data.at("author").get<string>();

    string category =
        data.at("category").get<string>();

    string isbn =
        data.at("isbn").get<string>();

    int quantity =
        data.at("quantity").get<int>();


    if(title.empty() ||
       author.empty() ||
       category.empty() ||
       isbn.empty()) {

        throw invalid_argument(
            "All book details are required"
        );

    }

    if(quantity <= 0) {

        throw invalid_argument(
            "Quantity must be greater than zero"
        );

    }


    auto stmt = prepare(

        "INSERT INTO Books "
        "(title, author, category, isbn, quantity, available) "
        "VALUES (?, ?, ?, ?, ?, ?)"

    );

    bindText(stmt.get(), 1, title);
    bindText(stmt.get(), 2, author);
    bindText(stmt.get(), 3, category);
    bindText(stmt.get(), 4, isbn);
    bindInt(stmt.get(), 5, quantity);
    bindInt(stmt.get(), 6, quantity);

    execute(stmt.get());


    success(
        req,
        res,
        {
            {"message", "Book added successfully!"},
            {"book_id", sqlite3_last_insert_rowid(db)}
        }
    );

}


/* ==========================================
   GET ALL MEMBERS
========================================== */

json getMembers() {

    json members = json::array();

    auto stmt = prepare(

        "SELECT member_id, name, email, phone, address "
        "FROM Members ORDER BY member_id DESC"

    );

    while(sqlite3_step(stmt.get()) == SQLITE_ROW) {

        json member;

        member["member_id"] =
            sqlite3_column_int(stmt.get(), 0);

        member["name"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 1)
            );

        member["email"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 2)
            );

        member["phone"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 3)
            );

        const unsigned char* address =
            sqlite3_column_text(stmt.get(), 4);

        member["address"] =
            address
            ? reinterpret_cast<const char*>(address)
            : "";

        members.push_back(member);

    }

    return members;

}


/* ==========================================
   REGISTER MEMBER
========================================== */

void addMember(
    const httplib::Request& req,
    httplib::Response& res
) {

    json data = json::parse(req.body);

    string name =
        data.at("name").get<string>();

    string email =
        data.at("email").get<string>();

    string phone =
        data.at("phone").get<string>();

    string address =
        data.value("address", "");


    if(name.empty() ||
       email.empty() ||
       phone.empty()) {

        throw invalid_argument(
            "Name, email and phone are required"
        );

    }


    auto stmt = prepare(

        "INSERT INTO Members "
        "(name, email, phone, address) "
        "VALUES (?, ?, ?, ?)"

    );

    bindText(stmt.get(), 1, name);
    bindText(stmt.get(), 2, email);
    bindText(stmt.get(), 3, phone);
    bindText(stmt.get(), 4, address);

    execute(stmt.get());


    success(
        req,
        res,
        {
            {"message", "Member registered successfully!"},
            {"member_id", sqlite3_last_insert_rowid(db)}
        }
    );

}


/* ==========================================
   ISSUE BOOK
========================================== */

void issueBook(
    const httplib::Request& req,
    httplib::Response& res
) {

    json data = json::parse(req.body);

    int bookId =
        data.at("book_id").get<int>();

    int memberId =
        data.at("member_id").get<int>();

    string dueDate =
        data.at("due_date").get<string>();


    if(!validId(bookId) ||
       !validId(memberId)) {

        throw invalid_argument(
            "Invalid book or member ID"
        );

    }

    if(!validDate(dueDate)) {

        throw invalid_argument(
            "Invalid due date"
        );

    }


    char* error = nullptr;

    if(sqlite3_exec(
        db,
        "BEGIN IMMEDIATE TRANSACTION",
        nullptr,
        nullptr,
        &error
    ) != SQLITE_OK) {

        string message = error ? error : "Transaction failed";

        sqlite3_free(error);

        throw runtime_error(message);

    }


    try {

        // Check whether book exists and is available.

        auto book = prepare(

            "SELECT available FROM Books "
            "WHERE book_id = ?"

        );

        bindInt(book.get(), 1, bookId);


        if(sqlite3_step(book.get()) != SQLITE_ROW) {

            throw runtime_error("Book not found");

        }

        int available =
            sqlite3_column_int(book.get(), 0);

        if(available <= 0) {

            throw runtime_error(
                "Book is currently unavailable"
            );

        }


        // Check whether member exists.

        auto member = prepare(

            "SELECT member_id FROM Members "
            "WHERE member_id = ?"

        );

        bindInt(member.get(), 1, memberId);

        if(sqlite3_step(member.get()) != SQLITE_ROW) {

            throw runtime_error("Member not found");

        }


        // Reduce available quantity.

        auto updateBook = prepare(

            "UPDATE Books "
            "SET available = available - 1 "
            "WHERE book_id = ? AND available > 0"

        );

        bindInt(updateBook.get(), 1, bookId);

        execute(updateBook.get());


        if(sqlite3_changes(db) != 1) {

            throw runtime_error(
                "Book availability changed. Try again."
            );

        }


        // Create issue record.

        auto issue = prepare(

            "INSERT INTO IssueRecords "
            "(book_id, member_id, due_date, status) "
            "VALUES (?, ?, ?, 'Issued')"

        );

        bindInt(issue.get(), 1, bookId);
        bindInt(issue.get(), 2, memberId);
        bindText(issue.get(), 3, dueDate);

        execute(issue.get());


        if(sqlite3_exec(
            db,
            "COMMIT",
            nullptr,
            nullptr,
            nullptr
        ) != SQLITE_OK) {

            throw runtime_error(sqlite3_errmsg(db));

        }


        success(
            req,
            res,
            {
                {"message", "Book issued successfully!"},
                {"issue_id", sqlite3_last_insert_rowid(db)}
            }
        );

    }

    catch(...) {

        sqlite3_exec(
            db,
            "ROLLBACK",
            nullptr,
            nullptr,
            nullptr
        );

        throw;

    }

}


/* ==========================================
   RETURN BOOK
========================================== */

void returnBook(
    const httplib::Request& req,
    httplib::Response& res
) {

    json data = json::parse(req.body);

    int issueId =
        data.at("issue_id").get<int>();


    if(!validId(issueId)) {

        throw invalid_argument("Invalid issue ID");

    }


    if(sqlite3_exec(
        db,
        "BEGIN IMMEDIATE TRANSACTION",
        nullptr,
        nullptr,
        nullptr
    ) != SQLITE_OK) {

        throw runtime_error(sqlite3_errmsg(db));

    }


    try {

        auto findIssue = prepare(

            "SELECT book_id, status "
            "FROM IssueRecords WHERE issue_id = ?"

        );

        bindInt(findIssue.get(), 1, issueId);


        if(sqlite3_step(findIssue.get()) != SQLITE_ROW) {

            throw runtime_error(
                "Issue record not found"
            );

        }


        int bookId =
            sqlite3_column_int(findIssue.get(), 0);

        string status =
            reinterpret_cast<const char*>(
                sqlite3_column_text(findIssue.get(), 1)
            );


        if(status == "Returned") {

            throw runtime_error(
                "This book has already been returned"
            );

        }


        // Mark issue record as returned.

        auto updateIssue = prepare(

            "UPDATE IssueRecords "
            "SET status = 'Returned', "
            "return_date = CURRENT_DATE "
            "WHERE issue_id = ? AND status = 'Issued'"

        );

        bindInt(updateIssue.get(), 1, issueId);

        execute(updateIssue.get());


        if(sqlite3_changes(db) != 1) {

            throw runtime_error(
                "Return operation failed"
            );

        }


        // Increase available quantity.

        auto updateBook = prepare(

            "UPDATE Books "
            "SET available = available + 1 "
            "WHERE book_id = ?"

        );

        bindInt(updateBook.get(), 1, bookId);

        execute(updateBook.get());


        if(sqlite3_exec(
            db,
            "COMMIT",
            nullptr,
            nullptr,
            nullptr
        ) != SQLITE_OK) {

            throw runtime_error(sqlite3_errmsg(db));

        }


        success(
            req,
            res,
            {
                {"message", "Book returned successfully!"}
            }
        );

    }

    catch(...) {

        sqlite3_exec(
            db,
            "ROLLBACK",
            nullptr,
            nullptr,
            nullptr
        );

        throw;

    }

}


/* ==========================================
   ISSUE AND RETURN HISTORY
========================================== */

json getRecords() {

    json records = json::array();

    auto stmt = prepare(R"SQL(

        SELECT
            I.issue_id,
            B.title,
            M.name,
            I.issue_date,
            I.due_date,
            I.return_date,
            I.status

        FROM IssueRecords I

        JOIN Books B
        ON I.book_id = B.book_id

        JOIN Members M
        ON I.member_id = M.member_id

        ORDER BY I.issue_id DESC

    )SQL");


    while(sqlite3_step(stmt.get()) == SQLITE_ROW) {

        json record;

        record["issue_id"] =
            sqlite3_column_int(stmt.get(), 0);

        record["title"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 1)
            );

        record["member_name"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 2)
            );

        record["issue_date"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 3)
            );

        record["due_date"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 4)
            );

        const unsigned char* returnDate =
            sqlite3_column_text(stmt.get(), 5);

        record["return_date"] =
            returnDate
            ? reinterpret_cast<const char*>(returnDate)
            : "";

        record["status"] =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt.get(), 6)
            );

        records.push_back(record);

    }

    return records;

}


/* ==========================================
   DASHBOARD STATISTICS
========================================== */

json getDashboard() {

    json result;


    auto books = prepare(

        "SELECT "
        "COALESCE(SUM(quantity), 0), "
        "COALESCE(SUM(available), 0) "
        "FROM Books"

    );

    if(sqlite3_step(books.get()) == SQLITE_ROW) {

        result["total_books"] =
            sqlite3_column_int(books.get(), 0);

        result["available_books"] =
            sqlite3_column_int(books.get(), 1);

    }


    auto members = prepare(

        "SELECT COUNT(*) FROM Members"

    );

    if(sqlite3_step(members.get()) == SQLITE_ROW) {

        result["total_members"] =
            sqlite3_column_int(members.get(), 0);

    }


    auto issued = prepare(

        "SELECT COUNT(*) FROM IssueRecords "
        "WHERE status = 'Issued'"

    );

    if(sqlite3_step(issued.get()) == SQLITE_ROW) {

        result["issued_books"] =
            sqlite3_column_int(issued.get(), 0);

    }


    return result;

}


/* ==========================================
   MAIN FUNCTION AND HTTP SERVER
========================================== */

int main() {

    try {

        connectDatabase();

        initializeDatabase();


        httplib::Server server;


        // GET BOOKS

        server.Get(
            "/api/books",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    success(req, res, getBooks());

                }

                catch(const exception& e) {

                    failure(res, e.what(), 500);

                }

            }
        );


        // ADD BOOK

        server.Post(
            "/api/books",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    addBook(req, res);

                }

                catch(const exception& e) {

                    failure(res, e.what());

                }

            }
        );


        // GET MEMBERS

        server.Get(
            "/api/members",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    success(req, res, getMembers());

                }

                catch(const exception& e) {

                    failure(res, e.what(), 500);

                }

            }
        );


        // REGISTER MEMBER

        server.Post(
            "/api/members",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    addMember(req, res);

                }

                catch(const exception& e) {

                    failure(res, e.what());

                }

            }
        );


        // ISSUE BOOK

        server.Post(
            "/api/issue",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    issueBook(req, res);

                }

                catch(const exception& e) {

                    failure(res, e.what());

                }

            }
        );


        // RETURN BOOK

        server.Post(
            "/api/return",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    returnBook(req, res);

                }

                catch(const exception& e) {

                    failure(res, e.what());

                }

            }
        );


        // ISSUE RECORDS

        server.Get(
            "/api/records",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    success(req, res, getRecords());

                }

                catch(const exception& e) {

                    failure(res, e.what(), 500);

                }

            }
        );


        // DASHBOARD

        server.Get(
            "/api/dashboard",
            [](const httplib::Request& req,
               httplib::Response& res) {

                try {

                    success(req, res, getDashboard());

                }

                catch(const exception& e) {

                    failure(res, e.what(), 500);

                }

            }
        );


        // SERVE HTML FRONTEND

        server.set_mount_point(
            "/",
            "../frontend"
        );


        cout << "\n====================================\n";

        cout << "LIBRARY MANAGEMENT SYSTEM STARTED\n";

        cout << "Open: http://localhost:8080\n";

        cout << "====================================\n";


        server.listen(
            "127.0.0.1",
            8080
        );


        sqlite3_close(db);

    }

    catch(const exception& e) {

        cerr << "ERROR: " << e.what() << endl;

        if(db != nullptr) {

            sqlite3_close(db);

        }

        return 1;

    }

    return 0;

}

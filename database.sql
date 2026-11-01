
-- ==========================================
-- LIBRARY MANAGEMENT SYSTEM DATABASE
-- ==========================================

PRAGMA foreign_keys = ON;

-- 1. BOOKS TABLE
CREATE TABLE IF NOT EXISTS Books (
    book_id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    author TEXT NOT NULL,
    category TEXT NOT NULL,
    isbn TEXT UNIQUE NOT NULL,
    quantity INTEGER NOT NULL CHECK(quantity >= 0),
    available INTEGER NOT NULL CHECK(available >= 0),
    CHECK(available <= quantity)
);

-- 2. MEMBERS TABLE
CREATE TABLE IF NOT EXISTS Members (
    member_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    phone TEXT NOT NULL,
    address TEXT
);

-- 3. ISSUE RECORDS TABLE
CREATE TABLE IF NOT EXISTS IssueRecords (
    issue_id INTEGER PRIMARY KEY AUTOINCREMENT,
    book_id INTEGER NOT NULL,
    member_id INTEGER NOT NULL,
    issue_date TEXT NOT NULL DEFAULT CURRENT_DATE,
    due_date TEXT NOT NULL,
    return_date TEXT,
    status TEXT NOT NULL DEFAULT 'Issued'
        CHECK(status IN ('Issued', 'Returned')),

    FOREIGN KEY(book_id) REFERENCES Books(book_id),
    FOREIGN KEY(member_id) REFERENCES Members(member_id)
);

-- 4. SAMPLE BOOK RECORDS
INSERT OR IGNORE INTO Books
(title, author, category, isbn, quantity, available)
VALUES
('The C++ Programming Language', 'Bjarne Stroustrup',
 'Programming', '9780321563842', 10, 10),

('Introduction to Algorithms', 'Thomas H. Cormen',
 'Computer Science', '9780262046305', 8, 8),

('Database System Concepts', 'Abraham Silberschatz',
 'Database', '9780078022159', 5, 5),

('Operating System Concepts', 'Abraham Silberschatz',
 'Operating Systems', '9781119800361', 7, 7),

('Clean Code', 'Robert C. Martin',
 'Programming', '9780132350884', 6, 6);

-- 5. SAMPLE MEMBER
INSERT OR IGNORE INTO Members
(name, email, phone, address)
VALUES
('Rahul Sharma', 'rahul@example.com',
 '9876543210', 'Delhi');

-- 6. VIEW ALL BOOKS
SELECT * FROM Books;

-- 7. VIEW ALL MEMBERS
SELECT * FROM Members;

-- 8. VIEW ISSUED BOOKS
SELECT * FROM IssueRecords
WHERE status = 'Issued';

-- 9. VIEW COMPLETE ISSUE HISTORY
SELECT
    IssueRecords.issue_id,
    Books.title,
    Members.name,
    IssueRecords.issue_date,
    IssueRecords.due_date,
    IssueRecords.return_date,
    IssueRecords.status
FROM IssueRecords
JOIN Books ON IssueRecords.book_id = Books.book_id
JOIN Members ON IssueRecords.member_id = Members.member_id;


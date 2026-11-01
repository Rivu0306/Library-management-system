
<!DOCTYPE html>
<html lang="en">

<head>
<meta charset="UTF-8">
<meta name="viewport"
content="width=device-width, initial-scale=1.0">

<title>Library Management System</title>

<style>

/* GENERAL DESIGN */

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: Arial, sans-serif;
}

body {
    background: #f2f5fa;
    color: #263238;
}

/* HEADER */

header {
    background: #172554;
    color: white;
    padding: 22px;
    text-align: center;
}

header h1 {
    margin-bottom: 7px;
}

/* NAVIGATION */

nav {
    background: #1e40af;
    padding: 15px;
    display: flex;
    justify-content: center;
    flex-wrap: wrap;
    gap: 12px;
}

nav button {
    padding: 11px 18px;
    border: none;
    background: white;
    color: #1e40af;
    border-radius: 5px;
    cursor: pointer;
    font-weight: bold;
}

nav button:hover {
    background: #dbeafe;
}

/* MAIN CONTENT */

.container {
    max-width: 1200px;
    margin: 25px auto;
    padding: 20px;
}

.section {
    background: white;
    padding: 25px;
    margin-bottom: 25px;
    border-radius: 10px;
    box-shadow: 0 2px 10px #00000012;
}

.section h2 {
    margin-bottom: 20px;
    color: #1e3a8a;
}

/* DASHBOARD */

.dashboard {
    display: grid;
    grid-template-columns:
    repeat(auto-fit, minmax(180px, 1fr));
    gap: 20px;
}

.stat-card {
    padding: 25px;
    color: white;
    background: #2563eb;
    border-radius: 10px;
    text-align: center;
}

.stat-card h3 {
    font-size: 16px;
    margin-bottom: 12px;
}

.stat-card p {
    font-size: 30px;
    font-weight: bold;
}

/* FORMS */

form {
    display: grid;
    grid-template-columns:
    repeat(auto-fit, minmax(220px, 1fr));
    gap: 15px;
}

input, select, textarea {
    width: 100%;
    padding: 12px;
    border: 1px solid #cbd5e1;
    border-radius: 5px;
    font-size: 14px;
}

textarea {
    resize: vertical;
}

.action-btn {
    padding: 12px;
    background: #2563eb;
    color: white;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-weight: bold;
}

.action-btn:hover {
    background: #1d4ed8;
}

/* TABLE */

.table-wrapper {
    overflow-x: auto;
}

table {
    width: 100%;
    border-collapse: collapse;
    margin-top: 15px;
}

th, td {
    padding: 13px;
    border: 1px solid #ddd;
    text-align: left;
}

th {
    background: #1e40af;
    color: white;
}

tr:nth-child(even) {
    background: #f1f5f9;
}

/* SEARCH */

.search-box {
    margin-bottom: 15px;
}

/* SECTION VISIBILITY */

.hidden {
    display: none;
}

/* STATUS */

.status {
    margin-top: 15px;
    font-weight: bold;
    color: #15803d;
}

.error {
    color: #dc2626;
}

footer {
    background: #172554;
    color: white;
    padding: 20px;
    text-align: center;
}

@media(max-width:600px) {
    .container {
        padding: 10px;
    }

    nav button {
        width: 45%;
    }
}

</style>
</head>

<body>

<header>
    <h1>LIBRARY MANAGEMENT SYSTEM</h1>
    <p>Manage Books, Members and Transactions</p>
</header>

<nav>
    <button onclick="showSection('dashboard')">
        Dashboard
    </button>

    <button onclick="showSection('books')">
        Books
    </button>

    <button onclick="showSection('members')">
        Members
    </button>

    <button onclick="showSection('issue')">
        Issue Book
    </button>

    <button onclick="showSection('returns')">
        Return Book
    </button>

    <button onclick="showSection('records')">
        Records
    </button>
</nav>

<div class="container">

<!-- DASHBOARD -->

<section id="dashboard" class="section">

<h2>Library Dashboard</h2>

<div class="dashboard">

    <div class="stat-card">
        <h3>Total Books</h3>
        <p id="totalBooks">0</p>
    </div>

    <div class="stat-card">
        <h3>Available Books</h3>
        <p id="availableBooks">0</p>
    </div>

    <div class="stat-card">
        <h3>Total Members</h3>
        <p id="totalMembers">0</p>
    </div>

    <div class="stat-card">
        <h3>Issued Books</h3>
        <p id="issuedBooks">0</p>
    </div>

</div>

</section>


<!-- BOOK MANAGEMENT -->

<section id="books" class="section hidden">

<h2>Book Management</h2>

<h3>Add New Book</h3>

<form id="bookForm">

<input type="text" id="bookTitle"
placeholder="Book Title" required>

<input type="text" id="bookAuthor"
placeholder="Author Name" required>

<input type="text" id="bookCategory"
placeholder="Book Category" required>

<input type="text" id="bookISBN"
placeholder="ISBN Number" required>

<input type="number" id="bookQuantity"
placeholder="Quantity" min="1" required>

<button class="action-btn" type="submit">
    Add Book
</button>

</form>

<p id="bookMessage" class="status"></p>

<hr style="margin:25px 0">

<h3>Available Book Inventory</h3>

<input class="search-box" type="text"
id="bookSearch"
placeholder="Search books by title or author..."
oninput="renderBooks()">

<div class="table-wrapper">

<table>

<thead>
<tr>
<th>ID</th>
<th>Title</th>
<th>Author</th>
<th>Category</th>
<th>ISBN</th>
<th>Quantity</th>
<th>Available</th>
</tr>
</thead>

<tbody id="bookTable"></tbody>

</table>

</div>

</section>


<!-- MEMBER MANAGEMENT -->

<section id="members" class="section hidden">

<h2>Member Management</h2>

<h3>Register New Member</h3>

<form id="memberForm">

<input type="text" id="memberName"
placeholder="Full Name" required>

<input type="email" id="memberEmail"
placeholder="Email Address" required>

<input type="tel" id="memberPhone"
placeholder="Phone Number" required>

<input type="text" id="memberAddress"
placeholder="Address">

<button class="action-btn" type="submit">
    Register Member
</button>

</form>

<p id="memberMessage" class="status"></p>

<hr style="margin:25px 0">

<h3>Registered Members</h3>

<div class="table-wrapper">

<table>

<thead>
<tr>
<th>ID</th>
<th>Name</th>
<th>Email</th>
<th>Phone</th>
<th>Address</th>
</tr>
</thead>

<tbody id="memberTable"></tbody>

</table>

</div>

</section>


<!-- ISSUE BOOK -->

<section id="issue" class="section hidden">

<h2>Issue Book</h2>

<form id="issueForm">

<input type="number" id="issueBookId"
placeholder="Enter Book ID" min="1" required>

<input type="number" id="issueMemberId"
placeholder="Enter Member ID" min="1" required>

<label>
    Due Date:
    <input type="date" id="dueDate" required>
</label>

<button class="action-btn" type="submit">
    Issue Book
</button>

</form>

<p id="issueMessage" class="status"></p>

</section>


<!-- RETURN BOOK -->

<section id="returns" class="section hidden">

<h2>Return Book</h2>

<form id="returnForm">

<input type="number" id="returnIssueId"
placeholder="Enter Issue ID" min="1" required>

<button class="action-btn" type="submit">
    Return Book
</button>

</form>

<p id="returnMessage" class="status"></p>

</section>


<!-- ISSUE RECORDS -->

<section id="records" class="section hidden">

<h2>Issued Book Records</h2>

<button class="action-btn"
onclick="loadRecords()">
    Refresh Records
</button>

<div class="table-wrapper">

<table>

<thead>
<tr>
<th>Issue ID</th>
<th>Book</th>
<th>Member</th>
<th>Issue Date</th>
<th>Due Date</th>
<th>Return Date</th>
<th>Status</th>
</tr>
</thead>

<tbody id="recordTable"></tbody>

</table>

</div>

</section>

</div>

<footer>
    <p>Library Management System</p>
    <p>Developed using HTML, CSS, C++ and SQL</p>
</footer>


<script>

/* BACKEND SERVER ADDRESS */

const API = "http://localhost:8080";


/* SECTION NAVIGATION */

function showSection(sectionId) {

    document.querySelectorAll(".section")
    .forEach(section => {
        section.classList.add("hidden");
    });

    document.getElementById(sectionId)
    .classList.remove("hidden");

    if(sectionId === "dashboard") {
        loadDashboard();
    }

    if(sectionId === "books") {
        loadBooks();
    }

    if(sectionId === "members") {
        loadMembers();
    }

    if(sectionId === "records") {
        loadRecords();
    }
}


/* COMMON API FUNCTION */

async function apiRequest(path, method = "GET", data = null) {

    const options = {
        method: method,
        headers: {
            "Content-Type": "application/json"
        }
    };

    if(data !== null) {
        options.body = JSON.stringify(data);
    }

    const response = await fetch(API + path, options);

    const result = await response.json();

    if(!response.ok) {
        throw new Error(result.error || "Request failed");
    }

    return result;
}


/* DASHBOARD */

async function loadDashboard() {

    try {

        const data = await apiRequest("/api/dashboard");

        document.getElementById("totalBooks")
        .innerText = data.total_books;

        document.getElementById("availableBooks")
        .innerText = data.available_books;

        document.getElementById("totalMembers")
        .innerText = data.total_members;

        document.getElementById("issuedBooks")
        .innerText = data.issued_books;

    } catch(error) {
        alert(error.message);
    }
}


/* LOAD BOOKS */

let allBooks = [];

async function loadBooks() {

    try {

        allBooks = await apiRequest("/api/books");

        renderBooks();

    } catch(error) {
        alert(error.message);
    }
}


/* DISPLAY BOOKS */

function renderBooks() {

    const search =
    document.getElementById("bookSearch")
    .value.toLowerCase();

    const filtered = allBooks.filter(book =>
        book.title.toLowerCase().includes(search) ||
        book.author.toLowerCase().includes(search)
    );

    const table = document.getElementById("bookTable");

    table.innerHTML = "";

    filtered.forEach(book => {

        const row = document.createElement("tr");

        [
            book.book_id,
            book.title,
            book.author,
            book.category,
            book.isbn,
            book.quantity,
            book.available
        ].forEach(value => {

            const cell = document.createElement("td");

            cell.textContent = value;

            row.appendChild(cell);

        });

        table.appendChild(row);

    });

}


/* ADD BOOK */

document.getElementById("bookForm")
.addEventListener("submit", async function(event) {

    event.preventDefault();

    const book = {

        title: document.getElementById("bookTitle").value,

        author: document.getElementById("bookAuthor").value,

        category: document.getElementById("bookCategory").value,

        isbn: document.getElementById("bookISBN").value,

        quantity: Number(
            document.getElementById("bookQuantity").value
        )

    };

    try {

        const result = await apiRequest(
            "/api/books", "POST", book
        );

        document.getElementById("bookMessage")
        .textContent = result.message;

        this.reset();

        loadBooks();
        loadDashboard();

    } catch(error) {

        document.getElementById("bookMessage")
        .textContent = error.message;

    }

});


/* LOAD MEMBERS */

async function loadMembers() {

    try {

        const members = await apiRequest("/api/members");

        const table = document.getElementById("memberTable");

        table.innerHTML = "";

        members.forEach(member => {

            const row = document.createElement("tr");

            [
                member.member_id,
                member.name,
                member.email,
                member.phone,
                member.address
            ].forEach(value => {

                const cell = document.createElement("td");

                cell.textContent = value;

                row.appendChild(cell);

            });

            table.appendChild(row);

        });

    } catch(error) {
        alert(error.message);
    }

}


/* ADD MEMBER */

document.getElementById("memberForm")
.addEventListener("submit", async function(event) {

    event.preventDefault();

    const member = {

        name: document.getElementById("memberName").value,

        email: document.getElementById("memberEmail").value,

        phone: document.getElementById("memberPhone").value,

        address: document.getElementById("memberAddress").value

    };

    try {

        const result = await apiRequest(
            "/api/members", "POST", member
        );

        document.getElementById("memberMessage")
        .textContent = result.message;

        this.reset();

        loadMembers();
        loadDashboard();

    } catch(error) {

        document.getElementById("memberMessage")
        .textContent = error.message;

    }

});


/* ISSUE BOOK */

document.getElementById("issueForm")
.addEventListener("submit", async function(event) {

    event.preventDefault();

    const issue = {

        book_id: Number(
            document.getElementById("issueBookId").value
        ),

        member_id: Number(
            document.getElementById("issueMemberId").value
        ),

        due_date: document.getElementById("dueDate").value

    };

    try {

        const result = await apiRequest(
            "/api/issue", "POST", issue
        );

        document.getElementById("issueMessage")
        .textContent = result.message;

        this.reset();

        loadDashboard();

    } catch(error) {

        document.getElementById("issueMessage")
        .textContent = error.message;

    }

});


/* RETURN BOOK */

document.getElementById("returnForm")
.addEventListener("submit", async function(event) {

    event.preventDefault();

    const issue = {

        issue_id: Number(
            document.getElementById("returnIssueId").value
        )

    };

    try {

        const result = await apiRequest(
            "/api/return", "POST", issue
        );

        document.getElementById("returnMessage")
        .textContent = result.message;

        this.reset();

        loadDashboard();

    } catch(error) {

        document.getElementById("returnMessage")
        .textContent = error.message;

    }

});


/* LOAD ISSUE RECORDS */

async function loadRecords() {

    try {

        const records = await apiRequest("/api/records");

        const table = document.getElementById("recordTable");

        table.innerHTML = "";

        records.forEach(record => {

            const row = document.createElement("tr");

            [
                record.issue_id,
                record.title,
                record.member_name,
                record.issue_date,
                record.due_date,
                record.return_date || "-",
                record.status
            ].forEach(value => {

                const cell = document.createElement("td");

                cell.textContent = value;

                row.appendChild(cell);

            });

            table.appendChild(row);

        });

    } catch(error) {
        alert(error.message);
    }

}


/* INITIAL LOAD */

loadDashboard();

</script>

</body>
</html>

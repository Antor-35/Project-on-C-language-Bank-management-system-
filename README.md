```
╔══════════════════════════════════════════════════════════════════╗
║                                                                  ║
║   ██████╗  █████╗ ███╗   ██╗██╗  ██╗    ███████╗██╗   ██╗███████╗║
║   ██╔══██╗██╔══██╗████╗  ██║██║ ██╔╝    ██╔════╝╚██╗ ██╔╝██╔════╝║
║   ██████╔╝███████║██╔██╗ ██║█████╔╝     ███████╗ ╚████╔╝ ███████╗║
║   ██╔══██╗██╔══██║██║╚██╗██║██╔═██╗     ╚════██║  ╚██╔╝  ╚════██║║
║   ██████╔╝██║  ██║██║ ╚████║██║  ██╗    ███████║   ██║   ███████║║
║   ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝   ╚══════╝   ╚═╝   ╚══════╝║
║                                                                  ║
║          MANAGEMENT SYSTEM  //  Written in C  //  CLI App        ║
║                   [ Antor-35 / DIU CSE Project ]                 ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝
```

<div align="center">

![Language](https://img.shields.io/badge/Language-C-00599C?style=flat-square&logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-555555?style=flat-square&logo=linux&logoColor=white)
![Lines](https://img.shields.io/badge/Lines%20of%20Code-1234-00d4aa?style=flat-square)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=flat-square)
![Author](https://img.shields.io/badge/Author-Anas%20Khan%20Antor-0A66C2?style=flat-square&logo=github&logoColor=white)

</div>

---

## `$ ./bank --about`

A fully-featured **terminal-based Banking Management System** built in C as a university project at **Daffodil International University (DIU)**. The system simulates real banking operations with a colored, interactive CLI interface — file-based data persistence, secure PIN input, dual-theme UI, and both user & admin panels.

```
┌─────────────────────────────────────────────────────────────┐
│  project     : Bank Management System                       │
│  language    : C (C99 standard)                             │
│  lines       : 1,234                                        │
│  file        : Bank management system.c                     │
│  storage     : Binary file I/O (users.dat)                  │
│  ui          : Terminal / CLI with ANSI color themes        │
│  platform    : Cross-platform (Linux & Windows)             │
│  author      : Anas Khan Antor  @  DIU CSE                  │
└─────────────────────────────────────────────────────────────┘
```

---

## `$ ./bank --features`

<table>
<tr>
<td width="50%">

### 👤 User Module
```
✔  Account creation (signup)
✔  Secure login (ID + hidden PIN)
✔  Deposit money
✔  Withdraw money
✔  Transfer money to other accounts
✔  Check balance
✔  Transaction history (last 100)
✔  Export transaction history to .txt
✔  Change PIN
✔  Logout
```

</td>
<td width="50%">

### 🔐 Admin Module
```
✔  Admin login (ID: 0, PIN: 1234)
✔  View all registered accounts
✔  Delete any user account
✔  Export all user data to backup file
✔  View total bank balance
✔  Red-themed admin UI (Theme 5)
✔  Separate access control
```

</td>
</tr>
</table>

### ⚙️ System Features
```
✔  Arrow-key cursor navigation (menus)
✔  Hidden PIN entry (no echo, asterisk masking)
✔  ANSI color themes — Gray (user) / Red (admin)
✔  Cross-platform: Windows (_getch) + Linux (termios)
✔  Binary file storage (users.dat) — persistent data
✔  ASCII box UI for centered, styled terminal output
✔  Login animation with typewriter effect
✔  Receipt printing after every transaction
```

---

## `$ ./bank --architecture`

```
main()
├── show_login_animation()
└── main_menu()
    ├── login()           →  account_menu()
    │                         ├── check_balance()
    │                         ├── deposit()
    │                         ├── withdraw()
    │                         ├── transfer()
    │                         ├── ShowHistory()
    │                         ├── export_history()
    │                         └── change_pin()
    ├── signup()
    └── admin_login()     →  admin_panel()
                              ├── view_all_users()
                              ├── delete_user()
                              ├── export_all_data()
                              └── view_total_balance()
```

---

## `$ ./bank --data-structures`

```c
/* Transaction Record */
typedef struct {
    char  action[50];      // "Deposit", "Withdraw", "Transfer Sent"
    float amount;          // Amount involved
    int   balance;         // Balance after transaction
} transaction;

/* User Account */
typedef struct {
    char        name[50];          // Account holder name
    int         id;                // Unique account ID
    int         pin;               // 4–6 digit PIN (hidden input)
    int         balance;           // Current balance
    transaction history[100];      // Last 100 transactions
    int         history_count;     // Transaction count
} user;
```

---

## `$ gcc --compile`

### Prerequisites
- GCC compiler (`gcc` on Linux / MinGW on Windows)
- Terminal with ANSI color support

### Linux / macOS
```bash
gcc -o bank "Bank management system.c"
./bank
```

### Windows (MinGW)
```bash
gcc -o bank.exe "Bank management system.c"
bank.exe
```

> **Note:** On Windows 10+, ANSI colors are enabled automatically via `SetConsoleMode`. On older Windows, the system gracefully falls back to plain text.

---

## `$ ./bank --demo`

```
╔──────────────────────╗
│      MAIN MENU       │
╚──────────────────────╝

  ▶ Login
    Create Account
    Admin Login
    Exit

[ Use ↑ ↓ arrow keys + ENTER to navigate ]
```

```
╔──────────────────────╗
│   DEPOSIT RECEIPT    │
╚──────────────────────╝

  Previous Balance:  500 USD
  Amount:           +200 USD
  New Balance:       700 USD
```

---

## `$ cat skills-demonstrated.txt`

```
CONCEPT                  APPLIED IN
──────────────────────────────────────────────────
Structs & typedef        user, transaction records
File I/O (binary)        fread, fwrite, fseek, ftell
Pointer handling         *acc passed across functions
Terminal control         termios (Linux), conio (Windows)
ANSI escape codes        color themes, cursor control
Cross-platform macros    #ifdef _WIN32 / #else
Input validation         PIN masking, balance checks
Modular functions        20+ functions, clean separation
──────────────────────────────────────────────────
```

---

## `$ cat notes.txt`

> ⚠️ **Default Admin Credentials** — `ID: 0` · `PIN: 1234`  
> Change these before deploying in any real environment.

> 📁 **Data File** — All accounts stored in `users.dat` (binary). Keep it in the same directory as the executable.

> 🔒 **PIN Security** — PIN input uses raw terminal mode. Characters are masked with `*` and never echoed to screen.

---

## `$ git log --author`

**Author:** Anas Khan Antor  
**University:** Daffodil International University (DIU)  
**Department:** B.Sc. in Computer Science & Engineering  
**Year:** 2nd Year  

[![GitHub](https://img.shields.io/badge/GitHub-Antor--35-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/Antor-35)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0A66C2?style=flat-square&logo=linkedin&logoColor=white)](https://linkedin.com/in/YOUR_LINKEDIN)

---

```
╔══════════════════════════════════════════════════════╗
║  Built with curiosity, C pointers, and zero malloc  ║
║  errors (eventually). 📄🔐                          ║
║                                    — Anas Khan Antor║
╚══════════════════════════════════════════════════════╝
```

// =========================================================
// BANKING MANAGEMENT SYSTEM
// =========================================================
// A complete terminal-based banking application with:
// - User authentication (login/signup)
// - Account management (deposit, withdraw, transfer)
// - Transaction history tracking
// - Admin panel for system management
// - Colored terminal UI with themes
// - Cross-platform support (Windows/Linux)
// =========================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

// Platform-specific includes
// - Windows: conio.h for console I/O (_getch function)
// - Linux/Mac: termios.h for terminal control (keyboard input)
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define STDIN_FILENO 0    // Standard input file descriptor
#define STDOUT_FILENO 1   // Standard output file descriptor
#endif

// -------------------------------------------------------------
// FILE NAME FOR STORAGE
// -------------------------------------------------------------
const char account_file[] = "users.dat";

// -------------------------------------------------------------
// COLOR MACROS
// -------------------------------------------------------------
#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDYELLOW "\033[1m\033[33m"
#define BOLDBLUE "\033[1m\033[34m"

#define REVERSE "\033[7m"

// =========================================================
// THEME DEFINITIONS - Two color schemes
// =========================================================

// Theme 4 — Minimal (Gray & White for normal users)
#define T4_TITLE "\033[1;37m"       // Bold white text
#define T4_BOX "\033[37m"           // Light gray borders
#define T4_HL "\033[1;30m\033[47m"  // Black text on white bg (highlight)

// Theme 5 — Red Admin (Red & Black for admin login)
#define T5_TITLE "\033[1;31m"       // Bold red text
#define T5_BOX "\033[31m"           // Red borders
#define T5_HL "\033[7;31m"          // Red inverted highlight

#define TX_RESET "\033[0m"          // Reset all formatting

// =========================================================
// GLOBAL VARIABLES FOR THEMING AND COLOR SUPPORT
// =========================================================
int CURRENT_THEME = 4;   // Currently active theme (4=user, 5=admin)
int USE_COLORS = 1;      // Flag: 1=colors enabled, 0=plain text (for compatibility)

// =========================================================
// WINDOWS ANSI COLOR SUPPORT
// =========================================================
// Windows console doesn't support ANSI codes by default.
// This function enables ANSI escape sequence support on Windows 10+.
// On Linux/Mac, ANSI codes work natively.
#ifdef _WIN32
#include <windows.h>
void enable_ansi_colors() {
    // Get the Windows console output handle
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        USE_COLORS = 0;  // Failed to get handle, disable colors
        return;
    }

    // Get current console mode
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        USE_COLORS = 0;  // Failed to read mode, disable colors
        return;
    }

    // Enable VIRTUAL_TERMINAL_PROCESSING (ANSI support)
    dwMode |= 0x0004;
    if (!SetConsoleMode(hOut, dwMode))
        USE_COLORS = 0;  // Failed to enable, disable colors
}
#else
// On Linux/Mac, ANSI codes are already supported
void enable_ansi_colors() { USE_COLORS = 1; }
#endif

// =========================================================
// COLOR HELPER FUNCTIONS
// =========================================================
// These functions return the appropriate color code based on
// the current theme. Returns empty string if colors disabled.

const char *TITLE_COLOR() {
    // Return title color for current theme
    return (USE_COLORS && CURRENT_THEME == 4) ? T4_TITLE :
           (USE_COLORS && CURRENT_THEME == 5) ? T5_TITLE : "";
}

const char *BOX_COLOR() {
    // Return border color for current theme
    return (USE_COLORS && CURRENT_THEME == 4) ? T4_BOX :
           (USE_COLORS && CURRENT_THEME == 5) ? T5_BOX : "";
}

const char *HIGHLIGHT_COLOR() {
    // Return highlight color for selected menu items
    return (USE_COLORS && CURRENT_THEME == 4) ? T4_HL :
           (USE_COLORS && CURRENT_THEME == 5) ? T5_HL : "";
}

const char *RESET_COLOR() {
    // Return reset code to clear formatting
    return USE_COLORS ? TX_RESET : "";
}

// =========================================================
// DATA STRUCTURES
// =========================================================

// Transaction record: stores individual transaction details
typedef struct
{
    char action[50];      // "Deposit", "Withdraw", "Transfer Sent", etc.
    float amount;         // Amount involved in transaction
    int balance;          // Account balance after transaction
} transaction;

// User account: complete user profile with account data
typedef struct
{
    char name[50];                  // Account holder's name
    int id;                         // Unique account ID
    int pin;                        // 4-digit PIN for authentication
    int balance;                    // Current account balance
    transaction history[100];       // Last 100 transactions
    int history_count;              // Number of transactions in history
} user;

// -------------------------------------------------------------
// FUNCTION DECLARATIONS
// -------------------------------------------------------------
void clear();
int visible_length(const char *text);
int get_terminal_width();
void print_center_box(const char *text);
void print_menu(const char *title, const char *items[], int count, int selected);
int cursor_select(const char *title, const char *items[], int count);
void print_centered(const char *text);
void view_total_balance();
void save_account_to_file(user *acc);
void print_receipt(const char *title, int before, int amount, int after);

int get_int_input(const char *prompt);
int get_pin_input(const char *prompt);  // prompt can be empty string

// BANKING SYSTEM
void main_menu();
void login();
void signup();
void account_menu(user *acc);
void admin_panel();
void admin_login();
void view_all_users();
void delete_user();
void export_all_data();

void check_balance(user *acc);
void deposit(user *acc);
void withdraw(user *acc);
void transfer(user *acc);
void ShowHistory(user *acc);
void export_history(user *acc);
void change_pin(user *acc);
void save_transaction(user *acc, const char *action, float amount);

// -------------------------------------------------------------
// MAIN
// -------------------------------------------------------------
void show_login_animation() {
    clear();

    // Title animation
    const char *title = "BANKING SYSTEM";
    int title_len = strlen(title);
    int width = get_terminal_width();

    // Typing animation
    printf("\n\n\n");
    for (int i = 0; i < title_len; i++) {
        int pad = (width - (i + 1)) / 2;
        printf("\r%*s%s%.*s%s", pad, "", T4_TITLE, i + 1, title, RESET_COLOR());
        fflush(stdout);
        usleep(100000); // 100ms delay
    }
    printf("\n");

    // Loading animation
    printf("\n");
    const char *loading_msgs[] = {
        "Initializing...",
        "Loading Secure Database...",
        "Verifying Encryption...",
        "Ready to Serve!"
    };

    for (int i = 0; i < 4; i++) {
        int pad = (width - strlen(loading_msgs[i])) / 2;
        printf("%*s%s%s%s\n", pad, "", T4_BOX, loading_msgs[i], RESET_COLOR());
        fflush(stdout);
        usleep(400000); // 400ms delay
    }

    printf("\n\n");
    printf("%sPress ENTER to continue...%s", T4_TITLE, RESET_COLOR());
    fflush(stdout);
    getchar();
    getchar();
}

int main() {
    enable_ansi_colors();
    // Force colors on for debugging
    USE_COLORS = 1;
    clear();
    show_login_animation();
    clear();
    main_menu();
    return 0;
}

// -------------------------------------------------------------
// UTILITY FUNCTIONS
// -------------------------------------------------------------
void clear()
{
    if (USE_COLORS)
        printf("\033[3J\033[H\033[2J");
    else
        for (int i = 0; i < 50; ++i) printf("\n"); // fallback: print blank lines
    fflush(stdout);
}

int visible_length(const char *text)
{
    int len = 0, esc = 0;
    for (int i = 0; text[i]; i++)
    {
        if (text[i] == '\033')
            esc = 1;
        else if (esc && text[i] == 'm')
            esc = 0;
        else if (!esc)
            len++;
    }
    return len;
}

int get_terminal_width()
{
#ifdef _WIN32
    return 80;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return 80;
    return (w.ws_col > 0) ? w.ws_col : 80;
#endif
}

void print_centered(const char *text)
{
    int width = get_terminal_width();
    int len = visible_length(text);

    int pad = (width - len) / 2;
    if (pad < 0)
        pad = 0;

    printf("%*s%s\n", pad, "", text);
}

// =========================================================
// DISPLAY FUNCTIONS - UI RENDERING
// =========================================================

// Print centered box with title (used for headers/messages)
// Creates ASCII box borders around text with theme colors
void print_center_box(const char *text)
{
    int width = get_terminal_width();
    int len = visible_length(text);
    int boxWidth = len + 6;
    int pad = (width - boxWidth) / 2;

    if (pad < 0)
        pad = 0;

    printf("%*s%s+", pad, "", BOX_COLOR());
    for (int i = 0; i < boxWidth - 2; i++)
        printf("-");
    printf("+%s\n", RESET_COLOR());

    printf("%*s|  %s%s  %s|%s\n",
           pad, "",
           TITLE_COLOR(), text, BOX_COLOR(), RESET_COLOR());

    printf("%*s+", pad, "");
    for (int i = 0; i < boxWidth - 2; i++)
        printf("-");
    printf("+%s\n", RESET_COLOR());
}

// -------------------------------------------------------------
// MENU PRINTER (CENTERED ITEMS + HIGHLIGHTED SELECTION)
// -------------------------------------------------------------
void print_menu(const char *title, const char *items[], int count, int selected)
{
    clear();
    print_center_box(title);
    printf("\n\n");

    for (int i = 0; i < count; i++)
    {
        int width = get_terminal_width();
        int len = visible_length(items[i]);
        int pad = (width - len) / 2;
        if (pad < 0)
            pad = 0;

        printf("%*s", pad, "");

        if (i == selected)
            printf("%s%s%s\n", HIGHLIGHT_COLOR(), items[i], RESET_COLOR());
        else
            printf("%s\n", items[i]);
    }

    fflush(stdout);
}

// -------------------------------------------------------------
// CURSOR SELECTION ENGINE (ARROW KEYS + ENTER)
// -------------------------------------------------------------
int cursor_select(const char *title, const char *items[], int count)
{
    char c;
    char seq[3];     // <-- FIX #1: Declare seq
    int selected = 0;

#ifndef _WIN32
    struct termios orig, raw;
    tcgetattr(STDIN_FILENO, &orig);
    raw = orig;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
#endif

    print_menu(title, items, count, selected);

    while (1)
    {
#ifdef _WIN32
        c = _getch();
#else
        read(STDIN_FILENO, &c, 1);
#endif

        // ENTER key (Windows: 13, Linux: '\n' or '\r')
        if (c == 13 || c == '\n' || c == '\r')
            break;

        // ARROW KEYS
#ifdef _WIN32
        if (c == 0 || c == 224)
        {
            c = _getch();  // arrow identifier
            if (c == 72)       // Up arrow
                selected = (selected - 1 + count) % count;
            else if (c == 80)  // Down arrow
                selected = (selected + 1) % count;
        }
#else
        if (c == '\033')
        {
            read(STDIN_FILENO, &seq[0], 1);
            read(STDIN_FILENO, &seq[1], 1);

            if (seq[0] == '[')
            {
                if (seq[1] == 'A')  // Up arrow
                    selected = (selected - 1 + count) % count;
                else if (seq[1] == 'B')  // Down arrow
                    selected = (selected + 1) % count;
            }
        }
#endif

        print_menu(title, items, count, selected);
    }

#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &orig);
#endif

    return selected + 1;
}

// -------------------------------------------------------------
// MAIN MENU
// -------------------------------------------------------------
void main_menu()
{
    const char *items[] = {
        "Login",
        "Create Account",
        "Admin Login",
        "Exit"};

    while (1)
    {
        int choice = cursor_select("MAIN MENU", items, 4);

        switch (choice)
        {
        case 1:
            login();
            break;
        case 2:
            signup();
            break;
        case 3:
            admin_login();
            break;
        case 4:
            clear();
            print_center_box("THANK YOU FOR BANKING WITH US!");
            return;
        }
    }
}

void wait()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// -------------------------------------------------------------
// LOGIN SYSTEM
// -------------------------------------------------------------
void login()
{
    FILE *file = fopen(account_file, "rb");
    if (!file)
    {
        clear();
        print_center_box("SERVER ERROR: Cannot open user database!");
        return;
    }

    clear();
    print_center_box("LOGIN");

    user acc;
    int id = get_int_input("Enter ID: ");
    int pin = get_pin_input("Enter PIN: ");

    int found = 0;

    while (fread(&acc, sizeof(acc), 1, file))
    {
        if (acc.id == id && acc.pin == pin)
        {
            found = 1;
            break;
        }
    }

    fclose(file);

    clear();
    if (!found)
    {
        print_center_box("INCORRECT ID OR PIN");
        wait();

        return;
    }

    account_menu(&acc);
}

// -------------------------------------------------------------
// SIGNUP SYSTEM
// -------------------------------------------------------------
void signup()
{
    clear();
    print_center_box("CREATE ACCOUNT");

    FILE *file = fopen(account_file, "ab+");
    FILE *file_check = fopen(account_file, "rb");

    if (!file)
    {
        clear();
        print_center_box("ERROR: Cannot open database!");
        return;
    }

    user acc;

    printf("Enter your name: ");
    scanf(" %[^\n]", acc.name);

    // Ensure unique ID
ID_RETRY:
    acc.id = get_int_input("Create a unique ID: ");

    user temp;
    if (file_check)
    {
        rewind(file_check);
        while (fread(&temp, sizeof(temp), 1, file_check))
        {
            if (temp.id == acc.id)
            {
                clear();
                print_center_box("ID ALREADY EXISTS!");
                goto ID_RETRY;
            }
        }
    }

    // PIN
    acc.pin = get_pin_input("Create PIN: ");
PIN_CONFIRM:
    int confirm = get_pin_input("Confirm PIN: ");

    if (confirm != acc.pin)
    {
        clear();
        print_center_box("PIN DOES NOT MATCH!");
        wait();

        goto PIN_CONFIRM;
    }

    // Initial Deposit
    acc.balance = get_int_input("Initial Deposit Amount: ");
    acc.history_count = 0;

    fwrite(&acc, sizeof(acc), 1, file);

    fclose(file);
    if (file_check)
        fclose(file_check);

    clear();
    print_center_box("ACCOUNT CREATED SUCCESSFULLY!");
    wait();
}

// -------------------------------------------------------------
// ADMIN LOGIN
// -------------------------------------------------------------
void admin_login()
{
    CURRENT_THEME = 5;  // Switch to red theme
    clear();
    print_center_box("ADMIN LOGIN");

    printf("%sEnter Admin ID: %s", T5_TITLE, RESET_COLOR());
    int id;
    scanf("%d", &id);

    printf("%sEnter Admin PIN: %s", T5_TITLE, RESET_COLOR());
    int pin = get_pin_input("");

    if (id == 0 && pin == 1234)
    {
        getchar(); // flush
        admin_panel();
        CURRENT_THEME = 4;
    }
    else
    {
        clear();
        print_center_box("INVALID ADMIN CREDENTIALS");
        wait();
        CURRENT_THEME = 4;
    }
}

// -------------------------------------------------------------
// ACCOUNT DASHBOARD
// -------------------------------------------------------------
void account_menu(user *acc)
{
    char title[100];
    snprintf(title, sizeof(title), "WELCOME %s", acc->name);

    const char *menu_items[] = {
        "Check Balance",
        "Deposit Money",
        "Withdraw Money",
        "Transfer Money",
        "Transaction History",
        "Change PIN",
        "Export Transaction History", // <-- ADD THIS LINE
        "Logout"};

    while (1)
    {
        int choice = cursor_select(title, menu_items, 8);

        switch (choice)
        {
        case 1:
            check_balance(acc);
            break;
        case 2:
            deposit(acc);
            break;
        case 3:
            withdraw(acc);
            break;
        case 4:
            transfer(acc);
            break;
        case 5:
            ShowHistory(acc);
            break;
        case 6:
            change_pin(acc);
            break;
        case 7:
            export_history(acc);
            break; // <-- Add this
        case 8:
            clear();
            print_center_box("LOGGED OUT SUCCESSFULLY");
            return;
        }
    }
}

// -------------------------------------------------------------
// ADMIN PANEL
// -------------------------------------------------------------
void admin_panel()
{
    const char *items[] = {
        "View All Accounts",
        "Delete Account",
        "Export All Data",
        "View Total Bank Balance",
        "Exit Admin Panel"};

    while (1)
    {
        int choice = cursor_select("ADMIN PANEL", items, 5);

        switch (choice)
        {
        case 1:
            view_all_users();
            break;
        case 2:
            delete_user();
            break;
        case 3:
            export_all_data();
            break;
        case 4:
            view_total_balance();
            break;
        case 5:
            clear();
            print_center_box("ADMIN LOGGED OUT");
            getchar();
            getchar();
            return;
        }
    }
}

void export_all_data()
{
    FILE *in = fopen(account_file, "rb");
    if (!in)
    {
        print_center_box("ERROR OPENING DATABASE!");
        wait();

        return;
    }

    FILE *out = fopen("all_users_backup.txt", "w");
    if (!out)
    {
        print_center_box("ERROR CREATING BACKUP FILE!");
        fclose(in);
        wait();

        return;
    }

    user u;
    fprintf(out, "ALL USER DATA BACKUP\n\n");

    while (fread(&u, sizeof(u), 1, in))
    {
        fprintf(out, "Name: %s\nID: %d\nBalance: %d\n\n",
                u.name, u.id, u.balance);
    }

    fclose(in);
    fclose(out);

    clear();
    print_center_box("DATA EXPORTED SUCCESSFULLY!");
    getchar();
    getchar();
}

// -------------------------------------------------------------
// GET INTEGER INPUT
// -------------------------------------------------------------
int get_int_input(const char *prompt) {
    int x;
    printf("%s ", prompt);
    if(scanf("%d", &x) != 1) {
        while(getchar() != '\n'); // flush invalid input
        return -1; // signal error
    }
    return x;
}


// -------------------------------------------------------------
// GET HIDDEN PIN INPUT (no echo)
// -------------------------------------------------------------
int get_pin_input(const char *prompt)
{
    if (prompt && prompt[0] != '\0')
        printf("%s ", prompt);

#ifndef _WIN32
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    char ch;
    char pin_str[10];
    int idx = 0;

    while (1)
    {
#ifdef _WIN32
        ch = _getch();
#else
        read(STDIN_FILENO, &ch, 1);
#endif

        if (ch == '\n' || ch == '\r')
            break;

        if (ch >= '0' && ch <= '9' && idx < 6)
        {
            pin_str[idx++] = ch;
            printf("*");
        }
    }

    pin_str[idx] = '\0';

#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    printf("\n");
    return atoi(pin_str);
}

// -------------------------------------------------------------
// SAVE TRANSACTION HISTORY
// -------------------------------------------------------------
void save_transaction(user *acc, const char *action, float amount)
{
    if (acc->history_count >= 100)
        return;

    strcpy(acc->history[acc->history_count].action, action);
    acc->history[acc->history_count].amount = amount;
    acc->history[acc->history_count].balance = acc->balance;

    acc->history_count++;
}

void print_receipt(const char *title, int before, int amount, int after)
{
    clear();
    print_center_box(title);

    printf("\n");
    printf("%s%s%s %d USD\n", USE_COLORS ? BOLDBLUE : "", "Previous Balance: ", RESET_COLOR(), before);
    printf("%s%s%s %d USD\n", USE_COLORS ? BOLDGREEN : "", "Amount:           ", RESET_COLOR(), amount);
    printf("%s%s%s %d USD\n", USE_COLORS ? BOLDYELLOW : "", "New Balance:      ", RESET_COLOR(), after);

    printf("\nPress ENTER to continue...");
    getchar();
    getchar();
}

// -------------------------------------------------------------
// SAVE ACCOUNT (WRITE CHANGES TO FILE)
// -------------------------------------------------------------
void save_account_to_file(user *acc)
{
    FILE *file = fopen(account_file, "rb+");
    if (!file)
        return;

    user temp;
    while (fread(&temp, sizeof(temp), 1, file))
    {
        if (temp.id == acc->id)
        {
            fseek(file, -sizeof(temp), SEEK_CUR);
            fwrite(acc, sizeof(*acc), 1, file);
            break;
        }
    }
    fclose(file);
}

// -------------------------------------------------------------
// CHECK BALANCE
// -------------------------------------------------------------
void check_balance(user *acc)
{
    clear();
    print_center_box("BALANCE INQUIRY");

    printf("\n");
    printf("%sYour Current Balance:%s\n", USE_COLORS ? BOLDGREEN : "", RESET_COLOR());

    char buf[100];
    snprintf(buf, sizeof(buf), "%s%d USD%s", USE_COLORS ? BOLDBLUE : "", acc->balance, RESET_COLOR());
    print_centered(buf);

    printf("\nPress ENTER to return...");
    getchar();
    getchar();
}

// -------------------------------------------------------------
// DEPOSIT MONEY
// -------------------------------------------------------------
void deposit(user *acc)
{
    clear();
    print_center_box("DEPOSIT MONEY");

    int amount = get_int_input("Enter amount to deposit: ");

    if (amount <= 0)
    {
        print_center_box("INVALID AMOUNT");
        wait();

        return;
    }

    acc->balance += amount;
    save_transaction(acc, "Deposit", amount);
    save_account_to_file(acc);

    print_receipt("DEPOSIT RECEIPT", acc->balance - amount, amount, acc->balance);
}

// -------------------------------------------------------------
// WITHDRAW MONEY
// -------------------------------------------------------------
void withdraw(user *acc)
{
    clear();
    print_center_box("WITHDRAW MONEY");

    int amount = get_int_input("Enter amount to withdraw: ");

    if (amount <= 0)
    {
        print_center_box("INVALID AMOUNT");
        return;
    }

    if (amount > acc->balance)
    {
        print_center_box("INSUFFICIENT BALANCE!");
        return;
    }

    acc->balance -= amount;
    save_transaction(acc, "Withdraw", amount);
    save_account_to_file(acc);

    print_receipt("WITHDRAWAL RECEIPT", acc->balance + amount, amount, acc->balance);
}

// -------------------------------------------------------------
// TRANSFER MONEY
// -------------------------------------------------------------
void transfer(user *acc)
{
    clear();
    print_center_box("MONEY TRANSFER");

    int receiver_id = get_int_input("Enter receiver ID: ");
    int amount = get_int_input("Enter transfer amount: ");

    if (amount <= 0)
    {
        print_center_box("INVALID AMOUNT");
        return;
    }

    if (amount > acc->balance)
    {
        print_center_box("INSUFFICIENT BALANCE!");
        return;
    }

    FILE *file = fopen(account_file, "rb+");
    if (!file)
    {
        print_center_box("ERROR: Cannot access database!");
        return;
    }

    user receiver;
    int found = 0;
    long pos;

    while (fread(&receiver, sizeof(receiver), 1, file))
    {
        if (receiver.id == receiver_id)
        {
            pos = ftell(file) - sizeof(receiver);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        clear();
        print_center_box("RECEIVER NOT FOUND!");
        fclose(file);
        return;
    }

    // Update balances
    acc->balance -= amount;
    receiver.balance += amount;

    // Save sender
    save_transaction(acc, "Transfer Sent", amount);
    save_account_to_file(acc);

    // Save receiver
    save_transaction(&receiver, "Transfer Received", amount);
    fseek(file, pos, SEEK_SET);
    fwrite(&receiver, sizeof(receiver), 1, file);

    fclose(file);

    print_receipt("TRANSFER RECEIPT", acc->balance + amount, amount, acc->balance);
}

// -------------------------------------------------------------
// SHOW TRANSACTION HISTORY
// -------------------------------------------------------------
void ShowHistory(user *acc)
{
    clear();
    print_center_box("TRANSACTION HISTORY");

    if (acc->history_count == 0)
    {
        print_center_box("NO TRANSACTIONS YET!");
        return;
    }

    printf("\n");
    for (int i = 0; i < acc->history_count; i++)
    {
        printf("%d. %s - %.2f USD (Balance: %d)\n",
               i + 1,
               acc->history[i].action,
               acc->history[i].amount,
               acc->history[i].balance);
    }

    printf("\nPress ENTER to return...");
    getchar();
    getchar();
}

void export_history(user *acc)
{
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_history.txt", acc->name);

    FILE *f = fopen(filename, "w");

    fprintf(f, "Transaction History for %s (ID: %d)\n\n", acc->name, acc->id);

    for (int i = 0; i < acc->history_count; i++)
    {
        fprintf(f, "%d. %s - %.2f (Balance: %d)\n",
                i + 1,
                acc->history[i].action,
                acc->history[i].amount,
                acc->history[i].balance);
    }

    fclose(f);

    clear();
    print_center_box("HISTORY EXPORTED SUCCESSFULLY!");
}

// -------------------------------------------------------------
// CHANGE PIN
// -------------------------------------------------------------
void change_pin(user *acc)
{
    clear();
    print_center_box("CHANGE PIN");

    int old_pin = get_pin_input("Enter old PIN: ");
    if (old_pin != acc->pin)
    {
        print_center_box("INCORRECT PIN!");
        return;
    }

    int new_pin = get_pin_input("Enter new PIN: ");
    int confirm = get_pin_input("Confirm new PIN: ");

    if (new_pin != confirm)
    {
        print_center_box("PINS DO NOT MATCH!");
        return;
    }

    acc->pin = new_pin;
    save_account_to_file(acc);

    clear();
    print_center_box("PIN CHANGED SUCCESSFULLY!");
}

// -------------------------------------------------------------
// VIEW ALL USERS (ADMIN)
// -------------------------------------------------------------
void view_all_users()
{
    clear();
    print_center_box("ALL USERS");

    FILE *file = fopen(account_file, "rb");
    if (!file)
    {
        print_center_box("ERROR: Cannot open database!");
        wait();

        return;
    }

    user acc;
    int count = 0;

    printf("\n");
    while (fread(&acc, sizeof(acc), 1, file))
    {
        printf("Name: %s, ID: %d, Balance: %d USD\n", acc.name, acc.id, acc.balance);
        count++;
    }

    fclose(file);

    if (count == 0)
    {
        print_center_box("NO USERS FOUND!");
    }

    printf("\nPress ENTER to return...");
    getchar();
    getchar();
}

// -------------------------------------------------------------
// VIEW TOTAL BANK BALANCE (ADMIN)
// -------------------------------------------------------------
void view_total_balance()
{
    clear();
    print_center_box("TOTAL BANK BALANCE");

    FILE *file = fopen(account_file, "rb");
    if (!file)
    {
        print_center_box("ERROR: Cannot open database!");
        wait();

        return;
    }

    user acc;
    int total = 0;

    while (fread(&acc, sizeof(acc), 1, file))
    {
        total += acc.balance;
    }

    fclose(file);

    printf("\n");
    printf("%sTotal Bank Balance:%s\n", USE_COLORS ? BOLDGREEN : "", RESET_COLOR());

    char buf[100];
    snprintf(buf, sizeof(buf), "%s%d USD%s", USE_COLORS ? BOLDBLUE : "", total, RESET_COLOR());
    print_centered(buf);

    printf("\nPress ENTER to return...");
    getchar();
    getchar();
}

// -------------------------------------------------------------
// DELETE USER (ADMIN)
// -------------------------------------------------------------
void delete_user()
{
    clear();
    print_center_box("DELETE USER");

    int delete_id = get_int_input("Enter User ID to delete: ");

    FILE *file = fopen(account_file, "rb");
    if (!file)
    {
        print_center_box("ERROR: Cannot open database!");
        wait();

        return;
    }

    FILE *temp = fopen("temp.dat", "wb");
    if (!temp)
    {
        fclose(file);
        print_center_box("ERROR: Cannot create temp file!");
        wait();

        return;
    }

    user acc;
    int found = 0;

    while (fread(&acc, sizeof(acc), 1, file))
    {
        if (acc.id != delete_id)
        {
            fwrite(&acc, sizeof(acc), 1, temp);
        }
        else
        {
            found = 1;
        }
    }

    fclose(file);
    fclose(temp);

    remove(account_file);
    rename("temp.dat", account_file);

    clear();
    if (found)
    {
        print_center_box("USER DELETED SUCCESSFULLY!");
    }
    else
    {
        print_center_box("USER NOT FOUND!");
    }

    getchar();
    getchar();
}

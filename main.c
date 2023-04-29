#include <stdio.h>
#include <sqlite3.h>

static void run_insert_statement(sqlite3 *connection)
{
    // Create a prepared statement
    sqlite3_stmt *stmt;
    int rc;

    sqlite3_prepare_v2(connection, "INSERT INTO foo (val) VALUES (?)", -1, &stmt, NULL);

    // Bind parameters. Params index begins with 1.
    sqlite3_bind_text(stmt, 1, "Hi domo", -1, SQLITE_STATIC);

    // Execute insert
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        const char *error = sqlite3_errmsg(connection);
        printf("Error (%d) %s\n", rc, error);
    }

    // Free prepared statement
    sqlite3_finalize(stmt);
}

static void run_select_statement(sqlite3 *connection)
{
    // Create a prepared statement
    sqlite3_stmt *stmt;
    int rc;

    sqlite3_prepare_v2(connection, "SELECT val FROM foo", -1, &stmt, NULL);

    // Execute and fetch first row
    rc = sqlite3_step(stmt);

    while (rc == SQLITE_ROW) {
        /*
         * SQLITE_DONE - Statement has finished.
         * SQLITE_ROW  - There are readable rows.
         */

        // Fetch value using sqlite3_column_* functions
        // Column index begins at 0, from left to right.
        //
        // SELECT a, b, c FROM xyz
        // 0 -> a
        // 1 -> b
        // 2 -> c
        const unsigned char *val = sqlite3_column_text(stmt, 0);
        printf("Val: %s\n", val);

        // Fetch next row
        rc = sqlite3_step(stmt);

        if (rc == SQLITE_DONE) {
            // Nothing else to return, statement is done executing.
            break;
        }
    }

    // Free prepared statement
    sqlite3_finalize(stmt);
}

int main(void)
{
    sqlite3 *connection;
    int rc; // Result Code

    // Open SQLite connection
    // Creates a new empty database file if it does not exist.
    rc = sqlite3_open("./test.db", &connection);
    if (rc != SQLITE_OK) {
        const char *error = sqlite3_errmsg(connection);
        printf("Error (%d): %s\n", rc, error);
    }

    // Setup database properties
    sqlite3_exec(connection, "PRAGMA page_size=4096", NULL, NULL, NULL);
    sqlite3_exec(connection, "PRAGMA journal_mode=wal", NULL, NULL, NULL);

    // Create a table
    sqlite3_exec(connection,
        "CREATE TABLE foo ("
        "   id  INTEGER PRIMARY KEY NOT NULL,"
        "   val TEXT NOT NULL"
        ")",
        NULL, NULL, NULL // Ignore these arguments
    );

    // Start a transaction
    sqlite3_exec(connection, "BEGIN", NULL, NULL, NULL);

    run_insert_statement(connection);
    run_select_statement(connection);

    // Commit transaction
    sqlite3_exec(connection, "COMMIT", NULL, NULL, NULL);

    // Close SQLite connection
    sqlite3_close(connection);
    return 0;
}

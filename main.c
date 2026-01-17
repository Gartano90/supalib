/**
 * @file main.c
 * @brief Entry point for the Supabase CLI application.
 * * Demonstrates how to use the Supabase C library to handle user
 * registration, login, logout, and account management simulations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/supabase.h"
#include "include/private_key.h"

/**
 * @brief Clears the input buffer (stdin).
 * * Discards characters until a newline or EOF is found.
 */
void clear_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Prompts the user for email and password.
 * * @param email Buffer to store the email.
 * @param password Buffer to store the password.
 */
void ask_credentials(char* email, char* password) {
    printf("Enter Email: ");
    scanf("%99s", email);
    printf("Enter Password: ");
    scanf("%99s", password);
}

/**
 * @brief Main execution function.
 * * Runs the interactive CLI loop.
 * * @return int Exit status code.
 */
int main() {
    // --- CONFIGURATION ---
    SupabaseClient* client = supabase_init(SUPABASE_URL, SUPABASE_KEY);
    if (!client) return 1;

    int choice;
    int running = 1;
    char email[100];
    char password[100];

    while(running) {
        bool logged = supabase_is_logged_in(client);

        printf("\n=============================\n");
        printf("      SUPABASE CLI C\n");
        printf("=============================\n");
        
        // DYNAMIC MENU DISPLAY
        if (!logged) {
            printf(" 1. Login (Sign in)\n");
            printf(" 2. Register (New user)\n");
        } else {
            printf(" 1. Logout (Sign out)\n");
            printf(" 2. Delete Registration (Delete Account)\n");
            printf("    (Info: current token loaded in memory)\n");
        }
        
        printf(" 0. Close Program\n");
        printf("-----------------------------\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            clear_input();
            continue;
        }
        clear_input(); 

        // LOGIC HANDLING
        if (choice == 0) {
            running = 0;
        } 
        else if (choice == 1) {
            if (!logged) {
                // --> ACTION: LOGIN
                ask_credentials(email, password);
                if (supabase_login(client, email, password) == 0) {
                    printf("\n[OK] Login successful! You are in.\n");
                } else {
                    printf("\n[ERR] Wrong credentials.\n");
                }
            } else {
                // --> ACTION: LOGOUT
                supabase_logout(client);
                printf("\n[OK] Logout successful. See you soon!\n");
            }
        } 
        else if (choice == 2) {
            if (!logged) {
                // --> ACTION: REGISTRATION
                printf("--- NEW USER REGISTRATION ---\n");
                ask_credentials(email, password);
                
                if (supabase_signup(client, email, password) == 0) {
                    printf("\n[OK] User created!\n");
                    printf("NOTE: If 'Email Confirm' is active on Supabase, you must click the link in the email before logging in.\n");
                } else {
                    printf("\n[ERR] Unable to register user.\n");
                }
            } else {
                // --> ACTION: DELETE ACCOUNT
                printf("\n[WARNING] Delete Account Function:\n");
                printf("Supabase does not allow self-deletion via public API for security.\n");
                printf("To do this, an 'Edge Function' or admin query would be needed.\n");
                printf(">> Simulating local deletion (Forced Logout)...\n");
                
                supabase_logout(client);
                printf("[OK] Local data removed. Contact admin to remove data from DB.\n");
            }
        } 
        else {
            printf("Invalid choice.\n");
        }
    }

    supabase_free(client);
    return 0;
}
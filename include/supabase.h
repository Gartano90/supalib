/**
 * @file supabase.h
 * @brief Header file for the Supabase C client library.
 * * Contains function prototypes and type definitions for interacting
 * with Supabase Auth and Database services.
 */

#ifndef SUPABASE_H
#define SUPABASE_H

#include <stdbool.h>
#include <curl/curl.h>
#include "cJSON.h"

/**
 * @struct SupabaseClient
 * @brief Internal structure to hold client configuration and state.
 */
typedef struct SupabaseClient_s {
    char* url;
    char* anon_key;
    char* access_token;
    CURL* curl_handle;
} SupabaseClient;
 

/**
 * @brief Initializes a new Supabase client.
 * * @param url The base URL of the Supabase project.
 * @param anon_key The anonymous API key.
 * @return SupabaseClient* Pointer to the initialized client, or NULL on failure.
 */
SupabaseClient* supabase_init(const char* url, const char* anon_key);

/**
 * @brief Frees all resources associated with the Supabase client.
 * * @param client Pointer to the SupabaseClient to free.
 */
void supabase_free(SupabaseClient* client);

// --- AUTHENTICATION ---

/**
 * @brief Registers a new user with email and password.
 * * @param client The initialized Supabase client.
 * @param email The user's email address.
 * @param password The user's password.
 * @return int 0 on success, -1 on failure.
 */
int supabase_signup(SupabaseClient* client, const char* email, const char* password);

/**
 * @brief Logs in an existing user.
 * * If successful, the access token is stored within the client structure.
 * * @param client The initialized Supabase client.
 * @param email The user's email address.
 * @param password The user's password.
 * @return int 0 on success, -1 on failure.
 */
int supabase_login(SupabaseClient* client, const char* email, const char* password); 

/**
 * @brief Logs out the current user and clears the access token.
 * * @param client The initialized Supabase client.
 */
void supabase_logout(SupabaseClient* client);

/**
 * @brief Checks if a user is currently logged in.
 * * @param client The initialized Supabase client.
 * @return true If a valid access token exists.
 * @return false If no user is logged in.
 */
bool supabase_is_logged_in(SupabaseClient* client);

// --- DATABASE ---

/**
 * @brief Performs a GET request (SELECT) on a specific table.
 * * @note If the user is logged in, the Authorization header is automatically included.
 * * @param client The initialized Supabase client.
 * @param table The name of the table to query.
 * @param query The query parameters (e.g., "id=eq.1").
 * @return char* The JSON response string (must be freed by caller), or NULL on error.
 */
char* supabase_get(SupabaseClient* client, const char* table, const char* query);

#endif
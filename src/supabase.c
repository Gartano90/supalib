/**
 * @file supabase.c
 * @brief Implementation of the Supabase C client library.
 */

#include "supabase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct MemoryStruct
 * @brief Helper structure for handling CURL response data.
 */
struct MemoryStruct {
    char *memory; /**< Pointer to the memory block. */
    size_t size;  /**< Current size of the memory block. */
};

/**
 * @brief CURL callback function to write received data into memory.
 * * @param contents Pointer to the data received.
 * @param size Size of one data element.
 * @param nmemb Number of elements.
 * @param userp Pointer to the user-defined data (MemoryStruct).
 * @return size_t Total bytes processed.
 */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

SupabaseClient* supabase_init(const char* url, const char* anon_key) {
    curl_global_init(CURL_GLOBAL_ALL);
    SupabaseClient* client = malloc(sizeof(SupabaseClient));
    if (!client) return NULL;
    client->url = strdup(url);
    client->anon_key = strdup(anon_key);
    client->access_token = NULL; // Not logged in initially
    client->curl_handle = curl_easy_init();
    return client;
}

bool supabase_is_logged_in(SupabaseClient* client) {
    return (client->access_token != NULL);
}

void supabase_logout(SupabaseClient* client) {
    if (client->access_token) {
        free(client->access_token);
        client->access_token = NULL;
    }
    printf(">> Logout successful.\n");
}


int supabase_signup(SupabaseClient* client, const char* email, const char* password) {
    CURL *curl = client->curl_handle;
    struct MemoryStruct chunk = {malloc(1), 0};

    // 1. URL Signup
    char full_url[1024];
    snprintf(full_url, sizeof(full_url), "%s/auth/v1/signup", client->url);

    // 2. Body JSON
    cJSON *json_body = cJSON_CreateObject();
    cJSON_AddStringToObject(json_body, "email", email);
    cJSON_AddStringToObject(json_body, "password", password);
    char *body_str = cJSON_PrintUnformatted(json_body);

    // 3. Headers (ApiKey + Content-Type)
    struct curl_slist *headers = NULL;
    char apiKeyHeader[512];
    snprintf(apiKeyHeader, sizeof(apiKeyHeader), "apikey: %s", client->anon_key);
    headers = curl_slist_append(headers, apiKeyHeader);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // 4. CURL Config
    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_str);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 

    // 5. Execute
    CURLcode res = curl_easy_perform(curl);
    
    // Cleanup
    cJSON_Delete(json_body);
    free(body_str);
    curl_slist_free_all(headers);

    int result = -1;

    if(res == CURLE_OK) {
        if (strstr(chunk.memory, "\"id\"") != NULL) {
            result = 0; // Success
        } else {
            printf(">> Signup Error: %s\n", chunk.memory);
        }
    } else {
        printf(">> Network error.\n");
    }

    free(chunk.memory);
    curl_easy_reset(curl); 

    return result;
}

int supabase_login(SupabaseClient* client, const char* email, const char* password) {
    CURL *curl = client->curl_handle;
    struct MemoryStruct chunk = {malloc(1), 0};

    // 1. URL Auth
    char full_url[1024];
    snprintf(full_url, sizeof(full_url), "%s/auth/v1/token?grant_type=password", client->url);

    // 2. Create JSON Body
    cJSON *json_body = cJSON_CreateObject();
    cJSON_AddStringToObject(json_body, "email", email);
    cJSON_AddStringToObject(json_body, "password", password);
    char *body_str = cJSON_PrintUnformatted(json_body);

    // 3. Headers
    struct curl_slist *headers = NULL;
    char apiKeyHeader[512];
    snprintf(apiKeyHeader, sizeof(apiKeyHeader), "apikey: %s", client->anon_key);
    headers = curl_slist_append(headers, apiKeyHeader);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // 4. Configure POST
    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_str);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    printf("\n[DEBUG] Calling URL: '%s'\n", full_url);
    
    // 5. Execute
    CURLcode res = curl_easy_perform(curl);
    
    // Partial cleanup
    cJSON_Delete(json_body);
    free(body_str);
    curl_slist_free_all(headers);

    if(res != CURLE_OK) {
        free(chunk.memory);
        return -1;
    }

    // 6. Parse response
    cJSON *response_json = cJSON_Parse(chunk.memory);
    cJSON *token_item = cJSON_GetObjectItemCaseSensitive(response_json, "access_token");

    int result = -1;
    if (cJSON_IsString(token_item) && (token_item->valuestring != NULL)) {
        if (client->access_token) free(client->access_token);
        client->access_token = strdup(token_item->valuestring);
        result = 0; // Success
    } else {
        printf(">> Login Error: %s\n", chunk.memory);
    }

    cJSON_Delete(response_json);
    free(chunk.memory);
    
    curl_easy_reset(curl); 

    return result;
}

void supabase_free(SupabaseClient* client) {
    if (client) {
        supabase_logout(client);
        if (client->curl_handle) curl_easy_cleanup(client->curl_handle);
        if (client->url) free(client->url);
        if (client->anon_key) free(client->anon_key);
        free(client);
    }
    curl_global_cleanup();
}

char* supabase_get(SupabaseClient* client, const char* table, const char* query) {
    // Placeholder: implementation to be completed
    return NULL; 
}
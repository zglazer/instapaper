/*
 * Instapaper - Instapaper API Library designed for command line applications.
                   For non command line applications, overwrite getCredentials() function.
 *
 * Author: Zach Glazer
 * Version: 1.0
 * Date: 12/24/2013
 *
 */

#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char *uname = NULL; /* instapaper username */
char *pass = NULL; /* instapaper password */
CURL *curl_handler; /* URL handler */

/* PROTOTYPES */

void instapaper_init();
void instapaper_clean();
void getCredentials();
int authenticate();
int drop(char *url);
size_t static write_function(void *buffer, size_t size, size_t nmemb, void *userp);


/* IMPLEMENTATION */

/*
 * instapaper_init - function used to initialize instapaper library
 *
 * NOTE: must be called before using any authenticate() or drop() functions
 *
 */
void instapaper_init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handler = curl_easy_init();
    if (!curl_handler) {
        fprintf(stderr, "Error establishing connection.\n");
        instapaper_clean();
    }
}

/*
 * instapaper_clean - function 
 *
 * NOTE: must be called when finished using library functions
 *
 */
void instapaper_clean() {
    curl_easy_cleanup(curl_handler);
    curl_global_cleanup();
}

/*
 * getCredentials - get user credentials from command line
 *
 *
 */
void getCredentials() {
    uname = malloc(1024);
    printf("Username: ");
    fgets(uname, 1024, stdin);
    size_t ln = strlen(uname) - 1;
    if (uname[ln] == '\n') {
        uname[ln] = '\0';
    }

    pass = getpass("Password: "); 
}


/*
 * setCredentials - sets user credentials (username & password) to be used for API calls
 *
 * PARAMETERS:
 *   *name - instapaper username or email address
 *   *password - instapaper password (optional)
 *
 */
void setCredentials(char *name, char *password) {
    uname = name;
    pass = password;
}


/*
 * authenticate - authenticates instapaper credentials
 *
 * NOTE: if username has not been set, getCredentials() is called
 *
 * RETURNS: HTTP Response Code
 *     200 - okay
 *     403 - invalid username or password
 *     500 - instapaper service encountered an error
 */
int authenticate() {
    if (uname == NULL) {
      getCredentials();
    }

    char *url = malloc( ((int) strlen(uname) + (int) strlen(pass) + 64) * sizeof(char) );
    if (url == NULL) {
        fprintf(stderr, "Error allocating space for authentication URL.\n");
        return -1;
    }

    url = strcat(url, "https://www.instapaper.com/api/authenticate?username=");
    url = strcat(url, uname);   
    url = strcat(url, "&password=");
    url = strcat(url, pass);

    curl_easy_reset(curl_handler);    
    curl_easy_setopt(curl_handler, CURLOPT_URL, url);
    curl_easy_setopt(curl_handler, CURLOPT_WRITEFUNCTION, write_function); 

    long httpCode;
    CURLcode res;
    res = curl_easy_perform(curl_handler);

    curl_easy_getinfo(curl_handler, CURLINFO_RESPONSE_CODE, &httpCode);
    
    return (int) httpCode;
}


/*
 * drop - send a URL to instapaper
 *
 * NOTE: if username has not been set, getCredentials() is called
 *
 * PARAMETERS:
 *   *url - url to send to instapaper
 *
 * RETURNS: HTTP Response Code
 *     -1 - parameter missing
 *     201 - url successfully added to instapaper
 *     400 - bad request or exceeded rate limit
 *     403 - invalid username or password
 *     500 - instapaper service encountered an error 
 *
 */
int drop(char *url) {
    if (url == NULL) {
        return -1;
    }

    if (uname == NULL) {
        getCredentials();
    }

    char *dropURL = malloc( ((int) strlen(uname) + (int) strlen(pass) + (int) strlen(url) + 60) * sizeof(char) );
    if (dropURL == NULL) {
        fprintf(stderr, "Error allocating space for add URL.\n");
        return -1;
    }

    dropURL = strcat(dropURL, "https://www.instapaper.com/api/add?username=");
    dropURL = strcat(dropURL, uname);
    dropURL = strcat(dropURL, "&password=");
    dropURL = strcat(dropURL, pass);
    dropURL = strcat(dropURL, "&url=");
    dropURL = strcat(dropURL, url);

    curl_easy_reset(curl_handler);
    curl_easy_setopt(curl_handler, CURLOPT_URL, dropURL);
    curl_easy_setopt(curl_handler, CURLOPT_WRITEFUNCTION, write_function);

    long httpCode;
    CURLcode res;
    res = curl_easy_perform(curl_handler);

    curl_easy_getinfo(curl_handler, CURLINFO_RESPONSE_CODE, &httpCode);

    return (int) httpCode;
}

/*
 *
 *
 */
size_t static write_function(void *buffer, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

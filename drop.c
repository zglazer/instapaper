/*
 * drop - example program to drop URL into Instapaper
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
#include "instapaper.h"

/*
 * drop - command line Instapaper client
 *  
 * usage: instapaper [username] [password] [url]
 *
 */
int main(int argc, char **argv) {
	if (argc != 2) {
        printf("usage: drop [url]");
        return 0;
    }
    
    getCredentials();
    instapaper_init();
    int code = drop(argv[1]);
    switch (code) {
        case  201 :
            printf("Saved to Instapaper!\n");
            break;

        case 403 : 
            printf("Invalid username or password.\n");
            break;

        default :
            printf("Error with Instapaper service.\n");
            break;

    }

    instapaper_clean();
    return 0;
}
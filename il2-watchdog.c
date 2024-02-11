#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/inotify.h>
#include <time.h>

//#include <libnotify/notify.h>

#define EXIT_SUCCESS 0
#define EXIT_ERR_2_FEW_ARGS 1
#define EXIT_ERR_INOTIFY_INIT 2
#define EXIT_ERR_ADD_WATCH 3
#define EXIT_ERR_NULL_BASE_PATH 4
#define EXIT_ERR_READ_INOTIFY 5

int iEventQueue = -1;
int iEventStatus = -1;

int main(int argc, char** argv)
{
    char* basePath = NULL;
    char* token = NULL;
//    char *buffPointer = NULL;
    char* notifMessage = NULL;

    char buffer[4096];
    int readLength;

    const struct inotify_event* watchEvent;
    
    time_t nowTime;

    //nowTime = NULL;
    char* dateAndTime = NULL;

    const uint32_t watchMask = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE | IN_MODIFY | IN_MOVE_SELF;

    if (argc < 2)
    {
        fprintf(stderr, "USAGE: il2w PATH\n");
        exit(EXIT_ERR_2_FEW_ARGS);
    }

    basePath = (char *)malloc(sizeof(char)*strlen(argv[1] + 1));

    token = strtok(basePath, "/");

    while (token != NULL)
    {
        basePath = token;
        token = strtok(NULL, "/");
    }
    
    if (basePath == NULL)
    {
        fprintf(stderr, "Error getting base path");
        exit(EXIT_ERR_NULL_BASE_PATH);
    }

    iEventQueue = inotify_init();
    if (iEventQueue == -1)
    {
        fprintf(stderr, "Error initializing inotify instance\n");
        exit(EXIT_ERR_INOTIFY_INIT);
    }

    iEventStatus = inotify_add_watch(iEventQueue, argv[1], watchMask);

    if (iEventStatus == -1)
    {
        fprintf(stderr, "Error adding file to watch list\n");
        exit(EXIT_ERR_ADD_WATCH);
    }

    while (true)
    {
        printf("*****\n");

        readLength = read(iEventQueue, buffer, sizeof(buffer));

        if (readLength == -1)
        {
            fprintf(stderr, "Error reading from inotify instance!\n");
            exit(EXIT_ERR_READ_INOTIFY);
        }

        for (char *buffPointer = buffer; buffPointer < buffer + readLength; buffPointer += sizeof(struct inotify_event) + watchEvent->len)
        {
            watchEvent = (const struct inotify_event *) buffPointer;

            notifMessage = NULL;
            //nowTime = NULL;
            time(&nowTime);
            dateAndTime = NULL;

            if (watchEvent->mask & IN_CREATE)
            {
                notifMessage = "File Created\n";
                dateAndTime = ctime(&nowTime);
            }

            if (watchEvent->mask & IN_DELETE)
            {
                notifMessage = "File Deleted\n";
                dateAndTime = ctime(&nowTime);
            }

            if (watchEvent->mask & IN_ACCESS)
            {
                notifMessage = "File Accessed\n";
                dateAndTime = ctime(&nowTime);
            }

            if (watchEvent->mask & IN_CLOSE_WRITE)
            {
                notifMessage = "File Written & Closed\n";
                dateAndTime = ctime(&nowTime);
            }

            if (watchEvent->mask & IN_MODIFY)
            {
                notifMessage = "File Modified\n";
                dateAndTime = ctime(&nowTime);
            }

            if (watchEvent->mask & IN_MOVE_SELF)
            {
                notifMessage = "File Moved\n";
                dateAndTime = ctime(&nowTime);
            }

            if (notifMessage == NULL)
            {
                continue;
            }

            if (dateAndTime != NULL)
            {
                printf("%s\t%s\n",dateAndTime, notifMessage);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
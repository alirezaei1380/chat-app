#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include "cJSON.h"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

char* respond1;
char* respond2;
int number[100];
int server_socket, client_socket;
struct sockaddr_in server, client;
char **token;
char **names;
char **current;
int usercounter;

void myconnect()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return;
    }
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else
        printf("Socket successfully created..\n");
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully bound..\n");
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed...\n");
        exit(0);
    }
    else
        printf("Server acccepted the client..\n");
}

void myregister()
{
    char *username1 = malloc(100), *password1 = malloc(100);
    sscanf(&respond1[9], "%s %s", username1, password1);
    username1[strlen(username1) - 1] = '\0';
    char temp[200];
    sprintf(temp, "%s.user.json", username1);
    FILE* user;
    user = fopen(temp, "r");
    if(user == NULL)
    {
        cJSON *username = cJSON_CreateString(username1);
        cJSON *password = cJSON_CreateString(password1);
        cJSON* myuser = cJSON_CreateObject();
        cJSON_AddItemToObject(myuser, "username", username);
        cJSON_AddItemToObject(myuser, "password", password);
        user = fopen(temp, "w");
        char* temp1 = malloc(1000);
        temp1 = cJSON_Print(myuser);
        fprintf(user, "%s", temp1);
        fclose(user);
        cJSON_Delete(myuser);
        cJSON* answer = cJSON_CreateObject();
        cJSON* type = cJSON_CreateString("Successful");
        cJSON* content = cJSON_CreateString("");
        cJSON_AddItemToObject(answer, "type", type);
        cJSON_AddItemToObject(answer, "content", content);
        respond2 = cJSON_Print(answer);
        cJSON_Delete(answer);
        send(client_socket, respond2, strlen(respond2), 0);
        printf("%s\n", respond2);
    }
    else
    {
        fclose(user);
        cJSON* answer = cJSON_CreateObject();
        cJSON* type = cJSON_CreateString("Error");
        cJSON* content = cJSON_CreateString("This user is not available");
        cJSON_AddItemToObject(answer, "type", type);
        cJSON_AddItemToObject(answer, "content", content);
        respond2 = cJSON_Print(answer);
        cJSON_Delete(answer);
        send(client_socket, respond2, strlen(respond2), 0);
        printf("%s\n", respond2);
    }
}

void mycreatechannel()
{
    char *channelname = malloc(100), *mytoken = malloc(50);
    sscanf(&respond1[15], "%s %s", channelname, mytoken);
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
            channelname[strlen(channelname) - 1] = '\0';
            char temp[200];
            sprintf(temp, "%s.channel.json", channelname);
            FILE* channel = fopen(temp, "r");
            if(channel == NULL)
            {
                cJSON *name = cJSON_CreateString(channelname);
                cJSON *message = cJSON_CreateArray();
                cJSON *temp1 = cJSON_CreateObject();
                cJSON_AddItemToObject(temp1, "name", name);
                cJSON_AddItemToObject(temp1, "messages", message);
                channel = fopen(temp, "w");
                char *temp2 = malloc(1000);
                temp2 = cJSON_Print(temp1);
                fprintf(channel, "%s", temp2);
                fclose(channel);
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Successful");
                cJSON* content = cJSON_CreateString("");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                current[i] = malloc(100);
                strcpy(current[i], channelname);
            }
            else
            {
                fclose(channel);
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("Channel name is not available");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
            }
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void myjoinchannel()
{
    char *channelname = malloc(100), *mytoken = malloc(33);
    sscanf(&respond1[13], "%s %s", channelname, mytoken);
    channelname[strlen(channelname) - 1] = '\0';
    char temp[200];
    sprintf(temp, "%s.channel.json", channelname);
    int i;
    for(i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
            FILE *channel = fopen(temp, "r");
            if(channel == NULL)
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("Channel not found");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
            }
            else
            {
                number[i] = 0;
                fclose(channel);
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Successful");
                cJSON* content = cJSON_CreateString("");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                current[i] = malloc(100);
                strcpy(current[i], channelname);
            }
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void mylogin()
{
    char *username1 = malloc(100), *password1 = malloc(100);
    sscanf(&respond1[6], "%s %s", username1, password1);
    username1[strlen(username1) - 1] = '\0';
    char temp[200];
    sprintf(temp, "%s.user.json", username1);
    FILE* user;
    user = fopen(temp, "r");
    if(user == NULL)
    {
        cJSON* answer = cJSON_CreateObject();
        cJSON* type = cJSON_CreateString("Error");
        cJSON* content = cJSON_CreateString("User not found");
        cJSON_AddItemToObject(answer, "type", type);
        cJSON_AddItemToObject(answer, "content", content);
        respond2 = cJSON_Print(answer);
        cJSON_Delete(answer);
        send(client_socket, respond2, strlen(respond2), 0);
        printf("%s\n", respond2);
    }
    else
    {
        fseek(user, 2, SEEK_SET);
        char c = fgetc(user);
        fscanf(user, "%[^\n]s", temp);
        memset(temp, 0, 200);
        char d = fgetc(user);
        fscanf(user, "%[^\n]s", temp);
        char* pass = malloc(100);
        temp[strlen(temp) - 1] = '\0';
        strcpy(pass, &temp[14]);
        for(int i = 0; i < usercounter; i++)
        {
            if((strcmp(token[i], "")) && (!strcmp(names[i], username1)))
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("User is already logged in");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                return;
            }
        }
        if(strcmp(pass, password1))
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong password");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
        }
        else
        {
            token[usercounter] = malloc(33);
            for(int i = 0; i < 32; i++)
                token[usercounter][i] = rand() % 125 + 35;
            token[usercounter][32] = '\0';
            names[usercounter] = malloc(100);
            strcpy(names[usercounter], username1);
            current[usercounter] = malloc(100);
            strcpy(current[usercounter], "");
            fclose(user);
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Authtoken");
            cJSON* content = cJSON_CreateString(token[usercounter]);
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            usercounter++;
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
        }
    }
}

void mylogout()
{
    char *mytoken = malloc(33);
    sscanf(&respond1[7], "%s", mytoken);
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
            strcpy(token[i], "");
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Successful");
            cJSON* content = cJSON_CreateString("");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void mysend()
{
    char *message = malloc(1000), *mytoken = malloc(33);
    sscanf(&respond1[5], "%[^,]s", message);
    sscanf(&respond1[6 + strlen(message)], "%s", mytoken);
    char *temp = malloc(200);
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
            if(!strcmp(current[i], ""))
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("User is not member of a channel");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                break;
            }
            sprintf(temp, "%s.channel.json", current[i]);
            char *all = malloc(1000);
            memset(all, 0, 1000);
            FILE *file = fopen(temp, "r");
            for(int j = 0; !feof(file); j++)
                all[j] = fgetc(file);
            fclose(file);
            cJSON *channel = cJSON_Parse(all);
            cJSON *messages = cJSON_GetObjectItem(channel, "messages");
            cJSON *mymessage = cJSON_CreateObject();
            cJSON *sender = cJSON_CreateString(names[i]);
            cJSON_AddItemToObject(mymessage, "sender", sender);
            cJSON *content = cJSON_CreateString(message);
            cJSON_AddItemToObject(mymessage, "content", content);
            cJSON_AddItemToArray(messages, mymessage);
            cJSON *name = cJSON_GetObjectItem(channel, "name");
            char *myname = cJSON_Print(name);
            myname[strlen(myname) - 1] = 0;
            cJSON *fname = cJSON_CreateString(&myname[1]);
            cJSON *mychannel = cJSON_CreateObject();
            cJSON_AddItemToObject(mychannel, "name", fname);
            cJSON_AddItemToObject(mychannel, "messages", messages);
            char *temp5 = cJSON_Print(mychannel);
            file = fopen(temp, "w");
            fprintf(file, "%s", temp5);
            fclose(file);
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Successful");
            cJSON* mycontent = cJSON_CreateString("");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", mycontent);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void myrefresh()
{
    char *mytoken = malloc(33);
    sscanf(&respond1[8], "%s", mytoken);
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
            if(!strcmp(current[i], ""))
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("User is not member of a channel");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                break;
            }
            char *temp = malloc(200);
            sprintf(temp, "%s.channel.json", current[i]);
            FILE *file = fopen(temp, "r");
            char *myfile = malloc(2000);
            for(int j = 0; !feof(file); j++)
                myfile[j] = fgetc(file);
            fclose(file);
            cJSON *all = cJSON_Parse(myfile);
            cJSON *messages = cJSON_GetObjectItem(all, "messages");
            cJSON *mymessages = cJSON_CreateArray();
            cJSON *mysender = cJSON_GetArrayItem(messages, number[i]);
            cJSON_AddItemToArray(mymessages, mysender);
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("List");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", mymessages);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            number[i] += cJSON_GetArraySize(mymessages);
            return;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void mychannelmembers()
{
    char *mytoken = malloc(33);
    sscanf(&respond1[16], "%s", mytoken);
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(token[i], mytoken))
        {
            if(!strcmp(current[i], ""))
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("User is not member of a channel");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                break;
            }
            cJSON *content = cJSON_CreateArray();
            for(int j = 0; j < usercounter; j++)
            {
                if(strcmp(token[j], ""))
                {
                    if(!strcmp(current[j], current[i]))
                    {
                        cJSON *member = cJSON_CreateString(names[j]);
                        cJSON_AddItemToArray(content, member);
                    }
                }
            }
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("List");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void mysearchuser()
{
    char *mytoken = malloc(33);
    char *myuser = malloc(100);
    sscanf(&respond1[12], "%s %s", myuser, mytoken);
    myuser[strlen(myuser) - 1] = '\0';
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
             if(!strcmp(current[i], ""))
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("User is not member of a channel");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                break;
            }
            for(int j = 0; j < usercounter; j++)
            {
                if((strcmp(token[j], "")) && (j != i) && (!strcmp(current[j], current[i])) && (!strcmp(names[j], myuser)))
                {
                    cJSON *answer = cJSON_CreateObject();
                    cJSON *type = cJSON_CreateString("User");
                    cJSON *content = cJSON_CreateString("was found");
                    cJSON_AddItemToObject(answer, "type", type);
                    cJSON_AddItemToObject(answer, "content", content);
                    respond2 = cJSON_Print(answer);
                    cJSON_Delete(answer);
                    send(client_socket, respond2, strlen(respond2), 0);
                    printf("%s\n", respond2);
                    break;
                }
                if((strcmp(token[j], "")) && (j == i) && (!strcmp(current[j], current[i])) && (!strcmp(names[j], myuser)))
                {
                    cJSON *answer = cJSON_CreateObject();
                    cJSON *type = cJSON_CreateString("User");
                    cJSON *content = cJSON_CreateString("Is you");
                    cJSON_AddItemToObject(answer, "type", type);
                    cJSON_AddItemToObject(answer, "content", content);
                    respond2 = cJSON_Print(answer);
                    cJSON_Delete(answer);
                    send(client_socket, respond2, strlen(respond2), 0);
                    printf("%s\n", respond2);
                    break;
                }
                if(j == usercounter - 1)
                {
                    cJSON *answer = cJSON_CreateObject();
                    cJSON *type = cJSON_CreateString("User");
                    cJSON *content = cJSON_CreateString("Was not found");
                    cJSON_AddItemToObject(answer, "type", type);
                    cJSON_AddItemToObject(answer, "content", content);
                    respond2 = cJSON_Print(answer);
                    cJSON_Delete(answer);
                    send(client_socket, respond2, strlen(respond2), 0);
                    printf("%s\n", respond2);
                    break;
                }
            }
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void myleave()
{
    char *mytoken = malloc(33);
    sscanf(&respond1[6], "%s", mytoken);
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
            if(!strcmp(current[i], ""))
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("User is not member of a channel");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                break;
            }
            strcpy(current[i], "");
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Successful");
            cJSON* content = cJSON_CreateString("");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

void mysearchmessage()
{
    char *mytoken = malloc(33);
    char *mymessage = malloc(100);
    sscanf(&respond1[15], "%s %s", mymessage, mytoken);
    mymessage[strlen(mymessage) - 1] = '\0';
    for(int i = 0; i < usercounter; i++)
    {
        if(!strcmp(mytoken, token[i]))
        {
            if(!strcmp(current[i], ""))
            {
                cJSON* answer = cJSON_CreateObject();
                cJSON* type = cJSON_CreateString("Error");
                cJSON* content = cJSON_CreateString("User is not member of a channel");
                cJSON_AddItemToObject(answer, "type", type);
                cJSON_AddItemToObject(answer, "content", content);
                respond2 = cJSON_Print(answer);
                cJSON_Delete(answer);
                send(client_socket, respond2, strlen(respond2), 0);
                printf("%s\n", respond2);
                break;
            }
            char *temp = malloc(200);
            sprintf(temp, "%s.channel.json", current[i]);
            FILE *file = fopen(temp, "r");
            char *all = malloc(2000);
            for(int j = 0; !feof(file); j++)
                all[j] = fgetc(file);
            fclose(file);
            cJSON *myall = cJSON_Parse(all);
            cJSON *messages = cJSON_GetObjectItem(myall, "messages");
            cJSON *answer = cJSON_CreateObject();
            cJSON *type = cJSON_CreateString("List");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON *content = cJSON_CreateArray();
            cJSON *mysender = cJSON_CreateObject();
            mysender = cJSON_GetArrayItem(messages, 0);
            cJSON_AddItemToArray(content, mysender);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            break;
        }
        if(i == usercounter - 1)
        {
            cJSON* answer = cJSON_CreateObject();
            cJSON* type = cJSON_CreateString("Error");
            cJSON* content = cJSON_CreateString("Wrong Authtoken");
            cJSON_AddItemToObject(answer, "type", type);
            cJSON_AddItemToObject(answer, "content", content);
            respond2 = cJSON_Print(answer);
            cJSON_Delete(answer);
            send(client_socket, respond2, strlen(respond2), 0);
            printf("%s\n", respond2);
            return;
        }
    }
}

int main()
{
    srand(time(NULL));
    respond1 = malloc(1000);
    token = malloc(100 * sizeof(char *));
    current = malloc(100 * sizeof(char *));
    names = malloc(100 * sizeof(char *));
    while(1)
    {
        myconnect();
        memset(respond1, 0, 1000);
        recv(client_socket, respond1, 1000, 0);
        printf("%s\n", respond1);
        if(!strncmp(respond1, "register", 8))
        {
            myregister();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "login", 5))
        {
            mylogin();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "create channel", 14))
        {
            mycreatechannel();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "join channel", 12))
        {
            myjoinchannel();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "logout", 6))
        {
            mylogout();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "send", 4))
        {
            mysend();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "refresh", 7))
        {
            myrefresh();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "channel members", 15))
        {
            mychannelmembers();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "leave", 5))
        {
            myleave();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "search user", 11))
        {
            mysearchuser();
            closesocket(server_socket);
            continue;
        }
        if(!strncmp(respond1, "search message", 14))
        {
            mysearchmessage();
            closesocket(server_socket);
            continue;
        }
    }
}

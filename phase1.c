#include <winsock2.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "cJSON.h"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

char respond1[1000], respond2[1000], authtoken[200];
int choose, menu, stay;
int client_socket;

void thirdmenu()
{
    system("color 34");
    stay = 0;
    while(!stay)
    {
        printf("channel menu:\n1: Send Message\n2: Refresh\n3: Channel Members\n4: Search User\n5: search Message\n6: Leave Channel\n(enter a number)\n");
        char *choice1 = malloc(100);
        scanf("%s", choice1);
        if(!strcmp(choice1, "1"))
        {
            system("cls");
            printf("enter your message\n");
            char c = getchar();
            char message[1000];
            scanf("%[^\n]s", message);
            memset(respond1, 0, 1000);
            sprintf(respond1, "send %s, %s", message, authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *ab = cJSON_Parse(respond1);
            cJSON *we = cJSON_GetObjectItem(ab, "type");
            strcpy(respond1,we->valuestring);
            cJSON *wf = cJSON_GetObjectItem(ab, "content");
            strcpy(respond2,wf->valuestring);
            printf("%s %s\n", respond1, respond2);
            continue;
        }
        if(!strcmp(choice1, "2"))
        {
            system("cls");
            memset(respond1, 0, 1000);
            sprintf(respond1, "refresh %s", authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            char respond3[3000];
            memset(respond3, 0, 3000);
            recv(client_socket, respond3, 3000, 0);
            cJSON *myrefresh = cJSON_Parse(respond3);
            cJSON *mycontent = cJSON_GetObjectItem(myrefresh, "content");
            cJSON *type = cJSON_GetObjectItem(myrefresh, "type");
            char *mytype = malloc(50);
            strcpy(mytype, type->valuestring);
            printf("%s:\n", mytype);
            int i = cJSON_GetArraySize(mycontent);
            for(int j = 0; j < i; j++)
            {
                memset(respond1, 0, 1000);
                memset(respond2, 0, 1000);
                cJSON *content = cJSON_GetArrayItem(mycontent, j);
                cJSON *gg = cJSON_GetObjectItem(content, "sender");
                strcpy(respond1,gg->valuestring);
                cJSON *jj = cJSON_GetObjectItem(content, "content");
                strcpy(respond2,jj->valuestring);
                printf("%s: %s\n", respond1, respond2);
            }
            continue;
        }
        if(!strcmp(choice1, "3"))
        {
            system("cls");
            memset(respond1, 0, 1000);
            sprintf(respond1, "channel members %s", authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *mylist = cJSON_Parse(respond1);
            cJSON *newcontent = cJSON_GetObjectItem(mylist, "content");
            int k  = cJSON_GetArraySize(newcontent);
            printf("List:\n");
            for(int j = 0; j < k; j++)
            {
                memset(respond1, 0, 1000);
                cJSON * iscontent = cJSON_GetArrayItem(newcontent, j);
                char *po = malloc(200);
                po = cJSON_Print(iscontent);
                po++;
                char c;
                while(*po != 34)
                {
                    c = *po;
                    putchar(c);
                    po++;
                }
                printf("\n");
            }
            continue;
        }
        if(!strcmp(choice1, "6"))
        {
            system("cls");
            memset(respond1, 0, 1000);
            sprintf(respond1, "leave %s", authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *tt = cJSON_Parse(respond1);
            cJSON *yy = cJSON_GetObjectItem(tt, "type");
            strcpy(respond1,yy->valuestring);
            cJSON *xx = cJSON_GetObjectItem(tt, "content");
            strcpy(respond2,xx->valuestring);
            printf("%s %s\n", respond1, respond2);
            stay++;
            secondmenu();
            continue;
        }
        if(!strcmp(choice1, "4"))
        {
            system("cls");
            printf("enter user's name\n(with no space)\n");
            char *temp = malloc(100);
            int c = getchar();
            scanf("%[^\n]s", temp);
            if(strchr(temp, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            memset(respond1, 0, 1000);
            sprintf(respond1, "search user %s, %s", temp, authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *qw = cJSON_Parse(respond1);
            cJSON *yz = cJSON_GetObjectItem(qw, "type");
            strcpy(respond1,yz->valuestring);
            cJSON *xz = cJSON_GetObjectItem(qw, "content");
            strcpy(respond2,xz->valuestring);
            printf("%s %s\n", respond1, respond2);
            continue;
        }
        if(!strcmp(choice1, "5"))
        {
            system("cls");
            printf("enter message\n(it's a word so you can't enter space)\n");
            char *mytemp = malloc(100);
            int c = getchar();
            scanf("%[^\n]s", mytemp);
            if(strchr(mytemp, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            memset(respond1, 0, 1000);
            sprintf(respond1, "search message %s, %s", mytemp, authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *qr = cJSON_Parse(respond1);
            cJSON *yr = cJSON_GetObjectItem(qr, "type");
            strcpy(respond1,yr->valuestring);
            cJSON *messages = cJSON_GetObjectItem(qr, "content");
            printf("%s:\n", respond1);
		int counter = 0;
            for(int i = 0; i < cJSON_GetArraySize(messages); i++)
            {
                memset(respond2, 0, 1000);
                cJSON *ty = cJSON_GetArrayItem(messages, i);
                cJSON *yu = cJSON_GetObjectItem(ty, "content");
                strcpy(respond2, yu->valuestring);
                if(strstr(respond2, mytemp))
		{
                    printf("%s\n", respond2);
			counter++;
		}
            }
		if(!counter)
			printf("Not found\n");
            continue;
        }
        printf("incorrect input\n");
    }
}

void secondmenu()
{
    system("color 56");
    menu = 0;
    while(!menu)
    {
        printf("Login menu:\n1: create channel\n2: join channel\n3:logout\n(enter a number)\n");
        char *choice1 = malloc(100);
        scanf("%s", choice1);
        if(!strcmp(choice1, "1"))
        {
            system("cls");
            printf("enter the name of your new channel(with no space)\n");
            int c = getchar();
            char channel_name[200];
            scanf("%[^\n]s", channel_name);
            if(strchr(channel_name, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            memset(respond1, 0, 1000);
            sprintf(respond1, "create channel %s, %s", channel_name, authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *ab = cJSON_Parse(respond1);
            cJSON *we = cJSON_GetObjectItem(ab, "type");
            strcpy(respond1,we->valuestring);
            cJSON *wf = cJSON_GetObjectItem(ab, "content");
            strcpy(respond2,wf->valuestring);
            printf("%s %s\n", respond1, respond2);
            if(strcmp(respond1, "Error"))
            {
                menu++;
                thirdmenu();
            }
            continue;
        }
        if(!strcmp(choice1, "2"))
        {
            system("cls");
            printf("enter the name of your channel(with no space)\n");
            char d = getchar();
            char channel_name2[200];
            scanf("%[^\n]s", channel_name2);
            if(strchr(channel_name2, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            memset(respond1, 0, 1000);
            sprintf(respond1, "join channel %s, %s", channel_name2, authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            closesocket(client_socket);
            cJSON *de = cJSON_Parse(respond1);
            cJSON *ef = cJSON_GetObjectItem(de, "type");
            strcpy(respond1,ef->valuestring);
            cJSON *wt = cJSON_GetObjectItem(de, "content");
            strcpy(respond2,wt->valuestring);
            printf("%s %s\n", respond1, respond2);
            if(strcmp(respond1, "Error"))
            {
                menu++;
                thirdmenu();
            }
            continue;
        }
        if(!strcmp(choice1, "3"))
        {
            system("cls");
            memset(respond1, 0, 1000);
            sprintf(respond1, "logout %s", authtoken);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *aa = cJSON_Parse(respond1);
            cJSON *bb = cJSON_GetObjectItem(aa, "type");
            strcpy(respond1,bb->valuestring);
            cJSON *cc = cJSON_GetObjectItem(aa, "content");
            strcpy(respond2,cc->valuestring);
            printf("%s %s\n", respond1, respond2);
            menu++;
            firstmenu();
            continue;
        }
        printf("incorrect input\n");
    }
}

void firstmenu(void)
{
    system("color 21");
    choose = 0;
    while(!choose)
    {
        printf("Account menu:\n1: register\n2: login\n3: exit\n(enter a number)\n");
        char *choice1 = malloc(100);
        scanf("%s", choice1);
        if(!strcmp(choice1, "1"))
        {
            system("cls");
            memset(respond1, 0, 1000);
            char * username1 = malloc(200);
            char * password1 = malloc(200);
            printf("enter your username(with no space)\n");
            int c = getchar();
            scanf("%[^\n]s", username1);
            if(strchr(username1, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            printf("enter your password(with no space)\n");
            int d = getchar();
            scanf("%[^\n]s", password1);
            if(strchr(password1, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            sprintf(respond1, "register %s, %s", username1, password1);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            cJSON *ab = cJSON_Parse(respond1);
            cJSON *we = cJSON_GetObjectItem(ab, "type");
            strcpy(respond1,we->valuestring);
            cJSON *wf = cJSON_GetObjectItem(ab, "content");
            strcpy(respond2,wf->valuestring);
            printf("%s %s\n", respond1, respond2);
            continue;
        }
        if(!strcmp(choice1, "2"))
        {
            system("cls");
            memset(respond1, 0, 1000);
            char * username2 = malloc(200);
            char * password2 = malloc(200);
            printf("enter your username(with no space)\n");
            int c = getchar();
            scanf("%[^\n]s", username2);
            if(strchr(username2, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            printf("enter your password(with no space)\n");
            int d = getchar();
            scanf("%[^\n]s", password2);
            if(strchr(password2, ' '))
            {
                printf("wrong input\n");
                continue;
            }
            sprintf(respond1, "login %s, %s", username2, password2);
            myconnect();
            send(client_socket, respond1, strlen(respond1), 0);
            memset(respond1, 0, 1000);
            recv(client_socket, respond1, 1000, 0);
            closesocket(client_socket);
            cJSON *ab = cJSON_Parse(respond1);
            cJSON *we = cJSON_GetObjectItem(ab, "type");
            strcpy(respond1,we->valuestring);
            if(!strcmp(respond1, "Error"))
            {
                cJSON *wf = cJSON_GetObjectItem(ab, "content");
                strcpy(respond2,wf->valuestring);
                printf("%s %s\n", respond1, respond2);
            }
            else
            {
                choose++;
                printf("successful\n");
                cJSON *wf = cJSON_GetObjectItem(ab, "content");
                strcpy(authtoken,wf->valuestring);
                secondmenu();
            }
            continue;
        }
        if(!strcmp(choice1, "3"))
        {
            exit(0);
            continue;
        }
        printf("incorrect input\n");
    }
}

void myconnect(void)
{
	struct sockaddr_in servaddr, cli;
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return;
    }
    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		exit(0);
	}
}

int main()
{
    firstmenu();
}

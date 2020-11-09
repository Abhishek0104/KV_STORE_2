#include "../KV_SERVER/KVHeader.h"


#define GET 1
#define PUT 2
#define DEL 3
#define CLOSE 4

int sock_fd;
// char message[513];

using namespace std;

int choice;

char * join(char * x, char * y, char * z){
	char * sum = (char *)malloc(513*sizeof(char));
	strcpy(sum, x);
	strcat(sum, y);
	strcat(sum, z);
	return sum;
}

void add_padding(char * str)
{
	int len = 0;
	printf("%s\n", str);
	while (str[len]!='\n'&&str[len]!='\0')
	{
		len++;
	}
	
	while(len < 256)
	{
		str[len++] = '-';
	}
	str[len] = '\0';
}

void get()
{
	char key[257];
	char value[257];
	char * message = (char *)malloc(513*sizeof(char));
	printf("Enter the Key:");
	scanf("%s", key);
	add_padding(key);
	// printf("%s\n", key);
	message = join("1", key, "");
	add_padding(message + 257);
	// cout<<message<<"\n";
	// send the data 
	write(sock_fd, message, strlen(message));

	// wait for status code

	// wait for data

}

void put()
{
	char key[257];
	char value[257];
	char * message = (char *)malloc(513*sizeof(char));

	printf("Enter the Key:");
	scanf("%s", key);
	printf("Enter the Value:\n");
	scanf("%s", value);
	add_padding(key);
	add_padding(value);
	// printf("%s\n", key);
	message = join("2", key, value);
	cout<<message<<"\n";
	// send the data 
	write(sock_fd, message, strlen(message));

	// wait for status code

	// wait for data
	
}

void del()
{
	char key[257];
	char * message = (char *)malloc(513*sizeof(char));
	
	printf("Enter the Key:");
	scanf("%s", key);
	add_padding(key);
	message = join("3", key, "");
	add_padding(message + 257);
	cout<<message<<"\n";
	// printf("%s\n", key);
	// join("3", key, NULL);
	// send the data 
	write(sock_fd, message, strlen(message));

	// wait for status code

	// wait for data

}


void sigterm_handler(int sig)
{
    printf("Got SIGTERM-Leaving\n");
    close(sock_fd);
    exit(0);
}

int main(int argc, char** argv)
{
    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler);
    
	int s;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    int temp = 1;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(int)) < 0)
    {
        perror("Setsockopt failed.\n");
    }
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr));

	printf("Client Connected\n");

	// char buffer[10];
	// while(fgets(buffer, 10, stdin) != NULL){
	// 	printf("SENDING: %s", buffer);
	// 	printf("===\n");
	// 	write(sock_fd, buffer, strlen(buffer));

	// 	memset(buffer, 0, sizeof(buffer));
	// 	// char resp[1000];
	// 	// int len = read(sock_fd, resp, 999);
	// 	// resp[len] = '\0';
	// 	// printf("%s\n", resp);
	// }
	while(1)
	{
		printf("1.\tGET\n2.\tPUT\n3.\tDEL\n4.\tEXIT\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice)
		{
		case GET:
			get();
			break;
		case PUT:
			put();
			break;
		
		case DEL:
			del();
			break;
		
		case CLOSE:
			// close(sock_fd);
			return 0;

		default:
			printf("Enter a valid choice.\n");
			break;
		}
	}

    close(sock_fd);
    return 0;
}
#ifdef WIN32
#include <windows.h>
#include <stdio.h>

#else

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h> /* for hostent */

#endif

struct data6
{
	unsigned int d4:6;
	unsigned int d3:6;
	unsigned int d2:6;
	unsigned int d1:6;
};

// Э���м��ܲ���ʹ�õ���base64����
char con628(char c6);
void base64(char *dbuf, char *buf128, int len);
void sendemail(char *email, char *body);
int open_socket(struct sockaddr *addr);

int main()
{
	char email[] = "vmario@126.com"; /*�ռ�������*/
	char body[] = "From: \"vhubin\"<vhubin@126.com>\r\n"
	"To: \"vmario\"<vmario@126.com>\r\n"
	"Subject: Hello\r\n\r\n"
	"Hello World, Hello Email!";
	sendemail(email, body);
	return 0;
}

char con628(char c6)
{
	char rtn = '\0';
	if (c6 < 26) rtn = c6 + 65;
	else if (c6 < 52) rtn = c6 + 71;
	else if (c6 < 62) rtn = c6 - 4;
	else if (c6 == 62) rtn = 43;
	else rtn = 47;
	return rtn;
}

// base64��ʵ��
void base64(char *dbuf, char *buf128, int len)
{
	struct data6 *ddd = NULL;
	int i = 0;
	char buf[256] = {0};
	char *tmp = NULL;
	char cc = '\0';
	memset(buf, 0, 256);
	strcpy(buf, buf128);
	for(i = 1; i <= len/3; i++)
	{
	tmp = buf+(i-1)*3;
	cc = tmp[2];
	tmp[2] = tmp[0];
	tmp[0] = cc;
	ddd = (struct data6 *)tmp;
	dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);
	dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);
	dbuf[(i-1)*4+2] = con628((unsigned int)ddd->d3);
	dbuf[(i-1)*4+3] = con628((unsigned int)ddd->d4);
	}
	if(len%3 == 1)
	{
	tmp = buf+(i-1)*3;
	cc = tmp[2];
	tmp[2] = tmp[0];
	tmp[0] = cc;
	ddd = (struct data6 *)tmp;
	dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);
	dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);
	dbuf[(i-1)*4+2] = '=';
	dbuf[(i-1)*4+3] = '=';
	}
	if(len%3 == 2)
	{
	tmp = buf+(i-1)*3;
	cc = tmp[2];
	tmp[2] = tmp[0];
	tmp[0] = cc;
	ddd = (struct data6 *)tmp;
	dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);
	dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);
	dbuf[(i-1)*4+2] = con628((unsigned int)ddd->d3);
	dbuf[(i-1)*4+3] = '=';
	}
	return;
}

// �����ʼ�
/***************************************************/
/* email: �ռ����ַ                                               */
/* body: �ʼ�����                                                     */
/***************************************************/


char serverHostName[] = "smtp.126.com";

/***************************************************/
/* email: �ռ�������                                               */
/* body: �ռ�������                                               */
/***************************************************/
void sendemail(char *email, char *body)
{
	int sockfd = 0;
	struct sockaddr_in their_addr = {0};
//	hostent *host;
	char buf[1500] = {0}; /* send buffer */
	char rbuf[1500] = {0}; /* recv buffer */
	char login[128] = {0};
	char pass[128] = {0};
	
	#ifdef WIN32
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	#endif

#if 0
    host = gethostbyname(serverHostName);
	if (NULL == host)
	{
		return -1
	}
#endif

	memset(&their_addr, 0, sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);
#if 0
	their_addr.sin_addr.s_addr = *(struct in_addr*)host->h_addr;//126 smtp ������
#else
	their_addr.sin_addr.s_addr = inet_addr("123.58.178.202"); //126 smtp ������
#endif

	// �����ʼ���������������Ӻ�û����Ӧ����2 �����������
	sockfd = open_socket((struct sockaddr *)&their_addr);
	memset(rbuf, 0, 1500);
	while (recv(sockfd, rbuf, 1500, 0) == 0)
	{
		printf("reconnect...\n");
		Sleep(2);
		//close(sockfd);
		sockfd = open_socket((struct sockaddr *)&their_addr);

		memset(rbuf, 0, 1500);
	}

	printf("%s\n", rbuf);

	// EHLO
	memset(buf, 0, 1500);
	sprintf(buf, "EHLO abcdefg-PC\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// AUTH LOGIN
	memset(buf, 0, 1500);
	sprintf(buf, "AUTH LOGIN\r\n");
	send(sockfd, buf, strlen(buf), 0);
	printf("%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// USER
	memset(buf, 0, 1500);
	sprintf(buf,"vhubin");//���126 �����˺�
	memset(login, 0, 128);
	base64(login, buf, strlen(buf));
	sprintf(buf, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	printf("%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// PASSWORD
	sprintf(buf, "digital2050");//���126 ��������
	memset(pass, 0, 128);
	base64(pass, buf, strlen(buf));
	sprintf(buf, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);
	printf("%s\n", buf);

	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// MAIL FROM
	memset(buf, 0, 1500);
	sprintf(buf, "MAIL FROM: <vhubin@126.com>\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// RCPT TO ��һ���ռ���
	sprintf(buf, "RCPT TO:<%s>\r\n", email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// DATA ׼����ʼ�����ʼ�����
	sprintf(buf, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// �����ʼ����ݣ�\r\n.\r\n���ݽ������
	sprintf(buf, "%s\r\n.\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// QUIT
	sprintf(buf, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

// VC2005 ��Ҫʹ��
	closesocket(sockfd);
//	close(sockfd);
	#ifdef WIN32
//	WSACleanup();
	#endif
	return;

}
	// ��TCP Socket����
int open_socket(struct sockaddr *addr)
{
		int sockfd = 0;
		sockfd = socket(PF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			fprintf(stderr, "Open sockfd(TCP) error!\n");
			exit(-1);
		}
		if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
		{
			fprintf(stderr, "Connect sockfd(TCP) error!\n");
			closesocket(sockfd);
			exit(-1);
		}
		return sockfd;
}


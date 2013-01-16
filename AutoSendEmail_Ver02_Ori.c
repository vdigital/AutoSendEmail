#include <windows.h>
#include <stdio.h>
#include <WinSock.h>
#include <iostream>
using namespace std;
#pragma  comment(lib, "ws2_32.lib")

struct Base64Date6
{
  unsigned int d4:6;
  unsigned int d3:6;
  unsigned int d2:6;
  unsigned int d1:6;
};
// Э���м��ܲ���ʹ�õ���base64����
char ConvertToBase64  (char   c6);
void EncodeBase64     (char   *dbuf, char *buf128, int len);
void SendMail         (char   *email,char *body);
int  OpenSocket       (struct sockaddr *addr);

int main()
{
  char EmailTo[]         = "hanyinlong@126.com";
  char EmailContents[]   = "From: \"lucy\"<hanyinlong@126.com>\r\n"
                           "To: \"dasiy\"<hanyinlong@126.com>\r\n"
                           "Subject: Hello\r\n\r\n"
                           "Hello World, Hello Email!";
  SendMail(EmailTo, EmailContents);
  return 0;
}

char ConvertToBase64(char uc)
{
  if(uc < 26)
  {
    return 'A' + uc;
  }
  if(uc < 52)
  {
    return 'a' + (uc - 26);
  }
  if(uc < 62)
  {
    return '0' + (uc - 52);
  }
  if(uc == 62)
  {
    return '+';
  }
  return '/';
}

// base64��ʵ��
void EncodeBase64(char *dbuf, char *buf128, int len)
{
  struct Base64Date6 *ddd      = NULL;
  int           i        = 0;
  char          buf[256] = {0};
  char          *tmp     = NULL;
  char          cc       = '\0';

  memset(buf, 0, 256);
  strcpy_s(buf, 256, buf128);
  for(i = 1; i <= len/3; i++)
  {
    tmp             = buf + (i-1)*3;
    cc              = tmp[2];
    tmp[2]          = tmp[0];
    tmp[0]          = cc;
    ddd             = (struct Base64Date6 *)tmp;
    dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
    dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
    dbuf[(i-1)*4+2] = ConvertToBase64((unsigned int)ddd->d3);
    dbuf[(i-1)*4+3] = ConvertToBase64((unsigned int)ddd->d4);
  }
  if(len % 3 == 1)
  {
    tmp             = buf + (i-1)*3;
    cc              = tmp[2];
    tmp[2]          = tmp[0];
    tmp[0]          = cc;
    ddd             = (struct Base64Date6 *)tmp;
    dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
    dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
    dbuf[(i-1)*4+2] = '=';
    dbuf[(i-1)*4+3] = '=';
  }
  if(len%3 == 2)
  {
    tmp             = buf+(i-1)*3;
    cc              = tmp[2];
    tmp[2]          = tmp[0];
    tmp[0]          = cc;
    ddd             = (struct Base64Date6 *)tmp;
    dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
    dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
    dbuf[(i-1)*4+2] = ConvertToBase64((unsigned int)ddd->d3);
    dbuf[(i-1)*4+3] = '=';
  }
  return;
}
// �����ʼ�
void SendMail(char *email, char *body)
{
  int     sockfd     = {0};
  char    buf[1500]  = {0};
  char    rbuf[1500] = {0};
  char    login[128] = {0};
  char    pass[128]  = {0};
  WSADATA WSAData;
  struct sockaddr_in their_addr = {0};
  WSAStartup(MAKEWORD(2, 2), &WSAData);
  memset(&their_addr, 0, sizeof(their_addr));
  
  their_addr.sin_family = AF_INET;
  their_addr.sin_port   = htons(25);
  hostent* hptr         = gethostbyname("smtp.126.com"); 
  memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length); 
  printf("IP of smpt.126.com is : %d:%d:%d:%d\n", 
          their_addr.sin_addr.S_un.S_un_b.s_b1, 
          their_addr.sin_addr.S_un.S_un_b.s_b2, 
          their_addr.sin_addr.S_un.S_un_b.s_b3, 
          their_addr.sin_addr.S_un.S_un_b.s_b4); 

  // �����ʼ���������������Ӻ�û����Ӧ����2 �����������
  sockfd = OpenSocket((struct sockaddr *)&their_addr);
  memset(rbuf, 0, 1500);
  while(recv(sockfd, rbuf, 1500, 0) == 0)
  {
    cout<<"reconnect..."<<endl;
    Sleep(2);
    sockfd = OpenSocket((struct sockaddr *)&their_addr);
    memset(rbuf, 0, 1500);
  }

  cout<<rbuf<<endl;

  // EHLO
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"EHLO REceive: "<<rbuf<<endl;

  // AUTH LOGIN
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Auth Login Receive: "<<rbuf<<endl;

  // USER
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "hanyinlong@126.com");//��������˺�
  memset(login, 0, 128);
  EncodeBase64(login, buf, strlen(buf));
  sprintf_s(buf, 1500, "%s\r\n", login);
  send(sockfd, buf, strlen(buf), 0);
  cout<<"Base64 UserName: "<<buf<<endl;
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"User Login Receive: "<<rbuf<<endl;

  // PASSWORD
  sprintf_s(buf, 1500, "hanyinlong881020");//�����������
  memset(pass, 0, 128);
  EncodeBase64(pass, buf, strlen(buf));
  sprintf_s(buf, 1500, "%s\r\n", pass);
  send(sockfd, buf, strlen(buf), 0);
  cout<<"Base64 Password: "<<buf<<endl;

  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Send Password Receive: "<<rbuf<<endl;

  // MAIL FROM
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "MAIL FROM: <hanyinlong@126.com>\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"set Mail From Receive: "<<rbuf<<endl;

  // RCPT TO ��һ���ռ���
  sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Tell Sendto Receive: "<<rbuf<<endl;

  // DATA ׼����ʼ�����ʼ�����
  sprintf_s(buf, 1500, "DATA\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Send Mail Prepare Receive: "<<rbuf<<endl;

  // �����ʼ����ݣ�\r\n.\r\n���ݽ������
  sprintf_s(buf, 1500, "%s\r\n.\r\n", body);
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Send Mail Receive: "<<rbuf<<endl;

  // QUIT
  sprintf_s(buf, 1500, "QUIT\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Quit Receive: "<<rbuf<<endl;

  //������
  closesocket(sockfd);
  WSACleanup();
  return;
}
// ��TCP Socket����
int OpenSocket(struct sockaddr *addr)
{
  int sockfd = 0;
  sockfd=socket(PF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    cout<<"Open sockfd(TCP) error!"<<endl;
    exit(-1);
  }
  if(connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
  {
    cout<<"Connect sockfd(TCP) error!"<<endl;
    exit(-1);
  }
  return sockfd;
} #ifdef WIN32
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
void base64(char *dbuf,char *buf128,int len);
void sendemail(char *email,char *body);
int open_socket(struct sockaddr *addr);

int main()
{
	char email[] = "987654321@qq.com";
	char body[] = "From: \"lucy\"<123456789@qq.com>\r\n"
	"To: \"dasiy\"<987654321@qq.com>\r\n"
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
void sendemail(char *email, char *body)
{
	int sockfd = 0;
	struct sockaddr_in their_addr = {0};
	char buf[1500] = {0};
	char rbuf[1500] = {0};
	char login[128] = {0};
	char pass[128] = {0};
	#ifdef WIN32
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	#endif
	memset(&their_addr, 0, sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);
	their_addr.sin_addr.s_addr = inet_addr("112.90.141.71");//qq smtp ������
	// �����ʼ���������������Ӻ�û����Ӧ����2 �����������
	sockfd = open_socket((struct sockaddr *)&their_addr);
	memset(rbuf,0,1500);
	while(recv(sockfd, rbuf, 1500, 0) == 0)
	{
		printf("reconnect...\n");
		Sleep(2);
		//close(sockfd);
		sockfd = open_socket((struct sockaddr *)&their_addr);

		memset(rbuf,0,1500);
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
	sprintf(buf,"12345678");//���qq��
	memset(login, 0, 128);
	base64(login, buf, strlen(buf));
	sprintf(buf, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	printf("%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	printf("%s\n", rbuf);

	// PASSWORD
	sprintf(buf, "qqpassword");//���qq����
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
	sprintf(buf, "MAIL FROM: <123456789@qq.com>\r\n");
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
	WSACleanup();
	#endif
	return;

}
	// ��TCP Socket����
int open_socket(struct sockaddr *addr)
{
		int sockfd = 0;
		sockfd=socket(PF_INET, SOCK_STREAM, 0);
		if(sockfd < 0)
		{
			fprintf(stderr, "Open sockfd(TCP) error!\n");
			exit(-1);
		}
		if(connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
		{
			fprintf(stderr, "Connect sockfd(TCP) error!\n");
			exit(-1);
		}
		return sockfd;
}


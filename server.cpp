#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <time.h>
#define HTTP_PORT 80
#define HTTP_BUF_SIZE 1024
#define NAME_SIZE 1024
#define PATH_SIZE 1024


struct doc_type
{
	char *suffix;// suffix
	char *type;  // type
};

struct doc_type file_type[] =
{
	{ "html","text/html" },
	{ "htm","text/html" },
	{ "txt","text/plain" },
	{ "jpg","image/jpeg" },
	{ "jpeg","image/jpeg" },
	{ "gif","image/gif" },
	{ "bmp","application/x-bmp" },
	{ "ico","application/x-ico" },
	{ "swf","application/x-shockwave-flash" },
	{ "pdf","application/pdf" },
	{ "mp4","video/mp4" },
	{ "mp3","audio/mp3" },
	{ "png","image/x-png" },
	{ NULL,NULL },
};

//response head
char *http_head_temp = "HTTP/1.1 200 OK\r\nServer:Kelvin & Harry <Version 1.0>\r\n"
"Content-Length:%d\r\nConnection:close\r\n"
"Content-Type:%s\r\n\r\n";


//**************************************gr*****************Error Code*********************8
const char* forbidden = "403 Forbidden. You are forbidden to visit. Please contact admin: Kelvin & Harry\r\n"
"Server: Kelvin&Harry simple httpd 1.0\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<html>"
"<head><title>error.com</title></head>\r\n"
"<body><p>600:You are forbidden to visit. Please contact admin</p></body>"
"</html>";

const char* indexMessage = "500 Internal Server Error. Cannot find this index. Please contact admin: Kelvin & Harry\r\n"
"Server: Kelvin&Harry simple httpd 1.0\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<html>"
"<head><title>error.com</title></head>\r\n"
"<body><p>600:You are forbidden to visit. Please contact admin</p></body>"
"</html>";

const char* notFound = "404 Not Found. Please contact admin: Kelvin & Harry\r\n"
"Server: Kelvin&Harry simple httpd 1.0\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<html>"
"<head><title>error.com</title></head>\r\n"
"<body><p>600:You are forbidden to visit. Please contact admin</p></body>"
"</html>";


//**************************************gr*****************


//user suffix to find content type
char *http_get_type_by_suffix(char *suffix)
{
	struct doc_type *ty;
	for (ty = file_type; (*ty).suffix; ty++)
		if (strcmp((*ty).suffix, suffix) == 0)
			return (*ty).type;
	return NULL;
}

//user url to find name and suffix
int http_analyse_url(char *buf, int buflen, char *file_path, char *file_name, char *suffix)
{
	int length = 0, i = 0, j, k = 0;
	char *begin, *end, *bias, *p, *start;
	for (j = 0; j < HTTP_BUF_SIZE; j++)
		if (buf[j] == ' ') k++;
	if (k < 2) return -1;
	begin = strchr(buf, ' ');
	begin++;
	for (p = begin; *p == '/'; p++) begin = p;
	start = begin;     //start postion does not have“/”
	end = strchr(begin, ' ');

	for (p = begin; p != end; p++)
		if (*p == '/') begin = p;
	begin++;   //make pointer locate before the last '/'
	for (p = begin; p != end; p++)
		if (*p == '.') i = 1;  // judge file or url by look at last is '.' or not

	if (i == 0)
	{
		length = end - start;   //get file path length
		memcpy(file_path, start, length);
	}
	else
	{
		length = begin - start;
		memcpy(file_path, start, length);
	}
	for (p = file_path; *p != NULL; p++)
		if (*p == '/') *p = '\\';    // change ,/, in the path to ,\,
	p--;
	if (*p != '\\')
	{
		// add '\' at last of file paths
		p++;
		*p = '\\';
	}
	if (i == 1)
	{
		length = end - begin;     //acquire file name length
		memset(file_name, NULL, NAME_SIZE);
		memcpy(file_name, begin, length);

		for (p = begin; p != end; p++)
			if (*p == '.') begin = p;    //locate last '.'
		begin++;
		length = end - begin; //acquire augmented name length
		memset(suffix, NULL, NAME_SIZE);
		memcpy(suffix, begin, length);
		return 1;   //locate access file
	}
	return 0;    //does not locate access file
}

void error(SOCKET soc)
{

	FILE *e = NULL;
	e = fopen("log_client.txt", "a");
	time_t t1;
	int file_len = 46, hdr_len;
	char http_header[HTTP_BUF_SIZE], *Content_type = "text/html";
	char error[HTTP_BUF_SIZE] = "<P><FONT color=#ff0000 size=7>500 ERROR</FONT></P>";   //46
	memset(http_header, NULL, HTTP_BUF_SIZE);
	hdr_len = sprintf(http_header, http_head_temp, file_len, Content_type); //Clear
	send(soc, http_header, hdr_len, 0);
	send(soc, error, file_len, 0);
	time(&t1);
	printf("%s500 Internal Server Error\n========================================================================\n", ctime(&t1));
	fprintf(e, "%s500 Internal Server Error\n========================================================================\n", ctime(&t1));
	fclose(e);
}
//**************************************gr*****************
void forbidden1(SOCKET soc)
{
	FILE *f = NULL;
	f = fopen("log_client.txt", "a");
	time_t t2;
	int file_len = 81, hdr_len;
	char http_header[HTTP_BUF_SIZE], *Content_type = "text/html";
	char error[HTTP_BUF_SIZE] = "<P><FONT color=#ff0000 size=7>ERROR</FONT></P>";   //46
	memset(http_header, NULL, HTTP_BUF_SIZE);
	hdr_len = sprintf(http_header, http_head_temp, file_len, Content_type); //Clear
	send(soc, http_header, hdr_len, 0);
	send(soc, forbidden, strlen(forbidden) + 1, 0);
	time(&t2);
	printf("%s403 Forbidden access\n========================================================================\n", ctime(&t2));
	fprintf(f, "%s403 Forbidden access\n========================================================================\n", ctime(&t2));
	fclose(f);
}


void notFound1(SOCKET soc)
{
	FILE *nf = NULL;
	time_t t3;
	nf = fopen("log_client.txt", "a");
	int file_len = 74, hdr_len;
	char http_header[HTTP_BUF_SIZE], *Content_type = "text/html";
	char error[HTTP_BUF_SIZE] = "<P><FONT color=#ff0000 size=7>ERROR</FONT></P>";
	memset(http_header, NULL, HTTP_BUF_SIZE);
	hdr_len = sprintf(http_header, http_head_temp, file_len, Content_type); //格式化首部内容
	send(soc, http_header, hdr_len, 0);
	send(soc, notFound, strlen(notFound) + 1, 0);
	time(&t3);
	printf("%s404 Not Found\n========================================================================\n", ctime(&t3));
	fprintf(nf, "%s404 Not Found\n========================================================================\n", ctime(&t3));
	fclose(nf);

}
//**************************************gr*****************
int http_send_response(SOCKET soc, char *buf, int buf_len)
{
	FILE *file = NULL;
	FILE *fp1 = NULL;
	FILE *fp2 = NULL;
	fp1 = fopen("log_client.txt", "a");
	fp2 = fopen("log_server.txt", "a");
	time_t t;	//get current time
	if (fp1 == NULL) //if open fail
	{
		time(&t);
		printf("%sFailed to open the file !\n", ctime(&t));
		exit(0); //close program
	}
	if (fp2 == NULL) //if open fail
	{
		time(&t);
		printf("%sFailed to open the file !\n", ctime(&t));
		exit(0); //close program
	}
	char http_header[HTTP_BUF_SIZE], file_name[NAME_SIZE] = "index.html", file_name2[NAME_SIZE] = "index.htm", file_path[PATH_SIZE], file_path_temp[PATH_SIZE], suffix[NAME_SIZE] = "html";
	char HTTP_Send_Buff[HTTP_BUF_SIZE], *Content_type = NULL, read_buf[HTTP_BUF_SIZE], *p, *q, target_file[NAME_SIZE];
	int file_len = 0, hdr_len = 0, send_len = 0, read_len = 0;
	int results, i = 0;
	memset(file_path, NULL, PATH_SIZE);
	if ((results = http_analyse_url(buf, HTTP_BUF_SIZE, file_path, file_name, suffix)) == -1)
	{
		notFound1(soc);
		return -1;
	}
	memset(target_file, NULL, NAME_SIZE);
	for (p = file_path; *p != NULL; p++)
	{
		if (*p == '\\')
		{
			p++;
			memset(file_path_temp, NULL, PATH_SIZE);
			for (q = p; *q == '\\'; q++);
			memcpy(file_path_temp, q, strlen(q));
			memcpy(p, file_path_temp, strlen(file_path_temp));
			if (strlen(file_path_temp) == 0)
			{
				*p = NULL;
				break;
			}
		}
	}
	strcat(target_file, &file_path[1]);
	strcat(target_file, file_name);
	if (results == 1)
	{
		if ((file = fopen(target_file, "rb")) == NULL)
		{
			time(&t);
			printf("%s404 Error,access\"%s\" fail, no this file.\n", ctime(&t), target_file);
			fprintf(fp2, "%s404 Error,access\"%s\"fail, no this file.\n", ctime(&t), target_file);
			notFound1(soc);
			return -1;
		}
	}
	else
	{
		if ((file = fopen(target_file, "rb")) == NULL)
		{
			memset(target_file, NULL, NAME_SIZE);
			strcat(target_file, &file_path[1]);
			strcat(target_file, file_name2);
			if ((file = fopen(target_file, "rb")) == NULL)
			{
				time(&t);
				printf("%s404 Error, no index file.\n", ctime(&t));
				fprintf(fp1, "%s404 Error, no index file.\n", ctime(&t));
				notFound1(soc);
				return -1;
			}
		}
	}
	time(&t);
	printf("%sAccess file:%s\n", ctime(&t), target_file);
	fprintf(fp2, "%sAccess file:%s\n", ctime(&t), target_file);
	//ftell(fp);利用函数 ftell() 也能方便地知道一个文件的长。如以下语句序列： fseek(fp, 0L,SEEK_END); len =ftell(fp); 
	//首先将文件的当前位置移到文件的末尾，然后调用函数ftell()获得当前位置相对于文件首的位移，该位移值等于文件所含字节数。
	fseek(file, 0, SEEK_END);
	file_len = ftell(file); 	//Get file size
	fseek(file, 0, SEEK_SET);
	if ((Content_type = http_get_type_by_suffix(suffix)) == NULL)
	{
		time(&t);
		printf("%s500 Error, no this type file.\n", ctime(&t));
		fprintf(fp2, "%s500 Error, no this type file.\n", ctime(&t));
		error(soc);
		return -1;
	}
	hdr_len = sprintf(http_header, http_head_temp, file_len, Content_type); //格式化首部内容
	time(&t);
	printf("%sServer sent http file：\n", ctime(&t));
	printf("%s\n", http_header);
	fprintf(fp2, "%sServer sent http file：\n", ctime(&t));
	fprintf(fp2, "%s\n", http_header);
	if ((send_len = send(soc, http_header, hdr_len, 0)) == SOCKET_ERROR)
	{
		return -1;
	}
	do
	{
		read_len = fread(read_buf, sizeof(char), HTTP_BUF_SIZE, file); // handle big file
		if (read_len > 0)
		{
			send_len = send(soc, read_buf, read_len, 0);
			file_len -= read_len;
		}
	} while ((read_len > 0) && (file_len > 0));
	fclose(file);
	fclose(fp1);
	fclose(fp2);
	return 1;
}

DWORD WINAPI SimpleHTTPServer(LPVOID lparam)
{

	WSADATA wsa;
	SOCKET srv_soc, acpt_soc;
	acpt_soc = (SOCKET)(LPVOID)lparam;
	srv_soc = (SOCKET)(LPVOID)lparam;
	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;
	char recv_buf[HTTP_BUF_SIZE];
	memset(recv_buf, NULL, sizeof(recv_buf));
	int port = HTTP_PORT, from_len = sizeof(from_addr), recv_len = 0;
	FILE *fp_c = NULL;	// client log file
	FILE *fp_s = NULL; // server log file
	time_t t1;
	fp_c = fopen("log_client.txt", "a");
	fp_s = fopen("log_server.txt", "a");
	time(&t1);
	//printf("%sNew thread created.\n", ctime(&t1));
	//fprintf(fp_s, "%sNew thread created.\n", ctime(&t1));

	int recv_bytes = recv(acpt_soc, recv_buf, HTTP_BUF_SIZE, 0);
	if (recv_bytes <= 0)
	{
		time(&t1);
		//printf("%sEmpty request.\n", ctime(&t1));
		//fprintf(fp_s, "%sEmpty request.\n", ctime(&t1));
		fclose(fp_c);
		fclose(fp_s);
		closesocket(acpt_soc);
		return 0;
	}

	recv_buf[recv_bytes] = '\0';




	time(&t1);
	printf("%sBrowser sent：\n", ctime(&t1));
	fprintf(fp_c, "%sBrowser sent：\n", ctime(&t1));
	printf("%s\n", recv_buf);
	fprintf(fp_c, "%s\n", recv_buf);
	fclose(fp_c);
	//fp_c = fopen("log_client.txt", "a");
	//Harry!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//FILE *fd; // File pointer
	char filename[] = "./log_client.txt"; // file to read
	char buff[1024];

	if ((fp_c = fopen(filename, "r")) != NULL) // open file
	{
		fseek(fp_c, 0, SEEK_SET); // make sure start from 0

		while (!feof(fp_c))
		{
			memset(buff, 0x00, 1024); // clean buffer
			fscanf(fp_c, "%[^\n]\n", buff); // read file *prefer using fscanf
		}
		//printf("Last Line :: %s,%d\n", buff);
	}
	fclose(fp_c);
	fp_c = fopen("log_client.txt", "a");
	FILE *ACCT = NULL;
	ACCT = fopen("account.txt", "r");

	if (ACCT == NULL) //if open fail
	{
		time(&t1);
		printf("%s500 Error, Failed to open the file !\n", ctime(&t1));
		exit(0); //close program
	}
	char addr1[1024];
	char *findx;
	int flag1 = 0;
	char str2[9];
	while (fgets(addr1, 1024, ACCT) != NULL)
	{
		findx = strchr(addr1, '\n');          //find '\n'
		if (findx)                            //if find is not an empty pointer
			*findx = '\0';


		int i = 0;
		for (; i < 8; i++) {
			str2[i] = buff[i];
		}
		str2[8] = '\0';

		//printf("***********%s************\n", addr);
		//printf("\nbuff#%s\n", buff);
		//printf("\naddr1#%s\n", addr1);
		//printf("\nstr2#%s\n", str2);
		if (strcmp(str2, "account=") == 0 && strcmp(buff, addr1) == 0) {
			printf("Enter VIP pages.\n");
			flag1 = 1;
			break;
		}
	}
	if (strcmp(str2, "account=") == 0 && flag1 == 0) {
		forbidden1(acpt_soc);
		//send(acpt_soc, estr, strlen(estr) + 1, 0); 
		printf("Wrong Account! \n============================================================\n");
		fprintf(fp_c, "Wrong Account!\n============================================================\n");
		fclose(fp_c);
		fclose(ACCT);
		closesocket(acpt_soc);
		return 0;
	}

	//Harry+++++++++++++++++++++++++++++++++++++++++++++++++++++

	http_send_response(acpt_soc, recv_buf, recv_len);
	closesocket(acpt_soc);
	fclose(fp_c);
	fclose(fp_s);

	return 0;

}

int main(int argc, char argv[])
{
	WSADATA wsa;
	SOCKET srv_soc, acpt_soc;
	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;
	char recv_buf[HTTP_BUF_SIZE];
	memset(recv_buf, NULL, sizeof(recv_buf));
	int port = HTTP_PORT, from_len = sizeof(from_addr), recv_len = 0;
	FILE *fp_c = NULL;	// client log file
	FILE *fp_s = NULL; // server log file
	fp_c = fopen("log_client.txt", "a");
	fp_s = fopen("log_server.txt", "a");
	time_t t1;
	if (fp_c == NULL) //if open fail
	{
		time(&t1);
		printf("%s500 Error, Failed to open the file !\n", ctime(&t1));
		exit(0); //close program
	}
	if (fp_s == NULL) //if open fail
	{
		time(&t1);
		printf("%s500 Error, Failed to open the file !\n", ctime(&t1));
		exit(0); //close program
	}

	/*if(argc==2)
	{
	port=atoi(argv[1]);
	}*/
	WSAStartup(MAKEWORD(2, 0), &wsa);
	if ((srv_soc = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) exit(0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(srv_soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		time(&t1);
		printf("%sBind address fail.\n", ctime(&t1));
		fprintf(fp_s, "%sBind address fail.\n", ctime(&t1));
		system("pause");
		exit(0);
	}
	if (listen(srv_soc, SOMAXCONN) != 0)
	{
		time(&t1);
		printf("%sSocket change mode fail.", ctime(&t1));
		fprintf(fp_s, "%sSocket change mode fail.\n", ctime(&t1));
		system("pause");
		exit(0);
	}
	time(&t1);
	printf("%sThe Service Started...\n", ctime(&t1));
	fprintf(fp_s, "%sThe Service Started...\n", ctime(&t1));
	fclose(fp_c);
	fclose(fp_s);
	while (1)
	{ 
		time(&t1);
		fp_c = fopen("log_client.txt", "a");
		fp_s = fopen("log_server.txt", "a");
		if (fp_c == NULL) //if open fail
		{
			time(&t1);
			printf("%sFailed to open the file !\n", ctime(&t1));
			exit(0); //close program
		}
		if (fp_s == NULL) //if open fail
		{
			time(&t1);
			printf("%sFailed to open the file !\n", ctime(&t1));
			exit(0); //close program
		}
		acpt_soc = accept(srv_soc, (struct sockaddr *)&from_addr, &from_len);
		if (acpt_soc == INVALID_SOCKET)
		{
			time(&t1);
			printf("%sAccept client fail.\n", ctime(&t1));
			fprintf(fp_s, "%sAccept client fail.\n", ctime(&t1));
			closesocket(acpt_soc);
			continue;
		}

		char *remote_addr = inet_ntoa(from_addr.sin_addr);//lmy
		printf("%sClient %s access...\n", ctime(&t1), inet_ntoa(from_addr.sin_addr));
		fprintf(fp_s, "%sClient %s access\n", ctime(&t1), inet_ntoa(from_addr.sin_addr));

		//**************************************gr*****************

		FILE *IP = NULL;
		IP = fopen("IP.txt", "r");

		if (IP == NULL) //if open fail
		{
			time(&t1);
			printf("%sFailed to open the file !\n", ctime(&t1));
			exit(0); //close program
		}
		char addr[1024];
		char *find;
		int flag = 0;
		while (fgets(addr, 1024, IP) != NULL)
		{
			find = strchr(addr, '\n');          //find '\n'
			if (find)                            //if find is not null pointer
				*find = '\0';
			//printf("***********%s************\n", addr);
			if (strcmp(remote_addr, addr) == 0) {
				forbidden1(acpt_soc);
				//send(acpt_soc, estr, strlen(estr) + 1, 0);
				printf("Refusing IP %s\n============================================================\n", remote_addr);
				closesocket(acpt_soc);
				flag = 1;
				break;
			}
		}
		if (flag == 1) {
			fclose(IP);
			continue;
		}
		fclose(IP);

		//**************************************gr*****************
		DWORD ThreadID;
		CreateThread(NULL, 0, SimpleHTTPServer, (LPVOID)acpt_soc, 0, &ThreadID);



		fclose(fp_c);
		fclose(fp_s);

	}
	closesocket(srv_soc);
	WSACleanup();

}
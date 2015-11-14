
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	SHOW = 1,
	INPUT,
	EDIT,
	DELETE,
	SEARCH,
	EXIT,
} MODE_SELECT;

struct address_data {
	int no;
	char name[1024];
	char address[1024];
};

int menu(void);
int input_data(struct address_data *);
int append_data_to_file(struct address_data);
int show_data(void);
int search(void);
struct address_data decode_data(char *);

const char *filename = "address.txt";

int address(void)
{
	struct address_data data;

	for(;;) {
		switch(menu()) {
		case INPUT:
			input_data(&data);
			append_data_to_file(data);
			break;
		case SHOW:
			show_data();
			break;
		case EDIT:
			break;
		case DELETE:
			break;
		case SEARCH:
			search();
			break;
		case EXIT:
			exit(0);
			break;
		default:
			break;
		}
	}

	return 0;
}

int menu(void)
{
	char buf[1024];
	int ret;

	do {
		printf("Please choose mode.\n");
		printf(
			"1:Show, 2:Input, 3:Edit, 4:Delete, 5:Search, "
			"6:Exit\n");
		fgets(buf, sizeof(buf), stdin);
		ret = atoi(buf);
	} while(!(ret >= 1 && ret <= 6));
	return ret;
}

int input_data(struct address_data *data)
{
	char buf[1024];
	char *str;

	printf("Enter name: ");
	fgets(buf, sizeof(buf), stdin);
	for(str = buf; *str != '\0'; str++)
		if(*str == '\n') *str = '\0';
	strncpy(data->name, buf, sizeof(data->name));
	printf("Enter address: ");
	fgets(buf, sizeof(buf), stdin);
	for(str = buf; *str != '\0'; str++)
		if(*str == '\n') *str = '\0';
	strncpy(data->address, buf, sizeof(data->address));
	data->no = 1;

	return 0;
}

int append_data_to_file(struct address_data data)
{
	FILE *fp;

	fp = fopen(filename, "a");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	fprintf(fp, "%d,%s,%s\n",
		data.no,
		data.name,
		data.address);
	fclose(fp);
	return 0;
}

int show_data(void)
{
	FILE *fp;
	char buf[1024];

	fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	printf("=====================================\n");
	printf("ID, Name, Address\n");
	while(1) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		printf("%s", buf);
	}
	printf("=====================================\n");
	fclose(fp);
	return 0;
}

int search(void)
{
	int type;
	int match_flag = 0;
	char buf[1024];
	char input_flag = 0;
	FILE *fp;

	struct address_data search, data;
	enum {
		ID,
		NAME,
		ADDRESS,
	};

	do {
		puts(
			"select search type\n"
			"ID, Name, Address");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		if(!strcmp(buf, "id") |
		!strcmp(buf, "Id") |
		!strcmp(buf, "ID")) {
			type = ID;
			input_flag = 1;
		} else if(!strcmp(buf, "name") |
		!strcmp(buf, "Name") |
		!strcmp(buf, "NAME")) {
			type = NAME;
			input_flag = 1;
		} else if(!strcmp(buf, "address") |
		!strcmp(buf, "Address") |
		!strcmp(buf, "ADDRESS")) {
			type = ADDRESS;
			input_flag = 1;
		}
	} while(input_flag == 0);

	switch(type) {
	case ID:
		do {
			puts("Please enter id for search");
			printf("> ");
			fgets(buf, sizeof(buf), stdin);
			search.no = atoi(buf);
		} while(search.no == 0);
		break;
	case NAME:
		puts("Please enter name for search");
		printf("> ");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		strncpy(search.name, buf, sizeof(search.name));
		break;
	case ADDRESS:
		puts("Please enter address for search");
		printf("> ");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		strncpy(search.address, buf, sizeof(search.address));
		break;
	}

	fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}

	for(match_flag = 0;;) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		data = decode_data(buf);
		switch(type) {
		case ID:
			if(search.no == data.no)
				match_flag = 1;
			break;
		case NAME:
			if(!strcmp(search.name, data.name))
				match_flag = 1;
			break;
		case ADDRESS:
			if(!strcmp(search.address, data.address))
				match_flag = 1;
			break;
		}
		if(match_flag != 0) {
			puts("=========================");
			puts("Match data!");
			printf("%d,%s,%s\n", data.no, data.name, data.address);
			puts("=========================");
		}
	}
	fclose(fp);
	return 0;
}

struct address_data decode_data(char *str)
{
	struct address_data ret;
	char *p, *begin;

	p = strchr(str, ',');
	*p = '\0';
	begin = ++p;
	ret.no = atoi(str);
	p = strchr(begin, ',');
	*p = '\0';
	strcpy(ret.name, begin);
	begin = ++p;
	p = strchr(begin, '\n');
	*p = '\0';
	strcpy(ret.address, begin);

	return ret;
}


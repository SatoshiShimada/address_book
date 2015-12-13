
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const int END_OF_PHONE_NUMBER = -1;

enum {
	SHOW = 1,
	INPUT,
	EDIT,
	DELETE,
	SEARCH,
	EXIT,
	CONFIG,
	STATUS,
} MODE_SELECT;

struct address_data {
	int  no;
	char name[1024];
	char address[1024];
	int  phone[20];
};

typedef struct {
	char filename[100];
	int  last_id;
} ADDRESS_BOOK;

/* Function prototype */
int menu(void);
int input_data(ADDRESS_BOOK *, struct address_data *);
int append_data_to_file(ADDRESS_BOOK, struct address_data);
int load_file(ADDRESS_BOOK *config);
int show_data(ADDRESS_BOOK);
int search(ADDRESS_BOOK);
int show_status(ADDRESS_BOOK config);
struct address_data decode_data(char *);

int address(void)
{
	struct address_data data;
	ADDRESS_BOOK address_config;

	/* initialize config of address book */
	strcpy(address_config.filename, "address.txt");
	address_config.last_id = 1;

	load_file(&address_config);

	for(;;) {
		switch(menu()) {
		case INPUT:
			input_data(&address_config, &data);
			append_data_to_file(address_config, data);
			break;
		case SHOW:
			show_data(address_config);
			break;
		case EDIT:
			break;
		case DELETE:
			break;
		case SEARCH:
			search(address_config);
			break;
		case EXIT:
			exit(0);
			break;
		case CONFIG:
			break;
		case STATUS:
			show_status(address_config);
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
			"6:Exit, 7:Config, 8:Status\n");
		fgets(buf, sizeof(buf), stdin);
		ret = atoi(buf);
	} while(!(ret >= 1 && ret <= 8));
	return ret;
}

int input_data(ADDRESS_BOOK *config, struct address_data *data)
{
	char buf[1024];
	char *str;
	char conv[2] = "0";
	int i;

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
	data->no = ++config->last_id;
	printf("Enter phone number: ");
	fgets(buf, sizeof(buf), stdin);
	for(i = 0; buf[i] != '\0'; i++) {
		conv[0] = buf[i];
		data->phone[i] = atoi(conv);
		if(i > 12) break;
	}

	return 0;
}

int append_data_to_file(ADDRESS_BOOK config, struct address_data data)
{
	FILE *fp;
	char *filename = config.filename;
	int i;

	fp = fopen(filename, "a");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	fprintf(fp, "%d,%s,%s,",
		data.no,
		data.name,
		data.address);
	for(i = 0; i < 12; i++) {
		fprintf(fp, "%d", data.phone[i]);
	}
	fprintf(fp, "\n");
	fclose(fp);
	return 0;
}

int load_file(ADDRESS_BOOK *config)
{
	FILE *fp;
	char buf[1024];
	char *filename = config->filename;

	fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	while(1) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		config->last_id = atoi(buf);
	}
	fclose(fp);
	return 0;
}

int show_data(ADDRESS_BOOK config)
{
	FILE *fp;
	char buf[1024];
	char *filename = config.filename;

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

int search(ADDRESS_BOOK config)
{
	int type;
	int match_flag = 0;
	int phone_flag = 0;
	int count;
	int i;
	char buf[1024];
	char input_flag = 0;
	FILE *fp;
	char *filename = config.filename;

	struct address_data search, data;
	enum {
		ID,
		NAME,
		ADDRESS,
		PHONE,
	};

	do {
		puts(
			"select search type\n"
			"[ID, Name, Address, Phone]");
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
		} else if(!strcmp(buf, "phone") |
		!strcmp(buf, "Phone") |
		!strcmp(buf, "PHONE")) {
			type = PHONE;
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
	case PHONE:
		puts("Please enter phone number for search");
		printf("> ");
		fgets(buf, sizeof(buf), stdin);
		for(i = 0; buf[i] != '\n'; i++) {
			search.phone[i] = buf[i] - '0';
		}
		search.phone[i] = END_OF_PHONE_NUMBER;
		break;
	}

	fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}

	puts("=========================");
	for(count = 0;;) {
		match_flag = 0;
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
			else {
				for(i = 0; data.name[i] != '\0'; i++) {
					buf[i] = tolower(data.name[i]);
				}
				buf[i] = '\0';
				if(!strcmp(search.name, buf))
					match_flag = 1;
			}
			break;
		case ADDRESS:
			if(!strcmp(search.address, data.address))
				match_flag = 1;
			break;
		case PHONE:
			for(i = 0; search.phone[i] != END_OF_PHONE_NUMBER && data.phone[i] != END_OF_PHONE_NUMBER; i++) {
				phone_flag = 0;
				if(search.phone[i] != data.phone[i]) break;
				phone_flag = 1;
			}
			if(phone_flag)
				match_flag = 1;
			break;
		}
		if(match_flag != 0) {
			count++;
			puts("Match data!");
			printf("ID  : Name : Address : Phone number\n");
			printf("%3d : %s : %s : ", data.no, data.name, data.address);
			for(i = 0; data.phone[i] != END_OF_PHONE_NUMBER; i++) {
				printf("%d", data.phone[i]);
			}
			printf("\n");
		}
	}
	if(count == 0) {
		puts("Not found!");
	}
	puts("=========================");
	fclose(fp);
	return 0;
}

struct address_data decode_data(char *str)
{
	struct address_data ret;
	char *p, *begin;
	int i;

	p = strchr(str, ',');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		return ret;
	}
	*p = '\0';
	begin = ++p;
	ret.no = atoi(str);
	p = strchr(begin, ',');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		return ret;
	}
	*p = '\0';
	strcpy(ret.name, begin);
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		return ret;
	}
	begin = ++p;
	p = strchr(begin, ',');
	*p = '\0';
	strcpy(ret.address, begin);
	begin = ++p;
	p = strchr(begin, '\n');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		return ret;
	}
	*p = '\0';
	for(i = 0; begin[i] != '\0'; i++) {
		if(i > sizeof(ret.phone)/sizeof(int)) break;
		ret.phone[i] = begin[i] - '0';
	}
	ret.phone[i] = END_OF_PHONE_NUMBER;

	return ret;
}

int show_status(ADDRESS_BOOK config)
{
	FILE *fp;
	int record;
	char buf[1024];

	fp = fopen(config.filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", config.filename);
		return -1;
	}
	printf("=====================================\n");
	for(record = 0; ;) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		record ++;
	}
	printf("Count of record: %d\n", record);
	printf("=====================================\n");
	fclose(fp);
	return 0;
}


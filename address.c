
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "address.h"

const int END_OF_PHONE_NUMBER = -1;

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
			edit_data(&address_config);
			break;
		case DELETE:
			delete_data(address_config);
			break;
		case SEARCH:
			search(address_config);
			break;
		case EXIT:
			exit(0);
			break;
		case CONFIG:
			config(&address_config);
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
		if(i > sizeof(data->phone)) break;
	}
	buf[i] = END_OF_PHONE_NUMBER;
	data->status = (0x01 << ACTIVE);

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
	fprintf(fp, ",%d\n", data.status);
	fclose(fp);
	return 0;
}

int load_file(ADDRESS_BOOK *config)
{
	FILE *fp;
	char buf[1024];
	char *filename = config->filename;
	int width_name, width_address;
	struct address_data data;

	fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	while(1) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		config->last_id = atoi(buf);
	}
	fseek(fp, 0, SEEK_SET);
	for(width_name = 0, width_address = 0; ;) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		data = decode_data(buf);
		if(width_name < strlen(data.name))
			width_name = strlen(data.name);
		if(width_address < strlen(data.address))
			width_address = strlen(data.address);
	}
	config->width_name = width_name;
	config->width_address = width_address;
	fclose(fp);
	return 0;
}

int show_data(ADDRESS_BOOK config)
{
	FILE *fp;
	struct address_data data;
	char buf[1024];
	char format_str[1024];
	char *filename = config.filename;
	int i;
	int str_count;

	fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	sprintf(format_str, "| %%%ds | %%%ds | %%11s |\n", config.width_name, config.width_address);
	str_count = sprintf(buf, format_str, "Name", "Address", "Phone") - 1; // ignore new line charactor
	for(i = str_count; i > 0; i--)
		printf("=");
	printf("\n");
	printf("%s", buf);
	for(i = str_count; i > 0; i--)
		printf("=");
	printf("\n");
	sprintf(format_str, "| %%%ds | %%%ds | ", config.width_name, config.width_address);
	while(1) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		data = decode_data(buf);
		if(data.no == 0) continue;
		if(data.status == (0x01 << HIDE)) continue;
		printf(format_str, data.name, data.address);
		for(i = 0; data.phone[i] != END_OF_PHONE_NUMBER; i++) {
			printf("%d", data.phone[i]);
		}
		printf(" |\n");
	}
	for(i = str_count; i > 0; i--)
		printf("=");
	printf("\n");
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
	int first_flag = 1;
	char buf[1024];
	char format_str[1024];
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
		if(data.no == 0) continue;
		if(data.status == (0x01 << HIDE)) continue;
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
			if(first_flag) {
				sprintf(format_str, "| %%%ds | %%%ds | %%11s |\n", config.width_name, config.width_address);
				printf(format_str, "Name", "Address", "Phone number");
				first_flag = 0;
			}
			sprintf(format_str, "| %%%ds | %%%ds | ", config.width_name, config.width_address);
			printf(format_str, data.name, data.address);
			for(i = 0; data.phone[i] != END_OF_PHONE_NUMBER; i++) {
				printf("%d", data.phone[i]);
			}
			printf(" |\n");
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
	char *p;
	int i;

	p = strchr(str, ',');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		return ret;
	}
	*p = '\0';
	ret.no = atoi(str);
	str = ++p;
	p = strchr(str, ',');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		ret.status = 0;
		return ret;
	}
	*p = '\0';
	strcpy(ret.name, str);
	str = ++p;
	p = strchr(str, ',');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		ret.status = 0;
		return ret;
	}
	*p = '\0';
	strcpy(ret.address, str);
	str = ++p;
	p = strchr(str, ',');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		ret.status = 0;
		return ret;
	}
	for(i = 0; str[i] != ','; i++) {
		if(i > sizeof(ret.phone)/sizeof(int)) break;
		ret.phone[i] = str[i] - '0';
	}
	ret.phone[i] = END_OF_PHONE_NUMBER;
	str = ++p;
	p = strchr(str, '\n');
	if(p == NULL) {
		ret.no = 0;
		ret.name[0] = '\0';
		ret.address[0] = '\0';
		ret.status = 0;
		return ret;
	}
	*p = '\0';
	ret.status = atoi(str);

	return ret;
}

int show_status(ADDRESS_BOOK config)
{
	FILE *fp;
	struct address_data data;
	int record, record_active, record_hide;
	char buf[1024];

	fp = fopen(config.filename, "r");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", config.filename);
		return -1;
	}
	printf("=====================================\n");
	for(record = 0, record_active = 0, record_hide = 0; ;) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		record ++;
		data = decode_data(buf);
		if(data.status == (0x01 << ACTIVE)) {
			record_active++;
		} else if(data.status == (0x01 << HIDE)) {
			record_hide++;
		}
	}
	printf("Number of record: %d\n", record);
	printf("Number of active record: %d\n", record_active);
	printf("=====================================\n");
	fclose(fp);
	return 0;
}

int delete_data(ADDRESS_BOOK config)
{
	int type;
	int match_flag = 0;
	int phone_flag = 0;
	int count;
	int index;
	int i, j, k;
	char buf[1024];
	char format_str[1024];
	char input_flag = 0;
	FILE *fp;
	char *filename = config.filename;

	struct address_data search, data;
	struct address_data list[1024];
	int list_index[1024];

	enum {
		ID,
		NAME,
		ADDRESS,
		PHONE,
	};

	do {
		puts(
			"Please choose entry\n"
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
	for(count = 0, index = 0;;) {
		match_flag = 0;
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		data = decode_data(buf);
		if(data.no == 0) continue;
		list[count] = data;
		list_index[count] = 0;
		if(data.status == (0x01 << HIDE)) {
			count++;
			continue;
		}
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
			list_index[count] = 1;
			index++;
		}
		count++;
	}
	if(index == 0) {
		puts("Not found!");
	} else if(index == 1) {
		puts("Deleted!");
		for(i = 0; i < count; i++) {
			if(list_index[i] == 1) {
				list[i].status = (0x01 << HIDE);
				break;
			}
		}
	} else if(index > 1) {
		printf("Find some entries!\nChoose entry\n");
		for(i = 0, j = 0; i < count; i++) {
			if(list_index[i] == 1) {
				sprintf(format_str, "| %%2d | %%%ds | %%%ds | ", config.width_name, config.width_address);
				printf(format_str, j + 1, list[i].name, list[i].address);
				for(k = 0; list[i].phone[k] != END_OF_PHONE_NUMBER; k++) {
					printf("%d", list[i].phone[k]);
				}
				printf(" |\n");
				j++;
			}
		}
		fgets(buf, sizeof(buf), stdin);
		for(i = 0, j = 0; i < count; i++) {
			if(list_index[i] == 1) {
				if((j - 1) == atoi(buf)) {
					list[i].status = (0x01 << HIDE);
					break;
				}
				j++;
			}
		}
	}
	puts("=========================");
	fclose(fp);
	fp = fopen(filename, "w");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	for(i = 0; i < count; i++) {
		fprintf(fp, "%d,%s,%s,", list[i].no, list[i].name, list[i].address);
		for(k = 0; list[i].phone[k] != END_OF_PHONE_NUMBER; k++) {
			fprintf(fp, "%d", list[i].phone[k]);
		}
		fprintf(fp, ",%d\n", list[i].status);
	}
	fclose(fp);
	return 0;
}

int config(ADDRESS_BOOK *config)
{
	char buf[1024];

	printf(
		"1: filename\n"
		"2: quit\n");

	fgets(buf, sizeof(buf), stdin);
	switch(atoi(buf)) {
	case 1:
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0'; // delete new line charactor
		strcpy(config->filename, buf);
		break;
	case 2:
		break;
	}
	return 0;
}

int edit_data(ADDRESS_BOOK *config)
{
	int type;
	int match_flag = 0;
	int phone_flag = 0;
	int count;
	int index;
	int i, j, k;
	char buf[1024];
	char format_str[1024];
	char input_flag = 0;
	FILE *fp;
	char *filename = config->filename;

	struct address_data search, data;
	struct address_data list[1024];
	int list_index[1024];

	enum {
		ID,
		NAME,
		ADDRESS,
		PHONE,
	};

	do {
		puts(
			"Please choose entry\n"
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
			puts("Please enter id for edit");
			printf("> ");
			fgets(buf, sizeof(buf), stdin);
			search.no = atoi(buf);
		} while(search.no == 0);
		break;
	case NAME:
		puts("Please enter name for edit");
		printf("> ");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		strncpy(search.name, buf, sizeof(search.name));
		break;
	case ADDRESS:
		puts("Please enter address for edit");
		printf("> ");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		strncpy(search.address, buf, sizeof(search.address));
		break;
	case PHONE:
		puts("Please enter phone number for edit");
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

	for(count = 0, index = 0;;) {
		match_flag = 0;
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		data = decode_data(buf);
		if(data.no == 0) continue;
		list[count] = data;
		list_index[count] = 0;
		if(data.status == (0x01 << HIDE)) {
			count++;
			continue;
		}
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
			list_index[count] = 1;
			index++;
		}
		count++;
	}
	if(index == 0) {
		puts("Not found!");
	} else if(index == 1) {
		for(i = 0; i < count; i++) {
			if(list_index[i] == 1) {
				list[i].status = (0x01 << HIDE);
				break;
			}
		}
	} else if(index > 1) {
		printf("Find some entries!\nChoose entry\n");
		for(i = 0, j = 0; i < count; i++) {
			if(list_index[i] == 1) {
				sprintf(format_str, "| %%2d | %%%ds | %%%ds | ", config->width_name, config->width_address);
				printf(format_str, j + 1, list[i].name, list[i].address);
				for(k = 0; list[i].phone[k] != END_OF_PHONE_NUMBER; k++) {
					printf("%d", list[i].phone[k]);
				}
				printf(" |\n");
				j++;
			}
		}
		fgets(buf, sizeof(buf), stdin);
		for(i = 0, j = 0; i < count; i++) {
			if(list_index[i] == 1) {
				if((j - 1) == atoi(buf)) {
					list[i].status = (0x01 << HIDE);
					break;
				}
				j++;
			}
		}
	}
	fclose(fp);
	fp = fopen(filename, "w");
	if(!fp) {
		fprintf(stderr, "Error: file open [%s]\n", filename);
		return -1;
	}
	for(i = 0; i < count; i++) {
		fprintf(fp, "%d,%s,%s,", list[i].no, list[i].name, list[i].address);
		for(k = 0; list[i].phone[k] != END_OF_PHONE_NUMBER; k++) {
			fprintf(fp, "%d", list[i].phone[k]);
		}
		fprintf(fp, ",%d\n", list[i].status);
	}
	fclose(fp);

	input_data(config, &data);
	append_data_to_file(*config, data);
	return 0;
}


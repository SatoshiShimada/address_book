
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	INPUT = 1,
	SHOW,
	EDIT,
	DELETE,
} MODE_SELECT;

struct address_data {
	int no;
	char name[128];
	char addr[128];
};

int menu(void);
int input_data(struct address_data *);
int append_data_to_file(struct address_data);
int show_data(void);

char *filename = "address.txt";

int address(void)
{
	struct address_data data;

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
	}

	return 0;
}

int menu(void)
{
	char buf[1024];
	int ret;

	do {
		printf("Please choose mode.\n");
		printf("1:Input, 2:Show, 3:Edit, 4:Delete\n");
		fgets(buf, sizeof(buf), stdin);
		ret = atoi(buf);
	} while(!(ret >= 1 && ret <= 4));
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
	strncpy(data->addr, buf, sizeof(data->addr));
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
		data.addr);
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
	while(1) {
		if(fgets(buf, sizeof(buf), fp) == 0) break;
		printf("%s", buf);
	}
	return 0;
}


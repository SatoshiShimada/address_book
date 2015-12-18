
#ifndef __ADDRESS_H__
#define __ADDRESS_H__

enum {
	ACTIVE = 0,
	HIDE,
} RECODE_STATUS;

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
	unsigned int  status;
};

typedef struct {
	char filename[100];
	int  last_id;
	int  width_name, width_address;
} ADDRESS_BOOK;

/* Function prototype */
int address(void);
int menu(void);
int input_data(ADDRESS_BOOK *, struct address_data *);
int append_data_to_file(ADDRESS_BOOK, struct address_data);
int load_file(ADDRESS_BOOK *config);
int show_data(ADDRESS_BOOK);
int delete_data(ADDRESS_BOOK);
int search(ADDRESS_BOOK);
int show_status(ADDRESS_BOOK config);
int config(ADDRESS_BOOK *);
int edit_data(ADDRESS_BOOK *);
struct address_data decode_data(char *);

#endif // __ADDRESS_H__

// messaging.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

struct users {
	char userName[15];
	long category[5];
};

struct categories {
	char categoryName[15];
	long msgOffset;
};

struct messages {
	char user[15];
	long nextMessageOffset;
	char msg[108];
};

void convertLower(char *str) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] >= 65 && str[i] <= 90)
			str[i] = str[i] + 32;
	}
}

void copyName(char *dest, char *source) {
	int i;
	for (i = 0; source[i] != '\0'; i++) {
		dest[i] = source[i];
	}
	dest[i] = '\0';
}

int compare(char *str1, char *str2) {
	int i;
	for (i = 0; str1[i] != '\0'; i++) {
		if (str1[i] != str2[i]) 
			return 0;
	}
	if (str2[i] != '\0')
		return 0;
	return 1;
}

int signUp(FILE *file) {
	struct users newUser;
	struct users user;
	int i;
	int userCount;
	printf("Enter new user name: ");
	scanf("%s", newUser.userName);
	convertLower(newUser.userName);
	fseek(file, 0, SEEK_SET);
	fread(&userCount, sizeof(int), 1, file);
	for (i = 0; i < userCount; i++) {
		fread(&user, sizeof(struct users), 1, file);
		if (compare(newUser.userName, user.userName)) {
			printf("User already exists\n");
			getch();
			exit(0);
		}
	}
	for (int i = 0; i < 5; i++)
		newUser.category[i] = 0;
	fseek(file, sizeof(int) + sizeof(struct users) * userCount , SEEK_SET);
	fwrite(&newUser, sizeof(struct users), 1, file);
	userCount += 1;
	fseek(file, 0, SEEK_SET);
	fwrite(&userCount, sizeof(int), 1, file);
	return userCount - 1;
}

int logIn(FILE *file) {
	char name[15];
	int index;
	int userCount;
	struct users user;
	printf("Enter user name: ");
	scanf("%s", name);
	convertLower(name);
	fseek(file, 0, SEEK_SET);
	fread(&userCount, sizeof(int), 1, file);
	for (index = 0; index < userCount; index++) {
		fread(&user, sizeof(struct users), 1, file);
		if (compare(name, user.userName)) {
			break;
		}
	}
	if (index == userCount) {
		printf("Invalid login Id\n");
		getch();
		exit(0);
	}
	return index;
}

void createCategory(FILE *file, int index) {
	int categoryCount;
	int position;
	struct categories newCategory;
	struct users logedInUser;
	int i;

	fseek(file, sizeof(int) + sizeof(struct users) * index, SEEK_SET);
	fread(&logedInUser, sizeof(struct users), 1, file);
	for (i = 0; i < 5; i++) {
		if (logedInUser.category[i] == 0) {
			break;
		}
	}
	if (i == 5) {
		printf("No new category is allowed\n");
		getch();
		exit(0);
	}

	fseek(file, sizeof(int) + sizeof(struct users) * 20, SEEK_SET);
	fread(&categoryCount, sizeof(int), 1, file);
	fseek(file, sizeof(struct categories) * categoryCount, SEEK_CUR);
	printf("Enter new category: ");
	scanf("%s", newCategory.categoryName);
	newCategory.msgOffset = 0;
	position = ftell(file);
	fwrite(&newCategory, sizeof(struct categories), 1, file);
	fseek(file, 0, SEEK_SET);
	logedInUser.category[i] = position - ftell(file);
	fseek(file, sizeof(int) + sizeof(struct users) * index, SEEK_SET);
	fwrite(&logedInUser, sizeof(struct users), 1, file);
	fseek(file, sizeof(int) + sizeof(struct users) * 20, SEEK_SET);
	categoryCount += 1;
	fwrite(&categoryCount, sizeof(int), 1, file);
}

int displayUsers(FILE *file) {
	int choice;
	int userCount;
	struct users user;
	fseek(file, 0, SEEK_SET);
	fread(&userCount, sizeof(int), 1, file); 
	if (userCount == 0) {
		printf("No users to display\n");
	} else {
		for (int i = 0; i < userCount; i++) {
			fread(&user, sizeof(struct users), 1, file);
			printf("%d. %s\n", i + 1, user.userName);
		}
	}
	printf("Enter 300 to go back\n");
	printf("Enter your chocie: ");
	scanf("%d", &choice);
	return choice - 1;
}

int displayCategories(FILE *file) {
	int categoryCount;
	struct categories category;
	int choice;
	fseek(file, sizeof(int) + sizeof(struct users) * 20, SEEK_SET);
	fread(&categoryCount, sizeof(int), 1, file);
	if (categoryCount == 0) {
		printf("No Categories Available\n");
	} else {
		for (int i = 0; i < categoryCount; i++) {
			fread(&category, sizeof(struct categories), 1, file);
			printf("%d. %s\n", i + 1, category.categoryName);
		}
	}
	printf("Enter 300  to go back\n");
	printf("Enter your choice: ");
	scanf("%d", &choice);
	return choice - 1;
}

int showUserCategories(int index, FILE *file) {
	struct users user;
	struct categories category;
	int choice;
	int i;
	fseek(file, sizeof(int) + sizeof(struct users) * index, SEEK_SET);
	fread(&user, sizeof(struct users), 1, file);
	for (i = 0; i < 5; i++) {
		if (user.category[i] == 0)
			break;
		fseek(file, user.category[i] , SEEK_SET);
		fread(&category, sizeof(struct categories), 1, file);
		printf("%d. %s\n", i + 1, category.categoryName);
	}
	if (i == 0) 
		printf("No categories to display\n");
	printf("Enter 300 to go back\n");
	printf("Enter your choice: ");
	scanf("%d", &choice);
	return choice - 1;
}

long writeMessage(FILE *file, int userId) {
	struct messages message;
	struct users user;
	char bitVectorValue;
	int position;
	long i;
	fflush(stdin);
	printf("Enter your message: ");
	scanf("%[^\n]", message.msg);
	message.nextMessageOffset = 0;
	fseek(file, sizeof(int) + sizeof(struct users) * userId, SEEK_SET);
	fread(&user, sizeof(struct users), 1, file);
	copyName(message.user, user.userName);
	fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * 100, SEEK_SET);
	for (i = 0; i < 60000; i++) {
		fread(&bitVectorValue, sizeof(char), 1, file);
		if (bitVectorValue == '\0')
			break;
	}
	if (i == 60000 && bitVectorValue != '0') {
		printf("Memory overflow\n");
		return 0;
	}
	bitVectorValue = '1';
	fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * 100 + sizeof(char) * i, SEEK_SET);
	fwrite(&bitVectorValue, sizeof(char), 1, file);
	fseek(file, (sizeof(char) * 60000 - i - 1) + (sizeof(struct messages) * i), SEEK_CUR);
	position = ftell(file);
	fwrite(&message, sizeof(struct messages), 1, file);
	fseek(file, 0, SEEK_SET);
	return position - ftell(file);
}

int viewMessages(long offset, FILE *file) {
	struct messages message;
	int choice;
	int i = 1;
	if (offset == 0) {
		printf("No messages yet\n\n");
	} else {
		fseek(file, offset, SEEK_SET);
		do {
			fread(&message, sizeof(struct messages), 1, file);
			printf("%d. %s:\n", i++, message.user);
			printf("%s\n\n", message.msg);
			fseek(file, message.nextMessageOffset, SEEK_SET);
		}  while (message.nextMessageOffset != 0);
	}
	printf("1. Message or Reply to the message\n");
	printf("2. Delete message\n");
	printf("Enter 300 to go back\n");
	printf("Enter your choice: ");
	scanf("%d", &choice);
	return choice - 1;
}

int setOffset(long offset, struct categories category, FILE *file) {
	struct categories testCategory;
	struct messages message;
	int categoryCount;
	long beforeOffset;
	int i;
	fseek(file, sizeof(int) + sizeof(struct users) * 20, SEEK_SET);
	fread(&categoryCount, sizeof(int), 1, file);
	for (i = 0; i < categoryCount; i++) {
		fread(&testCategory, sizeof(struct categories), 1, file);
		if (compare(category.categoryName, testCategory.categoryName)) {
			break;
		}
	}
	if (testCategory.msgOffset == 0) {
		testCategory.msgOffset = offset;
		fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * i, SEEK_SET);
		fwrite(&testCategory, sizeof(struct categories), 1, file);
	} else {
		beforeOffset = category.msgOffset;
		fseek(file, category.msgOffset, SEEK_SET);
		fread(&message, sizeof(struct messages), 1, file);
		while (message.nextMessageOffset != 0) {
			beforeOffset = message.nextMessageOffset;
			fseek(file, message.nextMessageOffset, SEEK_SET);
			fread(&message, sizeof(struct messages), 1, file);
		}
		message.nextMessageOffset = offset;
		fseek(file, beforeOffset, SEEK_SET);
		fwrite(&message, sizeof(struct messages), 1, file);
	}
	return i;
}

void deleteMessage(FILE *file, struct categories category) {
	int categoryCount;
	int index;
	struct categories testCategory;
	int choice;
	long position;
	struct messages message;
	struct messages msg;
	long msgStartAdd;
	long beforeOffset;
	int bitVectorValue = '\0';
	fseek(file, sizeof(int) + sizeof(struct users) * 20, SEEK_SET);
	fread(&categoryCount, sizeof(int), 1, file);
	for (index = 0; index < categoryCount; index++) {
		fread(&testCategory, sizeof(struct categories), 1, file);
		if (compare(testCategory.categoryName, category.categoryName)) {
			break;
		}
	}
	if (testCategory.msgOffset == 0) {
		printf("No messages to delete\n");
		return;
	} 
	printf("Enter message number to be deleted: ");
	scanf("%d", &choice);
	fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * 100 + sizeof(char) * 60000, SEEK_SET);
	msgStartAdd = ftell(file);
	fseek(file, category.msgOffset, SEEK_SET);
	if (choice == 1) {
		position = ftell(file);
		fread(&message, sizeof(struct messages), 1, file);
		category.msgOffset = message.nextMessageOffset;
		fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * index, SEEK_SET);
		fwrite(&category, sizeof(struct categories), 1, file);
	} else {
		while (choice != 1) {
			beforeOffset = ftell(file);
			fread(&message, sizeof(message), 1, file);
			fseek(file, message.nextMessageOffset, SEEK_SET);
			position = ftell(file);
			choice--;
		}
		fread(&msg, sizeof(struct messages), 1, file);
		message.nextMessageOffset = msg.nextMessageOffset;
		fseek(file, beforeOffset, SEEK_SET);
		fwrite(&message, sizeof(struct messages), 1, file);
	}
	choice = (position - msgStartAdd) / sizeof(struct messages);
	fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * 100 + sizeof(char) * choice, SEEK_SET);
	fwrite(&bitVectorValue, sizeof(char), 1, file);
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *file;
	int choice;
	int i;
	struct users user;
	struct categories category;
	long offset;
	long offsetted;
	int userIndex;
	file = fopen("data.bin", "rb+");
	if (!file) {
		printf("File not exists\n");
		getch();
		exit(0);
	}
	fseek(file, sizeof(int) + sizeof(struct users) * 20, SEEK_SET);
	printf("1. Sign up\n");
	printf("2. Log in\n");
	printf("Enter your option:  ");
	scanf("%d", &choice);
	switch(choice) {
		case 1:
			userIndex = signUp(file);
			break;
		case 2:
			userIndex = logIn(file);
			break;
		default:
			printf("Invalid option\n");
			getch();
			exit(0);
	}

	choice = 299;
	while (choice == 299) {
		printf("1. Show users\n");
		printf("2. Show all categories\n");
		printf("3. Show user created categories\n");
		printf("4. Create own category\n");
		printf("5. Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);
		switch(choice) {
			case 1:
				choice = displayUsers(file);
				if (choice != 299) {
					fseek(file, sizeof(int) + sizeof(struct users) * choice, SEEK_SET);
					fread(&user, sizeof(struct users), 1, file);
					choice = showUserCategories(choice, file);
					fseek(file, user.category[choice], SEEK_SET);
				}
				break;
			case 2:
				choice = displayCategories(file);
				fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * choice, SEEK_SET);
				break;
			case 3:
				choice = showUserCategories(userIndex, file);
				fseek(file, sizeof(int) + sizeof(struct users) * userIndex, SEEK_SET);
				fread(&user, sizeof(struct users), 1, file);
				fseek(file, user.category[choice], SEEK_SET);
				break;
			case 4: 
				createCategory(file, userIndex);
				choice = 299;
				break;
			default:
				printf("Bye\n");
				getch();
				exit(0);
		}
		if (choice != 299) {
			fread(&category, sizeof(struct categories), 1, file);
			offset = category.msgOffset;
			i = -1;
			while (choice != 299) {
				choice = viewMessages(offset, file);
				switch(choice) {
					case 0: 
						offsetted = writeMessage(file, userIndex);
						if (offsetted)
							i = setOffset(offsetted, category, file);
						break;
					case 1:
						deleteMessage(file, category);
						break;
					case 299:
						break;
					default:
						printf("Invalid option\n");
						break;	
				}
				if (choice != 299 && i != -1) {
					fseek(file, 2 * sizeof(int) + sizeof(struct users) * 20 + sizeof(struct categories) * i, SEEK_SET);
					fread(&category, sizeof(struct categories),1 , file);
					offset = category.msgOffset;
				}
			}
		}
	}
	printf("Bye");
	getch();
	return 0;
}
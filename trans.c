// Bank Account Management System using Random Access File

#include <stdio.h>
#include <stdlib.h>

// structure definition
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    // open file; create if doesn't exist
    cfPtr = fopen("credit.dat", "rb+");
    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            exit(1);
        }
    }

    // menu loop
    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        default:
            printf("Incorrect choice\n");
        }
    }

    fclose(cfPtr);

    return 0;
}

// display menu
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\nEnter your choice\n");
    printf("1 - Store formatted text file\n");
    printf("2 - Update an account\n");
    printf("3 - Add a new account\n");
    printf("4 - Delete an account\n");
    printf("5 - Exit\n");
    printf("? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        // clear invalid input
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return 0;
    }

    return menuChoice;
}

// create text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        printf("File could not be opened.\n");
    }
    else
    {
        rewind(readPtr);

        fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
                "Acct", "Last Name", "First Name", "Balance");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n",
                        client.acctNum,
                        client.lastName,
                        client.firstName,
                        client.balance);
            }
        }

        fclose(writePtr);

        printf("accounts.txt created successfully.\n");
    }
}

// update account
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client = {0, "", "", 0.0};

    printf("Enter account number to update (1 - 100): ");
    if (scanf("%u", &account) != 1 || account < 1 || account > 100)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error reading account.\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
    }
    else
    {
        printf("%u %s %s %.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);

        printf("Enter charge (+) or payment (-): ");
        if (scanf("%lf", &transaction) != 1)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("Invalid transaction amount.\n");
            return;
        }

        client.balance += transaction;

        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);

        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        printf("Account updated successfully.\n");
    }
}

// add new account
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error reading account.\n");
        return;
    }

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
    }
    else
    {
        printf("Enter lastname firstname balance:\n");
        if (scanf("%14s%9s%lf",
                  client.lastName,
                  client.firstName,
                  &client.balance) != 3)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("Invalid input.\n");
            return;
        }

        client.acctNum = accountNum;

        fseek(fPtr,
              (accountNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        printf("Account added successfully.\n");
    }
}

// delete account
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error reading account.\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
    }
    else
    {
        fseek(fPtr,
              (accountNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        fwrite(&blankClient,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Account deleted successfully.\n");
    }
}

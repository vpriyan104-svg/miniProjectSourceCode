// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void initializeFile(FILE *fPtr);
void clearInput(void);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    cfPtr = fopen("credit.dat", "rb+");
    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL)
        {
            fprintf(stderr, "%s: File could not be opened.\n", argv[0]);
            return EXIT_FAILURE;
        }

        initializeFile(cfPtr);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1: // create text file from record file
            textFile(cfPtr);
            break;
        case 2: // update record
            updateRecord(cfPtr);
            break;
        case 3: // create record
            newRecord(cfPtr);
            break;
        case 4: // delete existing record
            deleteRecord(cfPtr);
            break;
        default: // display if user does not select valid choice
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr); // fclose closes the file
    return EXIT_SUCCESS;
}

// initialize the random-access file with blank records
void initializeFile(FILE *fPtr)
{
    struct clientData blankClient = {0, "", "", 0.0};

    rewind(fPtr);
    for (unsigned int i = 0; i < MAX_ACCOUNTS; ++i)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
    fflush(fPtr);
    rewind(fPtr);
}

// flush any leftover input after invalid scanf usage
void clearInput(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    struct clientData client = {0, "", "", 0.0};

    writePtr = fopen("accounts.txt", "w");
    if (writePtr == NULL)
    {
        puts("File could not be opened.");
        return;
    }

    rewind(readPtr);
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

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
}

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - %u): ", MAX_ACCOUNTS);
    if (scanf("%u", &account) != 1 || account < 1 || account > MAX_ACCOUNTS)
    {
        clearInput();
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        puts("Error reading account record.");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("%-6u%-16s%-11s%10.2f\n\n",
           client.acctNum,
           client.lastName,
           client.firstName,
           client.balance);

    printf("Enter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1)
    {
        clearInput();
        puts("Invalid transaction amount.");
        return;
    }

    client.balance += transaction;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated account:\n%-6u%-16s%-11s%10.2f\n",
           client.acctNum,
           client.lastName,
           client.firstName,
           client.balance);
}

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter account number to delete (1 - %u): ", MAX_ACCOUNTS);
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        clearInput();
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        puts("Error reading account record.");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    puts("Account deleted.");
}

// create and insert record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1 - %u): ", MAX_ACCOUNTS);
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        clearInput();
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        puts("Error reading account record.");
        return;
    }

    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
        return;
    }

    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3)
    {
        clearInput();
        puts("Invalid input.");
        return;
    }

    client.acctNum = accountNum;
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
}

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called\n"
           "    \"accounts.txt\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        clearInput();
        return 0;
    }

    return menuChoice;
} // end function enterChoice

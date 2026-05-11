// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// clientData structure definition
#pragma pack(1)
struct clientData
{
    unsigned int acctnum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;  
    int pin;              // account PIN
};
#pragma pack()            // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *credit_data);
void newRecord(FILE *credit_data);
void deleteRecord(FILE *credit_data);
void displayTextFile(void);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // Try to open file for reading/writing; create if doesn't exist
    cfPtr = fopen("credit.dat", "r+b");
    if (cfPtr == NULL)
    {
        // File doesn't exist, create it
        cfPtr = fopen("credit.dat", "w+b");
    }
    
    if (cfPtr == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        // update record
        case 1:
            updateRecord(cfPtr);
            break;
        // create record
        case 2:
            newRecord(cfPtr);
            break;
        // delete existing record
        case 3:
            deleteRecord(cfPtr);
            break;
        // display if user does not select valid choice
        case 4:
            textFile(cfPtr);   // create accounts.txt
            displayTextFile(); // show it in terminal
            break;
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            // write single record to text file
            if (result != 0 && client.acctnum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctnum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *credit_data)
{
    unsigned int account;      // account number
    unsigned int option;       // deposit or withdrawal choice
    double transaction;        // transaction amount
    int inputPin;              // PIN entered by user
    int attempts = 0;          // PIN attempt counter
    int authenticated = 0;     // authentication flag
    struct clientData client;  // account data

    printf("%s", "Enter account to update (1 - 100): ");
    fflush(stdout);
    scanf("%u", &account);

    fseek(credit_data, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, credit_data);

    if (client.acctnum == 0)
    {
        printf("Account #%d has no information.\n", account);
        return;
    }

    // PIN AUTHENTICATION - 3 attempts allowed
    while (attempts < 3)
    {
        printf("Enter PIN: ");
        fflush(stdout);
        scanf("%d", &inputPin);

        if (inputPin == client.pin)
        {
            authenticated = 1;
            printf("PIN verified. Access granted.\n");
            printf("Welcome %s %s\n", client.firstName, client.lastName);
            break;
        }
        else
        {
            attempts++;
            if (attempts < 3) {
                printf("Incorrect PIN. You have %d attempts remaining.\n", 3 - attempts);
            } else {
                printf("Incorrect PIN. Too many incorrect attempts. Access denied.\n");
            }
        }
    }

    if (authenticated == 0)
    {
        return;
    }

    // Display current balance
    printf("Account: %d | %.2f\n", client.acctnum, client.balance);

    // Request transaction type
    printf("%s", "Enter choice ");
    printf("%s","\n1-Deposit");
    printf("%s","\n2-Withdrawal\n");
    printf("%s", "3-Balance Enquiry\n:");
    scanf("%d", &option);
    
    switch (option) 
    {
    case 1:
        printf("%s", "Enter deposit amount: ");
        scanf("%lf", &transaction);
        if (transaction <= 0) {
            printf("Invalid deposit amount! Please enter a positive number.\n");
        } else {
            client.balance += transaction; 
            fseek(credit_data, -(long)sizeof(struct clientData), SEEK_CUR); 
            fwrite(&client, sizeof(struct clientData), 1, credit_data); 
            printf("Deposit successful\n"); 
            // Display updated balance
            printf("Updated Balance: %.2f\n", client.balance);
        }
        break;
        
    case 2:
        printf("%s", "Enter withdrawal amount: ");
        scanf("%lf", &transaction);
        if (transaction > client.balance) {
            printf("Insufficient balance! You only have %f.\n", client.balance);
        } else {
            client.balance -= transaction; 
            fseek(credit_data, -(long)sizeof(struct clientData), SEEK_CUR); 
            fwrite(&client, sizeof(struct clientData), 1, credit_data); 
            printf("Withdrawal successful\n");
            printf("Please take your cash.\n");
            // Display updated balance
            printf("Updated Balance: %.2f\n", client.balance);
        }
        break;
    case 3:
        printf("\n===== BALANCE ENQUIRY =====\n");
        printf("Account Number : %d\n", client.acctnum);
        printf("Name           : %s %s\n", client.firstName, client.lastName);
        printf("Current Balance: %.2f\n", client.balance);
        printf("===========================\n");
        break;
    default:
        printf("Invalid choice!\n");
        break;
    }

    
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *credit_data)
{
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", 0, 0}; // blank client (including pin)
    unsigned int accountNum;                        // account number

    // obtain number of account to delete
    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(credit_data, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, credit_data);
    // display error if record does not exist
    if (client.acctnum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        // move file pointer to correct record in file
        fseek(credit_data, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // replace existing record with blank record
        fwrite(&blankClient, sizeof(struct clientData), 1, credit_data);
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *credit_data)
{
    // create clientData with default information - memset to ensure all zeros
    struct clientData client;
    memset(&client, 0, sizeof(struct clientData));
    unsigned int accountNum; // account number

    // obtain number of account to create
    printf("%s", "Enter new account number ( 1 - 100 ):\n ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(credit_data, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, credit_data);
    // display error if account already exists
    if (client.acctnum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctnum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        memset(&client, 0, sizeof(struct clientData)); // Reset again before filling
        
        printf("%s", "Enter firstname :");
        scanf("%9s", client.firstName);
        printf("%s", "Enter lastname :");
        scanf("%14s", client.lastName);
        printf("%s", "Enter balance :");
        scanf("%lf", &client.balance);
        printf("%s", "Enter pin :");
        scanf("%d", &client.pin);

        client.acctnum = accountNum;
        // move file pointer to correct record in file
        fseek(credit_data, (client.acctnum - 1) * sizeof(struct clientData), SEEK_SET);
        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, credit_data);
        fflush(credit_data);  // Ensure data is written to disk
    } // end else
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    // display available options
    printf("%s", "\nEnter your choice\n"
                 "1 - Transaction\n"
                 "2 - Add a new account\n"
                 "3 - Delete an account\n"
                 "4 - Display all account\n"
                 "5 - End program \n:");

     scanf("%u", &menuChoice); // receive choice from user
    return menuChoice;
} // end function enterChoice

// display text file
void displayTextFile(void)
{
    FILE *readPtr; // accounts.txt file pointer
    char line[100]; // buffer to store each line
    
    // fopen opens the file; exits if file cannot be opened
    if ((readPtr = fopen("accounts.txt", "r")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        // read and display each line from accounts.txt
        printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
        printf("----------------------------------------------\n");
        while (fgets(line, sizeof(line), readPtr) != NULL)
        {
            printf("%s", line);
        } // end while
        
        fclose(readPtr); // fclose closes the file
    } // end else
} // end function displayTextFile

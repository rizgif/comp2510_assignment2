#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    DOMESTIC,
    INTERNATIONAL
} StudentType;

typedef struct DomesticStudent
{
    char firstName[50];
    char lastName[50];
    char month[4];
    int day;
    int year;
    char gpaStr[10]; // Change gpa to gpaStr
    char status;
} DomesticStudent;

typedef struct InternationalStudent
{
    char firstName[50];
    char lastName[50];
    char month[4];
    int day;
    int year;
    char gpaStr[10]; // Change gpa to gpaStr
    char status;
    int toefl;
} InternationalStudent;

// Structure to represent a node in a linked list of students.
typedef struct StudentNode
{
    StudentType type;
    union
    {
        DomesticStudent domestic;
        InternationalStudent international;
    } student;
    struct StudentNode *next;
} StudentNode;

// Function to handle errors by writing to an output file and exiting.
void handleError(const char *errorMessage)
{
    FILE *errorFile = fopen("output.txt", "w"); // Open a file for writing errors.
    if (errorFile == NULL)
    {
        fprintf(stderr, "Error: Unable to open error output file\n");
        exit(EXIT_FAILURE);
    }

    fprintf(errorFile, "%s\n", errorMessage); // Write the error message to the file.
    fclose(errorFile);                        // Close the file.

    exit(EXIT_FAILURE); // Exit the program.
}

int monthToNumber(const char *month)
{
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++)
    {
        if (strcmp(months[i], month) == 0)
            return i + 1;
    }
    return -1;
}

// Convert month number back to string for output.
const char *numberToMonth(int num)
{
    const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    if (num >= 1 && num <= 12)
        return months[num - 1];
    return NULL;
}

int checkMonthCapital(const char *month, FILE *output)
{
    if (month[0] < 'A' || month[0] > 'Z')
    {
        handleError("Error: The first letter of the month must be capitalized.\n");
        return 0;
    }
    return 1;
}

// Function to compare two students
int compareStudents(const StudentNode *a, const StudentNode *b)
{

    // Compare year of birth, earliest comes first
    int yearA = (a->type == DOMESTIC) ? a->student.domestic.year : a->student.international.year;
    int yearB = (b->type == DOMESTIC) ? b->student.domestic.year : b->student.international.year;
    if (yearA != yearB)
        return yearA - yearB;

    // Compare month of birth, earliest comes first
    int monthA = monthToNumber((a->type == DOMESTIC) ? a->student.domestic.month : a->student.international.month);
    int monthB = monthToNumber((b->type == DOMESTIC) ? b->student.domestic.month : b->student.international.month);
    if (monthA != monthB)
        return monthA - monthB;
    // Compare day of birth, earliest comes first
    int dayA = (a->type == DOMESTIC) ? a->student.domestic.day : a->student.international.day;
    int dayB = (b->type == DOMESTIC) ? b->student.domestic.day : b->student.international.day;
    if (dayA != dayB)
        return dayA - dayB;

    // Compare last name, alphabetical order
    int lastNameComp = strcmp((a->type == DOMESTIC) ? a->student.domestic.lastName : a->student.international.lastName,
                              (b->type == DOMESTIC) ? b->student.domestic.lastName : b->student.international.lastName);
    if (lastNameComp != 0)
        return lastNameComp;

    // Compare first name, alphbetical order
    int firstNameComp = strcmp((a->type == DOMESTIC) ? a->student.domestic.firstName : a->student.international.firstName,
                               (b->type == DOMESTIC) ? b->student.domestic.firstName : b->student.international.firstName);
    if (firstNameComp != 0)
        return firstNameComp;

    // Compare GPA, highest comes first
    const char *gpaStrA = (a->type == DOMESTIC) ? a->student.domestic.gpaStr : a->student.international.gpaStr;
    const char *gpaStrB = (b->type == DOMESTIC) ? b->student.domestic.gpaStr : b->student.international.gpaStr;

    float gpaA = atof(gpaStrA);
    float gpaB = atof(gpaStrB);

    if (gpaA != gpaB)
        return (gpaA > gpaB) ? -1 : 1;

    // Compare TOEFL scores, highest comes first
    if (a->type == INTERNATIONAL && b->type == INTERNATIONAL)
    {
        if (a->student.international.toefl != b->student.international.toefl)
            return a->student.international.toefl - b->student.international.toefl;
    }

    // Domestic students come before international students
    if (a->type != b->type)
        return (a->type == DOMESTIC) ? -1 : 1;

    return 0; // If all else is equal, consider them equivalent
}

// Function to read students from a file and create a linked list
StudentNode *readStudentsFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Unable to open input file");
        return NULL;
    }

    StudentNode *head = NULL, *tail = NULL;
    char line[200];

    while (fgets(line, sizeof(line), file))
    {
        char firstName[50], lastName[50];
        int day, year;
        char month[4];
        int toefl = -1;
        char gpaStr[10];
        char typeChar;
        StudentType type;
        int endPosition = 0;
        int scanned, tokenCount = 0;
        char *token, buffer[200];
        char extraChar;

        strcpy(buffer, line);
        token = strtok(buffer, " \t\n");
        while (token != NULL)
        {
            tokenCount++;
            token = strtok(NULL, " \t\n");
        }

        // Parse the line
        scanned = sscanf(line, "%49s %49s %3s-%d-%d %9s %c %d %c",
                         firstName, lastName, month, &day, &year, gpaStr, &typeChar, &toefl, &extraChar);

        // Convert typeChar to StudentType
        type = (typeChar == 'D') ? DOMESTIC : (typeChar == 'I') ? INTERNATIONAL
                                                                : -1;

        // Check if Student has correct number of fields (more than 6)
        if (scanned < 7)
        {
            handleError("Error: Incomplete student record found");
            continue;
        }

        // Check if month is valid
        int monthNumber = monthToNumber(month);
        if (monthNumber < 1 || monthNumber > 12)
        {
            handleError("Error: Invalid month in date of birth");
            continue;
        }

        // Check is GPA is a valid value
        float gpa = atof(gpaStr); // Convert GPA string to float
        if (gpa < 0.0 || gpa > 4.30001)
        {
            handleError("Error: GPA out of valid range (0.0 to 4.3)");
            continue;
        }

        // Check if year is a valid value
        if (year < 1950 || year > 2010)
        {
            handleError("Error: Year of birth out of valid range (1950 to 2010)");
            continue;
        }

        // Check if toefl is a valid value
        if (type == INTERNATIONAL && (toefl < 0 || toefl > 120))
        {
            handleError("Error: Invalid TOEFL score");
            continue;
        }

        // Check if extra characters are present after expected fields
        if (scanned > 7 + (type == INTERNATIONAL ? 1 : 0) || scanned == EOF)
        {
            handleError("Error: Student record format is incorrect");
            continue;
        }

        // Check if extra characters are present after expected fields
        if (scanned > 8 + (type == INTERNATIONAL ? 1 : 0) || scanned == EOF)
        {
            handleError("Error: Student record format is incorrect");
            continue;
        }

        // Allocate memory for a new StudentNode
        StudentNode *newNode = (StudentNode *)malloc(sizeof(StudentNode));
        if (!newNode)
        {
            perror("Memory allocation for new student node failed");
            continue;
        }
        newNode->next = NULL;

        // Put data into student node
        if (type == DOMESTIC)
        {
            newNode->type = DOMESTIC;
            strcpy(newNode->student.domestic.firstName, firstName);
            strcpy(newNode->student.domestic.lastName, lastName);
            strcpy(newNode->student.domestic.gpaStr, gpaStr); // Copy GPA string
            newNode->student.domestic.day = day;
            strcpy(newNode->student.domestic.month, month);
            newNode->student.domestic.year = year;
            newNode->student.domestic.status = typeChar;
        }
        else if (type == INTERNATIONAL)
        {
            newNode->type = INTERNATIONAL;
            strcpy(newNode->student.international.firstName, firstName);
            strcpy(newNode->student.international.lastName, lastName);
            strcpy(newNode->student.international.gpaStr, gpaStr); // Copy GPA string
            newNode->student.international.toefl = toefl;
            newNode->student.international.day = day;
            strcpy(newNode->student.international.month, month);
            newNode->student.international.year = year;
            newNode->student.international.status = typeChar;
        }
        else
        {
            // If there is no student type, throw an error
            handleError("Error: Invalid student type");
            free(newNode);
            continue;
        }

        // Append the new node to the linked list
        if (tail)
        {
            tail->next = newNode;
        }
        else
        {
            head = newNode;
        }
        tail = newNode;
    }

    fclose(file);
    return head;
}

// Function to find the middle of the linked list
StudentNode *findMiddle(StudentNode *head)
{
    if (head == NULL || head->next == NULL)
    {
        return head;
    }

    StudentNode *slow = head;
    StudentNode *fast = head->next;

    while (fast != NULL && fast->next != NULL)
    {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

// Function to merge two sorted linked lists
StudentNode *mergeLists(StudentNode *a, StudentNode *b)
{
    if (a == NULL)
    {
        return b;
    }

    if (b == NULL)
    {
        return a;
    }

    if (compareStudents(a, b) <= 0)
    {
        a->next = mergeLists(a->next, b);
        return a;
    }
    else
    {
        b->next = mergeLists(a, b->next);
        return b;
    }
}

// Function to perform merge sort on a linked list
void mergeSort(StudentNode **headRef)
{
    StudentNode *head = *headRef;
    if (head == NULL || head->next == NULL)
    {
        return;
    }

    // Find the middle of the list
    StudentNode *mid = findMiddle(head);
    StudentNode *midNext = mid->next;

    // Split the list into two halves
    mid->next = NULL;

    // Recursively sort the two halves
    mergeSort(&head);
    mergeSort(&midNext);

    // Merge the sorted halves
    *headRef = mergeLists(head, midNext);
}

void writeStudentsToFile(const char *filename, StudentNode *students, char option)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        handleError("Error opening output file");
        return;
    }

    // Loop through the linked list of students
    while (students != NULL)
    {
        // Check the option and filter accordingly
        switch (option)
        {
        case 'D':
            // Write only domestic students
            if (students->type == DOMESTIC)
            {
                // Inside the 'D' case
                char gpaString[20];
                strcpy(gpaString, students->student.domestic.gpaStr);

                fprintf(file, "%s %s %s-%d-%d %s %c\n",
                        students->student.domestic.firstName,
                        students->student.domestic.lastName,
                        students->student.domestic.month,
                        students->student.domestic.day,
                        students->student.domestic.year,
                        gpaString,
                        students->student.domestic.status);

                // Convert GPA string back to float
                float gpa = atof(gpaString); // Use atof to convert string to float
            }
            break;

        case 'I':
            // Write only international students
            if (students->type == INTERNATIONAL)
            {
                // Inside the 'I' case
                char gpaString[20];
                strcpy(gpaString, students->student.international.gpaStr);

                fprintf(file, "%s %s %s-%d-%d %s %c %d\n",
                        students->student.international.firstName,
                        students->student.international.lastName,
                        students->student.international.month,
                        students->student.international.day,
                        students->student.international.year,
                        gpaString,
                        students->student.international.status,
                        students->student.international.toefl);

                // Convert GPA string back to float
                float gpa = atof(gpaString); // Use atof to convert string to float
            }
            break;
        case 'A':
            // Write all students
            if (students->type == DOMESTIC)
            {
                // Inside the 'D' case
                char gpaString[20];
                strcpy(gpaString, students->student.domestic.gpaStr);

                fprintf(file, "%s %s %s-%d-%d %s %c\n",
                        students->student.domestic.firstName,
                        students->student.domestic.lastName,
                        students->student.domestic.month,
                        students->student.domestic.day,
                        students->student.domestic.year,
                        gpaString,
                        students->student.domestic.status);

                // Convert GPA string back to float
                float gpa = atof(gpaString); // Use atof to convert string to float
            }
            else if (students->type == INTERNATIONAL)
            {
                // Inside the 'I' case
                char gpaString[20];
                strcpy(gpaString, students->student.international.gpaStr);

                fprintf(file, "%s %s %s-%d-%d %s %c %d\n",
                        students->student.international.firstName,
                        students->student.international.lastName,
                        students->student.international.month,
                        students->student.international.day,
                        students->student.international.year,
                        gpaString,
                        students->student.international.status,
                        students->student.international.toefl);

                // Convert GPA string back to float
                float gpa = atof(gpaString); // Use atof to convert string to float
            }
            break;
        default:
            fprintf(stderr, "Invalid option: %c\n", option);
            break;
        }

        // Move to the next node in the linked list
        students = students->next;
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    // Check for the correct number of arguments
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <inputfile> <outputfile> <option>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *inputFileName = argv[1];
    const char *outputFileName = argv[2];
    int option = atoi(argv[3]); // Convert option to an integer

    // Read students from the file
    StudentNode *students = readStudentsFromFile(inputFileName);

    // Check if the file was successfully read
    if (students == NULL)
    {
        handleError("Error reading students from file");
        return EXIT_FAILURE;
    }

    // Sort the students using merge sort
    mergeSort(&students);

    // Write students to the output file based on the specified option
    switch (option)
    {
    case 1:
        writeStudentsToFile(outputFileName, students, 'D');
        break;
    case 2:
        writeStudentsToFile(outputFileName, students, 'I');
        break;
    case 3:
        writeStudentsToFile(outputFileName, students, 'A');
        break;
    default:
        fprintf(stderr, "Invalid option: %d\n", option);
        break;
    }

    // Free the allocated memory for the linked list
    StudentNode *current = students;
    while (current != NULL)
    {
        StudentNode *temp = current;
        current = current->next;
        free(temp);
    }

    char *ANum = "A00874466_A01174802_A01357980";
    FILE *outputFile = fopen(ANum, "w");
    if (outputFile == NULL)
    {
        printf("Failed to create the output file.\n");
        return 1;
    }
    fclose(outputFile);

    return EXIT_SUCCESS;
}

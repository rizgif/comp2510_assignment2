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
    float gpa;
    char status;
    int day;
    char month[4];
    int year;
} DomesticStudent;

typedef struct InternationalStudent
{
    char firstName[50];
    char lastName[50];
    float gpa;
    char status;
    int toefl;
    int day;
    char month[4];
    int year;
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
    float gpaA = (a->type == DOMESTIC) ? a->student.domestic.gpa : a->student.international.gpa;
    float gpaB = (b->type == DOMESTIC) ? b->student.domestic.gpa : b->student.international.gpa;
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
        float gpa;
        char typeChar;
        StudentType type;

        // Parse the line
        int scanned = sscanf(line, "%49s %49s %3s-%d-%d %f %c %d",
                             firstName, lastName, month, &day, &year, &gpa, &typeChar, &toefl);

        type = (typeChar == 'D') ? DOMESTIC : INTERNATIONAL;

        // Check if Student has correct number of fields (more than 6)
        if (scanned < 6)
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

        // Check is GPA is a valif value
        if (gpa < 0.0 || gpa > 4.3)
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

        // Check if the number of fields is correct for domestic student
        if (type == DOMESTIC && scanned != 6)
        {
            handleError("Error: Incorrect record for domestic student");
            continue;
        }

        // Check if the number of fields is correct for international student
        if (type == INTERNATIONAL && scanned != 7)
        {
            handleError("Error: Incorrect record for international student");
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
            newNode->student.domestic.gpa = gpa;
            newNode->student.domestic.day = day;
            strcpy(newNode->student.domestic.month, month);
            newNode->student.domestic.year = year;
        }
        else if (type == INTERNATIONAL)
        {
            newNode->type = INTERNATIONAL;
            strcpy(newNode->student.international.firstName, firstName);
            strcpy(newNode->student.international.lastName, lastName);
            newNode->student.international.gpa = gpa;
            newNode->student.international.toefl = toefl;
            newNode->student.international.day = day;
            strcpy(newNode->student.international.month, month);
            newNode->student.international.year = year;
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

Student *mergeSort(Student *head, int (*comparator)(Student *, Student *));
void writeStudentsToFile(const char *filename, Student *students, char option);

// Main function.
int main(int argc, char *argv[])
{

    // Check for the correct number of arguments
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <inputfile> <outputfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *inputFileName = argv[1];
    const char *outputFileName = argv[2];

    // Read students from the file
    Student *students = readStudentsFromFile(inputFileName);
    if (students == NULL)
    {
        handleError("Error reading students from file");
        return EXIT_FAILURE;
    }

    char *ANum = "A00874466_A01174802";
    FILE *outputFile = fopen(ANum, "w");
    if (outputFile == NULL)
    {
        printf("Failed to create the output file.\n");
        return 1;
    }
    fclose(outputFile);
}

// Implement error handling.
void handleError(const char *errorMessage)
{
    FILE *file = fopen("output.txt", "w"); // Open output.txt for writing
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open output file\n");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Error: %s\n", errorMessage); // Write the error message to output.txt
    fclose(file);                               // Close the file

    exit(EXIT_FAILURE); // Exit the program
}

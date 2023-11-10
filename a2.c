#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    float gpa;
    char status;
} DomesticStudent;

typedef struct InternationalStudent
{
    char firstName[50];
    char lastName[50];
    char month[4];
    int day;
    int year;
    float gpa;
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

// Function to get the year of birth for domestic students
int getDomesticYear(const DomesticStudent *domestic)
{
    return domestic->year;
}

// Function to get the year of birth for international students
int getInternationalYear(const InternationalStudent *international)
{
    return international->year;
}

// Similarly, create separate functions for other attributes...

// Function to get the year of birth
int getYear(const StudentNode *student)
{
    return (student->type == DOMESTIC) ? getDomesticYear(&student->student.domestic) : getInternationalYear(&student->student.international);
}

// Function to get the month of birth
const char *getMonth(const StudentNode *student)
{
    return (student->type == DOMESTIC) ? student->student.domestic.month : student->student.international.month;
}

// Function to get the day of birth
int getDay(const StudentNode *student)
{
    return (student->type == DOMESTIC) ? student->student.domestic.day : student->student.international.day;
}

// Function to get the last name
const char *getLastName(const StudentNode *student)
{
    return (student->type == DOMESTIC) ? student->student.domestic.lastName : student->student.international.lastName;
}

// Function to get the first name
const char *getFirstName(const StudentNode *student)
{
    return (student->type == DOMESTIC) ? student->student.domestic.firstName : student->student.international.firstName;
}

// Function to get the GPA
float getGPA(const StudentNode *student)
{
    return (student->type == DOMESTIC) ? student->student.domestic.gpa : student->student.international.gpa;
}

// Function to get the TOEFL score (for international students)
int getTOEFL(const StudentNode *student)
{
    return (student->type == INTERNATIONAL) ? student->student.international.toefl : 0; // Assuming default value for domestic students
}

// Function to check if a student is domestic
int isDomestic(const StudentNode *student)
{
    return (student->type == DOMESTIC) ? 1 : 0;
}

// Function to check if both students are international
int areBothInternational(const StudentNode *a, const StudentNode *b)
{
    return (a->type == INTERNATIONAL && b->type == INTERNATIONAL) ? 1 : 0;
}

// Function to compare two students
int compareStudents(const StudentNode *a, const StudentNode *b)
{
    int priorityOrder[] = {
        // Priority order of attributes
        0, // Year of birth
        1, // Month of birth
        2, // Day of birth
        3, // Last name
        4, // First name
        5, // GPA
        6, // TOEFL score (for international students)
        7  // Type (Domestic vs International)
    };

    for (int i = 0; i < sizeof(priorityOrder) / sizeof(priorityOrder[0]); ++i)
    {
        int result = 0; // Initialize result to consider them equivalent
        const char *lastNameA, *firstNameA, *lastNameB, *firstNameB;

        switch (priorityOrder[i])
        {
        case 0: // Year of birth
            result = getYear(a) - getYear(b);
            break;
        case 1: // Month of birth
            result = monthToNumber(getMonth(a)) - monthToNumber(getMonth(b));
            break;
        case 2: // Day of birth
            result = getDay(a) - getDay(b);
            break;
        case 3: // Last name
            lastNameA = getLastName(a);
            lastNameB = getLastName(b);
            result = strcmp(lastNameA, lastNameB);
            break;
        case 4: // First name
            firstNameA = getFirstName(a);
            firstNameB = getFirstName(b);
            result = strcmp(firstNameA, firstNameB);
            break;
        case 5: // GPA
            result = getGPA(a) - getGPA(b);
            break;
        case 6: // TOEFL score (for international students)
            if (areBothInternational(a, b))
            {
                result = getTOEFL(a) - getTOEFL(b);
            }
            break;
        case 7: // Type (Domestic vs International)
            result = isDomestic(a) ? -1 : 1;
            break;
        }

        if (result != 0)
            return result; // If the current attribute is enough to determine the order, return the result
    }

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

        // Check if the number of fields is correct for domestic student
        if (type == DOMESTIC && scanned != 7)
        {
            handleError("Error: Incorrect record for domestic student");
            continue;
        }

        // Check if the number of fields is correct for international student
        if (type == INTERNATIONAL && scanned != 8)
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
            newNode->student.domestic.status = typeChar;
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

StudentNode *mergeSort(StudentNode *head, int (*comparator)(StudentNode *, StudentNode *));

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
                fprintf(file, "%s %s %s-%d-%d %.2f %c\n",
                        students->student.domestic.firstName,
                        students->student.domestic.lastName,
                        students->student.domestic.month,
                        students->student.domestic.day,
                        students->student.domestic.year,
                        students->student.domestic.gpa,
                        students->student.domestic.status);
            }
            break;
        case 'I':
            // Write only international students
            if (students->type == INTERNATIONAL)
            {
                fprintf(file, "%s %s %s-%d-%d %.2f %c %d\n",
                        students->student.international.firstName,
                        students->student.international.lastName,
                        students->student.international.month,
                        students->student.international.day,
                        students->student.international.year,
                        students->student.international.gpa,
                        students->student.international.status,
                        students->student.international.toefl);
            }
            break;
        case 'A':
            // Write all students
            if (students->type == DOMESTIC)
            {
                fprintf(file, "%s %s %s-%d-%d %.2f %c\n",
                        students->student.domestic.firstName,
                        students->student.domestic.lastName,
                        students->student.domestic.month,
                        students->student.domestic.day,
                        students->student.domestic.year,
                        students->student.domestic.gpa,
                        students->student.domestic.status);
            } else if (students->type == INTERNATIONAL)
            {
                fprintf(file, "%s %s %s-%d-%d %.2f %c %d\n",
                        students->student.international.firstName,
                        students->student.international.lastName,
                        students->student.international.month,
                        students->student.international.day,
                        students->student.international.year,
                        students->student.international.gpa,
                        students->student.international.status,
                        students->student.international.toefl);
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

    return EXIT_SUCCESS;

    char *ANum = "A00874466_A01174802_A01357980";
    FILE *outputFile = fopen(ANum, "w");
    if (outputFile == NULL)
    {
        printf("Failed to create the output file.\n");
        return 1;
    }
    fclose(outputFile);
}



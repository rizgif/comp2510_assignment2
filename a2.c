#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for a domestic student.
typedef struct DomesticStudent
{
    char firstName[50];
    char lastName[50];
    float gpa;
    Date dob;
} DomesticStudent;

// Structure for an international student.
typedef struct InternationalStudent
{
    char firstName[50];
    char lastName[50];
    float gpa;
    int toefl;
    Date dob;
} InternationalStudent;

// Student structure (international or domestic).
typedef struct Student
{
    char firstName[50];
    char lastName[50];
    float gpa;
    Date dob;
    int toefl;
    char type; // 'D' for domestic, 'I' for international
} Student;

// Structure to represent a node in a linked list of students.
typedef struct StudentNode
{
    DomesticStudent *domesticStudent;
    InternationalStudent *internationalStudent;
    struct StudentNode *next;
} StudentNode;

// Structure to hold date information.
typedef struct Date
{
    int day;
    int month;
    int year;
} Date;

// Takes Date from input and returns a Date struct.
Date parseDate(const char *dateStr)
{
    Date date;
    char monthStr[4];
    sscanf(dateStr, "%s-%d-%d", monthStr, &date.day, &date.year);
    date.month = monthToNumber(monthStr);
    return date;
}

// Convert month string to number for sorting.
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

// Function to compare students for sorting.
int compareStudents(const void *a, const void *b)
{
    const Student *studentA = *(const Student **)a;
    const Student *studentB = *(const Student **)b;

    // Compare by year of birth
    if (studentA->dob.year != studentB->dob.year)
        return studentA->dob.year - studentB->dob.year;

    // Then by month of birth
    if (studentA->dob.month != studentB->dob.month)
        return studentA->dob.month - studentB->dob.month;

    // Then by day of birth
    if (studentA->dob.day != studentB->dob.day)
        return studentA->dob.day - studentB->dob.day;

    // Then by last name
    int lastNameComparison = strcmp(studentA->lastName, studentB->lastName);
    if (lastNameComparison != 0)
        return lastNameComparison;

    // Then by first name
    int firstNameComparison = strcmp(studentA->firstName, studentB->firstName);
    if (firstNameComparison != 0)
        return firstNameComparison;

    // Then by GPA (higher GPA first)
    double gpaDifference = studentB->gpa - studentA->gpa;
    if (fabs(gpaDifference) > 0.000001)
        return (gpaDifference > 0) ? 1 : -1;

    // If both students are international, compare by TOEFL score
    if (studentA->type == 'I' && studentB->type == 'I')
    {
        if (studentA->toefl != studentB->toefl)
            return studentB->toefl - studentA->toefl;
    }

    // If one student is domestic and the other is international, Domestic comes first
    if (studentA->type != studentB->type)
        return (studentA->type == 'D') ? -1 : 1;

    return 0; // If all else is equal
}

// Function prototypes.
Student *readStudentsFromFile(const char *filename);
Student *mergeSort(Student *head, int (*comparator)(Student *, Student *));
void writeStudentsToFile(const char *filename, Student *students, char option);
void handleError(const char *errorMessage);

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

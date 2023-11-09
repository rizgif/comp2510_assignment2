#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for a domestic student
typedef struct DomesticStudent
{
    char firstName[50];
    char lastName[50];
    float gpa;
    Date dob; // Date of birth
} DomesticStudent;

// Structure for an international student
typedef struct InternationalStudent
{
    char firstName[50];
    char lastName[50];
    float gpa;
    int toefl;
    Date dob; // Date of birth
} InternationalStudent;

// Student structure (international or domestic)
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
typedef struct StudentNode {
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

// Function to convert month string to number for sorting
int monthToNumber(const char *month)
{
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++)
    {
        if (strcmp(months[i], month) == 0)
            return i + 1;
    }
    return -1; // Return -1 if month is not found
}

// Helper function to convert month number back to string for output
const char* numberToMonth(int num) {
    const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    if (num >= 1 && num <= 12)
        return months[num - 1];
    return NULL; // Return NULL if month number is out of range
}

// Comparator function for sorting
int compareStudents(const void *a, const void *b) {
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

    // Then by GPA
    if (studentA->gpa != studentB->gpa)
        return (studentB->gpa - studentA->gpa > 0) ? 1 : -1; // higher GPA comes first

    // Then by TOEFL score if both students are international
    if (studentA->type == 'I' && studentB->type == 'I') {
        if (studentA->toefl != studentB->toefl)
            return studentB->toefl - studentA->toefl; // higher TOEFL comes first
    }

    // If one student is domestic and the other is international
    if (studentA->type != studentB->type)
        return (studentA->type == 'D') ? -1 : 1; // Domestic comes first

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
}

// Implement error handling.
void handleError(const char *errorMessage)
{
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct for student data.
typedef struct Student {
    char firstName[50];
    char lastName[50];
    char birthMonth[4];
    int birthDay;
    int birthYear;
    float gpa;
    int toeflScore;
    char status; 
    struct Student* next;
} Student;

// Function prototypes.
Student* readStudentsFromFile(const char* filename);
Student* mergeSort(Student* head, int (*comparator)(Student*, Student*));
void writeStudentsToFile(const char* filename, Student* students, char option);
int compareStudents(Student* a, Student* b);
void handleError(const char* errorMessage);

// Main function.
int main(int argc, char* argv[]) {


}

// Implement error handling.
void handleError(const char* errorMessage) {
   
}
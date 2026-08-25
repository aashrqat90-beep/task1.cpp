// ==========================================================
// Program: Student Semester GPA & Cumulative CGPA Calculator
// Language: C++ (C++11 or higher)
// Description: Takes input for number of courses, grades,
//              and credit hours, computes total credits,
//              total grade points, Semester GPA, and overall CGPA.
// ==========================================================

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// Structure to store information for each course
struct Course {
    string name;
    double grade;       // Grade points (e.g. 4.0, 3.7, 3.3, 3.0, etc.)
    double creditHours; // Credit hours for the course
    double gradePoints; // (grade * creditHours)
};

int main() {
    cout << "===============================================
";
    cout << "      STUDENT GPA & CGPA CALCULATOR (C++)      
";
    cout << "===============================================

";

    // --- Step 1: Check for Previous Cumulative History (Optional for CGPA) ---
    char hasPrevious;
    double previousCGPA = 0.0;
    double previousCredits = 0.0;
    double previousPoints = 0.0;

    cout << "Do you have previous semester records? (y/n): ";
    cin >> hasPrevious;

    if (hasPrevious == 'y' || hasPrevious == 'Y') {
        cout << "Enter your previous CGPA (e.g. 3.45): ";
        cin >> previousCGPA;
        cout << "Enter your total previous completed credit hours: ";
        cin >> previousCredits;
        previousPoints = previousCGPA * previousCredits;
    }

    // --- Step 2: Input Number of Courses ---
    int numCourses;
    cout << "
Enter the number of courses taken this semester: ";
    cin >> numCourses;

    while (numCourses <= 0) {
        cout << "Invalid number! Please enter at least 1 course: ";
        cin >> numCourses;
    }

    vector<Course> courses(numCourses);
    double semesterCredits = 0.0;
    double semesterGradePoints = 0.0;

    // --- Step 3: Input Grade and Credit Hours for Each Course ---
    cout << "
-----------------------------------------------
";
    cout << "  Please enter details for each course:
";
    cout << "-----------------------------------------------
";

    for (int i = 0; i < numCourses; i++) {
        cout << "
--- Course #" << (i + 1) << " ---
";
        cout << "Course Name / Code (e.g. CS101): ";
        cin >> courses[i].name;

        cout << "Grade Points (e.g. 4.0=A, 3.7=A-, 3.0=B, 2.0=C): ";
        cin >> courses[i].grade;

        cout << "Credit Hours (e.g. 3): ";
        cin >> courses[i].creditHours;

        // Calculate Grade Points for this course = (Grade * Credit Hours)
        courses[i].gradePoints = courses[i].grade * courses[i].creditHours;

        // Accumulate totals
        semesterCredits += courses[i].creditHours;
        semesterGradePoints += courses[i].gradePoints;
    }

    // --- Step 4: Compute Semester GPA & Cumulative CGPA ---
    double semesterGPA = (semesterCredits > 0) ? (semesterGradePoints / semesterCredits) : 0.0;

    double totalCumulativeCredits = previousCredits + semesterCredits;
    double totalCumulativePoints = previousPoints + semesterGradePoints;
    double finalCGPA = (totalCumulativeCredits > 0) ? (totalCumulativePoints / totalCumulativeCredits) : semesterGPA;

    // --- Step 5: Display Formatted Results ---
    cout << "

=================================================================
";
    cout << "                    SEMESTER GRADE REPORT                        
";
    cout << "=================================================================
";
    cout << left << setw(8)  << "No."
         << setw(16) << "Course Name"
         << setw(12) << "Grade"
         << setw(14) << "Credit Hrs"
         << setw(14) << "Grade Points" << "
";
    cout << "-----------------------------------------------------------------
";

    cout << fixed << setprecision(2);
    for (int i = 0; i < numCourses; i++) {
        cout << left << setw(8)  << (i + 1)
             << setw(16) << courses[i].name
             << setw(12) << courses[i].grade
             << setw(14) << courses[i].creditHours
             << setw(14) << courses[i].gradePoints << "
";
    }

    cout << "=================================================================
";
    cout << "                      CALCULATION SUMMARY                        
";
    cout << "=================================================================
";
    cout << "Semester Total Credits       : " << semesterCredits << " hrs
";
    cout << "Semester Total Grade Points  : " << semesterGradePoints << " pts
";
    cout << "Semester GPA                 : " << semesterGPA << " / 4.00
";
    cout << "-----------------------------------------------------------------
";

    if (hasPrevious == 'y' || hasPrevious == 'Y') {
        cout << "Previous CGPA                : " << previousCGPA << "
";
        cout << "Previous Completed Credits   : " << previousCredits << " hrs
";
        cout << "Total Cumulative Credits     : " << totalCumulativeCredits << " hrs
";
        cout << "Total Cumulative Points      : " << totalCumulativePoints << " pts
";
    }

    cout << ">>> FINAL OVERALL CGPA       : " << finalCGPA << " / 4.00 <<<
";
    cout << "=================================================================
";

    // Academic Standing
    cout << "Academic Standing: ";
    if (finalCGPA >= 3.50) cout << "Excellent (First Class Honors / Dean's List)
";
    else if (finalCGPA >= 3.00) cout << "Very Good
";
    else if (finalCGPA >= 2.50) cout << "Good
";
    else if (finalCGPA >= 2.00) cout << "Pass / Satisfactory
";
    else cout << "Academic Warning (Probation)
";

    cout << "=================================================================
";

    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <queue>
#include <fstream>
#include <sstream>

using namespace std;


struct Course {
    string name;
    string syllabus;
    int creditHours;
    string instructor;
    vector<string> prerequisites;
};

struct Grade {
    string courseName;
    string semester;
    double value;
};

struct Student {
    string id;
    string name;
    string password;
    queue<string> registeredCourses;
    vector<Grade> grades;
};

struct Admin {
    string username;
    string password;
};


map<string, Course> courses;
map<string, Student> students;
Admin admin = { "admin", "admin" };


double convertToGPA(double grade) {
    if (grade >= 90) return 4.0;
    else if (grade >= 85) return 3.7;
    else if (grade >= 80) return 3.3;
    else if (grade >= 75) return 3.0;
    else if (grade >= 70) return 2.7;
    else if (grade >= 65) return 2.3;
    else if (grade >= 60) return 2.0;
    else if (grade >= 55) return 1.7;
    else if (grade >= 50) return 1.0;
    else return 0.0;
}

bool checkPrerequisites(const Student& student, const Course& course) {
    for (const string& pre : course.prerequisites) {
        bool found = false;
        for (const Grade& g : student.grades) {
            if (g.courseName == pre && g.value >= 50)
                found = true;
        }
        if (!found) return false;
    }
    return true;
}

bool isCourseAlreadyRegistered(const Student& student, const string& courseName) {
    queue<string> temp = student.registeredCourses;
    while (!temp.empty()) {
        if (temp.front() == courseName) return true;
        temp.pop();
    }
    return false;
}


void saveCourses() {
    ofstream fout("courses.txt");
    for (const auto& pair : courses) {
        const Course& c = pair.second;
        fout << "name: " << c.name << '\n';
        fout << "syllabus: " << c.syllabus << '\n';
        fout << "credits: " << c.creditHours << '\n';
        fout << "instructor: " << c.instructor << '\n';
        fout << "prerequisites: " << c.prerequisites.size() << '\n';
        for (const string& pre : c.prerequisites) fout << "- " << pre << '\n';
        fout << "==================================================\n";
    }
}

void loadCourses() {
    ifstream fin("courses.txt");
    if (!fin) {
        ofstream create("courses.txt");
        return;
    }
    while (fin) {
        Course c;
        string line;
        getline(fin, line);
        if (line.empty()) break;
        c.name = line.substr(6);

        getline(fin, line); c.syllabus = line.substr(10);
        getline(fin, line); c.creditHours = stoi(line.substr(9));
        getline(fin, line); c.instructor = line.substr(12);

        getline(fin, line); int n = stoi(line.substr(15));
        c.prerequisites.clear();
        for (int i = 0; i < n; ++i) {
            getline(fin, line);
            c.prerequisites.push_back(line.substr(2));
        }

        courses[c.name] = c;
        getline(fin, line);
    }
}

void saveStudents() {
    ofstream fout("students.txt");
    for (const auto& pair : students) {
        const Student& s = pair.second;
        fout << "id: " << s.id << '\n';
        fout << "name: " << s.name << '\n';
        fout << "password: " << s.password << '\n';
        fout << "registered: " << s.registeredCourses.size() << '\n';
        queue<string> temp = s.registeredCourses;
        while (!temp.empty()) {
            fout << "- " << temp.front() << '\n';
            temp.pop();
        }
        fout << "grades: " << s.grades.size() << '\n';
        for (const Grade& g : s.grades) {
            fout << "* " << g.courseName << '\n';
            fout << "semester: " << g.semester << '\n';
            fout << "grade: " << g.value << '\n';
        }
        fout << "==================================================\n";
    }
}

void loadStudents() {
    ifstream fin("students.txt");
    if (!fin) {
        ofstream create("students.txt");
        return;
    }
    while (fin) {
        Student s;
        string line;
        getline(fin, line);
        if (line.empty()) break;
        s.id = line.substr(4);

        getline(fin, line); s.name = line.substr(6);
        getline(fin, line); s.password = line.substr(10);

        getline(fin, line); int n = stoi(line.substr(11));
        for (int i = 0; i < n; ++i) {
            getline(fin, line);
            s.registeredCourses.push(line.substr(2));
        }

        getline(fin, line); int m = stoi(line.substr(8));
        for (int i = 0; i < m; ++i) {
            Grade g;
            getline(fin, line); g.courseName = line.substr(2);
            getline(fin, line); g.semester = line.substr(10);
            getline(fin, line); g.value = stod(line.substr(7));
            s.grades.push_back(g);
        }
        students[s.id] = s;
        getline(fin, line);
    }
}


void studentMenu(Student& student);

void studentSignUp() {
    Student s;
    cout << "Enter ID: ";
    cin >> s.id;

    if (students.count(s.id)) {
        cout << "Error: Student ID already exists!\n";
        return;
    }

    cout << "Enter Name: ";
    cin.ignore();
    getline(cin, s.name);
    cout << "Enter Password: ";
    cin >> s.password;
    students[s.id] = s;
    saveStudents();
    cout << "Sign up successful!\n";
    studentMenu(students[s.id]);
}

void studentSignIn() {
    string id, password;
    cout << "Enter ID: "; cin >> id;
    cout << "Enter Password: "; cin >> password;
    if (students.count(id) && students[id].password == password) {
        studentMenu(students[id]);
    }
    else {
        cout << "Invalid credentials!\n";
    }
}

void viewGrades(const Student& student) {
    cout << "Grades:\n";
    for (const Grade& g : student.grades) {
        cout << g.courseName << " | " << g.semester << " | Grade: " << g.value << endl;
    }
}

void viewRegisteredCourses(const Student& student) {
    queue<string> temp = student.registeredCourses;
    cout << "Registered Courses:\n";
    if (temp.empty()) {
        cout << "No courses registered yet.\n";
        return;
    }
    while (!temp.empty()) {
        cout << "- " << temp.front() << endl;
        temp.pop();
    }
}

void registerCourse(Student& student) {
    cout << "Available Courses:\n";
    for (auto& coursePair : courses) {
        cout << "- " << coursePair.first << endl;
    }
    string courseName;
    cout << "Enter course name to register: ";
    cin.ignore(); getline(cin, courseName);
    if (courses.count(courseName)) {
        if (isCourseAlreadyRegistered(student, courseName)) {
            cout << "You already registered this course.\n";
            return;
        }
        if (checkPrerequisites(student, courses[courseName])) {
            student.registeredCourses.push(courseName);
            cout << "Course registered successfully!\n";
            saveStudents();
        }
        else {
            cout << "Missing prerequisites!\n";
        }
    }
    else {
        cout << "Course not found.\n";
    }
}

void checkStudentPrerequisites(const Student& student) {
    for (auto& coursePair : courses) {
        cout << coursePair.first << " prerequisites: ";
        for (const string& p : coursePair.second.prerequisites) cout << p << ", ";
        cout << endl;
    }
}

void generateReport(const Student& student) {
    cout << "\n==================================================\n";
    cout << "Academic Report for: " << student.name << " (" << student.id << ")\n";
    cout << "==================================================\n\n";

    if (student.grades.empty()) {
        cout << "No grades available yet.\n";
        return;
    }

    cout << "Course Name\t\tSemester\tGrade\tGPA (0-4)\n";
    cout << "--------------------------------------------------\n";

    double totalGPA = 0.0;
    int count = 0;

    for (const Grade& g : student.grades) {
        double courseGPA = convertToGPA(g.value);
        totalGPA += courseGPA;
        count++;

        cout << g.courseName << "\t\t";
        if (g.courseName.length() < 8) cout << "\t";
        cout << g.semester << "\t";
        cout << g.value << "\t";
        cout << courseGPA << endl;
    }

    double overallGPA = (count > 0) ? (totalGPA / count) : 0.0;

    cout << "\n--------------------------------------------------\n";
    cout << "Overall GPA (0-4 scale): " << overallGPA << endl;
    cout << "==================================================\n\n";
}

void studentMenu(Student& student) {
    int choice;
    do {
        cout << "\n--- Student Menu ---\n";
        cout << "1. Search Courses\n2. Register for Course\n3. View Registered Courses\n4. View Grades\n5. Check Prerequisites\n6. Make Report\n7. Logout\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            for (auto& coursePair : courses) cout << "- " << coursePair.first << endl;
            break;
        case 2: registerCourse(student); break;
        case 3: viewRegisteredCourses(student); break;
        case 4: viewGrades(student); break;
        case 5: checkStudentPrerequisites(student); break;
        case 6: generateReport(student); break;
        }
    } while (choice != 7);
}


void adminMenu();

void adminSignIn() {
    string username, password;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;
    if (username == "admin" && password == "admin") {
        adminMenu();
    }
    else {
        cout << "Wrong credentials!\n";
    }
}

void uploadCourseDescription() {
    Course c;
    cout << "Enter Course Name: "; cin.ignore(); getline(cin, c.name);
    cout << "Syllabus: "; getline(cin, c.syllabus);
    cout << "Credit Hours: "; cin >> c.creditHours;
    cout << "Instructor: "; cin.ignore(); getline(cin, c.instructor);
    int n;
    cout << "How many prerequisites? "; cin >> n;
    c.prerequisites.clear();
    cin.ignore();
    for (int i = 0; i < n; i++) {
        string p;
        cout << "Prerequisite " << (i + 1) << ": "; getline(cin, p);
        c.prerequisites.push_back(p);
    }
    courses[c.name] = c;
    saveCourses();
    cout << "Course uploaded.\n";
}

void setCoursePrerequisites() {
    string courseName;
    cout << "Enter course name: "; cin.ignore(); getline(cin, courseName);
    if (!courses.count(courseName)) {
        cout << "Course not found!\n";
        return;
    }
    Course& c = courses[courseName];
    int n;
    cout << "How many prerequisites? "; cin >> n;
    c.prerequisites.clear();
    cin.ignore();
    for (int i = 0; i < n; i++) {
        string p;
        cout << "Prerequisite " << (i + 1) << ": "; getline(cin, p);
        c.prerequisites.push_back(p);
    }
    saveCourses();
    cout << "Updated prerequisites.\n";
}

void manageGrades() {
    int choice;
    cout << "1. Add Grade\n2. Update Grade\n3. Display All Grades of Students\nChoice: ";
    cin >> choice;

    if (choice == 1) {
        string id;
        cout << "Enter student ID: "; cin >> id;
        if (!students.count(id)) {
            cout << "Student not found!\n";
            return;
        }

        string courseName;
        cout << "Course Name: "; cin.ignore(); getline(cin, courseName);

        if (!isCourseAlreadyRegistered(students[id], courseName)) {
            cout << "Error: student not registered in this course.\n";
            return;
        }

        Grade g;
        g.courseName = courseName;
        cout << "Semester: "; getline(cin, g.semester);
        cout << "Grade: "; cin >> g.value;
        students[id].grades.push_back(g);
        saveStudents();
        cout << "Grade added.\n";
    }
    else if (choice == 2) {
        string id;
        cout << "Enter student ID: "; cin >> id;
        if (!students.count(id)) {
            cout << "Student not found!\n";
            return;
        }
        string courseName;
        cout << "Enter course name: "; cin.ignore(); getline(cin, courseName);
        bool found = false;
        for (Grade& g : students[id].grades) {
            if (g.courseName == courseName) {
                cout << "Current grade: " << g.value << "\nEnter new grade: ";
                cin >> g.value;
                found = true;
                break;
            }
        }
        if (found) {
            saveStudents();
            cout << "Grade updated.\n";
        }
        else {
            cout << "Course not found for this student.\n";
        }
    }
    else if (choice == 3) {
        for (const auto& studentPair : students) {
            cout << "Student: " << studentPair.second.name << endl;
            for (const Grade& g : studentPair.second.grades) {
                cout << g.courseName << " - " << g.semester << " - Grade: " << g.value << endl;
            }
            cout << "-----------------------------------------\n";
        }
    }
}

void adminMenu() {
    int choice;
    do {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Upload Course Description\n2. Set Course Prerequisites\n3. Manage Grades\n4. Logout\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1: uploadCourseDescription(); break;
        case 2: setCoursePrerequisites(); break;
        case 3: manageGrades(); break;
        }
    } while (choice != 4);
}

int main() {
    loadCourses();
    loadStudents();

    int choice;
    do {
        cout << "--- Main Menu ---\n";
        cout << "1. Student Sign In\n2. Student Sign Up\n3. Admin Sign In\n4. Exit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1: studentSignIn(); break;
        case 2: studentSignUp(); break;
        case 3: adminSignIn(); break;
        }
    } while (choice != 4);

    return 0;
}
#include <iostream> 
#include <fstream>				// filestream - for reading CSV files from disk
#include <sstream>				// stringstream - for splitting CSV lines by comma
#include <chrono>				// high_resolution_clock - for timing operations in microseconds
#include <iomanip>				// setw(), setprecision(), left/fixed
using namespace std;

const int MAX_SIZE = 30050;		// largest Student dataset size
const int MAX_PROGRAMMES = 25;

// This is the fields of one student and one programme record together.
// IDs are shuffled, sort algorithm must sort it.
struct Programme {
	string ProgrammeCode;
	string ProgrammeName;
	string Faculty;
	int DurationYears;
};

struct Student {
	string StudentID;
	string FullName;
	string ProgrammeCode;
	int YearOfStudy;
	float CGPA;
	string ContactNumber;
};

// Functions Prototype
int menu();

string trimCR(string s); // trim character \r
string toLowerStr(string s); // to lowercase
void printHeader();
void printStudentRow(Student s);
void printProgrammeHeader();
void printProgrammeRow(Programme p);

void loadCSV(Student arr[], int& count, string filename);
void loadProgrammes(Programme arr[], int& count, string filename);
int findProgrammeIndex(Programme arr[], int count, string code);
bool isValidStudentIDFormat(string id);
bool isValidContactFormat(string contact);
bool isContactNumberTaken(Student arr[], int count, string contact);

int searchByID(Student arr[], int count, string id);
bool coreAddStudent(Student arr[], int& count, Student newStudent);
bool coreDeleteStudent(Student arr[], int& count, string id);
bool coreSearchByName(Student arr[], int count, string searchTerm);

void addStudent(Student arr[], int& count, Programme progs[], int progCount);
void deleteStudent(Student arr[], int& count);
void displayAll(Student arr[], int count);
void displayAllProgrammes(Programme arr[], int count);
void searchByIDMenu(Student arr[], int count);
void searchByName(Student arr[], int count);
void selectionSortByCGPA(Student arr[], int count, bool ascending);


// MENU FUNCTION


int menu() {
	int choice;
	cout << "\nWelcome to the Student Record Management System" << endl;
	cout << "1. Add Student" << endl;
	cout << "2. Delete Student" << endl;
	cout << "3. Search by Student ID" << endl;
	cout << "4. Search by Name" << endl;
	cout << "5. Display All Students" << endl;
	cout << "6. Display All Programmes" << endl;
	cout << "7. Sort by CGPA (Ascending)" << endl;
	cout << "8. Sort by CGPA (Descending)" << endl;
	cout << "0. Exit" << endl;
	cout << "Enter choice: ";
	cin >> choice;

	if (cin.fail()) {
		cin.clear();				// reset the error flag so cin works again
		cin.ignore(1000, '\n');		// discard the bad input still sitting in the buffer
		choice = -1;				// force it into the "Invalid choice" default case
	}

	return choice;
};


// PRINTING FUNCTIONALITIES


// Removes a trailing '\r' chracter from a string
// CSV files exported use "\r\n" as the line ending, C++ already strips the \n, so \r is left.
// Checks the last character of the string, if it's \r, erase it.
string trimCR(string s) {
	if (!s.empty() && s[s.size() - 1] == '\r')
		s.erase(s.size() - 1);
	return s;
}

// Converts a string to lowercase, letter by letter (lets name search)
// Match regardless of how the user capitalizes their input
string toLowerStr(string s) {
	for (int i = 0; i < (int)s.length(); i++) {
		s[i] = tolower(s[i]);
	}
	return s;
}

// Print the table header + separator line (shared by Display, SearchByID, SearchByName)
void printHeader() {
	cout << left
		<< setw(12) << "StudentID"
		<< setw(28) << "FullName"
		<< setw(16) << "ProgrammeCode"
		<< setw(13) << "YearOfStudy"
		<< setw(8) << "CGPA"
		<< setw(14) << "ContactNumber" << endl;
	cout << string(91, '-') << endl;
}

// Prints one student's data as a single formatted table row
void printStudentRow(Student s) {
	cout << fixed << setprecision(2)
		<< setw(12) << s.StudentID
		<< setw(28) << s.FullName
		<< setw(16) << s.ProgrammeCode
		<< setw(13) << s.YearOfStudy
		<< setw(8) << s.CGPA
		<< setw(14) << s.ContactNumber << endl;
}

// Prints the table header + separator line for Programme records
void printProgrammeHeader() {
	cout << left
		<< setw(16) << "ProgrammeCode"
		<< setw(66) << "ProgrammeName"
		<< setw(37) << "Faculty"
		<< setw(14) << "DurationYears" << endl;
	cout << string(133, '-') << endl;
}

// Prints one programme's data as a single formatted table row
void printProgrammeRow(Programme p) {
	cout << setw(16) << p.ProgrammeCode
		<< setw(66) << p.ProgrammeName
		<< setw(37) << p.Faculty
		<< setw(14) << p.DurationYears << endl;
}


// LOAD FUNCTIONALITIES


// Reads CSV files from disk and fills the Student array
// Info needed to be in arrays so that Add/Delete/Search/Sort can work
// arr[] - array to fill (passed so can be modified)
// count - passed by REFERENCE (&) so changes here visible back in main()
void loadCSV(Student arr[], int& count, string filename) {
	ifstream file(filename);	// open the file for reading
	if (!file.is_open()) {
		cout << "Error: The file cannot be opened: " << filename << endl;
		return;
	}

	string line;
	getline(file, line);		// skip header row (column names), no need to store that.

	while (getline(file, line) && count < MAX_SIZE) {
		line = trimCR(line);	// remove any leftover '\r'
		stringstream ss(line);	// treat this line as a stream we can extract from
		string token;			// holds one field at a time (between commas)

		getline(ss, token, ',');    arr[count].StudentID = token;
		getline(ss, token, ',');    arr[count].FullName = token;
		getline(ss, token, ',');    arr[count].ProgrammeCode = token;
		getline(ss, token, ',');    arr[count].YearOfStudy = stoi(token); // string to int
		getline(ss, token, ',');    arr[count].CGPA = stof(token); // string to float
		getline(ss, token, ',');    arr[count].ContactNumber = token;

		count++;				// one more record successfully stored, move to next array slot
	}

	file.close();
	cout << count << " records loaded from " << filename << endl;
}

void loadProgrammes(Programme arr[], int& count, string filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Error: The file cannot be opened: " << filename << endl;
		return;
	}

	string line;
	getline(file, line); // skip header row

	while (getline(file, line) && count < MAX_PROGRAMMES) {
		line = trimCR(line);
		stringstream ss(line);
		string token;

		getline(ss, token, ',');	arr[count].ProgrammeCode = token;
		getline(ss, token, ',');	arr[count].ProgrammeName = token;
		getline(ss, token, ',');	arr[count].Faculty = token;
		getline(ss, token, ',');	arr[count].DurationYears = stoi(token);

		count++;
	}
	file.close();
	cout << count << " Programmes Loaded From " << filename << endl;
}

// Linear Search through the Programme array for a given code
int findProgrammeIndex(Programme arr[], int count, string ProgrammeCode) {
	for (int i = 0; i < count; i++) {
		if (arr[i].ProgrammeCode == ProgrammeCode) {
			return i;
		}
	}
	return -1;
}

// Checks Student ID matches "TPXXXXXX" (letters T, P, then 6 digits)
bool isValidStudentIDFormat(string id) {
	if (id.length() != 8)				// ID must be exactly 8 characters
		return false;
	if (id[0] != 'T' || id[1] != 'P')	// first two characters must be "TP"
		return false;

	for (int i = 2; i < (int)id.length(); i++) {
		if (!isdigit(id[i]))			// remaining 6 characters must all be digit
			return false;
	}
	return true;
}

// Checks contact number matches "XXX-XXXXXXX"
bool isValidContactFormat(string contact) {
	if (contact.length() != 11)		// contact number must be 11 characters
		return false;
	if (contact[3] != '-')			// character at index 3 (4th place) must be '-'
		return false;

	for (int i = 0; i < (int)contact.length(); i++) {
		if (i == 3)					// skips the dash
			continue;
		if (!isdigit(contact[i]))	// everything must be in digits
			return false;
	}
	return true;
}

// Checks if a contact number is already used by another student
bool isContactNumberTaken(Student arr[], int count, string contact) {
	for (int i = 0; i < count; i++) {
		if (arr[i].ContactNumber == contact) {
			return true;
		}
	}
	return false;
}

// TIMING FUNCTIONALITY


// Classic Linear Search
// add and deleteStudent() uses it
int searchByID(Student arr[], int count, string id) {
	for (int i = 0; i < count; i++) {
		if (arr[i].StudentID == id) {
			return i;			// found it, stop early, no need to keep scanning
		}
	}
	return -1;					// array does not have -1 index, so not found.
}

// The actual algorithmic part of adding a student
// Takes already-built student record and inserts it.
bool coreAddStudent(Student arr[], int& count, Student newStudent) {
	if (count >= MAX_SIZE) {
		return false;
	}
	if (searchByID(arr, count, newStudent.StudentID) != -1) {
		return false;			// duplicate
	}
	arr[count] = newStudent;	// copy the validated temp record into the array
	count++;					// array now officially has one more student
	return true;
}

// The actual algorithmic part of deleting
// Takes the student ID as parameter
bool coreDeleteStudent(Student arr[], int& count, string id) {
	int idx = searchByID(arr, count, id);
	if (idx == -1) {
		return false; // ID not found
	}
	for (int i = idx; i < count - 1; i++) {
		arr[i] = arr[i + 1];
	}
	count--;
	return true; // If deleted
}

bool coreSearchByName(Student arr[], int count, string searchTerm) {
	string searchLower = toLowerStr(searchTerm);
	bool found = false;

	for (int i = 0; i < count; i++) {
		string nameLower = toLowerStr(arr[i].FullName);
		if (nameLower.find(searchLower) != string::npos) {
			if (!found) {
				printHeader();
				found = true;
			}
			printStudentRow(arr[i]);
		}
	}
	if (found) cout << string(91, '-') << endl;
	return found;
}


// APPLICABLE FUNCTIONALITIES


void addStudent(Student arr[], int& count, Programme progs[], int progCount) {
	if (count >= MAX_SIZE) {
		cout << "Error: Student cannot be added. The list is full." << endl;
		return;
	}

	Student temp;				 // build the new record first, only commit if valid

	do {
		cout << "Enter Student ID (format: TP123456): ";
		cin >> temp.StudentID;

		if (!isValidStudentIDFormat(temp.StudentID)) {
			cout << "Error: Student ID must be in the format TP followed by 6 digits" << endl;
		}
	} while (!isValidStudentIDFormat(temp.StudentID));

	// Error Handling: duplicate ID check
	if (searchByID(arr, count, temp.StudentID) != -1) {
		cout << "Error: Student ID " << temp.StudentID << " already exists." << endl;
		return;					 // stop here, dont insert a duplicate
	}

	cout << "Enter Full Name: ";
	cin.ignore();				 // clear leftover \n from previous cin >>
	getline(cin, temp.FullName); // getline allows names with spaces (e.g, "Hafiy Hensem")


	// Programme Code: show numbered list to pick, and validate choice
	int progChoice;
	int progIndex;
	do {
		cout << "\nSelect Programme: " << endl;
		for (int i = 0; i < progCount; i++) {
			cout << (i + 1) << ". " << progs[i].ProgrammeCode << " - " << progs[i].ProgrammeName
				<< " (" << progs[i].DurationYears << " years)" << endl;
		}
		cout << "Enter choice (1-" << progCount << "): ";
		cin >> progChoice;

		if (cin.fail()) {
			cin.clear();				// reset the error flag so cin works again
			cin.ignore(1000, '\n');		// discard the bad input still sitting in the buffer
			progChoice = -1;			// forces the range check below to fail and loop again
		}

		progIndex = progChoice - 1; // convert menu number to array index

		if (progIndex < 0 || progIndex >= progCount) {
			cout << "Error: Invalid choice. Please select a number from the list." << endl;
		}
	} while (progIndex < 0 || progIndex >= progCount);

	temp.ProgrammeCode = progs[progIndex].ProgrammeCode;

	// Year of Study: must be within which programme's duration
	int maxYear = progs[progIndex].DurationYears;
	do {
		cout << "Enter Year of Study (1-" << maxYear << " for " << temp.ProgrammeCode << "): ";
		cin >> temp.YearOfStudy;

		if (cin.fail()) {
			cin.clear();				// reset the error flag so cin works again
			cin.ignore(1000, '\n');		// discard the bad input still sitting in the buffer
			temp.YearOfStudy = -1;		// forces the range check below to fail and loop again
		}

		if (temp.YearOfStudy < 1 || temp.YearOfStudy > maxYear) {
			cout << "Error: Year must be between 1 and " << maxYear << " for this programme." << endl;
		}
	} while (temp.YearOfStudy < 1 || temp.YearOfStudy > maxYear);

	// CGPA: 0.0 <= CGPA <= 4.0
	do {
		cout << "Enter CGPA (0.0 - 4.0): ";
		cin >> temp.CGPA;

		if (cin.fail()) {
			cin.clear();				// reset the error flag so cin works again
			cin.ignore(1000, '\n');		// discard the bad input still sitting in the buffer
			temp.CGPA = -1;				// forces the range check below to fail and loop again
		}

		if (temp.CGPA < 0.0 || temp.CGPA > 4.0) {
			cout << "Error: CGPA must be between 0.0 and 4.0." << endl;
		}
	} while (temp.CGPA < 0.0 || temp.CGPA > 4.0);

	// Contact Number: Must match XXX-XXXXXXX
	do {
		cout << "Enter Contact Number (format: 012-3456789): ";
		cin >> temp.ContactNumber;

		if (!isValidContactFormat(temp.ContactNumber)) {
			cout << "Error: Contact number must be in the format XXX-XXXXXXX." << endl;
		}
		else if (isContactNumberTaken(arr, count, temp.ContactNumber)) {
			cout << "Error: This contact number is already registered to another student." << endl;
		}
	} while (!isValidContactFormat(temp.ContactNumber) || isContactNumberTaken(arr, count, temp.ContactNumber));

	auto start = chrono::high_resolution_clock::now();
	coreAddStudent(arr, count, temp); // perform the actual insert
	auto end = chrono::high_resolution_clock::now();
	long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

	cout << "Student added successfully! Current total records: " << count << endl;
	cout << "Core add operation took " << duration << " microseconds." << endl;
}

void deleteStudent(Student arr[], int& count) {
	// Empty array
	if (count == 0) {
		cout << "Error: This list is empty. Nothing to delete." << endl;
		return;
	}

	string id;

	// Student ID: must match "TPXXXXXX" format
	do {
		cout << "Enter Student ID to delete (format: TP123456): ";
		cin >> id;

		if (!isValidStudentIDFormat(id)) {
			cout << "Error: Student ID must be in the format TP followed by 6 digits." << endl;
		}
	} while (!isValidStudentIDFormat(id));

	// Timer strictly around the algorithmic deletion
	auto start = chrono::high_resolution_clock::now();
	bool isDeleted = coreDeleteStudent(arr, count, id);
	auto end = chrono::high_resolution_clock::now();
	long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

	if (!isDeleted) {
		cout << "Error: Student ID " << id << " not found." << endl;
		return;
	}

	cout << "Student " << id << " deleted successfully! Current total records: " << count << endl;
	cout << "Core delete operation took " << duration << " microseconds." << endl;
}

// Display all student by printing header (printHeader) and rows (printStudentRow)
void displayAll(Student arr[], int count) {
	if (count == 0) {
		cout << "Error: This list is empty. Nothing to display." << endl;
		return;
	}

	auto start = chrono::high_resolution_clock::now();

	printHeader();
	for (int i = 0; i < count; i++) {
		printStudentRow(arr[i]);
	}

	auto end = chrono::high_resolution_clock::now();
	long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

	cout << string(91, '-') << endl;
	cout << "Total records displayed: " << count << endl;
	cout << "Display operation took " << duration << " microseconds." << endl;
}

// Displays all loaded programmes in a formatted table
// Able to verify all 25 loaded correctly without going through addStudent()
void displayAllProgrammes(Programme arr[], int count) {
	if (count == 0) {
		cout << "Error: No programmes loaded." << endl;
		return;
	}

	printProgrammeHeader();
	for (int i = 0; i < count; i++) {
		printProgrammeRow(arr[i]);
	}
	cout << string(133, '-') << endl;
	cout << "Total programmes displayed: " << count << endl;
}

// Prompts for a Student ID and displays that one record if found.
// Reuses searchByID()
void searchByIDMenu(Student arr[], int count) {
	if (count == 0) {
		cout << "Error: The list is empty. Nothing to search." << endl;
		return;
	}

	string id;
	do {
		cout << "Enter Student ID to search (format: TP123456): ";
		cin >> id;

		if (!isValidStudentIDFormat(id)) {
			cout << "Error: Student ID must be in the format TP followed by 6 digits." << endl;
		}
	} while (!isValidStudentIDFormat(id));

	auto start = chrono::high_resolution_clock::now();
	int idx = searchByID(arr, count, id);
	auto end = chrono::high_resolution_clock::now();
	long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

	if (idx == -1) {
		cout << "Student ID " << id << " not found." << endl;
		cout << "Core search by ID operation took " << duration << " microseconds." << endl;
		return;
	}

	printHeader();
	printStudentRow(arr[idx]);
	cout << string(91, '-') << endl;
	cout << "Core search by ID operation took " << duration << " microseconds." << endl;
}

// Searches for ALL students whose FullName contains the given text
// case-insensitive
void searchByName(Student arr[], int count) {
	if (count == 0) {
		cout << "Error: The list is empty. Nothing to search." << endl;
		return;
	}

	cout << "Enter name (or part of name) to search: ";
	cin.ignore();		// clear leftover '\n' from the previous cin >>
	string searchTerm;
	getline(cin, searchTerm); // getline allows multi-word names

	// Timer around the core search function
	auto start = chrono::high_resolution_clock::now();
	bool isFound = coreSearchByName(arr, count, searchTerm);
	auto end = chrono::high_resolution_clock::now();
	long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

	if (!isFound) {
		cout << "No students found with name containing \"" << searchTerm << "\"." << endl;
	}

	cout << "Core search by name operation took " << duration << " microseconds." << endl;
}

// Sort the CGPA by selection
// Assume the student AT position i is the "best so far"
// Scan the REST of the array (i+1 onward) to see if any other student beats the current best
// Track the index of whicever student is currently best in targetIndex
// After scanning, SWAP the student at position i with student at targetIdx.
// This locks in the correct value at position i permanently
// Move to i+1 and repeat, scanning a progressively SMALLER remaining portion each time,
// since everything before i is already finalized
void selectionSortByCGPA(Student arr[], int count, bool ascending) {
	auto start = chrono::high_resolution_clock::now();
	for (int i = 0; i < count - 1; i++) {
		int targetIndex = i; // assume current position holds the best value so far

		for (int j = i + 1; j < count; j++) {
			if (ascending) {
				if (arr[j].CGPA < arr[targetIndex].CGPA) {
					targetIndex = j; // found a smaller CGPA, remember its position
				}
			}
			else {
				if (arr[j].CGPA > arr[targetIndex].CGPA) {
					targetIndex = j; // found a larget CGPA, remember its position
				}
			}
		}

		// Swap the best-found student into position i
		if (targetIndex != i) {
			Student temp = arr[i];
			arr[i] = arr[targetIndex];
			arr[targetIndex] = temp;
		}
	}

	auto end = chrono::high_resolution_clock::now();
	long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

	// Ternary Operator ? "Ascending" (TRUE) : "Descending" (FALSE)
	string sortType = ascending ? "Ascending" : "Descending";
	cout << "Selection Sort (" << sortType << ") operation took " << duration << " microseconds." << endl;
}

int main() {
	// heap allocated, no max limit
	Student* students = new Student[MAX_SIZE]; // array of records, fixed max capacity
	int count = 0;				// tracks how many slots are currently used

	Programme* programmes = new Programme[MAX_PROGRAMMES];
	int progCount = 0;

	loadCSV(students, count, "students_500.csv");
	loadProgrammes(programmes, progCount, "programmes.csv");

	int choice;
	do {
		choice = menu();
		switch (choice) {
		case 1: // Add Student			
			addStudent(students, count, programmes, progCount);
			break;
		case 2: // Delete Student			
			deleteStudent(students, count);
			break;
		case 3: // Search by Student ID			
			searchByIDMenu(students, count);
			break;
		case 4: // Search by Name
			searchByName(students, count);
			break;
		case 5: // Display All Students			
			displayAll(students, count);
			break;
		case 6: // Display All Programmes
			displayAllProgrammes(programmes, progCount);
			break;
		case 7: // Sort by CGPA (Ascending)			
			selectionSortByCGPA(students, count, true);
			break;
		case 8: // Sort by CGPA (Descending)			
			selectionSortByCGPA(students, count, false);
			break;
		case 0:
			cout << "Exiting program..." << endl;
			cout << "Thank you for using!!" << endl;
			break;
		default:
			cout << "Invalid choice!!" << endl;
		}
	} while (choice != 0);

	delete[] students; // free heap memory once the whole program is done
	delete[] programmes;
	return 0;
}
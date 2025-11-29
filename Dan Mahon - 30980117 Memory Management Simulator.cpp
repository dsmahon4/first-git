/*****************************************************
Dan Mahon
Semester Project - Memory Management Simulator
Date Completed: November, 2025
Description: Educational tool to visualize stack/heap memory
Version: 1.0
*******************************************************/

#include <iostream>
#include <string>
#include <iomanip> // for formatting
#include <sstream> // for string tokens
#include <vector>
#include <climits> // for INT_MIN and INT_MAX
#include <fstream> // for testing

using namespace std;

int globalVarCounter = -1;
int globalHeapCounter = -1;
const int baseStackAddress = 0x1000;
const int baseHeapAddress = 0x5000;

struct intVar {
	string value;
	string address;
	string type;
	string name;
	string scope;
	bool isFreed; // Track if heap memory is freed
};



string formatAddress(int addr) {
	stringstream newAddress;
	newAddress << "0x" << hex << setfill('0') << setw(4) << addr;
	return newAddress.str();
}



void nameValidation(string& tokenName) {
	bool isValidName = false;
	while (!isValidName) { // is tokenName a valid identifier??
		isValidName = true;
		for (char c : tokenName) {

			if (c == '_')
				break;	// We can allow '_'

			if ((!(isalnum(c))) || (!(isalpha(tokenName[0])))) {
				isValidName = false;
				cerr << "Stop trying to make '" << tokenName << "' happen. It's not going to happen.\n"
					<< "Enter a real name (identifiers cannot have special chars or begin with a number): ";
				getline(cin, tokenName);
				break;
			}
		}
	}
}



void displayMenu() {
	cout << left;
	cout << "Available Commands: \n\n";
	cout << setw(35) << "int <name> = <value>" << "- Declare integer variable\n";
	cout << setw(35) << "int* <pointer> = new int(<value>)" << "- Allocate memory for variable on heap\n";
	cout << setw(35) << "delete <pointer>" << "- Free heap memory\n";
	cout << setw(35) << "<var> = <value>" << "- Assign new value\n";
	cout << setw(35) << "*<pointer> = <value>" << "- Dereference and assign\n";
	cout << setw(35) << "<pointer> = nullptr" << "- Set pointer to null\n";
	cout << setw(35) << "function <name>" << "- Enter new function\n";
	cout << setw(35) << "return" << "- Exit current function\n";
	cout << setw(35) << "show" << "- Display memory state\n";
	cout << setw(35) << "check" << "- Check for memory errors\n";
	cout << setw(35) << "clear" << "- Reset simulator\n";
	cout << setw(35) << "help" << "- Display list of commands\n";
	cout << setw(35) << "quit" << "- Exit program\n\n\n";
}



/*--------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------*/

intVar* createIntVar(string userCommand, string& currentScope, vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs) {
	string tokenType, tokenName, tokenDiscard, tokenValue;
	bool isValidInt = false;

	// Test userCommand and assign tokens
	while (userCommand != "back") {
		stringstream ss(userCommand);
		ss >> tokenType >> tokenName >> tokenDiscard >> tokenValue;
		if ((tokenType != "int") || (ss.fail()) || (tokenDiscard != "=") || (ss.peek() != EOF) || (tokenValue == "-")) {
			cerr << "You're killing me, Smalls! To initialize an integer -> (int <name> = <value>) \n"
				"or type 'back' to return to menu: ";
			ss.clear();
			getline(cin, userCommand);
			continue;
		}
		break;
	}

	if (userCommand == "back")
		return nullptr;

	nameValidation(tokenName); // tokenName is good!! Moving on --->

	while (!isValidInt) {
		// skip negative sign
		isValidInt = true;
		int index = (tokenValue[0] == '-') ? 1 : 0;

		for (int i = index; i < tokenValue.length(); ++i) {
			if (!isdigit(tokenValue[i])) {
				isValidInt = false;
				break;
			}
		}

		if (!isValidInt) {
			cerr << "Stop trying to make '" << tokenValue << "' happen. It's not going to happen.\n"
				<< "Enter a real integer: ";
			getline(cin, tokenValue);
		}

		++globalVarCounter;
		intVar* newInt = new intVar;
		(*newInt).name = tokenName;
		(*newInt).type = tokenType;
		(*newInt).value = tokenValue;
		(*newInt).address = formatAddress(baseStackAddress + (4 * globalVarCounter));
		(*newInt).scope = currentScope;
		(*newInt).isFreed = false;

		cout << "\nStack variable created: " << tokenName << "\n\n";
		return newInt;
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------*/

void createIntVarPtr(string userCommand, string& currentScope, vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs) {
	string tokenType, tokenName, tokenDiscard1, tokenNew, tokenValue, extractValue;
	char c;
	bool isValidInt = false, isValidName = false;

	while (userCommand != "back") {
		stringstream ss(userCommand);
		ss >> tokenType >> tokenName >> tokenDiscard1 >> tokenNew;

		if ((tokenNew == "new")) {
			ss >> tokenValue;
		}
		if ((tokenType != "int*") || (ss.fail()) || (tokenDiscard1 != "=") || (ss.peek() != EOF)) {
			cerr << "Ah, Ah, Ah! You didn't say the magic word! To initialize a pointer -> (int* <name> = &variable) \n"
				<< "To allocate heap memory -> (int* <name> = new int(<value>)\n"
				<< "or type 'back' to return to menu: ";
			ss.clear();
			getline(cin, userCommand);
			continue;
		}

		break;
	}


	if (userCommand == "back")
	{
		return;
	}

	nameValidation(tokenName); // tokenName is good!! Moving on --->

	++globalVarCounter;
	intVar* newPtr = new intVar;
	(*newPtr).name = tokenName;
	(*newPtr).type = tokenType;
	(*newPtr).isFreed = false;

	if (tokenNew == "new") {

		// skip negative sign
		isValidInt = true;
		int index = (tokenValue[4] == '-') ? 5 : 4;

		for (int i = index; i < (tokenValue.length() - 1); ++i) {  // length-1 to accommodate closing parenthesis in token
			extractValue += tokenValue[i];
			if (!isdigit(tokenValue[i])) {
				isValidInt = false;
				break;
			}
		}

		if (tokenValue[(tokenValue.length() - 1)] != ')')
			isValidInt = false;

		while (!isValidInt) { // invalid input loop for VALUE
			cerr << "Please try again. Ensure a valid integer is used to assign as value.";
			getline(cin, extractValue);
			isValidInt = true;

			int index = (tokenValue[0] == '-') ? 1 : 0;
			for (int i = index; i < extractValue.length(); ++i) {
				if (!isdigit(extractValue[i])) {
					isValidInt = false;
					break;
				}
			}
		}

		intVar* heapVar = new intVar;
		(*heapVar).type = "int";

		(*heapVar).value = extractValue;

		++globalHeapCounter;
		(*newPtr).address = formatAddress(baseStackAddress + (4 * globalVarCounter));
		(*newPtr).value = formatAddress(baseHeapAddress + (4 * globalHeapCounter));
		(*heapVar).address = formatAddress(baseHeapAddress + (4 * globalHeapCounter));
		(*heapVar).name = "";
		(*heapVar).scope = currentScope;
		(*heapVar).isFreed = false;
		newHeapVarPtrs.push_back(heapVar);

		cout << "\nHeap memory allocated at " << (*heapVar).address << "\n";
		cout << "Stack pointer created: " << (*newPtr).name << " -> " << (*heapVar).address << "\n\n";
	}

	else if (tokenNew != "new") {
		// extract name from string tokenNew


		while (!isValidName) {
			for (intVar* intVar : intVarPtrs) {
				if ((*intVar).name == tokenNew.substr(1)) { // tokenNew.substr(1) will remove the expected & from tokenNew[0]
					(*newPtr).value = (*intVar).address;
					isValidName = true;
					break;
				}
			}
			if (!isValidName) {
				cerr << "The truth is out there... but '" << tokenNew << "' isn't. (Variable address not found)\n"
					<< "Try again. \n\n";
				return;
			}
		}
		(*newPtr).address = formatAddress(baseStackAddress + (4 * globalVarCounter));

		cout << endl << (*newPtr).name << " now points to same location as " << tokenNew << " (" << (*newPtr).value << ")\n\n";
	}

	(*newPtr).scope = currentScope;
	intVarPtrs.push_back(newPtr);
	return;
}

/*--------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------*/

void deletePointer(string userCommand, vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs) {
	if (globalHeapCounter < 0) {
		cerr << "\nNo heap memory is currently allocated.\n\n";
		return;
	} // Be on the lookout! There's heap memory allocated somewhere!

	stringstream ss(userCommand);
	string tokenDelete, tokenName;
	ss >> tokenDelete >> tokenName;

	if (tokenName.empty()) {
		cerr << "Tool 'delete' requires a function name. Please try again.";
		return;
	} // Looks like the user actually entered a name! Lets see if we can find a match ->

	// Find the pointer in stack
	string heapAddress = "";
	bool ptrFound = false;

	for (intVar* intVarPtr : intVarPtrs) {
		if ((*intVarPtr).name == tokenName && (*intVarPtr).type == "int*") {
			heapAddress = (*intVarPtr).value;
			ptrFound = true;
			break;
		}
	}

	if (!ptrFound) {
		cerr << "These are not the pointers you are looking for. (" << tokenName << " not found)\n"
			"Please enter a different command.\n\n";
		return;
	} // If we made it here, we should have confirmed a matching name and an address! Lets see if its a HEAP address ->

	// Find and mark the heap block as freed
	bool heapFound = false;
	int danglingCount = 0;

	for (intVar* heapVar : newHeapVarPtrs) {
		if ((*heapVar).address == heapAddress) {
			(*heapVar).isFreed = true;
			heapFound = true;	// if we made it here, we confirmed the address is on the HEAP and is now marked freed!

			// Count how many pointers still point to this address
			for (intVar* ptr : intVarPtrs) {
				if ((*ptr).type == "int*" && (*ptr).value == heapAddress) {
					danglingCount++;
				}
			}

			cout << "\nHeap memory at " << heapAddress << " freed\n";
			if (danglingCount >= 1) {
				cout << "WARNING - " << danglingCount << " pointer(s) still point to this freed memory!\n";
			}
			cout << "\n";
			break;
		}
	}

	if (!heapFound) { // if we made it here, we have a valid address but it points to STACK memory
		cerr << "\nNo Heap for you! Come back, one year! \n(Pointer does not point to valid heap memory)\n"
			"Please enter a different command. \n\n";
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------*/

void setNullptr(string userCommand, vector<intVar*>& intVarPtrs) {
	stringstream ss(userCommand);
	string tokenName, tokenDiscard1, tokenDiscard2;
	ss >> tokenName >> tokenDiscard1 >> tokenDiscard2;

	for (intVar* intVarPtr : intVarPtrs) {
		if ((*intVarPtr).name == tokenName && (*intVarPtr).type == "int*") {
			(*intVarPtr).value = "N/A";
			cout << endl << tokenName << " set to nullptr\n\n";
			return;
		}
	}
	cerr << "These are not the pointers you are looking for. (" << tokenName << " not found)\n"
		"Please enter a different command.\n\n";
}

/*--------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------*/

void changeValue(string userCommand, vector<intVar*>& intVarPtrs) {
	string tokenName, tokenDiscard, tokenValue, varType;
	bool isValidInt = false, isValidName = false;

	while (userCommand != "back") {
		stringstream ss(userCommand);
		ss >> tokenName >> tokenDiscard >> tokenValue;

		if ((ss.fail()) || (tokenDiscard != "=") || (ss.peek() != EOF)) {
			cerr << "What kind of garbage was that? Let's try that again and remember: SYNTAX IS IMPORTANT. \n"
				"\"<var> = <value>\" or type 'back' to return to menu: ";
			ss.clear();
			getline(cin, userCommand);
			continue;
		}
		break;
	} // Alright, we've got tokens! Syntax seems okay. Let's see what they are --->

	if (userCommand == "back")
		return;

	for (intVar* var : intVarPtrs) { //Are we changing an int value, or ptr value? AND is tokenName legit?
		if (tokenName == (*var).name) {
			varType = (*var).type;
			isValidName = true;
			break;
		}
	}

	if (!isValidName) {
		cerr << "\nYeah... if you could go ahead and define '" << tokenName << "' first, that would be great. mmmkay?\n"
			"Please try a different command. \n\n";
		return;
	} //If we're here, then we've got a valid tokenName and a type!

	if (varType == "int") { // COOL! We're changing an int's value!

		while (!isValidInt) { // iterate through tokenValue to ensure its a valid int
			isValidInt = true;
			int index = (tokenValue[0] == '-') ? 1 : 0; // check if tokenValue is negative

			for (int i = index; i < tokenValue.length(); ++i) {
				if (!isdigit(tokenValue[i])) {		// is every element in tokenValue a digit?
					isValidInt = false;
					break;
				}
			}

			if (!isValidInt) {
				cerr << "No, Patrick, '" << tokenValue << "' is not an integer.\n"
					"Horseradish is not an integer either. Try an ACTUAL integer: ";
				getline(cin, tokenValue);
			}
		}
	} // Great, tokenValue is a good int!

	else if (varType == "int*") { // No worries! We're changing a pointers value!
		tokenValue.erase(0, 1); // ditch the &
		bool validAddress = false;
		for (intVar* newAddress : intVarPtrs) {	// do we have a valid address??
			if (tokenValue == (*newAddress).name) {
				tokenValue = (*newAddress).address;
				validAddress = true;
				break;
			}
		}
		if (!validAddress) {
			cerr << "Ah, Ah, Ah! You forgot to say the magic word!\n"
				"Don't forget the '&' and ensure your variable is defined.\n"
				"Try again!\n\n";
			return;
		}
	} // nnnnNOW we have a good address... or good int?

	for (intVar* intVarPtr : intVarPtrs) {	// Now we can update the value of tokenName!
		if ((*intVarPtr).name == tokenName) {
			string oldValue = (*intVarPtr).value;
			(*intVarPtr).value = tokenValue;
			cerr << "\nVariable " << tokenName << " updated: " << oldValue << " -> " << tokenValue << "\n\n";
			return;
		}
	}
	return;
}

/*--------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------*/

void dereferenceAndAssign(string userCommand, vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs) {
	string tokenName, tokenDiscard, tokenValue;
	bool isValidInput = false;

	stringstream ss(userCommand);
	ss >> tokenName >> tokenDiscard >> tokenValue;

	tokenName.erase(0, 1); // Remove asterisk from pointer name

	if ((ss.fail()) || (tokenDiscard != "=") || (ss.peek() != EOF)) {
		cerr << "Wait... what?? Follow the correct syntax (*<pointer> = <value>) \n"
			"Try again. ";
		return;
	}

	// Checking for good ints!
	while (!isValidInput) {
		isValidInput = true;
		int index = (tokenValue[0] == '-') ? 1 : 0; // start checking tokenValue AFTER the unary

		for (int i = index; i < tokenValue.length(); ++i) {
			if (!isdigit(tokenValue[i])) {			// If an element isnt a digit, she no good!
				isValidInput = false;
				break;
			}
		}

		if (!isValidInput) { // So, we know this ISNT an integer. Is it a variable??
			for (intVar* nameCheck : intVarPtrs) {
				if (tokenValue.substr(1) == (*nameCheck).name) {
					if ((*nameCheck).type == "int") {	// Yep! Its a variable! BUT, is it an INT!?
						isValidInput = true;
						tokenValue = (*nameCheck).value; // tokenValue is now == to the value found at the input address
					}
					else { // these are int pointers! Not pointer pointers!
						cerr << "Your pointer is a pointer that can't point to other pointers.\n"
							"Try again. \n\n";
						return;
					}
				}
				else { // this isnt an int OR an address
					cerr << "No, Patrick, '" << tokenValue << "' is not an integer.\n"
						"Horseradish is not a valid address either. Try again. \n\n";
					return;
				}
			}
		}
	} // Woohoo! We've finally got a good int here! And good syntax!

	if (userCommand == "back")
		return;

	// Find the pointer
	string simAddress = "";
	bool ptrFound = false;

	for (intVar* intVarPtr : intVarPtrs) {
		if ((*intVarPtr).name == tokenName && (*intVarPtr).type == "int*") {
			simAddress = (*intVarPtr).value;
			ptrFound = true;
			break;
		}
	}

	if (!ptrFound) {
		cerr << "\nI don't know who you are. I don't know what you want. \n"
			"But I have a very particular set of skills. \n"
			"And finding '" << tokenName << "' is not one of them.\n\n"
			"Please enter a different command.";
		return;
	} // if we made it here, we should have a good pointer name! Lets check the address (value of ptr) ->

	if (simAddress == "N/A") {
		cerr << "\nERROR - Cannot dereference nullptr! " << tokenName << " points to null.\n\n";
		return;
	} // Woop! We've got an address!

	// Find the address in STACK and check if freed
	for (intVar* stackVar : intVarPtrs) {
		if ((*stackVar).address == simAddress) {
			if ((*stackVar).isFreed) {
				cerr << "\nERROR - Cannot dereference " << tokenName << "!\n";
				cerr << "       " << tokenName << " points to FREED memory at " << simAddress << ".\n";
				cerr << "       This would cause a segmentation fault in real C++!\n\n";
				return;
			}

			string oldValue = (*stackVar).value;
			(*stackVar).value = tokenValue;
			cout << "\nDereferenced " << tokenName << " and set value to " << tokenValue << " (was " << oldValue << ")\n\n";
			return;
		}
	}

	// Find the address in HEAP and check if freed
	for (intVar* heapVar : newHeapVarPtrs) {
		if ((*heapVar).address == simAddress) {
			if ((*heapVar).isFreed) {
				cerr << "\nERROR - Cannot dereference " << tokenName << "!\n";
				cerr << "       " << tokenName << " points to FREED memory at " << simAddress << ".\n";
				cerr << "       This would cause a segmentation fault in real C++!\n\n";
				return;
			}

			string oldValue = (*heapVar).value;
			(*heapVar).value = tokenValue;
			cout << "\nDereferenced " << tokenName << " and set value to " << tokenValue << " (was " << oldValue << ")\n\n";
			return;
		}
	} // If we're here, we have a valid pointer name but no address!

	cerr << "\nNo Heap for you! Come back, one year! \n(Pointer does not point to valid heap memory)\n"
		"Please enter a different command. \n\n";
}




void functionReturn(vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs, string& currentScope, vector<string>& functions) {
	if (currentScope == "global") {
		cerr << "\nERROR - You are already in global scope.\n\n";
		return;
	}

	vector<string> removedVars;

	// Removing variables from the current scope
	// This is interesting...
	// 
	for (int i = intVarPtrs.size() - 1; i >= 0; i--) {
		if ((*intVarPtrs[i]).scope == currentScope) {
			removedVars.push_back((*intVarPtrs[i]).name);	// remember the NAME to reference later
			--globalVarCounter;
			delete intVarPtrs[i];						// delete the REAL heap memory of data containing data 
			// for simulated stack variable

			intVarPtrs.erase(intVarPtrs.begin() + i);	// This erases the ADDRESS to the REAL heap memory for the
			// simulated stack variable. No need to set any REAL pointers
			// to nullptr, because they never existed past the function that made them.
		}
	}

	cout << "\nReturned from function: " << functions.back() << "\n";

	functions.pop_back(); // delete function from vector
	currentScope = functions.back(); // reassign current scope

	cout << "Stack frame destroyed\n";
	if (!removedVars.empty()) {
		cout << "Local variables removed: ";
		for (int i = 0; i < removedVars.size(); i++) {
			cout << removedVars[i];
			if (i < removedVars.size() - 1) cout << ", ";
		}
		cout << "\n";
	}
	cout << "\n";
}

void showMemory(string& currentScope, vector<string>& functions, vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs) {
	int localPointerCount = 0;
	vector<string> pointerRefNames;

	cout << "\n========== STACK ==========\n";

	for (string function : functions) {
		if (function == currentScope) {
			cout << "[" << function << "] <- CURRENT FRAME\n";
		}
		else {
			cout << "[" << function << "]\n";
		}

		bool hasVars = false;
		for (intVar* intVarPtr : intVarPtrs) {
			if ((*intVarPtr).scope == function) {
				hasVars = true;
				cout << "  " << left << setw(12) << (*intVarPtr).name
					<< " (" << setw(5) << (*intVarPtr).type << ") = "
					<< setw(12) << (*intVarPtr).value
					<< " [" << (*intVarPtr).address << "]";

				// If it's a pointer, show what it points to
				if ((*intVarPtr).type == "int*" && (*intVarPtr).value != "N/A") {


					for (intVar* ptrVar : intVarPtrs) {
						if ((*ptrVar).address == (*intVarPtr).value && (*ptrVar).isFreed)
							cout << " --> [FREED MEMORY!]";
						else if ((*ptrVar).address == (*intVarPtr).value && (!(*ptrVar).isFreed))
							cout << " --> [STACK: " << (*intVarPtr).value << "]";
					}

					for (intVar* heapVar : newHeapVarPtrs) {
						if ((*heapVar).address == (*intVarPtr).value && (*heapVar).isFreed)
							cout << " --> [FREED MEMORY!]";
						else if ((*heapVar).address == (*intVarPtr).value && (!(*heapVar).isFreed))
							cout << " --> [HEAP: " << (*intVarPtr).value << "]";
					}
				}
				else if ((*intVarPtr).type == "int*" && (*intVarPtr).value == "N/A") {
					cout << " (nullptr)";
				}

				cout << "\n";
			}
		}

		if (!hasVars) {
			cout << "  (no local variables)\n";
		}
		cout << "\n";
	}

	cout << "========== HEAP ===========\n";

	if (newHeapVarPtrs.empty()) {
		cout << "  (empty)\n";
	}
	else {
		for (intVar* displayVar : newHeapVarPtrs) {
			if ((*displayVar).isFreed) {
				cout << "  [" << (*displayVar).address << "] (freed)         ";

				// Count dangling pointers
				localPointerCount = 0;
				pointerRefNames.clear();

				for (intVar* checkAddr : intVarPtrs) {
					if ((*checkAddr).type == "int*" && (*checkAddr).value == (*displayVar).address) {
						localPointerCount++;
						pointerRefNames.push_back((*checkAddr).name);
					}
				}

				if (localPointerCount > 0) {
					cout << "WARNING - DANGLING! " << localPointerCount << " pointer(s) [";
					for (int i = 0; i < pointerRefNames.size(); i++) {
						cout << pointerRefNames[i];
						if (i < pointerRefNames.size() - 1) {
							cout << ", ";
						}
					}
					cout << "]\n";
				}
				else {
					cout << "\n";
				}
			}
			else {
				cout << "  [" << (*displayVar).address << "] (int) = "
					<< setw(12) << (*displayVar).value << " ";

				// Count how many pointers reference this address
				localPointerCount = 0;
				pointerRefNames.clear();

				for (intVar* checkAddr : intVarPtrs) {
					if ((*checkAddr).type == "int*" && (*checkAddr).value == (*displayVar).address) {
						localPointerCount++;
						pointerRefNames.push_back((*checkAddr).name);
					}
				}

				// Display reference status
				if (localPointerCount >= 1) {
					cout << "Referenced by " << localPointerCount << " pointer(s) [";
					for (int i = 0; i < pointerRefNames.size(); i++) {
						cout << pointerRefNames[i];
						if (i < pointerRefNames.size() - 1) {
							cout << ", ";
						}
					}
					cout << "]\n";
				}
				else {
					cout << "WARNING - MEMORY LEAK! (0 pointers)\n";
				}
			}
		}
	}

	// Count active allocations
	int activeHeap = 0;
	for (intVar* heapVar : newHeapVarPtrs) {
		if (!(*heapVar).isFreed) activeHeap++;
	}

	int activePointers = 0;
	for (intVar* ptr : intVarPtrs) {
		if ((*ptr).type == "int*") activePointers++;
	}

	cout << "\n===========================\n";
	cout << "Stack usage: " << (intVarPtrs.size() * 4) << " bytes\n";
	cout << "Heap usage: " << (activeHeap * 4) << " bytes\n";
	cout << "Active pointers: " << activePointers << "\n";
	cout << "===========================\n\n";
}

void checkErrors(vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs) {
	int leakCount = 0;
	int danglingCount = 0;
	vector<string> leakedAddresses;
	vector<string> danglingPointers;

	cout << "\n========== MEMORY ERROR REPORT ==========\n\n";

	// Check for memory leaks (allocated but no pointers)
	for (intVar* heapVar : newHeapVarPtrs) {
		if (!(*heapVar).isFreed) {
			int refCount = 0;
			for (intVar* ptr : intVarPtrs) {
				if ((*ptr).type == "int*" && (*ptr).value == (*heapVar).address) {
					refCount++;
				}
			}

			if (refCount == 0) {
				leakCount++;
				leakedAddresses.push_back((*heapVar).address);
			}
		}
	}

	// Check for dangling pointers (pointing to freed memory)
	for (intVar* ptr : intVarPtrs) {
		if ((*ptr).type == "int*" && (*ptr).value != "0") {
			for (intVar* heapVar : newHeapVarPtrs) {
				if ((*heapVar).address == (*ptr).value && (*heapVar).isFreed) {
					danglingCount++;
					danglingPointers.push_back((*ptr).name);
					break;
				}
			}
		}
	}

	// Display results
	if (leakCount > 0) {
		cout << "ERROR - MEMORY LEAKS DETECTED: " << leakCount << "\n";
		cout << "You promised you'd never let go, Jack. But you let go of the pointer without deleting it.\n\n";
		for (string addr : leakedAddresses) {
			cout << "  [" << addr << "]\n";
			cout << "    - Allocated: YES\n";
			cout << "    - Freed: NO\n";
			cout << "    - Pointers: 0\n";
			cout << "    - Status: LEAKED - No way to access or free this memory\n";
			cout << "    - Size: 4 bytes\n\n";
		}
	}
	else {
		cout << "No memory leaks detected!\n\n";
	}

	if (danglingCount > 0) {
		cout << "ERROR - DANGLING POINTERS DETECTED: " << danglingCount << "\n\n";
		for (string ptrName : danglingPointers) {
			string addr = "";
			for (intVar* ptr : intVarPtrs) {
				if ((*ptr).name == ptrName) {
					addr = (*ptr).value;
					break;
				}
			}
			cout << "  " << ptrName << " -> " << addr << " (freed memory)\n";
			cout << "    - This pointer cannot be safely used\n";
			cout << "    - Dereferencing it would crash the program\n\n";
		}
	}
	else {
		cout << "No dangling pointers detected!\n\n";
	}

	if (leakCount == 0 && danglingCount == 0) {
		cout << "==================================================\n";
		cout << "           ALL MEMORY PROPERLY MANAGED!           \n";
		cout << "==================================================\n\n";
	}
	else {
		cout << "TIPS FOR PREVENTION:\n";
		if (leakCount > 0) {
			cout << "  - Always pair 'new' with 'delete'\n";
			cout << "  - Before reassigning a pointer, delete its current target\n";
		}
		if (danglingCount > 0) {
			cout << "  - After deleting, set pointers to nullptr\n";
			cout << "  - Be careful when multiple pointers share the same address\n";
		}
		cout << "\n";
	}
}

void clearMemory(vector<intVar*>& intVarPtrs, vector<intVar*>& newHeapVarPtrs, vector<string>& functions, string& currentScope) {
	// Delete all stack variables
	for (intVar* ptr : intVarPtrs) {
		delete ptr;
	}
	intVarPtrs.clear();

	// Delete all heap variables
	for (intVar* ptr : newHeapVarPtrs) {
		delete ptr;
	}
	newHeapVarPtrs.clear();

	// Reset counters
	globalVarCounter = -1;
	globalVarCounter = -1;
	globalHeapCounter = -1;

	// Reset scope
	functions.clear();
	functions.push_back("global");
	currentScope = "global";

	cout << "\nMemory cleared and reset\n";
	cout << "All variables and allocations removed\n";
	cout << "Returned to global scope\n\n";
}

void processCommand(string& userCommand, vector<intVar*>& intVarPtrs,
	vector<intVar*>& newHeapVarPtrs, string& currentScope, vector<string>& functions) {

	stringstream ss(userCommand);
	string token1;
	ss >> token1;

	if (token1 == "int") {
		intVar* newVar = createIntVar(userCommand, currentScope, intVarPtrs, newHeapVarPtrs);
		if (newVar != nullptr) {
			intVarPtrs.push_back(newVar);
		}
		return;
	}

	else if (token1 == "int*") {
		createIntVarPtr(userCommand, currentScope, intVarPtrs, newHeapVarPtrs);
		return;
	}

	else if (token1 == "delete") {
		deletePointer(userCommand, intVarPtrs, newHeapVarPtrs);
		return;
	}

	else if (token1 == "function") {
		string funcName;
		ss >> funcName;
		functions.push_back(funcName);
		currentScope = funcName;
		cout << "\nEntered function: " << funcName << "\n";
		cout << "New stack frame created\n\n";
		return;
	}

	else if (token1 == "return") {
		functionReturn(intVarPtrs, newHeapVarPtrs, currentScope, functions);
		return;
	}

	else if (token1 == "show") {
		showMemory(currentScope, functions, intVarPtrs, newHeapVarPtrs);
		return;
	}

	else if (token1 == "check") {
		checkErrors(intVarPtrs, newHeapVarPtrs);
		return;
	}

	else if (token1 == "clear") {
		clearMemory(intVarPtrs, newHeapVarPtrs, functions, currentScope);
		return;
	}

	else if (token1 == "help") {
		displayMenu();
		return;
	}

	else if (token1 == "quit") {
		return;
	}

	// Check if it's a dereference assignment (*ptr = value)
	else if (token1[0] == '*') {
		dereferenceAndAssign(userCommand, intVarPtrs, newHeapVarPtrs);
		return;
	}

	// Check if setting pointer to nullptr
	else {
		string token2, token3;
		ss >> token2 >> token3;
		if (token2 == "=" && token3 == "nullptr") {
			setNullptr(userCommand, intVarPtrs);
			return;
		}
	}

	// Check if it's a regular variable assignment
	for (intVar* intVarPtr : intVarPtrs) {
		if ((*intVarPtr).name == token1) {
			changeValue(userCommand, intVarPtrs);
			return;
		}
	}

	cerr << "\nI'm sorry, Dave. I'm afraid I can't do that.\n";
	cerr << "Type 'help' to see available commands.\n\n";
}

int main() {
	string userInput, currentScope = "global";
	vector<string> functions;
	functions.push_back("global");
	vector<intVar*> intVarPtrs, newHeapVarPtrs;
	ifstream inputTestAll, inputTest1, inputTestEdge, inputTestValidation;
	inputTestAll.open("test_complete.txt");
	inputTest1.open("test1_basic_stack.txt");
	inputTestEdge.open("test_edge_cases.txt");
	inputTestValidation.open("test_input_validation.txt");

	int testIndex = 1; // iterates after every test command to identify which crashed program

	cout << "======================================" << endl;
	cout << "   MEMORY MANAGEMENT SIMULATOR v1.0" << endl;
	cout << "======================================" << endl << endl;
	cout << "Type 'help' for commands, 'quit' to exit" << endl << endl;
	cout << "Simulated memory initialized:" << endl;
	cout << " - Stack: 0x1000 - 0x1FFF" << endl;
	cout << " - Heap:  0x5000 - 0x5FFF" << endl << endl;

	displayMenu();

	cout << "> ";
	while (userInput != "quit") {
		getline(cin, userInput); // CHANGE STREAM TO TEST
		if (!userInput.empty()) {
			processCommand(userInput, intVarPtrs, newHeapVarPtrs, currentScope, functions);
			//testIndex++; //UNCOMMENT WHEN TESTING
		}
		cout << "> ";
		//cout << "cmd #" << testIndex;  // UNCOMMENT WHEN TESTING
	}
	// Cleanup
	cout << "\n======================================\n";
	cout << "Thank you for using Memory Management Simulator!\n";

	// Display final statistics
	int leaks = 0;
	for (intVar* heapVar : newHeapVarPtrs) {
		if (!(*heapVar).isFreed) {
			int refCount = 0;
			for (intVar* ptr : intVarPtrs) {
				if ((*ptr).type == "int*" && (*ptr).value == (*heapVar).address) {
					refCount++;
				}
			}
			if (refCount == 0) leaks++;
		}
	}

	cout << "\nFinal Memory Status:\n";
	cout << "  Stack variables: " << intVarPtrs.size() << "\n";
	cout << "  Heap allocations: " << newHeapVarPtrs.size() << "\n";
	cout << "  Memory leaks: " << leaks << "\n";

	if (leaks == 0) {
		cout << "\nExcellent work! No memory leaks detected.\n";
	}
	else {
		cout << "\nReminder: Always pair 'new' with 'delete'!\n";
	}

	cout << "======================================\n\n";

	// Free all memory
	for (intVar* ptr : intVarPtrs) {
		delete ptr;
	}
	for (intVar* ptr : newHeapVarPtrs) {
		delete ptr;
	}

	return 0;
}
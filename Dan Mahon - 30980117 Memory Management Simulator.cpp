/*****************************************************
Dan Mahon
Semester Project - Memory Management Simulator
Date Completed: November, 2025
Description: 
Version: 1.0
*******************************************************/

#include <iostream>
#include <string>
#include <iomanip> // for fomatting
#include <sstream> // for string tokens
#include <vector>
#include <climits> // for INT_MIN and INT_MAX

using namespace std;

int globalIntCounter = -1;
int globalPtrCounter = -1;
int globalHeapCounter = -1;
const int baseStackAddress = 0x1000;
const int baseHeapAddress = 0x5000;

string formatAddress(int addr) {
	stringstream newAddress;
	newAddress << "0x" << hex << setfill('0') << setw(4) << addr;
	return newAddress.str();
}

void displayMenu() {
	cout << left;
	cout << "Available Commands: \n\n";
	cout << setw(33) << "int <name> = <value>" << "- Declare integer variable\n";
	cout << setw(33) << "int* <pointer> = new int(<value>)" << "- Allocate memory for variable on heap\n";
	cout << setw(33) << "delete <pointer>" << "- Free heap memory\n";
	cout << setw(33) << "<var> = <value>" << "- Assign new value\n";
	cout << setw(33) << "*<pointer> = <value>" << "- Dereference and assign\n";
	cout << setw(33) << "function <name>" << "- Enter new function (allocate memory for variables within new scope\n";
	cout << setw(33) << "return" << "- Exit current function\n";
	cout << setw(33) << "show" << "- Display memory state\n";
	cout << setw(33) << "check" << "- Check for memory errors\n";
	cout << setw(33) << "clear" << "- Reset simulator\n";
	cout << setw(33) << "help" << "- Display list of commands\n";
	cout << setw(33) << "quit" << "- Exit program\n\n\n";
}




struct intVar {
	
	string value;
	string address;
	string type;
	string name;
	string scope;
};




intVar* createIntVar(string userCommand, string& currentScope, vector<intVar*>& intVarPtrs, vector<intVar*>& newIntVarPtrs) {
	string tokenType, tokenName, tokenDiscard, tokenValue;

	// Test userCommand and assign tokens
	while (userCommand != "back") {
		stringstream ss(userCommand);
		ss >> tokenType >> tokenName >> tokenDiscard >> tokenValue;
		if ((tokenType != "int") || (ss.fail()) || (tokenDiscard != "=") || (ss.peek() != EOF)) {
			cerr << "Please ensure the correct syntax is followed to declare a new integer variable (int <name> = <value>) \n"
				"or type 'back' to return to menu: ";
			ss.clear();
			getline(cin, userCommand);
			continue;
		}
		break;
	}

	if (userCommand == "back")
		return nullptr;
	
	++globalIntCounter;
	intVar* newInt = new intVar;
	(*newInt).name = tokenName;
	(*newInt).type = tokenType;
	(*newInt).value = tokenValue;
	(*newInt).address = formatAddress(baseStackAddress + (4 * globalIntCounter));
	(*newInt).scope = currentScope;
	return newInt;
}




void createIntVarPtr(string userCommand, string& currentScope, vector<intVar*>& intVarPtrs, vector<intVar*>& newIntVarPtrs) {
	string tokenType, tokenName, tokenDiscard1, tokenNew, tokenValue, tokenTemp;
	int extractValue;
	char c;

	while (userCommand != "back") {
		stringstream ss(userCommand);
		ss >> tokenType >> tokenName >> tokenDiscard1 >> tokenNew;

		if ((tokenNew == "new"))
			ss >> tokenValue;

		if ((tokenType != "int*") || (ss.fail()) || (tokenDiscard1 != "=") || (ss.peek() != EOF)) {
			cerr << "Please ensure the correct syntax is followed to allocate memory from heap (int* <name> = new int(<value>) ) \n"
				"or type 'back' to return to menu: ";
			ss.clear();
			getline(cin, userCommand);
			continue;
		}
		break;
	}

if (userCommand == "back")
return;

++globalPtrCounter;
intVar* newInt = new intVar;
(*newInt).name = tokenName;
(*newInt).type = tokenType;


if (tokenNew == "new") {
	++globalHeapCounter;
	(*newInt).address = baseStackAddress + (4 * globalPtrCounter);
	(*newInt).value = baseHeapAddress + (4 * globalHeapCounter);

	intVar* newInt2 = new intVar;
	(*newInt2).type = "int";
	// extract integer value from string token
	stringstream ss(tokenValue);
	while (ss >> c) {
		if (isdigit(c))
			tokenTemp += c;
	}
	extractValue = stoi(tokenTemp);
	(*newInt2).value = extractValue;
	(*newInt2).address = baseHeapAddress + (4 * globalHeapCounter);
	newIntVarPtrs.push_back(newInt2);
}

else if (tokenNew != "new") {
	// extract integer value from string token
	tokenNew.erase(0, 1);

	for (intVar* intVar : intVarPtrs) {
		if ((*intVar).name == tokenNew) {
			(*newInt).value = (*intVar).address;
		}
	}
	(*newInt).address = baseStackAddress + (4 * globalPtrCounter);

}

(*newInt).scope = currentScope;

intVarPtrs.push_back(newInt);
return;
}




void deletePointer(string userCommand, vector<intVar*>& newIntVarPtrs) {
	if (globalPtrCounter == -1) {
		cerr << "No variables are currently being allocated heap memory.\n\n";
		return;
	}
	stringstream ss(userCommand);
	string tokenName;
	ss >> tokenName;
	for (intVar* intVarPtr : newIntVarPtrs) {
		if ((*intVarPtr).name == tokenName) {
			--globalPtrCounter;
			delete intVarPtr;
			intVarPtr = nullptr;
			return;
		}
	}
	cerr << "No variable with that identifier was found. \n\n";
}




void changeValue(string userCommand, intVar*& intVarPtr) {
	string tokenName, tokenDiscard;
	int tokenValue;

	while (userCommand != "back") {
		stringstream ss(userCommand);
		ss >> tokenName >> tokenDiscard >> tokenValue;
		if ((ss.fail()) || (tokenDiscard != "=") || (ss.peek() != EOF)) {
			cerr << "Please ensure the correct syntax is followed to change the value of a variable \n"
				"or type 'back' to return to menu: ";
			ss.clear();
			getline(cin, userCommand);
			continue;
		}
		break;
	}
	intVarPtr->value = tokenValue;
}




void functionReturn(vector<intVar*>& intVarPtrs, vector<intVar*>& newIntVarPtrs, string currentScope, vector<string>& functions) {
	if (currentScope == "global") {
		cerr << "You are within global scope. ";
		return;
	}
	for (intVar* intVarPtr : intVarPtrs) {
		if ((*intVarPtr).scope == currentScope) {
			--globalIntCounter;
			delete intVarPtr;
			intVarPtr = nullptr;
		}
	}
	for (intVar* intVarPtr : newIntVarPtrs) {
		if ((*intVarPtr).scope == currentScope) {
			--globalPtrCounter;
			delete intVarPtr;
			intVarPtr = nullptr;
		}
	}
	functions.pop_back();
}




//void showMemory(string currentScope, vector<string>& functions, vector<intVar*>& intVarPtrs, vector<intVar*>& newIntVarPtrs) {
//	int localPointerCount = 0, index = 0;
//	vector<string> pointerRefNames;
//	
//	cout << "========== STACK ==========\n";
//
//	for (string function : functions) {
//		cout << "[" << function << "]\n";
//		for (intVar* intVarPtr : intVarPtrs) {
//			if ((*intVarPtr).scope == function) {
//				cout << left << "  " << (*intVarPtr).name << " (" << (*intVarPtr).type << ") = " <<
//					(*intVarPtr).value << "[" << (*intVarPtr).address << "]\n";
//			}
//		}
//		cout << endl;
//	}
//
//	cout << endl << "========== HEAP ===========";
//	for (intVar* displayVar : newIntVarPtrs) {
//		cout << left << setw(26) << "  " << (*displayVar).address << " (int) = " <<
//			(*displayVar).value;
//
//		for (intVar* checkAddr : intVarPtrs) {
//			if ((*checkAddr).value == (*displayVar).address) {
//				++localPointerCount;
//				pointerRefNames.push_back((*checkAddr).name);
//			}
//		}
//		if (localPointerCount >= 1) {
//			cout << "Referenced by " << localPointerCount << " pointer(s) [";
//			for (; index < (localPointerCount - 1); index++) {
//				cout << pointerRefNames[index] << ", ";
//			}
//			cout << pointerRefNames[++index] << "]\n";
//		}
//		else
//			cout << "MEMORY LEAK!!!\n";
//	localPointerCount = 0;
//	index = 0;
//	pointerRefNames.clear();
//	return;
//	}
//}

void showMemory(string currentScope, vector<string>& functions, vector<intVar*>& intVarPtrs, vector<intVar*>& newIntVarPtrs) {
	int localPointerCount = 0, index = 0;
	vector<string> pointerRefNames;

	cout << "\n========== STACK ==========\n";

	for (string function : functions) {
		cout << "[" << function << "]\n";
		for (intVar* intVarPtr : intVarPtrs) {
			if ((*intVarPtr).scope == function) {
				// Format: name (type) = value [address]
				cout << "  " << left << setw(10) << (*intVarPtr).name
					<< " (" << (*intVarPtr).type << ") = "
					<< setw(12) << (*intVarPtr).value
					<< " [" << (*intVarPtr).address << "]";

				// If it's a pointer, show what it points to
				if ((*intVarPtr).type == "int*" && (*intVarPtr).value != "0") {
					cout << " --> [HEAP: " << (*intVarPtr).value << "]";
				}
				else if ((*intVarPtr).type == "int*" && (*intVarPtr).value == "0") {
					cout << " (nullptr)";
				}

				cout << "\n";
			}
		}
	}

	cout << "\n========== HEAP ===========\n";

	if (newIntVarPtrs.empty()) {
		cout << "  (empty)\n";
	}
	else {
		for (intVar* displayVar : newIntVarPtrs) {
			cout << "  [" << (*displayVar).address << "] (int) = "
				<< (*displayVar).value << "  ";

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
				cout << "OK - Referenced by " << localPointerCount << " pointer(s) [";
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

	cout << "\n===========================\n";
	cout << "Stack usage: " << (intVarPtrs.size() * 4) << " bytes\n";
	cout << "Heap usage: " << (newIntVarPtrs.size() * 4) << " bytes\n";
	cout << "Active pointers: " << localPointerCount << "\n";
	cout << "===========================\n\n";
}



void processCommand(string& userCommand, vector<intVar*>& intVarPtrs, 
	vector<intVar*>& newIntVarPtrs, string& currentScope, vector<string>& functions) {

	stringstream ss(userCommand);
	string token1;
	ss >> token1;

	if (token1 == "int") {
		intVarPtrs.push_back(createIntVar(userCommand, currentScope, intVarPtrs, newIntVarPtrs));
		return;
	}

	else if (token1 == "int*") {
		createIntVarPtr(userCommand, currentScope, intVarPtrs, newIntVarPtrs);
		return;
		//
		//ss >> token1 >> token1 >> token1; //move through userCommand to check for "new"
		//if (token1 == "new") {
		//	newIntVarPtrs.push_back(
		//}
		//else
		//{
		//	intVarPtrs.push_back(createIntVar(userCommand, functions, intVarPtrs, newIntVarPtrs));
		//	return;
		//}
	}

	else if (token1 == "delete") {
		deletePointer(userCommand, newIntVarPtrs);
		return;
	}

	for (intVar* intVarPtr : intVarPtrs) {
		if ((*intVarPtr).name == token1) {
			changeValue(userCommand, intVarPtr);
			return;
		}
	}

	for (intVar* intVarPtr : intVarPtrs) {
		string temp = "*";
		temp += (*intVarPtr).name;
		if ((*intVarPtr).name == token1) {
			changeValue(userCommand, intVarPtr);
			return;
		}
	}

	if (token1 == "function") {
		ss >> token1;
		functions.push_back(token1);
		return;
	}
	
	if (token1 == "return") {
		functionReturn(intVarPtrs, newIntVarPtrs, currentScope, functions);
		return;
	}

	if (token1 == "show") {
		showMemory(currentScope, functions, intVarPtrs, newIntVarPtrs);
		return;
	}
	
	else {
		cerr << "Please enter a valid command found in the help menu. \n";
		cerr << "Enter 'help' if you would like to review the menu: ";
		getline(cin, userCommand);
	}

}




int main()
{
	string userInput, currentScope = "global";
	vector<string> functions;
	functions.push_back("global");
	vector<intVar*> intVarPtrs, newIntVarPtrs;

	
	cout << "======================================" << endl;
	cout << "   MEMORY MANAGEMENT SIMULATOR v1.0" << endl;
	cout << "======================================" << endl << endl;;
	cout << "Type 'help' for commands, 'quit' to exit" << endl << endl;
	cout << "Simulated memory initialized:" << endl;
	cout << " - Stack: 0x1000 - 0x1FFF" << endl;
	cout << " - Heap:  0x5000 - 0x5FFF" << endl << endl << endl;

	displayMenu();
	while (userInput != "quit") {

		getline(cin, userInput);

		processCommand(userInput, intVarPtrs, newIntVarPtrs, currentScope, functions);

		////for (intVar* testing : intVarPtrs) {
		////	cout << (*testing).name << " " << (*testing).address << " ";
		//}
	}









	//int addr = (0x1000);
	//cout << addr;
	//for (int i = 0; i < 1000; i++) {
	//	(addr += 4);
	//	cout << formatAddress(addr) << "  ";
	//}

}
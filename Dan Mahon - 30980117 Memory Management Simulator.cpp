/*****************************************************
Dan Mahon
Semester Project - Memory Management Simulator
Date Completed: November, 2025
Description: 
Version: 1.0
*******************************************************/

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

string formatAddress(int addr) {
	stringstream newAddress;
	newAddress << "0x" << hex << setfill('0') << setw(4) << addr;
	return newAddress.str();
}

int main()
{
	int addr = (0x1000);
	cout << addr;
	for (int i = 0; i < 1000; i++) {
		(addr += 4);
		cout << formatAddress(addr) << "  ";
	}

}
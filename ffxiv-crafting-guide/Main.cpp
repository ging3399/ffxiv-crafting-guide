#include<iostream>
#include<iomanip>
#include "Crafter.h"

using std::cout;
using std::cin;
using std::endl;
using std::setw;

int main() {
	cout << "Final Fantasy XIV crafting guide v0.0.1, by Renko Usami (Anima)" << endl << endl << "Suitable for game patch 5.5" << endl << endl;
	cout << "Please select your language: " << endl << "1. English, 2. ¼òÌåÖĞÎÄ" << endl;
	int language;
	cin >> language;
	cout << endl;
	Crafter crafter(language);
	crafter.solve_me();
	crafter.print();
	system("pause");
	return 0;
}
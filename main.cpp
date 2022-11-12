#include <iostream>
#include "commitViz.h"
using namespace std;

int main() {
	commitViz git ("tests/git");
	git.readFile();
	cout << git.getGraph();
	return 0;
}

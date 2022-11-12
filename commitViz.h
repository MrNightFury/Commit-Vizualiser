#ifndef __COMMITVIZ_H
#define __COMMITVIZ_H
#include <vector>
#include <map>
#include <string>
#include "C:/Users/MrCre/AppData/Local/Programs/Python/Python311/include/Python.h"
using namespace std;

struct filetree {
	vector<filetree> subdirs;
	map<string, string> files;
};

typedef vector<pair<string, string>> fileList;

class commitViz {
private:
	PyObject* func;
	vector<pair<string, string>> deps;
	map<string, string> branches;
	string HEAD;
	string curBranch;
	string gitDir;
	map<string, fileList> fileLists;

	void addOperation (const string& operation);
	static string formatName (const string& name);
	void loadFileLists();
	fileList getFiles (const string& hash, const string& prefix = "");
	static bool contains (fileList fl, string name);
	string dc (const string& hash);
	string getDiff (fileList fl1, fileList fl2);

public:
	commitViz (const string& dir);
	void readFile ();
	string getGraph ();
};
#endif
#include "commitViz.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

commitViz::commitViz (const string& dir) {
	gitDir = dir;
}

void commitViz::readFile () {
	ifstream fin;
	fin.open (gitDir + "/logs/head");

	while (!fin.eof()) {
		string line;
		getline (fin, line);
		if (line.empty()) {
			continue;
		}
		addOperation (line);
	}
	loadFileLists();
};

void commitViz::addOperation (const string& operation) {
	stringstream str = stringstream(operation);
	string prev, next, trash, desc, type;
	str >> prev >> next;
	getline (str, trash, '\t');
	getline (str, type, ' ');
	//cout << type << endl;
	if (prev == "0000000000000000000000000000000000000000") {
		curBranch = "master";
		branches.insert ({"master", next});
		HEAD = next;
	} else if (type == "commit:") {
		deps.push_back ({prev, next});
		branches[curBranch] = next;
		HEAD = next;
	} else if (type == "merge") {
		deps.push_back ({prev, next});
		string branch;
		getline (str, branch, ':');
		deps.push_back ({branches[branch], next});
	} else if (type == "checkout:") {
		string to;
		for (int i = 0; i < 4; i++) {
			getline (str, trash, ' ');
		}
		getline (str, to, ' ');
		if (branches.find(to) == branches.end()) {
			branches.insert ({to, next});
		}
		HEAD = next;
		curBranch = to;
	}
}

string commitViz::formatName (const string& name) {
	return "Commit " + name.substr(0, 8) + "...";
}

string commitViz::getGraph () {
	string graph = "digraph G {\n";
	for (int i = 0; i < deps.size(); i++) {
		graph += '\"' + formatName(deps[i].first) + "\" -> \"" + formatName(deps[i].second) + "\" ";
		fileList fl1 = fileLists[deps[i].first], fl2 = fileLists[deps[i].second];
		graph += "[label=\"" + getDiff(fl1, fl2) + "\"]\n";
	}
	auto it = branches.begin();
	for (; it != branches.end(); it++) {
		graph += "\"" + formatName(it->second) + "\" [xlabel=" + it->first + "]\n";
	}
	graph += "}";
	return graph;
};

void commitViz::loadFileLists () {
	putenv("PYTHONHOME=C:\\Users\\MrCre\\AppData\\Local\\Programs\\Python\\Python311");
	Py_Initialize();
	PyRun_SimpleString ("import sys");
	PyRun_SimpleString ("sys.path.append('..')");
	PyObject *pName, *pModule, *pDict;
	pName = PyUnicode_FromString("dc");
	pModule = PyImport_Import(pName);
	pDict = PyModule_GetDict(pModule);
	this->func = PyDict_GetItemString(pDict, "dc");
	for (int i = 0; i < deps.size(); i++) {
		if (fileLists.find (deps[i].second) == fileLists.end() && fileLists.end()->first != deps[i].first) {
			fileLists.insert({deps[i].second, getFiles(deps[i].second)});
		}
	}
	Py_Finalize();
}

fileList commitViz::getFiles(const string &hash, const string& prefix) {
	string object = dc(hash);
	stringstream ss = stringstream (object);
	string type, hashBuffer;
	if (ss.peek() == 't') {
		ss >> type;
		if (type == "tree") {
			ss >> hashBuffer;
			return getFiles(hashBuffer);
		}
	} else {
		vector<pair<string, string>> vect;
		while (!ss.eof()) {
			string itemType, filename, nHash;;
			ss >> itemType >> filename >> nHash;
			if (filename == "") {
				break;
			}
			if (itemType == "40000") {
				vector<pair<string, string>> vect2 = getFiles(nHash, filename + "/");
				vect.insert(vect.end(), vect2.begin(), vect2.end());
			} else {
				vect.push_back({filename, nHash});
			}
		}
		return vect;
	}
	return {};
}

string commitViz::dc (const string& hash) {
	string filename = gitDir + "/objects/" + hash.substr(0, 2) + "/" + hash.substr(2, 38);
	PyObject* pResult = PyObject_CallFunction(func, (char*)"(s)", filename.c_str());
	if(pResult == NULL)
		cout << "File not found\n";
	string result = string (PyUnicode_AsUTF8(pResult));
	return result;
}

bool commitViz::contains (fileList fl, string name) {
	for (int i = 0; i < fl.size(); i++) {
		if (fl[i].first == name) {
			return true;
		}
	}
	return false;
}

string commitViz::getDiff(fileList fl1, fileList fl2) {
	string diff = "";
	bool has = false;
	for (int i = 0; i < fl1.size(); i++) {
		auto check = find(fl2.begin(), fl2.end(), fl1[i]);
		if (!contains(fl2, fl1[i].first)) {
			diff += "-" + fl1[i].first + "\\n";
		} else if (check->second != fl1[i].second) {
			diff += "*" + fl1[i].first + "\\n";
		}
	}
	for (int i = 0; i < fl2.size(); i++) {
		if (!contains(fl1, fl2[i].first)) {
			diff += "+" + fl2[i].first + "\\n";
		}
	}
	return diff;
}
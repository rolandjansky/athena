/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef TREEREADER_H
#define TREEREADER_H

#include <map>
#include <stack>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <typeinfo>
#include "TreeVector.h"
#include "Node.h"
#include "TFile.h"
#include "AsgTools/AsgMessaging.h"

using namespace std;

enum Format {
    ASCII,
    BINARY,
    ROOTFILE,
    UNKNOWN
};

enum {
    ENDOFTREE = -1,
    LEAF = -2,
    POINTERLEAF = -3,
    GRAPH = -4,
    FUNC = -5,
    TRANS = -6,
    GRAPH2D = -7
};

class TreeReader : public asg::AsgMessaging {

    public:

        TreeReader(const string& filename);

        ~TreeReader(){}

        void setVariables(const map<string, const float*>* _floatVariables,
                          const map<string, const int*>* _intVariables)
        {
            this->floatVariables = _floatVariables;
            this->intVariables = _intVariables;
        }

        Node* build(bool checkTree = false);

        vector<string> getRequiredVariables(char type);
	
    private:

        Node* readTree(istream& treeFile,
                       TFile* rootFile,
                       Format format,
                       vector<string>& variableList,
                       vector<char>& variableTypeList,
                       unsigned int& numNodes);
        const string m_fileName;
        const map<string, const float*>* floatVariables;
        const map<string, const int*>* intVariables;
        Format m_format = UNKNOWN;
        int m_catTreePos = -1;
        istream* m_treeInfo;
        bool m_constructionFinished = false;
        vector<string> m_binningVariableList;
        vector<char> m_binningVariableTypeList;
        vector<string> m_variableList;
        vector<char> m_variableTypeList;
        TFile* m_file = nullptr;
        ifstream m_treeFile;
        istringstream m_treeString;
};

/** This templated class is used to convert a string to various data types.
This is used when tokenizing the BDT input file to build the boosted decision trees.*/
template <class T>
inline bool from_string(T& t, const string& s) {
    istringstream ss(s);
    return !(ss >> t).fail();
}

template <class T>
inline string to_string (const T& t) {
    stringstream ss;
    ss << t;
    return ss.str();
}
#endif

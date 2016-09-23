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

//using namespace std;

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

        TreeReader(const std::string& filename);

        ~TreeReader(){}

        void setVariables(const std::map<std::string, const float*>* floatVariables,
                          const std::map<std::string, const int*>* intVariables)
        {
            this->m_floatVariables = floatVariables;
            this->m_intVariables = intVariables;
        }

        Node* build(bool checkTree = false);

        std::vector<std::string> getRequiredVariables(char type);
	
    private:

        Node* readTree(std::istream& treeFile,
                       TFile* rootFile,
                       Format format,
                       std::vector<std::string>& variableList,
                       std::vector<char>& variableTypeList,
                       unsigned int& numNodes);
        const std::string m_fileName;
        const std::map<std::string, const float*>* m_floatVariables;
        const std::map<std::string, const int*>* m_intVariables;
        Format m_format = UNKNOWN;
        int m_catTreePos = -1;
        std::istream* m_treeInfo;
        bool m_constructionFinished = false;
        std::vector<std::string> m_binningVariableList;
        std::vector<char> m_binningVariableTypeList;
        std::vector<std::string> m_variableList;
        std::vector<char> m_variableTypeList;
        TFile* m_file = nullptr;
        std::ifstream m_treeFile;
        std::istringstream m_treeString;
};

/** This templated class is used to convert a string to various data types.
This is used when tokenizing the BDT input file to build the boosted decision trees.*/
template <class T>
inline bool from_string(T& t, const std::string& s) {
    std::istringstream ss(s);
    return !(ss >> t).fail();
}

template <class T>
inline std::string to_string (const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}
#endif

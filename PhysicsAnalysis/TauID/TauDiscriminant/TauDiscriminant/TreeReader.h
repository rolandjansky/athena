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
#ifndef __STANDALONE
    #include "GaudiKernel/MsgStream.h"
#endif

using namespace std;

enum Format {
    ASCII,
    BINARY,
    ROOTFILE
};

enum {
    ENDOFTREE = -1,
    LEAF = -2,
    POINTERLEAF = -3,
    GRAPH = -4,
    FUNC = -5,
    TRANS = -6
};

class TreeReader {

    public:

        #ifdef __STANDALONE
        TreeReader(bool _verbose = false):
            verbose(_verbose),
            floatVariables(0),
            intVariables(0)
        {}
        #else
        TreeReader(bool _verbose = false, MsgStream* _log = 0):
            verbose(_verbose),
            floatVariables(0),
            intVariables(0),
            log(_log)
        {}
        #endif

        ~TreeReader(){}

        void setVariables(const map<string,const float*>* _floatVariables,
                          const map<string,const int*>* _intVariables)
        {
            this->floatVariables = _floatVariables;
            this->intVariables = _intVariables;
        }

        Node* build(const string& filename, bool checkTree = false);

        void print(string message) const
        {
            #ifndef __STANDALONE
            if (log)
            {
                (*log) << "DEBUG" << message << endreq;
            }
            else
            #endif
                cout << message << endl;
        }

    private:

        Node* readTree(istream& treeFile,
                       TFile* rootFile,
                       Format format,
                       vector<string>& variableList,
                       vector<char>& variableTypeList,
                       unsigned int& numNodes);
        bool verbose;
        const map<string,const float*>* floatVariables;
        const map<string,const int*>* intVariables;
        #ifndef __STANDALONE
        MsgStream* log;
        #endif
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

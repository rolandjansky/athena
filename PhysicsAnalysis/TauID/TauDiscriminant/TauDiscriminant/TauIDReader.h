/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef TAUIDREADER_H
#define TAUIDREADER_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

#include "TTree.h"
#include "TFile.h"
#include "TauDiscriminant/MethodBase.h"
#include "TauDiscriminant/MethodBDT.h"
#include "TauDiscriminant/MethodCuts.h"
#include "TauDiscriminant/MethodLLH.h"
#include "TauDiscriminant/MethodDummy.h"
#include "TauDiscriminant/MethodTransform.h"
#include "TauDiscriminant/TauDetailsManagerStandalone.h"


using namespace std;

namespace TauID
{
    class TauIDReader
    {
        public:

            TauIDReader(bool _verbose = false):
                verbose(_verbose),
                vectorMode(false),
                output(0),
                own_output(true)
            {
	      m_tdms = new TauDetailsManagerStandalone();

	      
	    }

            ~TauIDReader()
            {
                vector<pair<MethodBase*, vector<vector<float>* > > >::iterator it(this->methods.begin());
                for (;it != this->methods.end(); ++it)
                {
                    vector<vector<float>* >::iterator it2(it->second.begin());
                    for (; it2 != it->second.end(); ++it2)
                    {
                        delete *it2;
                    }
                    delete it->first;
                }
                vector<float*>::iterator it_float(this->methodBuffer.begin());
                for(; it_float != this->methodBuffer.end(); ++it_float)
                {
                    delete *it_float;
                }
                it_float = this->floatVariables.begin();
                for(; it_float != this->floatVariables.end(); ++it_float)
                {
                    delete *it_float;
                }
                it_float = this->vectFloatVariables.begin();
                for(; it_float != this->vectFloatVariables.end(); ++it_float)
                {
                    delete *it_float;
                }
                vector<int*>::iterator it_int(this->intVariables.begin());
                for(; it_int != this->intVariables.end(); ++it_int)
                {
                    delete *it_int;
                }
                it_int = this->vectIntVariables.begin();
                for(; it_int != this->vectIntVariables.end(); ++it_int)
                {
                    delete *it_int;
                }
                if (this->own_output)
                {
                    delete this->output;
                }
                delete m_tdms;
            }

            bool bookMethod(Types::MethodType type, const string& name, const string& filename, unsigned int numResponses = 1);

            bool addVariable(const string& name, const string& type = "F", const string& branchName="");

            bool setOutput(const string& outputFileName, const string& outputDir = "");

            bool setOutput(TFile* outputFile, const string& outputDir = "");

            bool setOutput(TFile& outputFile, const string& outputDir = "");

            TTree* classify(TTree& tree, const string& outputTreeName, bool copyTree = false);

            TTree* classify(TTree* tree, const string& outputTreeName, bool copyTree = false);

            // return the number of entries in the tree or -1 if there was a problem
            int classify(const string& inputTreeName, const string& outputTreeName,
                const string& inputFileName, const string& inputDir = "",
                bool makeFriend = true,
                bool copyTree = false);

        private:

            bool checkBranch(TTree* tree, const char* name, string type, bool checkType = true);
            void print() const;

            bool verbose;
            bool vectorMode;

            TFile* output;
            string outputDir;
            bool own_output;

            vector<string> methodNames;
            vector<pair<MethodBase*, vector<vector<float>* > > > methods;
            vector<float*> methodBuffer;
            vector<pair<string,string> > methodVariables;

            vector<int*> intVariables;
            vector<string> intNames;
            vector<string> intBranches;
            vector<float*> floatVariables;
            vector<string> floatNames;
            vector<string> floatBranches;

            vector<int*> vectIntVariables;
            vector<string> vectIntNames;
            vector<string> vectIntBranches;
            vector<float*> vectFloatVariables;
            vector<string> vectFloatNames;
            vector<string> vectFloatBranches;

            vector<string> allVariableNames;
            vector<string> allBranchNames;
	    
	    TauDetailsManagerStandalone *m_tdms;
    };
}
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include "TauDiscriminant/TauIDReader.h"
#include "TROOT.h"
#include <iomanip>

using namespace TauID;

void TauIDReader::print() const
{
    ios_base::fmtflags original_flags = cout.flags();
    vector<string>::const_iterator name_it(this->methodNames.begin());
    vector<float*>::const_iterator it_float(this->methodBuffer.begin());
    cout << "Methods:" << endl;
    for(; it_float != this->methodBuffer.end(); ++it_float)
    {
        cout << left << setw(40) << setfill('.') << *(name_it++) << ' ' << **it_float << endl;
    }
    cout << "Variables:" << endl;
    name_it = this->floatNames.begin();
    it_float = this->floatVariables.begin();
    for(; it_float != this->floatVariables.end(); ++it_float)
    {
        cout << left << setw(40) << setfill('.') << *(name_it++) << ' ' << **it_float << endl;
    }
    name_it = this->vectFloatNames.begin();
    it_float = this->vectFloatVariables.begin();
    for(; it_float != this->vectFloatVariables.end(); ++it_float)
    {
        cout << left << setw(40) << setfill('.') << *(name_it++) << ' ' << **it_float << endl;
    }
    name_it = this->intNames.begin();
    vector<int*>::const_iterator it_int(this->intVariables.begin());
    for(; it_int != this->intVariables.end(); ++it_int)
    {
        cout << left << setw(40) << setfill('.') << *(name_it++) << ' ' << **it_int << endl;
    }
    name_it = this->vectIntNames.begin();
    it_int = this->vectIntVariables.begin();
    for(; it_int != this->vectIntVariables.end(); ++it_int)
    {
        cout << left << setw(40) << setfill('.') << *(name_it++) << ' ' << **it_int << endl;
    }
    cout.flags(original_flags);
}

bool TauIDReader::bookMethod(Types::MethodType type, const string& name, const string& filename, unsigned int numResponses) {

    if (name.size() == 0)
    {
        cout << "You must supply a non-empty name" << endl;
        return false;
    }

    if (numResponses == 0)
    {
        cout << "Number of responses must be non-zero" << endl;
        return false;
    }

    if (find(methodNames.begin(),methodNames.end(),name) != methodNames.end())
    {
        cout << "Method with name " << name << " was already booked." << endl;
        return false;
    }

    MethodBase* method;
    if (type == Types::CUTS)
    {
        method = new MethodCuts(name);
	
    }
    else if (type == Types::BDT)
    {
        method = new MethodBDT(name);
    }
    else if (type == Types::LLH)
    {
        method = new MethodLLH(name);
    }
    else if (type == Types::DUMMY)
    {
        method = new MethodDummy(name);
    }
    else if (type == Types::TRANSFORM)
    {
        method = new MethodTransform(name);
    }
    else
    {
        return false;
    }

    if (verbose) cout << "Booking method " << name << endl;

    // Add variables that reference the outputs of other methods
    vector<pair<string,string> >::const_iterator methodvar_it(this->methodVariables.begin());
    vector<string>::const_iterator method_name_it;
    for (; methodvar_it != this->methodVariables.end(); ++methodvar_it)
    {
        method_name_it = find(this->methodNames.begin(),this->methodNames.end(),methodvar_it->second);
        method->addVariable(methodvar_it->first,this->methodBuffer[method_name_it - this->methodNames.begin()],'F');
    }
    
    //vector<int*>::const_iterator it_val_int(intVariables.begin());
    vector<string>::const_iterator it_name(intNames.begin());
    for (; it_name != intNames.end();)
    {
        //method->addVariable(*(it_name++),*(it_val_int++),'I');
        string valname = *(it_name++);
	const map<string,int*> *dets = m_tdms->getIntDetails();
        method->addVariable(valname,dets->at(valname),'I');
    }

    //vector<float*>::const_iterator it_val_float(floatVariables.begin());
    it_name = floatNames.begin();
    for (;/* it_val_float != floatVariables.end() &&*/ it_name != floatNames.end();)
    {
        string valname = *(it_name++);     
	const map<string,float*> *dets = m_tdms->getFloatDetails();
	std::cout<<valname<<std::endl;	
        method->addVariable(valname,dets->at(valname),'F');
    }

    //it_val_int = vectIntVariables.begin();
    it_name = vectIntNames.begin();
    for (; /*it_val_int != vectIntVariables.end() &&*/ it_name != vectIntNames.end();)
    {
        string valname = *(it_name++);    
	const map<string,int*> *dets = m_tdms->getIntDetails();
        method->addVariable(valname,dets->at(valname),'I');
    }

    //it_val_float = vectFloatVariables.begin();
    it_name = vectFloatNames.begin();
    for (; /*it_val_float != vectFloatVariables.end() &&*/ it_name != vectFloatNames.end();)
    {
        string valname = *(it_name++);      
	const map<string,float*> *dets = m_tdms->getFloatDetails();
        method->addVariable(valname,dets->at(valname),'F');
    }

    if (!method->build(filename,false))
    {
        cout << "Initializing the method " << name << " failed." << endl;
        delete method;
        return false;
    }

    this->methodNames.push_back(name);
    this->methodBuffer.push_back(new float(0.));
    this->methods.push_back(pair<MethodBase*,vector<vector<float>* > >(method,vector<vector<float>* >(numResponses, 0)));
    vector<vector<float>* >::iterator it = methods.back().second.begin();
    for (; it != methods.back().second.end(); ++it)
    {
        *it = new vector<float>();
    }
    return true;
}

bool TauIDReader::addVariable(const string& name, const string& type, const string& branchName) {

    if (this->verbose) cout << "Adding variable " << name << " --> " << branchName << endl;
    if (find(this->allVariableNames.begin(), this->allVariableNames.end(), name) != this->allVariableNames.end())
    {
        cout << "Variable " << name << " has already been booked" << endl;
        return false;
    }
//     if (find(this->allBranchNames.begin(), this->allBranchNames.end(), branchName) != this->allBranchNames.end())
//     {
//         cout << "A variable referring to branch " << branchName << " has already been booked" << endl;
//         return false;
//     }
    vector<string>::const_iterator it(find(this->methodNames.begin(),this->methodNames.end(),branchName));
    if (it != this->methodNames.end())
    {
        if (this->verbose) cout << "Branch matches name of method previously booked" << endl;
        if (type != "F")
        {
            cout << "The type of all method outputs are scalar floats" << endl;
            cout << "Variable referring to branch " << branchName << " must be of type \"F\"" << endl;
            return false;
        }
        if (this->verbose) cout << "This variable will refer to the output of that method" << endl;
        this->allVariableNames.push_back(name);
        this->allBranchNames.push_back(branchName);
        this->methodVariables.push_back(pair<string,string>(name,branchName));
        return true;
    }
    if (type == "F")
    {
        this->floatNames.push_back(name);
        this->allVariableNames.push_back(name);
        //this->floatBranches.push_back(branchName);
        //this->allBranchNames.push_back(branchName);
        this->floatVariables.push_back(new float(0.));
        return true;
    }
    else if (type == "I")
    {
        this->intNames.push_back(name);
        this->allVariableNames.push_back(name);
        //this->intBranches.push_back(branchName);
        //this->allBranchNames.push_back(branchName);
        this->intVariables.push_back(new int(0));
        return true;
    }
    else if (type == "VF")
    {
        this->vectFloatNames.push_back(name);
        this->allVariableNames.push_back(name);
        //this->vectFloatBranches.push_back(branchName);
        //this->allBranchNames.push_back(branchName);
        this->vectFloatVariables.push_back(new float(0.));
        this->vectorMode = true;
        return true;
    }
    else if (type == "VI")
    {
        this->vectIntNames.push_back(name);
        this->allVariableNames.push_back(name);
        //this->vectIntBranches.push_back(branchName);
        //this->allBranchNames.push_back(branchName);
        this->vectIntVariables.push_back(new int(0));
        this->vectorMode = true;
        return true;
    }
    cout << "Unknown variable type " << type << " for variable " << name << endl;
    return false;
}

bool TauIDReader::checkBranch(TTree* tree, const char* name, string type, bool checkType)
{
    if (!tree)
    {
        return false;
    }
    TBranch* branch = tree->GetBranch(name);
    if (!branch)
    {
        cout << "Branch " << name << " does not exist!" << endl;
        return false;
    }
    if (checkType)
    {
        if (strcmp(type.c_str(), branch->GetClassName()))
        {
            cout << "Branch " << name << " does not contain the correct type: " << branch->GetClassName() << " (expected " << type << ")" << endl;
            return false;
        }
    }
    return true;
}

bool TauIDReader::setOutput(const string& outputFileName, const string& _outputDir)
{
    if (this->own_output)
    {
        delete this->output;
    }
    this->own_output = true;
    this->output = new TFile(outputFileName.c_str(),"NEW");
    gROOT->GetListOfFiles()->Remove(this->output);
    if (!this->output->IsOpen() || this->output->IsZombie())
    {
        cout << "Could not create " << outputFileName << endl;
        return false;
    }
    if (!this->output->GetDirectory(_outputDir.c_str()))
    {
        cout << "Could not find directory " << _outputDir << " in " << outputFileName << endl;
        return false;
    }
    this->output->cd(_outputDir.c_str());
    if (this->verbose)
    {
        cout << "Output is now " << outputFileName;
        if (_outputDir != "")
        {
            cout << "/" << _outputDir;
        }
        cout << endl;
    }
    this->outputDir = _outputDir;
    return true;
}

bool TauIDReader::setOutput(TFile& outputFile, const string& _outputDir)
{
    return this->setOutput(&outputFile, _outputDir);
}

bool TauIDReader::setOutput(TFile* outputFile, const string& _outputDir)
{
    if (!outputFile)
    {
        cout << "NULL file!" << endl;
        return false;
    }
    if (!outputFile->IsOpen() || outputFile->IsZombie())
    {
        cout << "File is not open!" << endl;
        return false;
    }
    if (this->own_output)
    {
        delete this->output;
    }
    this->own_output = false;
    this->output = outputFile;
    if (!this->output->GetDirectory(_outputDir.c_str()))
    {
        cout << "Could not find directory " << _outputDir << " in " << outputFile->GetName() << endl;
        return false;
    }
    this->output->cd(_outputDir.c_str());
    if (this->verbose)
    {
        cout << "Output is now " << outputFile->GetName();
        if (outputDir != "")
        {
            cout << "/" << _outputDir;
        }
        cout << endl;
    }
    this->outputDir = _outputDir;
    return true;
}

int TauIDReader::classify(const string& inputTreeName, const string& outputTreeName,
    const string& inputFileName, const string& inputDir,
    bool makeFriend,
    bool copyTree)
{
    unsigned int entries(0);
    TFile* input = new TFile(inputFileName.c_str(), "READ");
    if (!input->IsOpen() || input->IsZombie())
    {
        cout << "Could not open " << inputFileName << endl;
        delete input;
        return -1;
    }
    // Try to cd to desired directory in input ROOT file
    if (!input->GetDirectory(inputDir.c_str()))
    {
        delete input;
        cout << "Could not find directory " << inputDir << " in " << inputFileName << endl;
        return -1;
    }
    input->cd(inputDir.c_str());
    TTree* tree = (TTree*)input->Get(inputTreeName.c_str());
    if (!tree)
    {
        cout << "Could not find tree " << inputTreeName << " in file " << inputFileName;
        if (inputDir != "")
        {
            cout << "/" << inputDir << endl;
        }
        else
        {
            cout << endl;
        }
        delete input;
        return -1;
    }
    entries = tree->GetEntries();
    TTree* outputTree = this->classify(tree, outputTreeName, copyTree);
    if (!outputTree)
    {
        delete tree;
        delete input;
        return -1;
    }
    if (makeFriend)
    {
        if (verbose)
        {
            cout << "Making input tree a friend of output tree" << endl;
        }
        if (inputDir == "")
        {
            outputTree->AddFriend(inputTreeName.c_str(), inputFileName.c_str());
        }
        else
        {
            string fullInputDir(inputFileName + "/" + inputDir);
            outputTree->AddFriend(inputTreeName.c_str(), fullInputDir.c_str());
        }
    }
    this->output->cd(this->outputDir.c_str());
    outputTree->Write("",TObject::kOverwrite);
    delete tree;
    delete input;
    delete outputTree;
    return entries;
}

TTree* TauIDReader::classify(TTree& tree, const string& outputTreeName, bool copyTree)
{
    return this->classify(&tree, outputTreeName, copyTree);
}

TTree* TauIDReader::classify(TTree* tree, const string& outputTreeName, bool copyTree)
{
    if (!this->output)
    {
        cout << "No output file has been previously specified" << endl;
        return 0;
    }
    if (!this->output->IsOpen() || this->output->IsZombie())
    {
        cout << "Output file is not open" << endl;
        return 0;
    }
    if (!tree)
    {
        cout << "Input tree is NULL!" << endl;
        return 0;
    }
    int lastStatus(0);
    unsigned long entry(0);
    unsigned long numEntries(tree->GetEntries());
    if (numEntries == 0)
    {
        cout << "The input tree has no entries!" << endl;
        return 0;
    }
    if (this->methods.size()==0)
    {
        cout << "No methods were booked successfully!" << endl;
        return 0;
    }
    if (tree->GetNbranches() == 0)
    {
        cout << "Input tree contains no branches!" << endl;
        return 0;
    }
    
    
    
    m_tdms->initTree(tree);


    vector<pair<MethodBase*,vector<vector<float>* > > >::iterator method_it;
    vector<int*>::iterator intvalue_it;
    vector<float*>::iterator floatvalue_it;
    vector<string>::const_iterator intbranch_it;
    vector<string>::const_iterator floatbranch_it;
    vector<string>::const_iterator intname_it;
    vector<string>::const_iterator floatname_it;

    
    TTree* outputTree(0);
    // this has already been checked in setOutput to succeed
    this->output->cd(this->outputDir.c_str());

    // Create the output tree
    if (copyTree)
    {
        // Make sure that all branches are activated
        tree->SetBranchStatus("*",1);

        // Deactivate the branches we will use for the Methods (if they already exist in the input tree)
        for (method_it = methods.begin(); method_it != methods.end(); ++method_it)
        {
            for (unsigned int i(0); i<(method_it->second).size(); ++i)
            {
                string name = method_it->first->getName();
                if ((method_it->second).size() > 1)
                {
                    name += to_string<unsigned int>(i);
                }
                if (tree->GetBranch(name.c_str()))
                {
                    tree->SetBranchStatus(name.c_str(),0);
                }
            }
        }
        if (verbose)
        {
            cout << "Cloning input tree... " << flush;
        }
        outputTree = tree->CloneTree(-1,"fast");
        if (!outputTree)
        {
            if (verbose) cout << "fail" << endl;
            cout << "Could not clone tree" << endl;
            return 0;
        }
        outputTree->Write("",TObject::kOverwrite);
        if (verbose)
        {
            cout << "done" << endl;
        }
        if (outputTree->GetNbranches() == 0)
        {
            cout << "The output tree contains no branches" << endl;
            cout << "They were probably all deactivated before cloning the input tree" << endl;
            delete outputTree;
            return 0;
        }
        // Separate the cloned tree from the original
        tree->CopyAddresses(outputTree, true);
        //outputTree->ResetBranchAddresses();
        //outputTree->SetBranchStatus("*",0);
    }
    else
    {
        outputTree = new TTree(outputTreeName.c_str(),outputTreeName.c_str());
    }

    // Disable all branches of the input tree
    tree->SetBranchStatus("*",0);
    // Only the required branches are reactivated below

    m_tdms->initTree(tree);

    // Scalar variables:

    // Set the addresses of the int variables
//    intvalue_it = intVariables.begin();
//    intbranch_it = intBranches.begin();
  
    
//     for ( ; intvalue_it != intVariables.end() && intbranch_it != intBranches.end(); )
//     {
// 
//         if (!checkBranch(tree,intbranch_it->c_str(),string("int"),false))
//         {
//             delete outputTree;
//             return 0;
//         }
//         
//         tree->SetBranchStatus(intbranch_it->c_str(),1);
//         if (tree->SetBranchAddress((intbranch_it++)->c_str(),*(intvalue_it++)) != 0)
//         {
//             delete outputTree;
//             return 0;
//         }
//         
//     }
//     
//     // Set the addresses of the float variables
//     floatvalue_it = floatVariables.begin();
//     floatbranch_it = floatBranches.begin();
//     for (; floatvalue_it != floatVariables.end() && floatbranch_it != floatBranches.end(); )
//     {
//         if (!checkBranch(tree,floatbranch_it->c_str(),string("float"),false))
//         {
//             delete outputTree;
//             return 0;
//         }
//         tree->SetBranchStatus(floatbranch_it->c_str(),1);
//         if (tree->SetBranchAddress((floatbranch_it++)->c_str(),*(floatvalue_it++)) != 0)
//         {
//             delete outputTree;
//             return 0;
//         }
//     }
// 
//     // Vector variables:
// 
//     vector<vector<int>* > intBuffer(vectIntVariables.size(),0);
//     vector<vector<float>* > floatBuffer(vectFloatVariables.size(),0);
// 
//     // Set the addresses of the int vector variables
//     vector<vector<int>* >::iterator intbuffer_it(intBuffer.begin());
//     intbranch_it = vectIntBranches.begin();
//     for (; intbuffer_it != intBuffer.end() && intbranch_it != vectIntBranches.end(); )
//     {
//         if (!checkBranch(tree,intbranch_it->c_str(),string("vector<int>")))
//         {
//             delete outputTree;
//             return 0;
//         }
//         tree->SetBranchStatus(intbranch_it->c_str(),1);
//         if (tree->SetBranchAddress((intbranch_it++)->c_str(),&(*(intbuffer_it++))) != 0)
//         {
//             delete outputTree;
//             return 0;
//         }
//     }
// 
//     // Set the addresses of the float vector variables
//     vector<vector<float>* >::iterator floatbuffer_it(floatBuffer.begin());
//     floatbranch_it = vectFloatBranches.begin();
//     for (; floatbuffer_it != floatBuffer.end() && floatbranch_it != vectFloatBranches.end(); )
//     {
//         if (!checkBranch(tree,floatbranch_it->c_str(),string("vector<float>")))
//         {
//             delete outputTree;
//             return 0;
//         }
//         tree->SetBranchStatus(floatbranch_it->c_str(),1);
//         if (tree->SetBranchAddress((floatbranch_it++)->c_str(),&(*(floatbuffer_it++))) != 0)
//         {
//             delete outputTree;
//             return 0;
//         }
//     }

    vector<TBranch*> outputBranches;
    
    
    // Initialize new branches in the output tree
    for (method_it = methods.begin(); method_it != methods.end(); ++method_it)
    {
        for (unsigned int i(0); i<(method_it->second).size(); ++i)
        {
            string name = method_it->first->getName();
            if ((method_it->second).size() > 1)
            {
                name += to_string<unsigned int>(i);
            }
            method_it->second[i]->clear();
            if (this->vectorMode)
            {
                outputBranches.push_back(outputTree->Branch(name.c_str(),"std::vector<float>",&(method_it->second[i])));
            }
            else
            {
                method_it->second[i]->push_back(0.);
                outputBranches.push_back(outputTree->Branch(name.c_str(),&(*(method_it->second[i]->begin())),(name+"/F").c_str()));
            }
        }
    }

//    void* sampleVector(0);
//    bool sampleIsInt(false);
    unsigned int vectorLength;

//     if (this->vectorMode)
//     {
//         if (vectIntVariables.size() > 0)
//         {
//             sampleVector = intBuffer[0];
//             sampleIsInt = true;
//         }
//         else
//         {
//             sampleVector = floatBuffer[0];
//         }
//     }

    vector<TBranch*>::const_iterator outputBranches_it;
    vector<TBranch*>::const_iterator outputBranches_end(outputBranches.end());

    // The main event loop

    for (; entry < numEntries; ++entry)
    {
        tree->GetEntry(entry);
	m_tdms->updateEvent(entry);
	
	
        if (verbose)
        {
            int newStatus = int(100*(entry+1)/numEntries);
            if (newStatus != lastStatus || entry == 0)
            {
                cout << "\rClassifying " << numEntries << " entries... " << newStatus << '%' << flush;
                lastStatus = newStatus;
            }
        }
        if (this->vectorMode)
        {
            // Assume all vectors have the same length (as they should...)
            // TODO: protect against vectors of differing length
//             if (sampleIsInt)
//                 vectorLength = static_cast<vector<int>* >(sampleVector)->size();
//             else
//                 vectorLength = static_cast<vector<float>* >(sampleVector)->size();

	    vectorLength = m_tdms->getNtau();

            for (method_it = methods.begin(); method_it != methods.end(); ++method_it)
            {
                for (unsigned int i(0); i<(method_it->second).size(); ++i)
                {
                    *(method_it->second)[i] = vector<float>(vectorLength,0.);
                }
            }
	    
            for (unsigned int j(0); j < vectorLength; ++j)
            {
		
		m_tdms->update(j);
		
                // Copy buffers to stage
/*                intbuffer_it = intBuffer.begin();
                intvalue_it = this->vectIntVariables.begin();
                for (; intbuffer_it != intBuffer.end();)
                {
                    **(intvalue_it++) = (**(intbuffer_it++))[j];
                }
                floatbuffer_it = floatBuffer.begin();
                floatvalue_it = this->vectFloatVariables.begin();
                for (; floatbuffer_it != floatBuffer.end();)
                {
                    **(floatvalue_it++) = (**(floatbuffer_it++))[j];
                }*/
                method_it = methods.begin();
                unsigned int k(0);
                // Fill the output vector containing each Method's response
                for (; method_it != methods.end(); ++method_it)
                {
                    for (unsigned int i(0); i<(method_it->second).size(); ++i)
                    {
                        float response = method_it->first->response(i);
                        *(this->methodBuffer[k]) = response;
                        (*(method_it->second[i]))[j] = response;
                    }
                    ++k;
                }
            }
        }
        else
        {
            unsigned int j(0);
            // Get the response from each Method
            for (method_it = methods.begin(); method_it != methods.end(); ++method_it)
            {
                for (unsigned int i(0); i<(method_it->second).size(); ++i)
                {
                    float response = method_it->first->response(i);
                    *(this->methodBuffer[j]) = response;
                    (*((method_it->second)[i]))[0] = response;
                }
                ++j;
            }
        }
        if (copyTree)
        {
            outputBranches_it = outputBranches.begin();
            for(; outputBranches_it != outputBranches_end; ++outputBranches_it)
            {
                if ((*outputBranches_it)->Fill() < 1)
                {
                    if (verbose)
                    {
                        cout << endl;
                    }
                    cout << "There was an error when filling the output tree" << endl;
                    delete outputTree;
                    return 0;
                }
            }
        }
        else
        {
            if (outputTree->Fill() < 1)
            {
                if (verbose)
                {
                    cout << endl;
                }
                cout << "There was an error when filling the output tree" << endl;
                delete outputTree;
                return 0;
            }
        }
    }
    if (verbose)
    {
        cout << endl;
    }
    return outputTree;
}

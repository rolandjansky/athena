/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopAnalysis/Tools.h"

#include "TopAnalysis/ObjectLoaderBase.h"
#include "TopAnalysis/EventSaverBase.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "TopEventSelectionTools/ToolLoaderBase.h"
#include "xAODMetaData/FileMetaData.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <memory>

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

#include "xAODRootAccess/Init.h"

#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/CorrectionCode.h"

#ifndef ROOTCORE
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace top {

void xAODInit(bool failOnUnchecked) {
    if (!xAOD::Init()) {
        std::cout << "Failed xAOD::Init - no idea what to do, exiting\n";
        exit(1);
    }

    //fail on unchecked error codes
    if (failOnUnchecked) {
        xAOD::TReturnCode::enableFailure();
        CP::SystematicCode::enableFailure();
        CP::CorrectionCode::enableFailure();
        //StatusCode::enableFailure();
    }
}

bool isFilePrimaryxAOD(TFile* inputFile) {
    TTree* metaData = dynamic_cast<TTree*> (inputFile->Get("MetaData"));

    if (metaData) {
        metaData->LoadTree(0);

        TObjArray* ar = metaData->GetListOfBranches();
        for (int i = 0; i < ar->GetEntries(); ++i) {
            TBranch* b = (TBranch*) ar->At(i);
            std::string name = std::string(b->GetName());
	    //            std::cout << name << std::endl;
            if (name == "StreamAOD")
                return true;
        }
    } else {
        std::cout << "isFilePrimaryxAOD says MetaData tree missing from input file.\n Weird" << std::endl;
        exit(1);
    }

    return false;
}

bool isFileSimulation(TFile* inputFile, const std::string& eventInfoName) {
    xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
    top::check(xaodEvent.readFrom(inputFile), "Tools::isFileSimulation Failed to read file in");

    xaodEvent.getEntry(0);
    const xAOD::EventInfo* eventInfo(0);
    top::check(xaodEvent.retrieve(eventInfo, eventInfoName) , "Tools::isFileSimulation Failed to get " + eventInfoName);
    return eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
}

bool isTruthDxAOD(TFile* inputFile) {
  TTree* metaData = dynamic_cast<TTree*> (inputFile->Get("MetaData"));

  if (metaData) {
    metaData->LoadTree(0);
    
    TObjArray* ar = metaData->GetListOfBranches();
    for (int i = 0; i < ar->GetEntries(); ++i) {
      TBranch* b = (TBranch*) ar->At(i);
      std::string name = std::string(b->GetName());
      //            std::cout << name << std::endl;
      if (name.find("DAOD_TRUTH") != std::string::npos)
        return true;
    }
  } else {
    throw std::runtime_error("isTruthDxAOD says MetaData tree"
                             " missing from input file.\n Weird");
  }
  
  return false;
}


unsigned int getDSID(TFile* inputFile, const std::string& eventInfoName){

  xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
  top::check(xaodEvent.readFrom(inputFile), "Tools::isFileSimulation Failed to read file in");
  
  xaodEvent.getEntry(0);
  const xAOD::EventInfo* eventInfo(0);
  top::check(xaodEvent.retrieve(eventInfo, eventInfoName) , "Tools::isFileSimulation Failed to get " + eventInfoName);
  
  return eventInfo-> mcChannelNumber();

}


bool isFileFastSim(TFile* /*inputFile*/) {

    
  return false; 
  
}


std::string getDerivationStream(TFile* inputFile){

  TTree* metaData = dynamic_cast<TTree*> (inputFile->Get("MetaData"));

  if (metaData) {
    metaData->LoadTree(0);

    TObjArray* ar = metaData->GetListOfBranches();
    for (int i = 0; i < ar->GetEntries(); ++i) {
      TBranch* b = (TBranch*) ar->At(i);
      std::string name = std::string(b->GetName());
      // Find a variable name with Stream, then split off from DAOD
      // If we only need TOPQX, we just find "_", add 1 and substring from there instead
      if (name.find("Stream") != std::string::npos){
	auto cursor        = name.find("_");
	std::string stream = name.substr(cursor+1);
	return stream;
      }	
    }
  } 
  else {
    throw std::runtime_error("getDerivationStream says MetaData tree"
                             " missing from input file.\n Weird");
  }

  // If we don't already return, something unexpected has happened and we need to fix it!
  throw std::runtime_error("Cannot determine the derivation stream. Please report.");

}
  


xAOD::TEvent::EAuxMode guessAccessMode(const std::string& filename, const std::string& electronCollectionName) {

    //there must be a better way than this
    std::unique_ptr<TFile> inputFile(TFile::Open(filename.c_str()));

    xAOD::TEvent xaodEvent(xAOD::TEvent::kBranchAccess);
    top::check( xaodEvent.readFrom(inputFile.get()) , "Failed to guessAccessMode" );

    //if we're unlucky and none of the events have electrons we can't
    //tell which mode we need -> use ClassAccess
    bool fallback = true;
    xAOD::TEvent::EAuxMode mode = xAOD::TEvent::kBranchAccess;

    if (electronCollectionName != "None") {
        //look at the first n events
        unsigned int entries = xaodEvent.getEntries();

        for (unsigned int i = 0; i < entries; ++i) {
            xaodEvent.getEntry(i);

            const xAOD::ElectronContainer* electrons(0);
            top::check( xaodEvent.retrieve(electrons, electronCollectionName) , "Failed to guessAccessMode" );

            try {
                //loading the electron pt only works for class access for the first
                //round of xAODs made for DC14 8 TeV
                //class access doesn't let us make out mini-xAOD super small though

                //branch access is available for the xAOD->xAOD reprocessing,
                //derivations and 13 TeV DC14 MC samples
                if (!electrons->empty()) {
                    fallback = false;
                    electrons->at(0)->pt();
                    break;
                }
            } catch(...) {
                mode = xAOD::TEvent::kClassAccess;
                break;
            }
        }
    }

    if (fallback) {
        std::cout << "Falling back to kClassAccess" << std::endl;
        mode = xAOD::TEvent::kClassAccess;
    }

    //useful message
    if (mode == xAOD::TEvent::kClassAccess)
        std::cout << "guessAccessMode: Using kClassAccess\n";
    else if (mode == xAOD::TEvent::kBranchAccess)
        std::cout << "guessAccessMode: Using kBranchAccess\n";

    return mode;
}

std::vector<std::string> loadCuts(const std::string& filename) {
    std::vector<std::string> v;

    std::fstream in(filename.c_str());

    if (!in.is_open()) {
        std::cout << "Problem opening " << filename << "\n";
        exit(1);
    }

    std::string str;
    while (std::getline(in, str)) {
        std::string newstring(str);

        if (str.find("#") != std::string::npos)
            newstring = str.substr(0, str.find("#"));

        //leading and trailing white space removal
        while (std::isspace(*newstring.begin()))
            newstring.erase(newstring.begin());

        while (std::isspace(*newstring.rbegin()))
            newstring.erase(newstring.length() - 1);

        //make sure it contains something
        if (newstring.size() > 0)
            v.push_back(newstring);
    }

    return v;
}

unsigned int checkFiles(const std::vector<std::string>& filenames) {
    std::cout << "Input filenames:\n";
    unsigned int i = 0;
    unsigned int totalYield = 0;

    for (const auto& filename : filenames) {
        std::unique_ptr<TFile> f(TFile::Open(filename.c_str()));
        std::cout << "    " << i + 1 << "/" << filenames.size() << " File: " << filename;

        if (!f.get()) {
            std::cout << "\nDid not manage to open " << filename << std::endl;
            std::cout << "Can't continue" << std::endl;
            exit(1);
        }

        const TTree* const t = dynamic_cast<TTree* > (f->Get("CollectionTree"));

        //some files made by the derivation framework have no events passing the
        //cuts -> which means no collection tree.
        //the old behaviour was to end execution on this problem
        //not any more!
        int entries = 0;
        std::string note;
        if (t)
            entries = t->GetEntries();
        else
            note = " (No CollectionTree)";

        std::cout << " Entries: " << entries << note << "\n";
        totalYield += entries;

        ++i;
    }

    std::cout << "\n";

    return totalYield;
}

std::vector<std::string> fileList(const std::string& filename) {
    std::ifstream ifs(filename.c_str());

    if (!ifs) {
        std::cout << "File does not exist " << filename << std::endl;
        std::cout << "This should contain a list - comma separated list of" << std::endl;
        std::cout << "input files" << std::endl;
        std::cout << "Can't continue" << std::endl;
        exit(1);
    }

    //loop over the lines in the file
    std::stringstream wholefile;

    //read the file, and replace new lines with commas
    //if the line already has comma separated filenames (a la the grid)
    //these will be included automagically.
    std::string line;
    while (!ifs.eof()) {
        std::getline(ifs, line);
        wholefile << line << ',';
    }

    //somewhere to save the filenames
    std::vector<std::string> fileList;

    //now zoom through the whole file string and split at the comma
    std::string to;
    while (std::getline(wholefile, to, ',')) {
        if (to.size() > 0)
            fileList.push_back(to);
    }

    if (fileList.size() == 0) {
        std::cout << "Could not get a list of input files from " << filename << std::endl;
        std::cout << "Can't continue" << std::endl;
        exit(1);
    }

    return fileList;
}

void loadLibraries(const std::string& libraryNames) {
    std::vector<std::string> tokens;

    std::stringstream ss(libraryNames);
    std::string item;
    char delim = ' ';
    while (std::getline(ss, item, delim))
        tokens.push_back(item);

    std::vector<std::unique_ptr<top::ToolLoaderBase>> toolLoaders;
    for (const auto& toolLoaderName : tokens) {
        std::cout << "Attempting to load library: " << toolLoaderName << ".so\n";
        gSystem->Load((toolLoaderName + ".so").c_str());
    }
}

top::TopObjectSelection* loadObjectSelection(std::shared_ptr<top::TopConfig> config) {
    std::cout << "Attempting to load ObjectSelection: " << config->objectSelectionName() << std::endl;
    TClass* c = ::TClass::GetClass( config->objectSelectionName().c_str() );

    if (c == nullptr) {
        std::cout << "Didn't manage to load " << config->objectSelectionName() << std::endl;
        exit(1);
    }

    top::ObjectLoaderBase* bc = static_cast<top::ObjectLoaderBase*> (c->New());

    if (bc == nullptr) {
        std::cout << "Didn't manage to cast it to top::ObjectLoaderBase " << std::endl;
        exit(1);
    }

    top::TopObjectSelection* objectSelection = bc->init(config);

    if (objectSelection == nullptr) {
        std::cout << "Didn't manage to make a top::ObjectSelection class" << std::endl;
        exit(1);
    }

    return objectSelection;
}

top::EventSaverBase* loadEventSaver(std::shared_ptr<top::TopConfig> config) {
    std::cout << "Attempting to load OutputFormat: " << config->outputFormat() << std::endl;
    TClass* c = ::TClass::GetClass( config->outputFormat().c_str() );

    if (c == nullptr) {
        std::cout << "Didn't manage to load " << config->outputFormat() << std::endl;
        exit(1);
    }

    top::EventSaverBase* bc = static_cast<top::EventSaverBase*> (c->New());

    if (bc == nullptr) {
        std::cout << "Didn't manage to cast it to top::EventSaverBase " << std::endl;
        exit(1);
    }

    return bc;
}

}

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// Script for generating configuration files for MuonTriggerSFFilesTest script. Configuration files are generated so that they correspond to given scale factors file.

#include <string>
#include <map>
#include <fstream>
#include <set>

#include <TFile.h>
#include <TError.h>
#include <TH2.h>
#include <TKey.h>

int main(int argc, char* argv[]) {

    const char* APP_NAME = argv[0];

    if (argc < 3) {
        Error(APP_NAME, "No scale factors or configuration file name received!");
        Error(APP_NAME, "  Usage: %s [configuration file name] [scale factors file name]", APP_NAME);
        return 1;
    }
    
    std::ofstream confFileStream(argv[1], std::ofstream::out);

    TFile* file = TFile::Open(argv[2]);
    if (!file) {
        Error(APP_NAME, " Unable to load scale factors file");
        return 1;
    }
    
    std::string quality = "Medium"; // HighPt, Tight, Loose may also be used here 
    TDirectory* qualityDirectory = file->GetDirectory(quality.c_str());
    if (!qualityDirectory) {
        Error("MuonTriggerSFConfGenerator", " Directory for quality %s is missing", quality.c_str());
        return 1;
    }
    TKey* periodKey;
    TIter nextPeriod(qualityDirectory->GetListOfKeys());
    while ((periodKey = (TKey*)nextPeriod())) {
        std::string period = periodKey->GetName();
        TDirectory* periodDirectory = qualityDirectory->GetDirectory(period.c_str());
        confFileStream << period << ": ";
        
        TKey* triggerKey;
        TIter nextTrigger(periodDirectory->GetListOfKeys());
        bool isFirst = true;
        while ((triggerKey = (TKey*)nextTrigger())) {
            if (isFirst) isFirst = false;
            else confFileStream << ", ";
            std::string trigger = triggerKey->GetName();
            confFileStream << triggerKey->GetName();
        }
        confFileStream << ";\n";
    }
    file->Close();
    delete file;
    confFileStream.close();

    return 0;
}

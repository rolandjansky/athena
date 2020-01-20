/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// Script for checking whether muon trigger scale factors file contains correct histograms

#include <string>
#include <map>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>

#include <TFile.h>
#include <TError.h>
#include <TH2.h>
#include <TKey.h>

static const std::vector<std::string> qualities { "HighPt", "Tight", "Medium", "Loose" };
static const std::vector<std::string> types { "data", "mc" };
static const std::vector<std::string> regions { "barrel", "endcap" };
static const std::vector<std::string> bins { "coarse", "fine" };
static const std::vector<std::string> systematics { "nominal", "stat_up", "stat_down", "syst_up", "syst_down" };

int main(int argc, char* argv[]) {

    const char* APP_NAME = argv[0];

    if (argc < 3) {
        Error(APP_NAME, "No scale factors or configuration file name received!");
        Error(APP_NAME, "  Usage: %s [configuration file name] [scale factors file name]", APP_NAME);
        return 1;
    }
    
    std::ifstream confFileStream(argv[1], std::ifstream::in);
    if (!confFileStream.good()) {
        Error(APP_NAME, " Unable to load configuration file");
        return 1;
    }
    
    std::map<std::string, std::set<std::string>> triggersByPeriods;
    std::string periodInfo;
    while (std::getline(confFileStream, periodInfo, ';')) {
        // Ignore spaces and line changes
        periodInfo.erase(std::remove(periodInfo.begin(), periodInfo.end(), ' '), periodInfo.end());
        periodInfo.erase(std::remove(periodInfo.begin(), periodInfo.end(), '\n'), periodInfo.end());
        if (periodInfo.empty()) continue;
        
        std::stringstream ss;
        ss.str(periodInfo);
        std::string period;
        if (!std::getline(ss, period, ':')) {
            Error(APP_NAME, " Configuration file contains invalid period description: %s", periodInfo.c_str());
            return 1;
        }
        if (triggersByPeriods.find(period) != triggersByPeriods.end()) {
            Error(APP_NAME, " Period %s is defined multiple times in configuration file", period.c_str());
        }
        
        std::set<std::string> triggers;
        std::string trigger;
        while (std::getline(ss, trigger, ',')) {
            triggers.insert(trigger);
        }
        triggersByPeriods[period] = triggers;
    }
    confFileStream.close();


    TFile* file = TFile::Open(argv[2]);
    if (!file) {
        Error(APP_NAME, " Unable to load scale factors file");
        return 1;
    }
    
    for (size_t i = 0; i < qualities.size(); i++) {
        TDirectory* qualityDirectory = file->GetDirectory(qualities[i].c_str());
        if (!qualityDirectory) {
            Error(APP_NAME, " Directory for quality %s is missing", qualities[i].c_str());
            continue;
        }
        TKey* periodKey;
        TIter nextPeriod(qualityDirectory->GetListOfKeys());
        while ((periodKey = (TKey*)nextPeriod())) {
            if (triggersByPeriods.find(periodKey->GetName()) == triggersByPeriods.end()) {
                Error(APP_NAME, " Quality %s contains period %s, which is not defined in configuration file", qualities[i].c_str(), periodKey->GetName());
            }
        }
        
        for (auto const& x : triggersByPeriods)
        {
            std::string period = x.first;
            std::set<std::string> triggers = x.second;
            TDirectory* periodDirectory = qualityDirectory->GetDirectory(period.c_str());
            if (!periodDirectory) {
                Error(APP_NAME, " Configuration file contains period %s, but this is missing in scale factors file under quality %s", period.c_str(), qualities[i].c_str());
                continue;
            }
            
            TKey* triggerKey;
            TIter nextTrigger(periodDirectory->GetListOfKeys());
            while ((triggerKey = (TKey*)nextTrigger())) {
                if (triggers.find(triggerKey->GetName()) == triggers.end()) {
                    Error(APP_NAME, " Period %s under quality %s contains trigger %s, which is not defined in configuration file", period.c_str(), qualities[i].c_str(), triggerKey->GetName());
                }
            }
        
            for (auto const& trigger : triggers)
            {
                TDirectory* triggerDirectory = periodDirectory->GetDirectory(trigger.c_str());
                if (!triggerDirectory) {
                    Error(APP_NAME, " Configuration file contains trigger %s under period %s, but this is missing in scale factors file under quality %s", trigger.c_str(), period.c_str(), qualities[i].c_str());
                    continue;
                }
                
                for (size_t ibins = 0; ibins < bins.size(); ++ibins) {
                    for (size_t iregion = 0; iregion < regions.size(); ++iregion) {
                        for (size_t itype = 0; itype < types.size(); ++itype) {
                            for (size_t isys = 0; isys < systematics.size(); ++isys) {
                                if (types[itype].find("data") != std::string::npos && systematics[isys].find("syst") != std::string::npos) continue;
                                std::string path = "eff_etaphi_" + bins[ibins] + "_" + regions[iregion] + "_" + types[itype] + "_" + systematics[isys];
                                TH2* hist = dynamic_cast<TH2*>(triggerDirectory->Get(path.c_str()));
                                if (!hist) {
                                    Error(APP_NAME, " Histogram %s not found under trigger %s, period %s and quality %s", path.c_str(), trigger.c_str(), period.c_str(), qualities[i].c_str());
                                    continue;
                                }
                                if (hist->GetEntries() == 0) {
                                    Error(APP_NAME, " Histogram %s under trigger %s, period %s and quality %s is empty", path.c_str(), trigger.c_str(), period.c_str(), qualities[i].c_str());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    file->Close();
    delete file;

    return 0;
}

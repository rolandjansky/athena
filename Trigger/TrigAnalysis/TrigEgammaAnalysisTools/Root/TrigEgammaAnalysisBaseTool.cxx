/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaAnalysisBaseTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Generic Base tool class. Inherits from AsgTool.
 *      Used for handling histograms and TTrees in derived classes
 *      Relies on THistSvc. For dual-use need to find alternative 
 **********************************************************************/
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include "StoreGate/StoreGateSvc.h"
#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"
#include "TrigEgammaAnalysisTools/ValidationException.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
using namespace std;
//**********************************************************************

TrigEgammaAnalysisBaseTool::
TrigEgammaAnalysisBaseTool( const std::string& myname )
    : AsgTool(myname),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool") 
{
    declareProperty("MatchTool",m_matchTool);
    declareProperty("ElectronKey",m_offElContKey="Electrons");
    declareProperty("File",m_file="Validation_Zee");
    }

//**********************************************************************

StatusCode TrigEgammaAnalysisBaseTool::initialize() {

    ATH_MSG_VERBOSE( "Initialising tool " << name() );

    StatusCode sc;

    m_storeGate = 0;
    m_histsvc = 0;
    sc = service("StoreGateSvc", m_storeGate);
    if(sc.isFailure()) {
        ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
        return sc;
    }

    sc = service("THistSvc", m_histsvc);
    if( sc.isFailure() ) {
        ATH_MSG_ERROR("Unable to locate Service THistSvc");
        return sc;
    }

    /*if ((sc = service("ToolSvc", m_toolSvc)).isFailure()) {
      (*m_log) << MSG::ERROR << "Unable to get ToolSvc!" << endreq;
      return StatusCode::FAILURE;
      }*/

    // Clear map of histograms first ... don't delete anything, as ROOT takes care of this!
    if (m_hist1.size() != 0)
        m_hist1.clear();
    if (m_hist2.size() != 0)
        m_hist2.clear();
    if (m_tree.size() != 0)
        m_tree.clear();

    try {
        ATH_MSG_DEBUG("child Initialize " << name());
        sc = childInitialize();
    } catch(const ValidationException &e) {
        ATH_MSG_ERROR("Exception thrown: " << e.msg());
        return StatusCode::FAILURE;
    } catch(...) {
        ATH_MSG_ERROR("Unknown exception caught, while initializing");
        return StatusCode::FAILURE;
    }
    return sc;
}

StatusCode TrigEgammaAnalysisBaseTool::execute() {

    ATH_MSG_DEBUG( "Executing tool " << name() );

    StatusCode sc = StatusCode::FAILURE;
    try {
        ATH_MSG_DEBUG("Running execute() for " << name());
        sc = childExecute();
    } catch(const ValidationException &e) {
        ATH_MSG_ERROR("Exception thrown: " << e.msg() );
        return StatusCode::FAILURE;
    } catch(...) {
        sc.ignore();
        ATH_MSG_ERROR("Unknown exception caught, while filling histograms");
        return StatusCode::FAILURE;
    }
    return sc;
}

StatusCode TrigEgammaAnalysisBaseTool::finalize() {

    ATH_MSG_VERBOSE( "Finalizing tool " << name() );
    StatusCode sc;
    try {
        sc = childFinalize();
    } catch(const ValidationException &e) {
        ATH_MSG_ERROR("Exception thrown: " << e.msg() );
        return StatusCode::FAILURE;
    } catch(...) {
        ATH_MSG_ERROR("Unknown exception caught, while processing histograms");
        return StatusCode::FAILURE;
    }
    return sc;
}

void TrigEgammaAnalysisBaseTool::print() const {
    AsgTool::print();
    ATH_MSG_INFO("   Message: \"" << m_msg << "\"");
    ATH_MSG_INFO("  MsgLevel: " << MSG::name( msg().currentLevel() ) );
    ATH_MSG_ALWAYS("This message always appears.");
}

void TrigEgammaAnalysisBaseTool::cd(const std::string &dir) {
    // Check if it exists!
    m_currentDir = dir;
}

void TrigEgammaAnalysisBaseTool::addDirectory(const std::string &dir) {
    m_dir.push_back(dir);
    cd(dir);
}

void TrigEgammaAnalysisBaseTool::addHistogram(TH1 *h, const std::string &dir) {
    if (!h)
        throw ValidationException("TrigEgammaAnalysisBaseTool::addHistogram(TH1 *h == 0, ...)!");

    std::vector<std::string>::iterator dirItr;
    std::string theDir;
    if (dir == "") {
        theDir = m_currentDir;
    } else {
        theDir = dir;
    }

    dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
    if (dirItr == m_dir.end())
        throw ValidationException(std::string(std::string("Adding histogram ") + h->GetName()) + std::string(" failed: no directory named ") + theDir);

    std::stringstream ss;
    ss << "/" << m_file << "/" << theDir << "/" << h->GetName();

    StatusCode sc;
    sc = m_histsvc->regHist(ss.str(), h);
    if (sc.isFailure()) {
        throw ValidationException(std::string("Failure registering histogram ") + ss.str());
    }

    ATH_MSG_DEBUG("Registered histogram " << ss.str());
    m_hist1.insert(std::pair<std::string, TH1 *>(ss.str(), h));
}

void TrigEgammaAnalysisBaseTool::addHistogram(TH2 *h, const std::string &dir) {
    if (!h)
        throw ValidationException("TrigEgammaAnalysisBaseTool::addHistogram(TH2 *h == 0, ...)!");

    std::vector<std::string>::iterator dirItr;
    std::string theDir;
    if (dir == "") {
        theDir = m_currentDir;
    } else {
        theDir = dir;
    }

    dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
    if (dirItr == m_dir.end())
        throw ValidationException(std::string(std::string("Adding histogram ") + h->GetName()) + std::string(" failed: no directory named ") + theDir);

    std::stringstream ss;
    ss << "/" << m_file << "/" << theDir << "/" << h->GetName();

    StatusCode sc;
    sc = m_histsvc->regHist(ss.str(), h);
    if (sc.isFailure()) {
        throw ValidationException(std::string("Failure registering histogram ") + ss.str());
    }

    m_hist2.insert(std::pair<std::string, TH2 *>(ss.str(), h));
}

void TrigEgammaAnalysisBaseTool::addTree(TTree *t, const std::string &dir) {
    if (!t)
        throw ValidationException("TrigEgammaAnalysisBaseTool::addTree(TTree *t == 0, ...)!");

    std::vector<std::string>::iterator dirItr;
    std::string theDir;
    if (dir == "") {
        theDir = m_currentDir;
    } else {
        theDir = dir;
    }

    dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
    if (dirItr == m_dir.end())
        throw ValidationException(std::string(std::string("Adding tree ") + t->GetName()) + std::string(" failed: no directory named ") + theDir);

    std::stringstream ss;
    ss << "/" << m_file << "/" << theDir << "/" << t->GetName();

    StatusCode sc;
    sc = m_histsvc->regTree(ss.str(), t);
    if (sc.isFailure()) {
        throw ValidationException(std::string("Failure registering tree ") + ss.str());
    }

    m_tree.insert(std::pair<std::string, TTree *>(ss.str(), t));
}

void TrigEgammaAnalysisBaseTool::getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret) {
    for (std::map<std::string, TH1 *>::const_iterator i = m_hist1.begin(); i != m_hist1.end(); i++) {
        bool goodToGo = true;
        for (std::vector<std::string>::const_iterator pat_it = pattern.begin(); pat_it != pattern.end(); pat_it++) {
            if (i->first.find(*pat_it) == std::string::npos) {
                goodToGo = false;
                break;
            }
        }

        if (!goodToGo)
            continue;

        for (std::vector<std::string>::const_iterator pat_it = notpattern.begin(); pat_it != notpattern.end(); pat_it++) {
            if (i->first.find(*pat_it) != std::string::npos) {
                goodToGo = false;
                break;
            }
        }
        if (goodToGo)
            ret.insert(std::pair<std::string, TH1 *>(i->first, i->second));
    }
}

std::string TrigEgammaAnalysisBaseTool::getPath(const std::string &histName, const std::string &dir) {
    std::string theDir;

    std::vector<std::string>::iterator dirItr;
    if (dir == "") {
        theDir = m_currentDir;
    } else {
        theDir = dir;
    }

    dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
    if (dirItr == m_dir.end())
        throw ValidationException(std::string(std::string("Getting object ") + histName) + std::string(" failed: no directory named ") + theDir);

    std::stringstream ss;
    ss << "/" << m_file << "/" << theDir << "/" << histName;
    return ss.str();
}

TH1 *TrigEgammaAnalysisBaseTool::hist1(const std::string &histName, const std::string &dir) {
    std::string fullPath = getPath(histName, dir);
    std::map<std::string, TH1 *>::iterator histItr = m_hist1.find(fullPath);
    if (histItr == m_hist1.end())
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!histItr->second)
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

    return histItr->second;
}

TH2 *TrigEgammaAnalysisBaseTool::hist2(const std::string &histName, const std::string &dir) {
    std::string fullPath = getPath(histName, dir);
    std::map<std::string, TH2 *>::iterator histItr = m_hist2.find(fullPath);
    if (histItr == m_hist2.end())
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!histItr->second)
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

    return histItr->second;
}
TTree *TrigEgammaAnalysisBaseTool::tree(const std::string &treeName, const std::string &dir) {
    std::string fullPath = getPath(treeName, dir);
    std::map<std::string, TTree *>::iterator treeItr = m_tree.find(fullPath);
    if (treeItr == m_tree.end())
        throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!treeItr->second)
        throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

    return treeItr->second;
}


void TrigEgammaAnalysisBaseTool::parseTriggerName(const std::string trigger, std::string defaultPid,std::string &type, 
        float &threshold, float &l1threshold, std::string &l1type, std::string &pidname, bool &etcut, bool &perf){
    std::string l1item = getL1Item(trigger);
    ATH_MSG_DEBUG("Trigger L1item " << trigger << " " << l1item);
    std::vector<std::string> strs;
    boost::split(strs,trigger,boost::is_any_of("_"));
    for (std::vector<std::string>::iterator it = strs.begin(); it != strs.end(); ++it)
    {
        ATH_MSG_DEBUG("Trigger parse "  << *it);
    }
    // Set probe Pid from second part of trigger name
    // Non pid triggers use default Probe which is set as a property
   

    if(boost::contains(strs.at(0),"e")) type = "electron";
    else if(boost::contains(strs.at(0),"g")) type = "photon";
    else ATH_MSG_ERROR("Cannot set trigger type from name");
    if(boost::contains(strs.at(1),"perf")){
        pidname = defaultPid;
        perf=true;
        ATH_MSG_DEBUG("Perf " << perf << " " << pidname );
    }
    else if(boost::contains(strs.at(1),"L2Star")){
        pidname = defaultPid; 
        perf=true;
        ATH_MSG_DEBUG("L2Star " << perf << " " << pidname );
    }
    else if(boost::contains(strs.at(1),"hiptrt")){
        pidname = defaultPid; 
        perf=true;
        ATH_MSG_DEBUG("hiptrt " << perf << " " << pidname );
    }
    else if( strs.at(1)== "etcut"){
        pidname = defaultPid;
        etcut=true;
    }
    else pidname = getProbePid(strs.at(1));

    //Get the L1 information

    if(boost::contains(strs.back(),"L1")){
        std::string l1info = strs.back();
        l1info.erase(0,4);
        l1type = boost::trim_copy_if(l1info, boost::is_digit());
        std::string l1cut = boost::trim_copy_if(l1info, !boost::is_digit());
        l1threshold = atof(l1cut.c_str());

        ATH_MSG_DEBUG("L1 item " << l1info << " " << l1threshold << " " << l1type);
    }

    // Get the threshold
    std::string str_thr = strs.at(0);
    str_thr.erase( 0, 1);
    threshold = atof(str_thr.c_str());

    ATH_MSG_DEBUG(trigger << " " << type << " " << pidname << " " << threshold); 

}
std::string TrigEgammaAnalysisBaseTool::getProbePid(const std::string pidtype){
    static std::map<std::string,std::string> m_PidMap; //no longer class member but static
    if(m_PidMap.empty()){
        m_PidMap["vloose"]="Loose";
        m_PidMap["loose"]="Loose";
        m_PidMap["medium"]="Medium";
        m_PidMap["tight"]="Tight";
        m_PidMap["loose1"]="Loose";
        m_PidMap["medium1"]="Medium";
        m_PidMap["tight1"]="Tight";
        m_PidMap["lhvloose"]="LHLoose";
        m_PidMap["lhloose"]="LHLoose";
        m_PidMap["lhmedium"]="LHMedium";
        m_PidMap["lhtight"]="LHTight";
    }
    return m_PidMap[pidtype];
}

std::string TrigEgammaAnalysisBaseTool::getL1Item(std::string trigger){

    static std::map<std::string,std::string> m_triggerMap; //no longer class member but static
    if(m_triggerMap.empty()) { //Only initialize once
        m_triggerMap["e24_medium_iloose_L1EM18VH"]=           "L1_EM18VH";
        m_triggerMap["e24_medium_iloose_L1EM20VH"]=           "L1_EM20VH";
        m_triggerMap["e24_tight_iloose_L1EM20VH"]=            "L1_EM20VH";
        m_triggerMap["e24_tight_iloose"]=                     "L1_EM20VHI";
        m_triggerMap["e26_tight_iloose"]=                     "L1_EM22VHI";
        m_triggerMap["e60_medium"]=                           "L1_EM22VHI";
        m_triggerMap["e120_loose"]=                           "L1_EM22VHI";
        m_triggerMap["e140_loose"]=                           "L1_EM22VHI";
        m_triggerMap["e24_lhmedium_iloose_L1EM18VH"]=         "L1_EM18VH";
        m_triggerMap["e24_lhmedium_iloose_L1EM20VH"]=         "L1_EM20VH";
        m_triggerMap["e24_lhtight_iloose_L1EM20VH"]=          "L1_EM20VH";
        m_triggerMap["e24_lhtight_iloose"]=                   "L1_EM20VHI";
        m_triggerMap["e26_lhtight_iloose"]=                   "L1_EM22VHI";
        m_triggerMap["e60_lhmedium"]=                         "L1_EM22VHI";
        m_triggerMap["e120_lhloose"]=                         "L1_EM22VHI";
        m_triggerMap["e140_lhloose"]=                         "L1_EM22VHI";
        m_triggerMap["e24_vloose_L1EM18VH"]=                  "L1_EM18VH";
        m_triggerMap["e24_vloose_L1EM20VH"]=                  "L1_EM20VH";
        m_triggerMap["e26_vloose_L1EM20VH"]=                  "L1_EM20VH";
        m_triggerMap["e24_medium_L1EM18VH"]=                  "L1_EM18VH";
        m_triggerMap["e24_tight_L1EM20VH"]=                   "L1_EM20VH";
        m_triggerMap["e24_lhvloose_L1EM18VH"]=                "L1_EM18VH";
        m_triggerMap["e24_lhvloose_L1EM20VH"]=                "L1_EM20VH";
        m_triggerMap["e26_lhvloose_L1EM20VH"]=                "L1_EM20VH";
        m_triggerMap["e24_lhmedium_L1EM18VH"]=                "L1_EM18VH";
        m_triggerMap["e24_lhtight_L1EM20VH"]=                 "L1_EM20VH";
        m_triggerMap["e24_medium_iloose_L2StarA_L1EM20VH"]=   "L1_EM20VH";
        m_triggerMap["e24_medium_L2Star_idperf_L1EM20VH"]=    "L1_EM20VH";
        m_triggerMap["e24_lhmedium_iloose_L2StarA_L1EM20VH"]= "L1_EM20VH";
        m_triggerMap["e24_lhmedium_L2Star_idperf_L1EM20VH"]=  "L1_EM20VH";
        m_triggerMap["e5_loose"]=                             "L1_EM3";
        m_triggerMap["e5_loose_L2StarA"]=                     "L1_EM3";
        m_triggerMap["e5_loose_L2Star_idperf"]=               "L1_EM3";
        m_triggerMap["e5_loose_idperf"]=                     "L1_EM3";
        m_triggerMap["e24_medium_idperf_L1EM20VH"]=          "L1_EM20VH";
        m_triggerMap["e5_lhloose"]=                          "L1_EM3";
        m_triggerMap["e5_lhloose_L2StarA"]=                  "L1_EM3";
        m_triggerMap["e5_lhloose_L2Star_idperf"]=            "L1_EM3";
        m_triggerMap["e5_lhloose_idperf"]=                   "L1_EM3";
        m_triggerMap["e24_lhmedium_idperf_L1EM20VH"]=        "L1_EM20VH";
        m_triggerMap["e5_vloose"]=                           "L1_EM3";
        m_triggerMap["e10_vloose_L1EM7"]=                    "L1_EM7";
        m_triggerMap["e15_vloose_L1EM7"]=                    "L1_EM7";
        m_triggerMap["e20_vloose_L1EM12"]=                   "L1_EM12";
        m_triggerMap["e25_vloose_L1EM15"]=                   "L1_EM15";
        m_triggerMap["e30_vloose_L1EM15"]=                   "L1_EM15";
        m_triggerMap["e40_vloose_L1EM15"]=                   "L1_EM15";
        m_triggerMap["e50_vloose_L1EM15"]=                   "L1_EM15";
        m_triggerMap["e60_vloose"]=                          "L1_EM22VHI";
        m_triggerMap["e70_vloose"]=                          "L1_EM22VHI";
        m_triggerMap["e80_vloose"]=                          "L1_EM22VHI";
        m_triggerMap["e100_vloose"]=                         "L1_EM22VHI";
        m_triggerMap["e120_vloose"]=                         "L1_EM22VHI";
        m_triggerMap["e12_vloose_L1EM10VH"]=                 "L1_EM10VH";
        m_triggerMap["e15_vloose_L1EM13VH"]=                 "L1_EM13VH";
        m_triggerMap["e17_vloose"]=                          "L1_EM15VH";
        m_triggerMap["e20_vloose"]=                          "L1_EM15VH";
        m_triggerMap["e5_lhvloose"]=                         "L1_EM3";
        m_triggerMap["e10_lhvloose_L1EM7"]=                  "L1_EM7";
        m_triggerMap["e15_lhvloose_L1EM7"]=                  "L1_EM7";
        m_triggerMap["e20_lhvloose_L1EM12"]=                 "L1_EM12";
        m_triggerMap["e25_lhvloose_L1EM15"]=                 "L1_EM15";
        m_triggerMap["e30_lhvloose_L1EM15"]=                 "L1_EM15";
        m_triggerMap["e40_lhvloose_L1EM15"]=                 "L1_EM15";
        m_triggerMap["e50_lhvloose_L1EM15"]=                 "L1_EM15";
        m_triggerMap["e60_lhvloose"]=                        "L1_EM22VHI";
        m_triggerMap["e70_lhvloose"]=                        "L1_EM22VHI";
        m_triggerMap["e80_lhvloose"]=                        "L1_EM22VHI";
        m_triggerMap["e100_lhvloose"]=                       "L1_EM22VHI";
        m_triggerMap["e120_lhvloose"]=                       "L1_EM22VHI";
        m_triggerMap["e12_lhvloose_L1EM10VH"]=               "L1_EM10VH";
        m_triggerMap["e15_lhvloose_L1EM13VH"]=               "L1_EM13VH";
        m_triggerMap["e17_lhvloose"]=                        "L1_EM15VH";
        m_triggerMap["e20_lhvloose"]=                        "L1_EM15VH";
        m_triggerMap["e17_loose_L1EM15VHJJ1523ETA49"]=       "L1_EM15VH_JJ15.23ETA49";
        m_triggerMap["e17_lhloose_L1EM15VHJJ1523ETA49"]=     "L1_EM15VH_JJ15.23ETA49";
        m_triggerMap["e0_perf_L1EM15"]=              "L1_EM15";
        m_triggerMap["e0_L2Star_perf_L1EM15"]=       "L1_EM15";
        m_triggerMap["e5_etcut"]=                             "L1_EM3";
        m_triggerMap["e10_etcut_L1EM7"]=                      "L1_EM7";
        m_triggerMap["e15_etcut_L1EM7"]=                      "L1_EM7";
        m_triggerMap["e20_etcut_L1EM12"]=                     "L1_EM12";
        m_triggerMap["e25_etcut_L1EM15"]=                     "L1_EM15";
        m_triggerMap["e30_etcut_L1EM15"]=                     "L1_EM15";
        m_triggerMap["e40_etcut_L1EM15"]=                     "L1_EM15";
        m_triggerMap["e50_etcut_L1EM15"]=                     "L1_EM15";
        m_triggerMap["e60_etcut"]=                            "L1_EM22VHI";
        m_triggerMap["e80_etcut"]=                            "L1_EM22VHI";
        m_triggerMap["e100_etcut"]=                           "L1_EM22VHI";
        m_triggerMap["e120_etcut"]=                           "L1_EM22VHI";
        m_triggerMap["e4_etcut"]=                             "L1_EM3";
        m_triggerMap["e9_etcut"]=                             "L1_EM7";
        m_triggerMap["e14_etcut"]=                            "L1_EM12";
        m_triggerMap["e9_medium"]=                            "L1_EM7";
        m_triggerMap["e12_loose"]=                            "L1_EM8VH";
        m_triggerMap["e12_loose_L1EM10VH"]=                   "L1_EM8VH";
        m_triggerMap["e15_loose_L1EM13VH"]=                   "L1_EM13VH";
        m_triggerMap["e17_loose_L1EM15"]=                     "L1_EM15";
        m_triggerMap["e17_loose"]=                            "L1_EM15VH";
        m_triggerMap["e17_medium"]=                           "L1_EM15VH";
        m_triggerMap["e17_medium_iloose"]=                    "L1_EM15VH";
        m_triggerMap["e20_medium"]=                           "L1_EM15VH";
        m_triggerMap["e24_medium_L1EM15VH"]=                  "L1_EM15VH";
        m_triggerMap["e60_loose"]=                            "L1_EM22VHI";
        m_triggerMap["e70_loose"]=                            "L1_EM22VHI";
        m_triggerMap["e5_tight"]=                             "L1_EM3";
        m_triggerMap["e9_tight"]=                             "L1_EM7";
        m_triggerMap["e14_tight"]=                            "L1_EM12";
        m_triggerMap["e9_lhmedium"]=                          "L1_EM7";
        m_triggerMap["e12_lhloose"]=                          "L1_EM8VH";
        m_triggerMap["e12_lhloose_L1EM10VH"]=                 "L1_EM10VH";
        m_triggerMap["e15_lhloose_L1EM13VH"]=                 "L1_EM13VH";
        m_triggerMap["e17_lhloose_L1EM15"]=                   "L1_EM15";
        m_triggerMap["e17_lhloose"]=                          "L1_EM15VH";
        m_triggerMap["e17_lhmedium"]=                         "L1_EM15VH";
        m_triggerMap["e17_lhmedium_iloose"]=                  "L1_EM15VH";
        m_triggerMap["e20_lhmedium"]=                         "L1_EM15VH";
        m_triggerMap["e24_lhmedium_L1EM15VH"]=                "L1_EM15VH";
        m_triggerMap["e60_lhloose"]=                          "L1_EM22VHI";
        m_triggerMap["e70_lhloose"]=                          "L1_EM22VHI";
        m_triggerMap["e5_lhtight"]=                           "L1_EM3";
        m_triggerMap["e9_lhtight"]=                           "L1_EM7";
        m_triggerMap["e14_lhtight"]=                          "L1_EM12";
        m_triggerMap["e24_tight_iloose_L2EFCalo_L1EM20VH"]=    "L1_EM20VH";
        m_triggerMap["e24_lhtight_iloose_L2EFCalo_L1EM20VH"]=  "L1_EM20VH";
        m_triggerMap["e24_tight_iloose_HLTCalo_L1EM20VH"]=     "L1_EM20VH";
        m_triggerMap["e24_lhtight_iloose_HLTCalo_L1EM20VH"]=   "L1_EM20VH";
        m_triggerMap["e24_tight_iloose_etisem_L1EM20VH"]=      "L1_EM20VH";
        m_triggerMap["g120_loose_HLTCalo"]=                    "L1_EM22VHI";
        m_triggerMap["g35_medium_HLTCalo_g25_medium_HLTCalo"]= "L1_2EM15VH";
        m_triggerMap["e24_lhmedium_nod0_L1EM18VH"]=            "L1_EM18VH";
        m_triggerMap["e24_lhmedium_nodphi_L1EM18VH"]=          "L1_EM18VH";
        m_triggerMap["e24_lhmedium_nodphires_L1EM18VH"]=       "L1_EM18VH";
        m_triggerMap["e24_lhmedium_cutd0dphi_L1EM18VH"]=       "L1_EM18VH";
        m_triggerMap["e24_lhmedium_nod0_iloose_L1EM18VH"]=            "L1_EM18VH";
        m_triggerMap["e24_lhmedium_nodphi_iloose_L1EM18VH"]=          "L1_EM18VH";
        m_triggerMap["e24_lhmedium_nodphires_iloose_L1EM18VH"]=       "L1_EM18VH";
        m_triggerMap["e24_lhmedium_cutd0dphi_iloose_L1EM18VH"]=       "L1_EM18VH";
        m_triggerMap["e120_loose1"]=  "L1_EM22VHI";
        m_triggerMap["e24_medium1_iloose_L1EM18VH"]=  "L1_EM18VH";
        m_triggerMap["e24_medium1_L1EM18VH"]=            "L1_EM18VH";
        m_triggerMap["e24_tight1_iloose_L1EM20VH"]=      "L1_EM20VH";
        m_triggerMap["e24_tight1_iloose"]=               "L1_EM20VHI";
        m_triggerMap["e26_tight1_iloose"]=               "L1_EM22VHI";
        m_triggerMap["e60_medium1"]=                     "L1_EM22VHI";
        m_triggerMap["2e12_loose_L12EM10VH"]=            "L1_2EM10VH";
        m_triggerMap["2e15_loose_L12EM13VH"]=            "L1_2EM13VH";
        m_triggerMap["2e17_loose_L12EM15"]=              "L1_2EM15";
        m_triggerMap["2e17_loose"]=                      "L1_2EM15VH";
        m_triggerMap["2e12_lhloose_L12EM10VH"]=          "L1_2EM10VH";
        m_triggerMap["2e15_lhloose_L12EM13VH"]=          "L1_2EM13VH";
        m_triggerMap["2e17_lhloose"]=                    "L1_2EM15VH";
        m_triggerMap["2e17_lhloose_L12EM15"]=            "L1_2EM15";
        m_triggerMap["e17_medium_2e9_medium"]=           "L1_EM15VH_3EM7";
        m_triggerMap["e17_loose_2e9_loose"]=             "L1_EM15VH_3EM7";
        m_triggerMap["e17_lhloose_2e9_lhloose"]=         "L1_EM15VH_3EM7";
        m_triggerMap["e17_medium_iloose_2e9_medium"]=    "L1_EM15VH_3EM7";
        m_triggerMap["e17_lhmedium_2e9_lhmedium"]=       "L1_EM15VH_3EM7";
        m_triggerMap["e17_lhmedium_iloose_2e9_lhmedium"]="L1_EM15VH_3EM7";
        m_triggerMap["2e12_loose1_L12EM10VH"]=            "L1_2EM10VH";
        m_triggerMap["2e17_loose1"]=                      "L1_2EM15VH";
        m_triggerMap["e5_tight_e4_etcut"]=                                "L1_2EM3";
        m_triggerMap["e5_tight_e4_etcut_Jpsiee"]=                         "L1_2EM3";
        m_triggerMap["e9_tight_e4_etcut_Jpsiee"]=                         "L1_EM7_2EM3";
        m_triggerMap["e9_etcut_e5_tight_Jpsiee"]=                         "L1_EM7_2EM3";
        m_triggerMap["e14_tight_e4_etcut_Jpsiee"]=                        "L1_EM12_2EM3";
        m_triggerMap["e14_etcut_e5_tight_Jpsiee"]=                        "L1_EM12_2EM3";
        m_triggerMap["e5_lhtight_e4_etcut"]=                              "L1_2EM3";
        m_triggerMap["e5_lhtight_e4_etcut_Jpsiee"]=                       "L1_2EM3";
        m_triggerMap["e9_lhtight_e4_etcut_Jpsiee"]=                       "L1_EM7_2EM3";
        m_triggerMap["e9_etcut_e5_lhtight_Jpsiee"]=                       "L1_EM7_2EM3";
        m_triggerMap["e14_lhtight_e4_etcut_Jpsiee"]=                      "L1_EM12_2EM3";
        m_triggerMap["e14_etcut_e5_lhtight_Jpsiee"]=                      "L1_EM12_2EM3";
        m_triggerMap["e5_tight1_e4_etcut"]=         "L1_2EM3";
        m_triggerMap["e5_tight1_e4_etcut_Jpsiee"]=  "L1_2EM3";
        m_triggerMap["e5_tight_e4_etcut_L1JPSI-1M5"]=                                "L1_JPSI-1M5";
        m_triggerMap["e5_lhtight_e4_etcut_L1JPSI-1M5"]=                                "L1_JPSI-1M5";
        m_triggerMap["e5_tight1_e4_etcut_L1JPSI-1M5"]=                                "L1_JPSI-1M5";
        m_triggerMap["e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5"]=             "L1_JPSI-1M5";
        m_triggerMap["e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7"]=         "L1_JPSI-1M5-EM7";
        m_triggerMap["e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7"]=         "L1_JPSI-1M5-EM7";
        m_triggerMap["e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12"]=       "L1_JPSI-1M5-EM12";
        m_triggerMap["e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12"]=       "L1_JPSI-1M5-EM12";
        m_triggerMap["e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5"]=           "L1_JPSI-1M5";
        m_triggerMap["e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7"]=           "L1_JPSI-1M5-EM7";
        m_triggerMap["e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7"]=       "L1_JPSI-1M5-EM7";
        m_triggerMap["e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12"]=      "L1_JPSI-1M5-EM12";
        m_triggerMap["e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12"]=     "L1_JPSI-1M5-EM12";
        m_triggerMap["e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5"]=       "L1_JPSI-1M5";
        m_triggerMap["e13_etcut_trkcut"]= "L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0";
        m_triggerMap["e18_etcut_trkcut"]= "L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0";
        m_triggerMap["e24_lhtight_L1EM20VH_e15_etcut_Zee"]= "L1_EM20VH";
        m_triggerMap["e24_tight_L1EM20VH_e15_etcut_Zee"]=   "L1_EM20VH";
        m_triggerMap["e26_lhtight_e15_etcut_Zee"]=          "L1_EM22VHI";
        m_triggerMap["e26_tight_e15_etcut_Zee"]=            "L1_EM22VHI";
        m_triggerMap["g120_loose"]=                    "L1_EM22VHI";
        m_triggerMap["g140_loose"]=                    "L1_EM22VHI";
        m_triggerMap["g0_perf_L1EM15"]=                "L1_EM15";
        m_triggerMap["g20_etcut_L1EM12"]=              "L1_EM12";
        m_triggerMap["g15_loose_L1EM7"]=               "L1_EM7";
        m_triggerMap["g20_loose_L1EM12"]=              "L1_EM12";
        m_triggerMap["g40_loose_L1EM15"]=              "L1_EM15";
        m_triggerMap["g45_loose_L1EM15"]=              "L1_EM15";
        m_triggerMap["g50_loose_L1EM15"]=              "L1_EM15";
        m_triggerMap["g80_loose"]=                     "L1_EM22VHI";
        m_triggerMap["g100_loose"]=                    "L1_EM22VHI";
        m_triggerMap["g10_loose_L1EM7"]=               "L1_EM7";
        m_triggerMap["g25_loose_L1EM15"]=              "L1_EM15";
        m_triggerMap["g35_loose_L1EM15"]=              "L1_EM15";
        m_triggerMap["g60_loose"]=                     "L1_EM22VHI";
        m_triggerMap["g70_loose"]=                     "L1_EM22VHI";
        m_triggerMap["g10_medium_L1EM7"]=              "L1_EM7";
        m_triggerMap["g10_loose"]=                     "L1_EM8VH";
        m_triggerMap["g10_medium"]=                    "L1_EM8VH";
        m_triggerMap["g15_loose_L1EM3"]=               "L1_EM3";
        m_triggerMap["g15_loose"]=                     "L1_EM13VH";
        m_triggerMap["g20_loose_L1EM15"]=              "L1_EM15";
        m_triggerMap["g20_loose"]=                     "L1_EM15VH";
        m_triggerMap["g20_tight"]=                     "L1_EM15VH";
        m_triggerMap["g25_loose"]=                     "L1_EM15VH";
        m_triggerMap["g25_medium"]=                    "L1_EM15VH";
        m_triggerMap["g35_loose"]=                     "L1_EM15VH";
        m_triggerMap["g35_medium"]=                    "L1_EM15VH";
        m_triggerMap["g40_tight"]=                     "L1_EM20VH";
        m_triggerMap["g45_tight"]=                     "L1_EM22VHI";
        m_triggerMap["g50_loose"]=                     "L1_EM15VH";
        m_triggerMap["g60_loose_L1EM15VH"]=            "L1_EM15VH";
        m_triggerMap["g35_loose_L1EM15_g25_loose_L1EM15"]=       "L1_2EM15";
        m_triggerMap["g35_loose_g25_loose"]=                     "L1_2EM15VH";
        m_triggerMap["g35_medium_g25_medium"]=                   "L1_2EM15VH";
        m_triggerMap["2g20_loose_L12EM15"]=                      "L1_2EM15";
        m_triggerMap["2g20_tight"]=                              "L1_2EM15VH";
        m_triggerMap["2g50_loose"]=                              "L1_2EM15VH";
        m_triggerMap["2g60_loose_L12EM15VH"]=                    "L1_2EM15VH";
        m_triggerMap["3g15_loose"]=                              "L1_2EM13VH";
        m_triggerMap["g20_loose_2g15_loose_L12EM13VH"]=          "L1_2EM13VH";
        m_triggerMap["2g20_loose_g15_loose"]=                    "L1_2EM15VH";
        m_triggerMap["3g20_loose"]=                              "L1_2EM15VH";
        m_triggerMap["g0_hiptrt_L1EM18VH"]=                    "L1_EM18VH";
        m_triggerMap["g0_hiptrt_L1EM20VH"]=                    "L1_EM20VH";
        m_triggerMap["g0_hiptrt_L1EM20VHI"]=                   "L1_EM20VHI";
        m_triggerMap["g0_hiptrt_L1EM22VHI"]=                 "L1_EM22VHI";

    }
    if(boost::contains(trigger,"HLT")) trigger.erase(0,4);
    return m_triggerMap[trigger];

}

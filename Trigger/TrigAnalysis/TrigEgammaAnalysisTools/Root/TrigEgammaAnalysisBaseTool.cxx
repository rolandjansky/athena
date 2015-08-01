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
 *      Generic Base tool class for trigger egamma analysis. Inherits from AsgTool.
 *      Used for handling histograms and TTrees in derived classes
 *      Relies on THistSvc. For dual-use need to find alternative
 *      Class not entirely generic
 *      Method that maps HLT trigger to L1 trigger name
 *      Methods that maps trigger pid name to egamma Pid string
 *      Helper methods for plotting Egamma variables
 **********************************************************************/
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TProfile.h"
#include "TProfile2D.h"

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
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

using namespace std;
//**********************************************************************

TrigEgammaAnalysisBaseTool::
TrigEgammaAnalysisBaseTool( const std::string& myname )
    : AsgTool(myname),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool",this),
    m_lumiTool("LuminosityTool"),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
{
    declareProperty("MatchTool",m_matchTool);
    declareProperty("ElectronKey",m_offElContKey="Electrons");
    declareProperty("PhotonKey",m_offPhContKey="Photons");
    declareProperty("File",m_file="Validation_Zee");
    declareProperty("LuminosityTool", m_lumiTool, "Luminosity Tool");
    declareProperty("LuminosityToolOnline", m_lumiBlockMuTool, "Luminosity Tool Online");
    declareProperty("DetailedHistograms", m_detailedHists=false);
    declareProperty("DefaultProbePid", m_defaultProbePid="Loose");
    declareProperty("doJpsiee",m_doJpsiee=false);
    m_storeGate = nullptr;
    m_histsvc = nullptr;
    m_parent = nullptr;
    
    // just for compile
    HLT::TriggerElement* t = NULL;
    const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
    const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
    bool a1 = ancestorPassed<xAOD::TrigElectronContainer>(t);
    bool b1 = ancestorPassed<xAOD::ElectronContainer>(t);
    (void)a; (void)b;
    (void)a1; (void)b1;

    
}

//**********************************************************************
StatusCode TrigEgammaAnalysisBaseTool::initialize() {

    ATH_MSG_VERBOSE( "Initialising tool " << name() );

    StatusCode sc;

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

StatusCode TrigEgammaAnalysisBaseTool::book() {
    StatusCode sc = StatusCode::FAILURE;
    try {
        ATH_MSG_DEBUG("child Initialize " << name());
        sc = childBook();
    } catch(const ValidationException &e) {
        ATH_MSG_ERROR("ValidationException thrown: " << e.msg());
        return StatusCode::FAILURE;
    } catch(const GaudiException &e) {
        ATH_MSG_ERROR("GaudiException thrown: " << e.message());
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
    std::stringstream ss;
    // This is not needed, get this from the file path from histobase
    // Basically set via the m_dir property in all the tool classes
    //ss << "HLT/Egamma/" << dir; 
    //
    cd(dir);
}


void TrigEgammaAnalysisBaseTool::addHistogram(TH1 *h, const std::string &dir) {
    ATH_MSG_VERBOSE("Adding Histogram");
    if (!h)
        throw ValidationException("TrigEgammaAnalysisBaseTool::addHistogram(TH1 *h == 0, ...)!");


    h->Sumw2(); //Invoke Sumw2 for all histograms
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
    ATH_MSG_VERBOSE("Registering histogram " << theDir);
   


    if(m_parent){
        std::set<std::string>::const_iterator itr = m_mongroups.find( theDir );
        if ( itr==m_mongroups.end() ) { 
            m_mongroups.insert( theDir );
            /// create actual mongroup 
            m_parent->addMonGroup(new ManagedMonitorToolBase::MonGroup(m_parent,theDir,ManagedMonitorToolBase::run)); //Can be per run or per lumi block
        }
        ATH_MSG_VERBOSE("Book Histogram in folder " << theDir);
        m_parent->addHistogram(h,theDir);
        ATH_MSG_VERBOSE("IHLTMonTool Booked Histogram in folder " << theDir);
    }
    else{
        ATH_MSG_VERBOSE("Registering histogram with THistSvc");
        StatusCode sc;
        sc = m_histsvc->regHist(ss.str(), h);
        if (sc.isFailure()) {
            throw ValidationException(std::string("Failure registering histogram ") + ss.str());
        }
    }

    ATH_MSG_VERBOSE("Registered histogram " << ss.str());
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
    if(m_parent) {
        std::set<std::string>::const_iterator itr = m_mongroups.find( theDir );
        if ( itr==m_mongroups.end() ) { 
            m_mongroups.insert( theDir );
            /// create actual mongroup 
            m_parent->addMonGroup(new ManagedMonitorToolBase::MonGroup(m_parent,theDir,ManagedMonitorToolBase::run)); //Can be per run or per lumi block
        }
        m_parent->addHistogram(h,theDir);
    }
    else if(!m_parent){
        StatusCode sc;
        sc = m_histsvc->regHist(ss.str(), h);
        if (sc.isFailure()) {
            throw ValidationException(std::string("Failure registering histogram ") + ss.str());
        }
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

    if(m_parent){    
      std::set<std::string>::const_iterator itr = m_mongroups.find( theDir );
      if ( itr==m_mongroups.end() ) { 
        m_mongroups.insert( theDir );
        /// create actual mongroup 
        m_parent->addMonGroup(new ManagedMonitorToolBase::MonGroup(m_parent,theDir,ManagedMonitorToolBase::run)); //Can be per run or per lumi block
      }
      m_parent->addTree(t,theDir);
    
    }else if(!m_parent){
        StatusCode sc;
        sc = m_histsvc->regTree(ss.str(), t);
        if (sc.isFailure()) {
            throw ValidationException(std::string("Failure registering tree ") + ss.str());
        }
    }
    
    
    m_tree.insert(std::pair<std::string, TTree *>(ss.str(), t));
}

void TrigEgammaAnalysisBaseTool::setLabels(TH1* histo, const std::vector<std::string>& labels) {
    if ( ! labels.empty() ){
        for ( int i = 0; i < std::min( (int)labels.size(), (int)histo->GetNbinsX() ); ++i ) {
            int bin = i+1;
            histo->GetXaxis()->SetBinLabel(bin, labels[i].c_str());
            ATH_MSG_VERBOSE("setting label X" <<  labels[i] << " for bin " << bin);
        }

        for ( int i = (int)histo->GetNbinsX(); i < std::min( (int)labels.size(), (int)histo->GetNbinsX()+(int)histo->GetNbinsY() ); ++i ) {
            int bin = i+1-(int)histo->GetNbinsX();
            histo->GetYaxis()->SetBinLabel(bin, labels[i].c_str());
            ATH_MSG_VERBOSE("setting label Y" <<  labels[i] << " for bin " << bin);
        }
    }
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


void TrigEgammaAnalysisBaseTool::parseTriggerName(const std::string trigger, std::string defaultPid,bool &isL1,std::string &type, 
        float &threshold, float &l1threshold, std::string &l1type, std::string &pidname, bool &etcut, bool &perf){
    
    // Analyze L1 or HLT item
    bool result = boost::starts_with( trigger , "L1" );
    if (result) {
        std::string l1info = trigger;
        l1info.erase(0,4);
        l1type = boost::trim_copy_if(l1info, boost::is_digit());
        std::string l1cut = boost::trim_copy_if(l1info, !boost::is_digit());
        l1threshold = atof(l1cut.c_str());
        threshold = l1threshold;
        isL1=true;
        pidname = defaultPid;
        type = "electron"; // for now only electron L1 studies
    }
    else {

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

        isL1=false; 
        ATH_MSG_DEBUG(trigger << " " << type << " " << pidname << " " << threshold);
    }

}
bool TrigEgammaAnalysisBaseTool::splitTriggerName(const std::string trigger, std::string &p1trigger, std::string &p2trigger){

  p1trigger="";
  p2trigger="";

  std::vector<std::string> strs;
  boost::split(strs,trigger,boost::is_any_of("_"));

  if((strs.at(0))[0]=='2'){
    ((p1trigger+=((strs.at(0)).substr(1,(int)strs.at(0).find_last_of(strs.at(0)))))+="_")+=strs.at(1);
    p2trigger=p1trigger;
    return true;
  }
  else{

    if(strs.size()<4){
      return false;
    }

    ((p1trigger+=strs.at(0))+="_")+=strs.at(1);
    ((p2trigger+=strs.at(2))+="_")+=strs.at(3);
    return true;
  }


}
void TrigEgammaAnalysisBaseTool::bookAnalysisHistos(const std::string basePath){
    std::vector <std::string> dirnames;
    ATH_MSG_DEBUG("Booking Path " << basePath);
 
    // Binning as defined in TP framework
    double coarse_eta_bins[9] ={-2.47,-1.52,-1.37,-0.60,0.00,0.60,1.37,1.52,2.47};

    double coarse_et_bins[7] = {4.,20.,30.,40.,50.,60.,150.};

    double default_eta_bins[21] = {-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.80,-0.60,-0.10,0.00,
        0.10, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47};

    double default_et_bins[14] = {4.,7.,10.,15.,20.,25.,30.,35.,40.,
        45.,50.,60.,80.,150.};
    
    float etabins[21]={-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.8,-0.6,-0.1,
        0.0,0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47};
    // TH2 with variable bin x-Axis, but constant bin y-Axis takes only Double_t arrays
    float etbins[31];

    float m_mee_down=50.;
    float m_mee_up=150.;
    
    if(m_doJpsiee){

      float temp[31]={0. ,3. ,3.5,4. ,4.5,
                      5. ,5.5,6. ,6.5,7. ,
                      7.5,8. ,8.5,9. ,9.5,
                      10.,11.,12.,13.,14.,
                      15.,16.,17.,18.,19.,
                      20.,21.,22.,23.,24.,
                      25.};
      for (int i=0;i<31;i++)etbins[i]=temp[i];


      m_mee_down=1.;
      m_mee_up=8.;  
      
    }
    else
    {
    float temp[31]={0.,2.,4.,6.,8.,10.,
                    12.,14.,16.,18.,20.,22.,24.,26.,28.,
                    30.,32.,34.,36.,38.,40.,42.,44.,46.,48.,50.,55.,60.,65.,70.,100.};
    
      for (int i=0;i<31;i++)    etbins[i]=temp[i];
    }

    dirnames.push_back(basePath + "/Efficiency/L1Calo");
    dirnames.push_back(basePath + "/Efficiency/L2Calo");
    dirnames.push_back(basePath + "/Efficiency/L2");
    dirnames.push_back(basePath + "/Efficiency/EFCalo");
    dirnames.push_back(basePath + "/Efficiency/HLT");

    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("match_pt", "Trigger Matched Offline p_{T}; p_{T} [GeV] ; Count", 30,etbins)); 
        addHistogram(new TH1F("match_et", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 30,etbins));
        addHistogram(new TH1F("match_highet", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 100, 0., 2000.));
        addHistogram(new TH1F("match_eta", "Trigger Matched Offline #eta; #eta ; Count",20,etabins));
        addHistogram(new TH1F("match_phi", "Trigger Matched #phi; #phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("match_mu", "Trigger Matched <#mu>; <#mu> ; Count", 50, 0, 100));
        addHistogram(new TH1F("match_mee", "Trigger Matched Offline M(ee); m_ee [GeV] ; Count", 50, m_mee_down, m_mee_up));
        addHistogram(new TH2D("match_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TH2D("match_coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 
        
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count",30,etbins)); 
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", 30, etbins)); 
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eta", "Offline #eta; #eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "Offline #phi; #phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("mu", "<#mu>; <#mu> ; Count", 50, 0, 100));
        addHistogram(new TH1F("mee", "Offline M(ee); m_ee [GeV] ; Count", 50, m_mee_down, m_mee_up));
        addHistogram(new TH2D("et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TH2D("coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 
        
        addHistogram(new TProfile("eff_pt", "#epsilon(p_T); p_{T} ; #epsilon",30,etbins)); 
        addHistogram(new TProfile("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Count", 30,etbins)); 
        addHistogram(new TProfile("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TProfile("eff_eta", "#epsilon(#eta); #eta ; Count", 20, etabins));
        addHistogram(new TProfile("eff_phi", "#epsilon(#phi); #phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TProfile("eff_mu", "#epsilon(<#mu>; <#mu> ; Count", 50, 0, 100));
        addHistogram(new TProfile2D("eff_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TProfile2D("eff_coarse_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 

        addHistogram(new TH1F("IsEmFailLoose","IsEmFailLoose",36,0,36));
        addHistogram(new TH1F("IsEmFailMedium","IsEmFailMedium",36,0,36));
        addHistogram(new TH1F("IsEmFailTight","IsEmFailTight",36,0,36));
        addHistogram(new TH1F("IneffIsEmLoose","IsEmLoose",36,0,36));
        addHistogram(new TH1F("IneffIsEmMedium","IsEmMedium",36,0,36));
        addHistogram(new TH1F("IneffIsEmTight","IsEmTight",36,0,36));
        setLabels(hist1("IsEmFailLoose"),m_labels);
        setLabels(hist1("IsEmFailMedium"),m_labels);
        setLabels(hist1("IsEmFailTight"),m_labels);
        setLabels(hist1("IneffIsEmLoose"),m_labels);
        setLabels(hist1("IneffIsEmMedium"),m_labels);
        setLabels(hist1("IneffIsEmTight"),m_labels);
        addHistogram(new TH1F("IsEmLHFailLoose","IsEmLHFailLoose",11,0,11));
        addHistogram(new TH1F("IsEmLHFailMedium","IsEmLHFailMedium",11,0,11));
        addHistogram(new TH1F("IsEmLHFailTight","IsEmLHFailTight",11,0,11));
    }
    
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/Offline");
    dirnames.push_back(basePath + "/Distributions/HLT");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count", 30,etbins)); 
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", 30,etbins)); 
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "phi; phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("e011", "e011; e011 ; Count", 165, -15., 150.));
        addHistogram(new TH1F("e132", "e132; e132 ; Count", 165, -15., 150.));
        addHistogram(new TH1F("e237", "e237; e237 ; Count", 215, -15., 200.));
        addHistogram(new TH1F("e277", "e277; e277 ; Count", 215, -15., 200.));
        addHistogram(new TH1F("ethad", "ethad; ethad ; Count", 50, -0.5, 0.5));
        addHistogram(new TH1F("ethad1", "ethad1; ehad1 ; Count", 50, -0.5, 0.5));
        addHistogram(new TH1F("weta1", "weta1; weta1 ; Count", 50, 0., 1.));
        addHistogram(new TH1F("weta2", "weta2; weta2 ; Count", 50, 0., 0.05));
        addHistogram(new TH1F("wtots1", "wtots1; wtots1 ; Count", 50, 0., 0.05));
        addHistogram(new TH1F("f1", "f1; f1 ; Count", 50, -0.1, 1.1));
        addHistogram(new TH1F("f3", "f3; f3 ; Count", 50, -0.1, 0.25));
        addHistogram(new TH1F("e2tsts1", "e2tsts1; e2tsts1 ; Count", 50, 0., 100.));
        addHistogram(new TH1F("Reta", "Reta; Reta ; Count", 50, 0., 2.));
        addHistogram(new TH1F("Rphi", "Rphi; Rphi ; Count", 50, 0., 2.));
        addHistogram(new TH1F("Rhad", "Rhad; Rhad ; Count", 50, -0.25, 0.25));
        addHistogram(new TH1F("Rhad1", "Rhad1; Rhad1 ; Count", 50, -1., 1.));
        addHistogram(new TH1F("deta1", "deta1; deta1 ; Count", 90, -0.03, 0.03));
        if(m_detailedHists){
            addHistogram(new TH2F("deta1_vs_clusterEta", "HLT deta1 as function of cluster #eta; #eta; deta1; Count",
                        50, -2.47, 2.47,
                        90, -0.03, 0.03));
        }
        addHistogram(new TH1F("deta1_EMECA", "deta1 EMEC-A; deta1 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("deta1_EMECC", "deta1 EMEC-C; deta1 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("deta1_EMEBA", "deta1 EMEB-A; deta1 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("deta1_EMEBC", "deta1 EMEB-A; deta1 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("deta2", "deta2; deta2 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("dphi2", "dphi2; dphi2 ; Count", 100, -0.25, 0.25));
        addHistogram(new TH1F("dphiresc", "dphiresc; dphiresc ; Count", 100, -0.1, 0.1));
        addHistogram(new TH1F("d0", "d0; d0 ; Count", 150, -1.5, 1.5));
        addHistogram(new TH1F("d0sig", "d0sig; d0sig ; Count", 50, -10, 10));
        addHistogram(new TH1F("eratio","eratio; eratio; Count",50, 0, 2));
        addHistogram(new TH1F("eprobht","eProbHT; eProbHT; Count",50, 0, 1.1));
        addHistogram(new TH1F("nscthits","nSCTHit; nSCTHits; Count",30, 0, 30));
        addHistogram(new TH1F("npixhits","nPixHit; nPixHits; Count",10, 0, 10));
        addHistogram(new TH1F("charge","charge; charge; Count", 4,-2,2));
        addHistogram(new TH1F("ptcone20", "ptcone20; ptcone20; Count", 50, 0.0, 5.0));
        addHistogram(new TH1F("ptcone20_rel", "ptcone20/pt; ptcone20/pt; Count", 50, 0.0, 1.0));
    }
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/EFCalo");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("energyBE0", "Cluster Energy BE0; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energyBE1", "Cluster Energy BE1; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energyBE2", "Cluster Energy BE2; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energyBE3", "Cluster Energy BE3; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energy", "Cluster Energy BE3; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "phi; phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("eta_calo", "eta_calo; eta_calo ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("phi_calo", "phi_calo; phi_calo ; Count", 50, -3.14, 3.14));
    }
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L2Photon"); 
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
    
    } 
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L2Electron");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("trkClusDeta", "Trk Clus Deta; deta ; Count", 50, -0.5, 0.5));
        addHistogram(new TH1F("trkClusDphi", "Trk Clus Dphi; dphi ; Count", 50, -0.5, 0.5)); 
    }

    /*
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L2Calo");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
        addHistogram(new TH1F("eta", "#eta; #eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("phi", "#phi; #phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("ringer_nnOutput", "Discriminator distribution; nnOutput ; Count", 100, -1, 1));
        addHistogram(new TH2F("ringer_etVsEta", "ringer count as function of #eta and E_{t}; #eta; E_{t} [GeV]; Count",
                              NETABINS,ETABINS, NETBINS, ETBINS ));
        
        for(unsigned layer =0; layer < 7; ++layer){
          unsigned minRing, maxRing;  std::string strLayer;
          ringer::TrigCaloRingsHelper::parseCaloFeatures( layer, minRing, maxRing, strLayer );
          addDirectory(dirnames[i]+"/rings_"+strLayer);
          for(unsigned r=minRing; r<=maxRing; ++r){
            stringstream ss_title, ss;
            ss_title << "ringer_ring#" << r;  ss << "L2Calo ringer ("<< strLayer <<"); ring#" << r << " E [MeV]; Count";
            addHistogram(new TH1F(ss_title.str().c_str(), ss.str().c_str(), 200, -20, 1500.));
          }
        }///Loop for each calo layers    
    }///L2Calo monitoring
    */

    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L1Calo");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("energy", "Cluster Energy; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("roi_et", "RoI word Cluster Energy; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("emIso", "EM Isolation; E [GeV] ; Count", 50, -1., 20.));
        addHistogram(new TH1F("hadCore", "Hadronic Isolation; E [GeV] ; Count", 50, -1., 20.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 50, -2.5, 2.5));
        addHistogram(new TH1F("phi", "phi; phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH2F("emClusVsEta", "L1 Cluster Energy vs L1 #eta; #eta; E [GeV]; Count",
                              51, -2.55, 2.55,
                              100, 0, 100));
        addHistogram(new TH2F("emClusVsEmIsol", "L1 Cluster Energy vs emIsol; emIsol [GeV]; E [GeV]; Count",
                              20, -0.1, 9.9,
                              100, 0, 100));
        addHistogram(new TH2F("emClusVsHadCore", "L1 Cluster Energy vs hadCore; hadCore [GeV]; E [GeV]; Count",
                              10, -0.1, 4.9,
                              100, 0, 100));
    }
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/RoI");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("roi_eta", "RoI #eta; #eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("roi_phi", "RoI #phi; #phi ; Count", 50, -3.14, 3.14));
    }

    dirnames.clear();
    dirnames.push_back(basePath + "/Resolutions/HLT");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("pt", "HLT p_{T} resolution; (p_{T}(on)-p_{T}(off))/p_{T}(off) ; Count", 200, -1.5, 1.5));
        addHistogram(new TH1F("et", "HLT E_{T} resolution; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
        addHistogram(new TH1F("et_cnv", "HLT E_{T} resolution for converted Photons; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
        addHistogram(new TH1F("et_uncnv", "HLT E_{T} resolution for unconverted Photons; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
        addHistogram(new TH1F("eta", "#eta resolution; (#eta(on)-#eta(off))/#eta(off) ; Count", 40, -0.2, 0.2));
        addHistogram(new TH1F("phi", "#phi resolution; (#phi(on)-#phi(off))/#phi(off) ; Count", 40, -0.2, 0.2));

        addHistogram(new TH2F("res_etVsEta", "HLT E_{T} resolution as function of #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, -2.47, 2.47,
                              200, -0.1, 0.1));
        addHistogram(new TH2F("res_etVsEt", "HLT E_{T} resolution as function of E_{T}; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, 0., 100.,
                              200, -0.1, 0.1));
        
        addHistogram(new TH2F("res_cnv_etVsEta", "HLT E_{T} resolution as function of #eta for converted Photons; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, -2.47, 2.47,
                              200, -0.1, 0.1));
        addHistogram(new TH2F("res_cnv_etVsEt", "HLT E_{T} resolution as function of E_{T} for converted Photons; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, 0., 100.,
                              200, -0.1, 0.1));
        
        addHistogram(new TH2F("res_uncnv_etVsEta", "HLT E_{T} resolution as function of #eta for unconverted Photons; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, -2.47, 2.47,
                              200, -0.1, 0.1));
        addHistogram(new TH2F("res_uncnv_etVsEt", "HLT E_{T} resolution as function of E_{T} for unconverted Photons; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, 0., 100.,
                              200, -0.1, 0.1));
        
        addHistogram(new TH2F("res_ptcone20_relVsEta", "HLT ptcone20/pt resolution as function of #eta; #eta; (on-off)/off; Count",
                              50, -2.47, 2.47,
                              200, -0.1, 0.1));
        addHistogram(new TH2F("res_ptcone20_relVsEt", "HLT ptcone20/pt resolution as function of E_{T}; E_{T} [GeV]; (on-off)/off; Count",
                              50, 0., 100.,
                              200, -0.1, 0.1));
        addHistogram(new TH2F("res_ptcone20VsMu", "HLT ptcone20 resolution as function of avg #mu; #mu; (on-off)/off; Count",
                              50, 0, 100,
                              200, -0.2, 0.2));
        addHistogram(new TH2F("res_ptcone20_relVsMu", "HLT ptcone20/pt resolution as function of avg #mu; #mu; (on-off)/off; Count",
                              50, 0, 100,
                              200, -0.2, 0.2));

	addHistogram(new TH1F("res_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));

	addHistogram(new TH1F("res_cnv_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_cnv_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_cnv_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_cnv_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
        
	addHistogram(new TH1F("res_uncnv_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_uncnv_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_uncnv_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_uncnv_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));

        // Relative resolutions
        addHistogram(new TH1F("e011", "e011 resolution; (e011(on)-e011(off))/e011(off) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("e132", "e132 resolution; (e132(on)-e132(off))/e132(off) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("e237", "e237 resolution; (e237(on)-e237(off))/e237(off) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("e277", "e277 resolution; (e277(on)-e277(off))/e277(off) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("ethad", "ethad resolution; (ethad(on)-ethad(off))/ethad(off) ; Count", 100, -0.5, 0.5));
	if ( m_detailedHists ) {
            addHistogram(new TH2F("res_ethadVsEta", "HLT E_{T} Had resolution as function of #eta; #eta; (ethad(on)-ethad(off))/ethad(off); Count",
                        50, -2.47, 2.47,
                        50, -0.5, 0.5));
            addHistogram(new TH2F("res_ethadVsEt", "HLT E_{T} Had resolution as function of E_{T}; E_{T} [GeV]; (ethad(on)-ethad(off))/ethad(off); Count",
                        50, 0., 100.,
                        50, -0.5, 0.5));
	}
        addHistogram(new TH1F("ethad1", "ethad1 resolution; (ethad1(on)-ethad1(off))/ethad1(off) ; Count", 100, -0.5, 0.5));
	if ( m_detailedHists ) {
            addHistogram(new TH2F("res_ethad1VsEta", "HLT E_{T} Had1 resolution as function of #eta; #eta; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                        50, -2.47, 2.47,
                        50, -0.5, 0.5));
            addHistogram(new TH2F("res_ethad1VsEt", "HLT E_{T} Had1 resolution as function of E_{T}; E_{T} [GeV]; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                        50, 0., 100.,
                              50, -0.5, 0.5));
	}
        addHistogram(new TH1F("Rhad", "Rhad resolution; (Rhad(on)-Rhad(off))/Rhad(off) ; Count", 50, -10., 10.));
	if ( m_detailedHists ) {
        addHistogram(new TH2F("res_RhadVsEta", "HLT E_{T} Rhad resolution as function of #eta; #eta; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                              50, -2.47, 2.47,
                              50, -10, 10));
        addHistogram(new TH2F("res_RhadVsEt", "HLT E_{T} RHad resolution as function of E_{T}; E_{T} [GeV]; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                              50, 0., 100.,
                              50, -10, 10));
	}
        addHistogram(new TH1F("Rhad1", "Rhad1; Rhad1 resolution; (Rhad1(on)-Rhad1(off))/Rhad1(off)", 50, -10., 10.));
	if ( m_detailedHists ) {
        addHistogram(new TH2F("res_Rhad1VsEta", "HLT E_{T} Rhad1 resolution as function of #eta; #eta; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                              50, -2.47, 2.47,
                              50, -10, 10));
        addHistogram(new TH2F("res_Rhad1VsEt", "HLT E_{T} RHad1 resolution as function of E_{T}; E_{T} [GeV]; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                              50, 0., 100.,
                              50, -10, 10));
	}
        addHistogram(new TH1F("Reta", "Reta resolution; (Reta(on)-Reta(off))/Reta(off) ; Count", 80, -0.02, 0.02));
	if ( m_detailedHists ) {
        addHistogram(new TH2F("res_RetaVsEta", "HLT Reta resolution as function of #eta; #eta; (Reta(on)-Reta(off))/Reta(off); Count",
                              50, -2.47, 2.47,
                              50, -0.05, 0.05));
        addHistogram(new TH2F("res_RetaVsEt", "HLT Reta resolution as function of E_{T}; E_{T} [GeV]; (Reta(on)-Reta(off))/Reta(off); Count",
                              50, 0., 100.,
                              50, -0.05, 0.05));
	}
        addHistogram(new TH1F("Rphi", "Rphi resolution; (Rphi(on)-Rphi(off))/Rphi(off) ; Count", 80, -0.02, 0.02));
	if ( m_detailedHists ) {
            addHistogram(new TH2F("res_RphiVsEta", "HLT Rphi resolution as function of #eta; #eta; (Rphi(on)-Rphi(off))/Rphi(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05));
            addHistogram(new TH2F("res_RphiVsEt", "HLT Rphi resolution as function of E_{T}; E_{T} [GeV]; (Rphi(on)-Rphi(off))/Rphi(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05));
	}
        addHistogram(new TH1F("weta1", "weta1 resolution; (weta1(on)-weta1(off))/weta1(off) ; Count", 50, -0.05, 0.05));
	if ( m_detailedHists ) {
            addHistogram(new TH2F("res_weta1VsEta", "HLT weta1 resolution as function of #eta; #eta; (weta1(on)-weta1(off))/weta1(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05));
            addHistogram(new TH2F("res_weta1VsEt", "HLT weta1 resolution as function of E_{T}; E_{T} [GeV]; (weta1(on)-weta1(off))/weta1(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05));
	}
        addHistogram(new TH1F("weta2", "weta2 resolution; (weta2(on)-weta2(off))/weta2(off) ; Count", 50, -0.05, 0.05));
	if ( m_detailedHists ) {
            addHistogram(new TH2F("res_weta2VsEta", "HLT weta2 resolution as function of #eta; #eta; (weta2(on)-weta2(off))/weta2(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05));
            addHistogram(new TH2F("res_weta2VsEt", "HLT weta2 resolution as function of E_{T}; E_{T} [GeV]; (weta2(on)-weta2(off))/weta2(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05));
	}
        addHistogram(new TH1F("wtots1", "wtots1 resolution; (wtots1(on)-wtots1(off))/wtots1(off) ; Count", 50, -0.05, 0.05));
	if ( m_detailedHists ) {
            addHistogram(new TH2F("res_wtots1VsEta", "HLT wtots1 resolution as function of #eta; #eta; (wtots1(on)-wtots1off))/wtots1(off); Count",
                        50,-2.47,2.47,
                        50, -0.05, 0.05));
            addHistogram(new TH2F("res_wtots1VsEt", "HLT wtots1 resolution as function of E_{T}; E_{T} [GeV]; (wtots1(on)-wtots1(off))/wtots1(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05));
	}
        addHistogram(new TH1F("f1", "f1 resolution; (f1(on)-f1(off))/f1(off) ; Count", 80, -0.02, 0.02));
	if ( m_detailedHists ) {
            addHistogram(new TH2F("res_f1VsEta", "HLT f1 resolution as function of #eta; #eta; (f1(on)-f1(off))/f1(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05));
            addHistogram(new TH2F("res_f1VsEt", "HLT f1 resolution as function of E_{T}; E_{T} [GeV]; (f1(on)-f1(off))/f1(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05));
	}
        addHistogram(new TH1F("f3", "f3 resolution; (f3(on)-f3(off))/f3(off) ; Count", 50, -0.05, 0.05));
	if ( m_detailedHists ) {
        addHistogram(new TH2F("res_f3VsEta", "HLT f3 resolution as function of #eta; #eta; (f3(on)-f3(off))/f3(off); Count",
                              50, -2.47, 2.47,
                              50, -0.05, 0.05));
        addHistogram(new TH2F("res_f3VsEt", "HLT f3 resolution as function of E_{T}; E_{T} [GeV]; (f3(on)-f3(off))/f3(off); Count",
                              50, 0., 100.,
                              50, -0.05, 0.05));
	}
        addHistogram(new TH1F("e2tsts1", "e2tsts1 resolution; e2tsts1 ; Count", 50, -10, 10.));
	if ( m_detailedHists ) {
        addHistogram(new TH2F("res_e2tsts1VsEta", "HLT e2tsts1 resolution as function of #eta; #eta; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                              50, -2.47, 2.47,
                              50, -10, 10));
        addHistogram(new TH2F("res_e2tsts1VsEt", "HLT e2tsts1 resolution as function of E_{T}; E_{T} [GeV]; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                              50, 0., 100.,
                              50, -10, 10));
	}
        addHistogram(new TH1F("eratio", "eratio resolution; (eratio(on)-eratio(off))/eratio(off) ; Count", 60, -0.0015, 0.0015));
	if ( m_detailedHists ) {
        addHistogram(new TH2F("res_eratioVsEta", "HLT eratio resolution as function of #eta; #eta; (eratio(on)-eratio(off))/eratio(off); Count",
                              50, -2.47, 2.47,
                              50, -0.001, 0.001));
        addHistogram(new TH2F("res_eratioVsEt", "HLT eratio resolution as function of E_{T}; E_{T} [GeV]; (eratio(on)-eratio(off))/eratio(off); Count",
                              50, 0., 100.,
                              50, -0.001, 0.001));
	}
        addHistogram(new TH1F("deta1", "deta1; deta1 ; (deta1(on)-deta1(off))/deta1(off)", 100, -1., 1.));
        addHistogram(new TH1F("deta2", "deta2; deta2 ; (deta2(on)-deta2(off))/deta2(off)", 100, -1., 1.));
        addHistogram(new TH1F("dphi2", "dphi2; dphi2 ; (dphi2(on)-dphi2(off))/dphi2(off)", 100, -1., 1.));
        addHistogram(new TH1F("dphiresc", "dphiresc; (dphires(on)-dphires(off))/dphires(off) ; Count", 100, -1., 1.));
        addHistogram(new TH1F("d0", "resolution d0; (d0(on)-d0(off))/(d0(off)) ; Count", 100, -0.5, 0.5));
        addHistogram(new TH1F("d0sig", "resolution d0sig; (d0sig(on)-d0sig(off))/(d0sig(off) ; Count", 50, -10, 10));
        addHistogram(new TH1F("eprobht","resolution eProbHT; (eProbHT(on)-eProbHT(off))/eProbHT(off)); Count",200, -1, 1));
        addHistogram(new TH1F("nscthits","resolution nSCTHit; (nSCTHits(on)-nSCTHits(off); Count",20, -10, 10));
        addHistogram(new TH1F("npixhits","resolution nPixHit; (nPixHits(on)-nPixHits(off)); Count",10, -5, 5));
        addHistogram(new TH1F("ptcone20", "resolution ptcone20; ptcone20 (on-off)/off; Count", 200, -0.1, 0.1));
        addHistogram(new TH1F("ptcone20_rel", "resolution ptcone20/pt; ptcone20/pt (on-off)/off; Count", 200, -0.1, 0.1));
    }

    dirnames.clear();
    dirnames.push_back(basePath + "/AbsResolutions/HLT");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("pt", "HLT p_{T} resolution; (p_{T}(on)-p_{T}(off)) ; Count", 200, -1.5, 1.5));
        addHistogram(new TH1F("et", "HLT E_{T} resolution; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.5, 0.5));
        addHistogram(new TH1F("eta", "#eta resolution; (#eta(on)-#eta(off)) ; Count", 40, -0.2, 0.2));
        addHistogram(new TH1F("phi", "#phi resolution; (#phi(on)-#phi(off)) ; Count", 40, -0.2, 0.2));

        addHistogram(new TH2F("res_etVsEta", "HLT E_{T} resolution as function of #eta; #eta; (E_{T}(on)-E_{T}(off)); Count",
                              50, -2.47, 2.47,
                              200, -0.1, 0.1));
        addHistogram(new TH2F("res_etVsEt", "HLT E_{T} resolution as function of E_{T}; E_{T} [GeV]; (E_{T}(on)-E_{T}(off)); Count",
                              50, 0., 100.,
                              200, -0.1, 0.1));

        addHistogram(new TH2F("res_ptcone20_relVsEta", "HLT ptcone20/pt resolution as function of #eta; #eta; on-off; Count",
                              50, -2.47, 2.47,
                              200, -0.2, 0.2));
        addHistogram(new TH2F("res_ptcone20_relVsEt", "HLT ptcone20/pt resolution as function of E_{T}; E_{T} [GeV]; on-off; Count",
                              50, 0., 100.,
                              200, -0.2, 0.2));
        addHistogram(new TH2F("res_ptcone20VsMu", "HLT ptcone20 resolution as function of avg #mu; #mu; on-off; Count",
                              50, 0, 100,
                              200, -20, 20));
        addHistogram(new TH2F("res_ptcone20_relVsMu", "HLT ptcone20/pt resolution as function of avg #mu; #mu; on-off; Count",
                              50, 0, 100,
                              200, -0.2, 0.2));
        addHistogram(new TH2F("ptcone20_onVsOff", "online ptcone20 vs offline ptcone20; offline [MeV]; online [MeV]; Count",
                              200, 0.0, 10000.0,
                              200, 0.0, 10000.0));
        addHistogram(new TH2F("ptcone20_rel_onVsOff", "online ptcone20/pt vs offline ptcone20/pt; offline; online; Count",
                              200, 0.0, 0.2,
                              200, 0.0, 0.2));

      	addHistogram(new TH1F("res_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));
      	addHistogram(new TH1F("res_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));
      	addHistogram(new TH1F("res_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));
      	addHistogram(new TH1F("res_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));

        // Relative resolutions
        addHistogram(new TH1F("e011", "e011 resolution; (e011(on)-e011(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("e132", "e132 resolution; (e132(on)-e132(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("e237", "e237 resolution; (e237(on)-e237(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("e277", "e277 resolution; (e277(on)-e277(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("ethad", "ethad resolution; (ethad(on)-ethad(off)) ; Count", 100, -10, 10));
        addHistogram(new TH1F("ethad1", "ethad1 resolution; (ethad1(on)-ethad1(off)) ; Count", 100, -10, 10));
        addHistogram(new TH1F("Rhad", "Rhad resolution; (Rhad(on)-Rhad(off)) ; Count", 50, -10., 10.));
        addHistogram(new TH1F("Rhad1", "Rhad1; Rhad1 resolution; (Rhad1(on)-Rhad1(off));Count", 50, -10., 10.));
        addHistogram(new TH1F("Reta", "Reta resolution; (Reta(on)-Reta(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("Rphi", "Rphi resolution; (Rphi(on)-Rphi(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("weta1", "weta1 resolution; (weta1(on)-weta1(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("weta2", "weta2 resolution; (weta2(on)-weta2(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("f1", "f1 resolution; (f1(on)-f1(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("f3", "f3 resolution; (f3(on)-f3(off)) ; Count", 50, -0.05, 0.05));
        addHistogram(new TH1F("e2tsts1", "e2tsts1 resolution; e2tsts1 ; Count", 50, -10, 10.));
        addHistogram(new TH1F("eratio", "eratio resolution; (eratio(on)-eratio(off)) ; Count", 200, -0.001, 0.001));
        addHistogram(new TH1F("deta1", "deta1; deta1 ; (deta1(on)-deta1(off))", 100, -1., 1.));
        addHistogram(new TH1F("deta2", "deta2; deta2 ; (deta2(on)-deta2(off))", 100, -1., 1.));
        addHistogram(new TH1F("dphi2", "dphi2; dphi2 ; (dphi2(on)-dphi2(off))", 100, -1., 1.));
        addHistogram(new TH1F("dphiresc", "dphiresc; (dphires(on)-dphires(off)) ; Count", 100, -1., 1.));
        addHistogram(new TH1F("d0", "resolution d0; (d0(on)-d0(off)) ; Count", 100, -0.5, 0.5));
        addHistogram(new TH1F("d0sig", "resolution d0sig; (d0sig(on)-d0sig(off)) ; Count", 50, -10, 10));
        addHistogram(new TH1F("eprobht","resolution eProbHT; (eProbHT(on)-eProbHT(off)); Count",50, -1, 1));
        addHistogram(new TH1F("nscthits","resolution nSCTHit; (nSCTHits(on)-nSCTHits(off); Count",20, -10, 10));
        addHistogram(new TH1F("npixhits","resolution nPixHit; (nPixHits(on)-nPixHits(off)); Count",10, -5, 5));
        addHistogram(new TH1F("ptcone20", "resolution ptcone20; ptcone20 (on-off); Count", 200, -20, 20));
        addHistogram(new TH1F("ptcone20_rel", "resolution ptcone20/pt; ptcone20/pt (on-off); Count", 200, -0.1, 0.1));
        addHistogram(new TH1F("wtots1", "wtots1 resolution; (wtots1(on)-wtots1(off)) ; Count", 50, -0.05, 0.05));
    }
    
    dirnames.clear();
    //dirnames.push_back(basePath + "/Resolutions/L2");
    dirnames.push_back(basePath + "/Resolutions/L2Calo");
    dirnames.push_back(basePath + "/Resolutions/L2Calo_vs_HLT");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("pt", "L2Calo p_{T} resolution; (p_{T}(on)-p_{T}(off))/p_{T}(off) ; Count", 200, -1.5, 1.5));
        addHistogram(new TH1F("et", "L2Calo E_{T} resolution; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 100, -0.5, 0.5));
        addHistogram(new TH1F("eta", "L2Calo #eta resolution; (#eta(on)-#eta(off))/#eta(off) ; Count", 40, -0.2, 0.2));
        addHistogram(new TH1F("phi", "L2Calo #phi resolution; (#phi(on)-#phi(off))/#phi(off) ; Count", 40, -0.2, 0.2));

        addHistogram(new TH2F("res_etVsEta", "L2Calo E_{T} resolution as function of #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, -2.47, 2.47,
                              200, -0.1, 0.1));
        addHistogram(new TH2F("res_etVsEt", "L2Calo E_{T} resolution as function of E_{T}; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                              50, 0., 100.,
                              200, -0.1, 0.1));


        addHistogram(new TH1F("ethad", "ethad resolution; (ethad(on)-ethad(off))/ethad(off) ; Count", 100, -0.5, 0.5));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_ethadVsEta", "L2Calo E_{T} Had resolution as function of #eta; #eta; (ethad(on)-ethad(off))/ethad(off); Count",
                              50, -2.47, 2.47,
                              50, -0.5, 0.5));
        addHistogram(new TH2F("res_ethadVsEt", "L2Calo E_{T} Had resolution as function of E_{T}; E_{T} [GeV]; (ethad(on)-ethad(off))/ethad(off); Count",
                              50, 0., 100.,
                              50, -0.5, 0.5));
        }
        addHistogram(new TH1F("ethad1", "ethad1 resolution; (ethad1(on)-ethad1(off))/ethad1(off) ; Count", 100, -0.5, 0.5));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_ethad1VsEta", "L2Calo E_{T} Had1 resolution as function of #eta; #eta; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                              50, -2.47, 2.47,
                              50, -0.5, 0.5));
        addHistogram(new TH2F("res_ethad1VsEt", "L2Calo E_{T} Had1 resolution as function of E_{T}; E_{T} [GeV]; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                              50, 0., 100.,
                              50, -0.5, 0.5));
        }
        addHistogram(new TH1F("Rhad", "Rhad resolution; (Rhad(on)-Rhad(off))/Rhad(off) ; Count", 50, -10., 10.));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_RhadVsEta", "L2Calo E_{T} Rhad resolution as function of #eta; #eta; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                              50, -2.47, 2.47,
                              50, -10, 10));
        addHistogram(new TH2F("res_RhadVsEt", "L2Calo E_{T} RHad resolution as function of E_{T}; E_{T} [GeV]; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                              50, 0., 100.,
                              50, -10, 10));
        }
        addHistogram(new TH1F("Rhad1", "Rhad1; Rhad1 resolution; (Rhad1(on)-Rhad1(off))/Rhad1(off)", 50, -10., 10.));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_Rhad1VsEta", "L2Calo E_{T} Rhad1 resolution as function of #eta; #eta; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                              50, -2.47, 2.47,
                              50, -10, 10));
        addHistogram(new TH2F("res_Rhad1VsEt", "L2Calo E_{T} RHad1 resolution as function of E_{T}; E_{T} [GeV]; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                              50, 0., 100.,
                              50, -10, 10));
        }
        addHistogram(new TH1F("Reta", "Reta resolution; (Reta(on)-Reta(off))/Reta(off) ; Count", 50, -0.05, 0.05));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_RetaVsEta", "L2Calo Reta resolution as function of #eta; #eta; (Reta(on)-Reta(off))/Reta(off); Count",
                              50, -2.47, 2.47,
                              50, -0.05, 0.05));
        addHistogram(new TH2F("res_RetaVsEt", "L2Calo Reta resolution as function of E_{T}; E_{T} [GeV]; (Reta(on)-Reta(off))/Reta(off); Count",
                              50, 0., 100.,
                              50, -0.05, 0.05));
        }
        addHistogram(new TH1F("weta1", "weta1 resolution; (weta1(on)-weta1(off))/weta1(off) ; Count", 50, -0.05, 0.05));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_weta1VsEta", "L2Calo weta1 resolution as function of #eta; #eta; (weta1(on)-weta1(off))/weta1(off); Count",
                              50, -2.47, 2.47,
                              50, -0.05, 0.05));
        addHistogram(new TH2F("res_weta1VsEt", "L2Calo weta1 resolution as function of E_{T}; E_{T} [GeV]; (weta1(on)-weta1(off))/weta1(off); Count",
                              50, 0., 100.,
                              50, -0.05, 0.05));
        }
        addHistogram(new TH1F("weta2", "weta2 resolution; (weta2(on)-weta2(off))/weta2(off) ; Count", 50, -0.05, 0.05));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_weta2VsEta", "L2Calo weta2 resolution as function of #eta; #eta; (weta2(on)-weta2(off))/weta2(off); Count",
                              50, -2.47, 2.47,
                              50, -0.05, 0.05));
        addHistogram(new TH2F("res_weta2VsEt", "L2Calo weta2 resolution as function of E_{T}; E_{T} [GeV]; (weta2(on)-weta2(off))/weta2(off); Count",
                              50, 0., 100.,
                              50, -0.05, 0.05));
        }
        addHistogram(new TH1F("f1", "f1 resolution; (f1(on)-f1(off))/f1(off) ; Count", 50, -0.05, 0.05));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_f1VsEta", "L2Calo f1 resolution as function of #eta; #eta; (f1(on)-f1(off))/f1(off); Count",
                              50, -2.47, 2.47,
                              50, -0.05, 0.05));
        addHistogram(new TH2F("res_f1VsEt", "L2Calo f1 resolution as function of E_{T}; E_{T} [GeV]; (f1(on)-f1(off))/f1(off); Count",
                              50, 0., 100.,
                              50, -0.05, 0.05));
        }
        addHistogram(new TH1F("f3", "f3 resolution; (f3(on)-f3(off))/f3(off) ; Count", 50, -0.05, 0.05));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_f3VsEta", "L2Calo f3 resolution as function of #eta; #eta; (f3(on)-f3(off))/f3(off); Count",
                              50, -2.47, 2.47,
                              50, -0.05, 0.05));
        addHistogram(new TH2F("res_f3VsEt", "L2Calo f3 resolution as function of E_{T}; E_{T} [GeV]; (f3(on)-f3(off))/f3(off); Count",
                              50, 0., 100.,
                              50, -0.05, 0.05));
        }
        addHistogram(new TH1F("e2tsts1", "e2tsts1 resolution; e2tsts1 ; Count", 50, -10, 10.));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_e2tsts1VsEta", "L2Calo e2tsts1 resolution as function of #eta; #eta; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                              50, -2.47, 2.47,
                              50, -10, 10));
        addHistogram(new TH2F("res_e2tsts1VsEt", "L2Calo e2tsts1 resolution as function of E_{T}; E_{T} [GeV]; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                              50, 0., 100.,
                              50, -10, 10));
        }
        addHistogram(new TH1F("eratio", "eratio resolution; (eratio(on)-eratio(off))/eratio(off) ; Count", 200, -0.001, 0.001));
        if ( m_detailedHists ) {
        addHistogram(new TH2F("res_eratioVsEta", "L2Calo eratio resolution as function of #eta; #eta; (eratio(on)-eratio(off))/eratio(off); Count",
                              50, -2.47, 2.47,
                              50, -0.001, 0.001));
        addHistogram(new TH2F("res_eratioVsEt", "L2Calo eratio resolution as function of E_{T}; E_{T} [GeV]; (eratio(on)-eratio(off))/eratio(off); Count",
                              50, 0., 100.,
                              50, -0.001, 0.001));
        }
    }

    dirnames.clear();
    dirnames.push_back(basePath + "/Resolutions/L1Calo");
    ATH_MSG_DEBUG("Creating L1Calo resolution hists");
    for (int i = 0; i < (int) dirnames.size(); i++) {
      addDirectory(dirnames[i]);
      addHistogram(new TH2F("res_etVsEta", "L1 cluster Energy resolution as function of L1 #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                            50, -2.55, 2.55,
                            200, -1., 1.));
    }
    dirnames.clear();
    dirnames.push_back(basePath + "/AbsResolutions/L1Calo");
    ATH_MSG_DEBUG("Creating L1Calo Abs resolution hists");
    for (int i = 0; i < (int) dirnames.size(); i++) {
      addDirectory(dirnames[i]);
      addHistogram(new TH2F("res_etVsEta", "L1 cluster Energy resolution as function of L1 #eta; #eta; E_{T}(on)-E_{T}(off); Count",
                            51, -2.55, 2.55,
                            200, -100., 100.));
    }
}

void TrigEgammaAnalysisBaseTool::fillEfficiency(const std::string dir,bool isPassed,const float etthr,
        const float et, const float eta, const float phi,const float avgmu /*=0.*/,const float mass /*=0.*/){

    cd(dir);
    hist1("et")->Fill(et);
    hist1("highet")->Fill(et);
    hist1("mee")->Fill(mass);
    if(et > etthr+1.0){
        hist1("eta")->Fill(eta);
        hist1("phi")->Fill(phi);
        hist1("mu")->Fill(avgmu);
        hist2("et_eta")->Fill(et,eta);
        hist2("coarse_et_eta")->Fill(et,eta);
    }
    if(isPassed) {
        hist1("match_et")->Fill(et);
        hist1("match_highet")->Fill(et);
        hist1("match_mee")->Fill(mass);
        if(et > etthr+1.0){
            hist1("match_eta")->Fill(eta);
            hist1("match_phi")->Fill(phi);
            hist1("match_mu")->Fill(avgmu);
            hist2("match_et_eta")->Fill(et,eta);
            hist2("match_coarse_et_eta")->Fill(et,eta);

        }
        hist1("eff_et")->Fill(et,1);
        hist1("eff_highet")->Fill(et,1);
        if(et > etthr+1.0){
            hist1("eff_eta")->Fill(eta,1);
            hist1("eff_phi")->Fill(phi,1);
            hist1("eff_mu")->Fill(avgmu,1);
            hist2("eff_et_eta")->Fill(et,eta,1);
            hist2("eff_coarse_et_eta")->Fill(et,eta,1);
        }
    }
    else {
        hist1("eff_et")->Fill(et,0);
        hist1("eff_highet")->Fill(et,0);
        if(et > etthr+1.0){
            hist1("eff_eta")->Fill(eta,0);
            hist1("eff_phi")->Fill(phi,0);
            hist1("eff_mu")->Fill(avgmu,0);
            hist2("eff_et_eta")->Fill(et,eta,0);
            hist2("eff_coarse_et_eta")->Fill(et,eta,0);
        }
    }

}

void TrigEgammaAnalysisBaseTool::finalizeEfficiency(std::string dir){
    cd(dir);
    // No longer requires, using TProfile for efficiency
}

void TrigEgammaAnalysisBaseTool::fillL1Calo(const std::string dir, const xAOD::EmTauRoI *l1){
    cd(dir);
    ATH_MSG_DEBUG("Fill L1Calo distributions" << dir);
    hist1("eta")->Fill(l1->eta());
    hist1("phi")->Fill(l1->phi());
    hist1("energy")->Fill(l1->emClus()/1.e3);
    hist1("roi_et")->Fill(l1->eT()/1.e3);
    hist1("emIso")->Fill(l1->emIsol()/1.e3);
    hist1("hadCore")->Fill(l1->hadCore()/1.e3);
    hist2("emClusVsEta")->Fill(l1->eta(), l1->emClus()*0.001);
    hist2("emClusVsEmIsol")->Fill(l1->emIsol()*0.001, l1->emClus()*0.001);
    hist2("emClusVsHadCore")->Fill(l1->hadCore()*0.001, l1->emClus()*0.001);
}

void TrigEgammaAnalysisBaseTool::fillEFCalo(const std::string dir, const xAOD::CaloCluster *clus){
    cd(dir);
    ATH_MSG_DEBUG("Fill EFCalo distributions" << dir);
    ATH_MSG_DEBUG("Energy " << clus->e()/1.e3);
    ATH_MSG_DEBUG("eta " << clus->eta());
    ATH_MSG_DEBUG("phi " << clus->phi());
    
    double tmpeta = -999.;
    clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
    double tmpphi = -999.;
    clus->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi);
    ATH_MSG_DEBUG("etacalo " << tmpeta);
    ATH_MSG_DEBUG("phicalo " << tmpphi);
    
    hist1("energyBE0")->Fill(clus->energyBE(0)/1.e3);
    hist1("energyBE1")->Fill(clus->energyBE(1)/1.e3);
    hist1("energyBE2")->Fill(clus->energyBE(2)/1.e3);
    hist1("energyBE3")->Fill(clus->energyBE(3)/1.e3);
    hist1("energy")->Fill(clus->e()/1.e3);
    hist1("eta")->Fill(clus->eta());
    hist1("phi")->Fill(clus->phi());
    hist1("eta_calo")->Fill(tmpeta);
    hist1("phi_calo")->Fill(tmpphi);
}

void TrigEgammaAnalysisBaseTool::fillL2Electron(const std::string dir, const xAOD::TrigElectron *el){
    cd(dir);
    if(!el) ATH_MSG_DEBUG("TrigElectron NULL");

}

void TrigEgammaAnalysisBaseTool::fillL2Calo(const std::string dir, const xAOD::TrigEMCluster *emCluster){
  cd(dir);
  if(!emCluster) ATH_MSG_DEBUG("Online pointer fails"); 
  /*
  ATH_MSG_DEBUG("L2 Calo distributions.");
  bool hasRings = false;
  std::vector<float> ringsE;
  hist1("et")->Fill(emCluster->et()/1.e3);
  hist1("eta")->Fill(emCluster->eta());
  hist1("phi")->Fill(emCluster->phi());
  hasRings = getTrigCaloRings(emCluster, ringsE );
  if(hasRings){
    hist2("ringer_etVsEta")->Fill(emCluster->eta(), emCluster->et()/1.e3);
    ///Fill rings pdf for each ring
    for(unsigned layer =0; layer < 7; ++layer){
      unsigned minRing, maxRing;  std::string strLayer;
      ringer::TrigCaloRingsHelper::parseCaloFeatures( layer, minRing, maxRing, strLayer );
      cd(dir+"/rings_"+strLayer);
      for(unsigned r=minRing; r<=maxRing; ++r){
        stringstream ss;
        ss << "ringer_ring#" << r;
        hist1(ss.str())->Fill( ringsE.at(r) );
      }///loop into rings
    }///loop for each calo layer
  }*/
}


void TrigEgammaAnalysisBaseTool::fillShowerShapes(const std::string dir,const xAOD::Egamma *eg){
    cd(dir);
    ATH_MSG_DEBUG("Fill SS distributions " << dir);
    if(!eg) ATH_MSG_DEBUG("Online pointer fails"); 
    ATH_MSG_DEBUG("Shower Shapes");
    hist1("e011")->Fill(getShowerShape_e011(eg)/1e3);
    hist1("e132")->Fill(getShowerShape_e132(eg)/1e3);
    hist1("e237")->Fill(getShowerShape_e237(eg)/1e3);
    hist1("e277")->Fill(getShowerShape_e277(eg)/1e3);
    hist1("ethad")->Fill(getShowerShape_ethad(eg)/1e3);
    hist1("ethad1")->Fill(getShowerShape_ethad1(eg)/1e3);
    hist1("Rhad")->Fill(getShowerShape_Rhad(eg));
    hist1("Rhad1")->Fill(getShowerShape_Rhad(eg));
    hist1("Reta")->Fill(getShowerShape_Reta(eg));
    hist1("Rphi")->Fill(getShowerShape_Rphi(eg));
    hist1("weta1")->Fill(getShowerShape_weta1(eg));
    hist1("weta2")->Fill(getShowerShape_weta2(eg));
    hist1("wtots1")->Fill(getShowerShape_wtots1(eg));
    hist1("f1")->Fill(getShowerShape_f1(eg));
    hist1("f3")->Fill(getShowerShape_f3(eg));
    if(eg->type()==xAOD::Type::Electron){
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        hist1("et")->Fill(getEt(el)/1e3);
        hist1("highet")->Fill(getEt(el)/1e3);
        hist1("ptcone20")->Fill(getIsolation_ptcone20(el)/1e3);
        if (getEt(el) > 0) {
          hist1("ptcone20_rel")->Fill(getIsolation_ptcone20(el)/getEt(el));
        }
    }
    else if(eg->type()==xAOD::Type::Photon){
        hist1("et")->Fill(getCluster_et(eg)/1e3);
        hist1("highet")->Fill(getCluster_et(eg)/1e3);
    }
    hist1("eta")->Fill(eg->eta());
    hist1("phi")->Fill(eg->phi());

}

void TrigEgammaAnalysisBaseTool::fillTracking(const std::string dir, const xAOD::Electron *eg){
    cd(dir);  
    ATH_MSG_DEBUG("Fill tracking");
    if(!eg) ATH_MSG_DEBUG("Online pointer fails"); 
    float cleta = eg->caloCluster()->eta();
    hist1("deta1")->Fill(getCaloTrackMatch_deltaEta1(eg));
    if(cleta > 1.375 && cleta < 3.2)
        hist1("deta1_EMECA")->Fill(getCaloTrackMatch_deltaEta1(eg));
    if(cleta < -1.375 && cleta > -3.2)
        hist1("deta1_EMECC")->Fill(getCaloTrackMatch_deltaEta1(eg));
    if(cleta > 0 && cleta < 1.375)
        hist1("deta1_EMEBA")->Fill(getCaloTrackMatch_deltaEta1(eg));
    if(cleta < 0 && cleta > -1.375)
        hist1("deta1_EMEBC")->Fill(getCaloTrackMatch_deltaEta1(eg));
    hist1("deta2")->Fill(getCaloTrackMatch_deltaEta2(eg));
    hist1("dphi2")->Fill(getCaloTrackMatch_deltaPhi2(eg));
    hist1("dphiresc")->Fill(getCaloTrackMatch_deltaPhiRescaled2(eg));
    hist1("d0")->Fill(getTrack_d0(eg));
    hist1("d0sig")->Fill(getD0sig(eg));
    hist1("eratio")->Fill(getShowerShape_Eratio(eg));
    hist1("eprobht")->Fill(getTrackSummaryFloat_eProbabilityHT(eg));
    hist1("npixhits")->Fill(getTrackSummary_numberOfPixelHits(eg));
    hist1("nscthits")->Fill(getTrackSummary_numberOfSCTHits(eg));
    hist1("charge")->Fill(eg->charge());
    hist1("pt")->Fill(getTrack_pt(eg)/1e3);
    if (m_detailedHists ) {
        hist2("deta1_vs_clusterEta")->Fill(getCluster_eta(eg),getCaloTrackMatch_deltaEta1(eg));
    }
}

void TrigEgammaAnalysisBaseTool::fillHLTResolution(const std::string dir,const xAOD::Egamma *onl, const xAOD::Egamma *off){
   
    cd(dir);
    ATH_MSG_DEBUG("Fill Resolution");
    float getOnlEt=0;
    if(onl->type()==xAOD::Type::Electron){
        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        float deltaR = 999.0;
	deltaR = dR(elonl->caloCluster()->eta(),elonl->caloCluster()->phi(), eloff->caloCluster()->eta(),eloff->caloCluster()->phi() );
        hist1("pt")->Fill((getTrack_pt(elonl)-getTrack_pt(eloff))/getTrack_pt(eloff));
        hist1("et")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
        hist1("eta")->Fill((elonl->trackParticle()->eta()-eloff->trackParticle()->eta())/eloff->trackParticle()->eta());
        hist1("phi")->Fill((elonl->trackParticle()->phi()-eloff->trackParticle()->phi())/eloff->trackParticle()->phi());
	getOnlEt = getEt(elonl);

	hist2("res_etVsEta")->Fill(elonl->trackParticle()->eta(),
				   (getEt(elonl)-getEt(eloff))/getEt(eloff)
				   );
	hist2("res_etVsEt")->Fill( getEt(elonl)/1e3,
				   (getEt(elonl)-getEt(eloff))/getEt(eloff)
				   );
	float feta = fabs(elonl->trackParticle()->eta());
	if( feta < 1.37 )
	  hist1("res_etInEta0")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
	else if( feta >=1.37 && feta <= 1.52 )
	  hist1("res_etInEta1")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
	else if( feta >= 1.52 && feta < 1.81 )
	  hist1("res_etInEta2")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
	else if( feta >= 1.81 && feta < 2.47 )
	  hist1("res_etInEta3")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
        
        hist1("deta1")->Fill((getCaloTrackMatch_deltaEta1(elonl)-getCaloTrackMatch_deltaEta1(eloff))/getCaloTrackMatch_deltaEta1(eloff));
        hist1("deta2")->Fill((getCaloTrackMatch_deltaEta2(elonl)-getCaloTrackMatch_deltaEta2(eloff))/getCaloTrackMatch_deltaEta2(eloff));
        hist1("dphi2")->Fill((getCaloTrackMatch_deltaPhi2(elonl)-getCaloTrackMatch_deltaPhi2(eloff))/getCaloTrackMatch_deltaPhi2(eloff));
        hist1("dphiresc")->Fill((getCaloTrackMatch_deltaPhiRescaled2(elonl)-getCaloTrackMatch_deltaPhiRescaled2(eloff))/getCaloTrackMatch_deltaPhiRescaled2(eloff));
        hist1("d0")->Fill((getTrack_d0(elonl)-getTrack_d0(eloff))/getTrack_d0(eloff));
        hist1("d0sig")->Fill((getD0sig(elonl)-getD0sig(eloff))/getD0sig(eloff));
        hist1("eprobht")->Fill( (getTrackSummaryFloat_eProbabilityHT(elonl) - getTrackSummaryFloat_eProbabilityHT(eloff))/getTrackSummaryFloat_eProbabilityHT(eloff));
        hist1("npixhits")->Fill(getTrackSummary_numberOfPixelHits(elonl)-getTrackSummary_numberOfPixelHits(elonl));
        hist1("nscthits")->Fill(getTrackSummary_numberOfSCTHits(elonl)-getTrackSummary_numberOfSCTHits(elonl));

        // ptcone20 isolation
        if (getIsolation_ptcone20(eloff) > 0) {
          hist1("ptcone20")->Fill((getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff))/getIsolation_ptcone20(eloff));
          if (getEt(elonl) > 0 && getEt(eloff) > 0) {
            hist1("ptcone20_rel")->Fill(getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff))/(getIsolation_ptcone20(eloff)/getEt(eloff));
            hist2("res_ptcone20_relVsEta")->Fill(elonl->trackParticle()->eta(),
                                                 getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff))/(getIsolation_ptcone20(eloff)/getEt(eloff));
            hist2("res_ptcone20_relVsEt")->Fill(getEt(elonl)/1e3,
                                                getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff))/(getIsolation_ptcone20(eloff)/getEt(eloff));
            hist2("res_ptcone20_relVsMu")->Fill(getAvgMu(),
                                                getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff))/(getIsolation_ptcone20(eloff)/getEt(eloff));
            hist2("res_ptcone20VsMu")->Fill(getAvgMu(),
                                            getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff))/(getIsolation_ptcone20(eloff));
          }
        }
	
    } else{ 
      hist1("et")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      hist1("eta")->Fill((onl->eta()-off->eta())/off->eta());
      hist1("phi")->Fill((onl->phi()-off->phi())/off->phi());

      hist2("res_etVsEta")->Fill(onl->eta(),
				 (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
				 );
      hist2("res_etVsEt")->Fill( getCluster_et(onl)/1e3,
				 (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
				 );
      float feta = fabs(onl->eta());
      const xAOD::Photon* phoff =static_cast<const xAOD::Photon*> (off);
      if(xAOD::EgammaHelpers::isConvertedPhoton(phoff)) {
          hist1("et_cnv")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          hist2("res_cnv_etVsEta")->Fill(onl->eta(),
                  (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
                  );
          hist2("res_cnv_etVsEt")->Fill( getCluster_et(onl)/1e3,
                  (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
                  );

          if( feta < 1.37 )
              hist1("res_cnv_etInEta0")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          else if( feta >=1.37 && feta <= 1.52 )
              hist1("res_cnv_etInEta1")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          else if( feta >= 1.52 && feta < 1.81 )
              hist1("res_cnv_etInEta2")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          else if( feta >= 1.81 && feta < 2.47 )
              hist1("res_cnv_etInEta3")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      }
      else {
          hist1("et_uncnv")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          hist2("res_uncnv_etVsEta")->Fill(onl->eta(),
                  (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
                  );
          hist2("res_uncnv_etVsEt")->Fill( getCluster_et(onl)/1e3,
                  (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
                  );

          if( feta < 1.37 )
              hist1("res_uncnv_etInEta0")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          else if( feta >=1.37 && feta <= 1.52 )
              hist1("res_uncnv_etInEta1")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          else if( feta >= 1.52 && feta < 1.81 )
              hist1("res_uncnv_etInEta2")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
          else if( feta >= 1.81 && feta < 2.47 )
              hist1("res_uncnv_etInEta3")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      }
    
      if( feta < 1.37 )
	hist1("res_etInEta0")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      else if( feta >=1.37 && feta <= 1.52 )
	hist1("res_etInEta1")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      else if( feta >= 1.55 && feta < 1.8 )
	hist1("res_etInEta2")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      else if( feta >= 1.8 && feta < 2.45 )
	hist1("res_etInEta3")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
    }
    
    hist1("e011")->Fill((getShowerShape_e011(onl)-getShowerShape_e011(off))/getShowerShape_e011(off));
    hist1("e132")->Fill((getShowerShape_e132(onl)-getShowerShape_e132(off))/getShowerShape_e132(off));
    hist1("e237")->Fill((getShowerShape_e237(onl)-getShowerShape_e237(off))/getShowerShape_e237(off));
    hist1("e277")->Fill((getShowerShape_e277(onl)-getShowerShape_e277(off))/getShowerShape_e277(off));
    hist1("ethad")->Fill((getShowerShape_ethad(onl)-getShowerShape_ethad(off))/getShowerShape_ethad(off));
    if (m_detailedHists ) {
	hist2("res_ethadVsEta")->Fill(onl->eta(),
				   (getShowerShape_ethad(onl)-getShowerShape_ethad(off))/getShowerShape_ethad(off)
				      );
	hist2("res_ethadVsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_ethad(onl)-getShowerShape_ethad(off))/getShowerShape_ethad(off)
				   );
    }
    hist1("ethad1")->Fill((getShowerShape_ethad1(onl)-getShowerShape_ethad1(off))/getShowerShape_ethad1(off));
    if (m_detailedHists ) {
	hist2("res_ethad1VsEta")->Fill(onl->eta(),
				   (getShowerShape_ethad1(onl)-getShowerShape_ethad1(off))/getShowerShape_ethad1(off)
				   );
	hist2("res_ethad1VsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_ethad1(onl)-getShowerShape_ethad1(off))/getShowerShape_ethad1(off)
				   );
    }
    hist1("Rhad")->Fill((getShowerShape_Rhad(onl)-getShowerShape_Rhad(off))/getShowerShape_Rhad(off));
    if (m_detailedHists ) {
	hist2("res_RhadVsEta")->Fill(onl->eta(),
				   (getShowerShape_Rhad(onl)-getShowerShape_Rhad(off))/getShowerShape_Rhad(off)
				   );
	hist2("res_RhadVsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_Rhad(onl)-getShowerShape_Rhad(off))/getShowerShape_Rhad(off)
				   );
    }
    hist1("Rhad1")->Fill((getShowerShape_Rhad1(onl)-getShowerShape_Rhad1(off))/getShowerShape_Rhad1(off));
    if (m_detailedHists ) {
	hist2("res_Rhad1VsEta")->Fill(onl->eta(),
				   (getShowerShape_Rhad1(onl)-getShowerShape_Rhad1(off))/getShowerShape_Rhad1(off)
				   );
	hist2("res_Rhad1VsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_Rhad1(onl)-getShowerShape_Rhad1(off))/getShowerShape_Rhad1(off)
				   );
    }
    hist1("Reta")->Fill((getShowerShape_Reta(onl)-getShowerShape_Reta(off))/getShowerShape_Reta(off));
    if (m_detailedHists ) {
	hist2("res_RetaVsEta")->Fill(onl->eta(),
				   (getShowerShape_Reta(onl)-getShowerShape_Reta(off))/getShowerShape_Reta(off)
				   );
	hist2("res_RetaVsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_Reta(onl)-getShowerShape_Reta(off))/getShowerShape_Reta(off)
				   );
    }
    hist1("Rphi")->Fill((getShowerShape_Rphi(onl)-getShowerShape_Rphi(off))/getShowerShape_Rphi(off));
    if (m_detailedHists ) {
	hist2("res_RphiVsEta")->Fill(onl->eta(),
				   (getShowerShape_Rphi(onl)-getShowerShape_Rphi(off))/getShowerShape_Rphi(off)
				   );
	hist2("res_RphiVsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_Rphi(onl)-getShowerShape_Rphi(off))/getShowerShape_Rphi(off)
				   );
    }
    hist1("weta1")->Fill((getShowerShape_weta1(onl)-getShowerShape_weta1(off))/getShowerShape_weta1(off));
    if (m_detailedHists ) {
	hist2("res_weta1VsEta")->Fill(onl->eta(),
				   (getShowerShape_weta1(onl)-getShowerShape_weta1(off))/getShowerShape_weta1(off)
				   );
	hist2("res_weta1VsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_weta1(onl)-getShowerShape_weta1(off))/getShowerShape_weta1(off)
				   );
    }
    hist1("weta2")->Fill((getShowerShape_weta2(onl)-getShowerShape_weta2(off))/getShowerShape_weta2(off));
    if (m_detailedHists ) {
	hist2("res_weta2VsEta")->Fill(onl->eta(),
				   (getShowerShape_weta2(onl)-getShowerShape_weta2(off))/getShowerShape_weta2(off)
				   );
	hist2("res_weta2VsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_weta2(onl)-getShowerShape_weta2(off))/getShowerShape_weta2(off)
				   );
    }
    hist1("wtots1")->Fill((getShowerShape_wtots1(onl)-getShowerShape_wtots1(off))/getShowerShape_wtots1(off));
    if (m_detailedHists ) {
	hist2("res_weta2VsEta")->Fill(onl->eta(),
				   (getShowerShape_wtots1(onl)-getShowerShape_wtots1(off))/getShowerShape_wtots1(off)
				   );
	hist2("res_weta2VsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_wtots1(onl)-getShowerShape_wtots1(off))/getShowerShape_wtots1(off)
				   );
    }
    hist1("f1")->Fill((getShowerShape_f1(onl)-getShowerShape_f1(off))/getShowerShape_f1(off));
    if (m_detailedHists ) {
	hist2("res_f1VsEta")->Fill(onl->eta(),
				   (getShowerShape_f1(onl)-getShowerShape_f1(off))/getShowerShape_f1(off)
				   );
	hist2("res_f1VsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_f1(onl)-getShowerShape_f1(off))/getShowerShape_f1(off)
				   );
    }
    hist1("f3")->Fill((getShowerShape_f3(onl)-getShowerShape_f3(off))/getShowerShape_f3(off));
    if (m_detailedHists ) {
	hist2("res_f3VsEta")->Fill(onl->eta(),
				   (getShowerShape_f3(onl)-getShowerShape_f3(off))/getShowerShape_f3(off)
				   );
	hist2("res_f3VsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_f3(onl)-getShowerShape_f3(off))/getShowerShape_f3(off)
				   );
    }
    hist1("eratio")->Fill((getShowerShape_Eratio(onl)-getShowerShape_Eratio(off))/getShowerShape_Eratio(off));
    if (m_detailedHists ) {
	hist2("res_eratioVsEta")->Fill(onl->eta(),
				   (getShowerShape_Eratio(onl)-getShowerShape_Eratio(off))/getShowerShape_Eratio(off)
				   );
	hist2("res_eratioVsEt")->Fill( getOnlEt/1e3,
				   (getShowerShape_Eratio(onl)-getShowerShape_Eratio(off))/getShowerShape_Eratio(off)
				   );
    }
}
void TrigEgammaAnalysisBaseTool::fillHLTAbsResolution(const std::string dir,const xAOD::Egamma *onl, const xAOD::Egamma *off){
   
    cd(dir);
    ATH_MSG_DEBUG("Fill Abs Resolution");
    if(onl->type()==xAOD::Type::Electron){
        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        hist1("pt")->Fill((getTrack_pt(elonl)-getTrack_pt(eloff)));
        hist1("et")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
        hist1("eta")->Fill((elonl->trackParticle()->eta()-eloff->trackParticle()->eta()));
        hist1("phi")->Fill((elonl->trackParticle()->phi()-eloff->trackParticle()->phi()));

        hist2("res_etVsEta")->Fill(elonl->trackParticle()->eta(),
                                   (getEt(elonl)-getEt(eloff)));
        hist2("res_etVsEt")->Fill( getEt(elonl)/1e3,
                                   (getEt(elonl)-getEt(eloff)));

        hist1("ptcone20")->Fill(getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff));

        //ptcone20/pt
        if (getEt(elonl) > 0 && getEt(eloff) > 0) {
          hist1("ptcone20_rel")->Fill(getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
          hist2("res_ptcone20_relVsEta")->Fill(elonl->trackParticle()->eta(),
                                               getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
          hist2("res_ptcone20_relVsEt")->Fill(getEt(elonl)/1e3,
                                              getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
          hist2("res_ptcone20_relVsMu")->Fill(getAvgMu(),
                                              getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
        }
        //ptcone20
        hist2("res_ptcone20VsMu")->Fill(getAvgMu(),
                                        getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff));
        hist2("ptcone20_onVsOff")->Fill(getIsolation_ptcone20(eloff),
                                        getIsolation_ptcone20(elonl));
        if (getEt(elonl) > 0 && getEt(eloff) > 0) {
          hist2("ptcone20_rel_onVsOff")->Fill(getIsolation_ptcone20(eloff)/getEt(eloff),
                                              getIsolation_ptcone20(elonl)/getEt(elonl));
        }

	float feta = fabs(elonl->trackParticle()->eta());
	if( feta < 1.37 )
	  hist1("res_etInEta0")->Fill((getEt(elonl)-getEt(eloff)));
	else if( feta >=1.37 && feta <= 1.52 )
	  hist1("res_etInEta1")->Fill((getEt(elonl)-getEt(eloff)));
	else if( feta >= 1.55 && feta < 1.8 )
	  hist1("res_etInEta2")->Fill((getEt(elonl)-getEt(eloff)));
	else if( feta >= 1.8 && feta < 2.45 )
	  hist1("res_etInEta3")->Fill((getEt(elonl)-getEt(eloff)));
        
        hist1("deta1")->Fill((getCaloTrackMatch_deltaEta1(elonl)-getCaloTrackMatch_deltaEta1(eloff)));
        hist1("deta2")->Fill((getCaloTrackMatch_deltaEta2(elonl)-getCaloTrackMatch_deltaEta2(eloff)));
        hist1("dphi2")->Fill((getCaloTrackMatch_deltaPhi2(elonl)-getCaloTrackMatch_deltaPhi2(eloff)));
        hist1("dphiresc")->Fill((getCaloTrackMatch_deltaPhiRescaled2(elonl)-getCaloTrackMatch_deltaPhiRescaled2(eloff)));
        hist1("d0")->Fill((getTrack_d0(elonl)-getTrack_d0(eloff)));
        hist1("d0sig")->Fill((getD0sig(elonl)-getD0sig(eloff)));
        hist1("eprobht")->Fill( (getTrackSummaryFloat_eProbabilityHT(elonl) - getTrackSummaryFloat_eProbabilityHT(eloff)));
        hist1("npixhits")->Fill(getTrackSummary_numberOfPixelHits(elonl)-getTrackSummary_numberOfPixelHits(elonl));
        hist1("nscthits")->Fill(getTrackSummary_numberOfSCTHits(elonl)-getTrackSummary_numberOfSCTHits(elonl));
    }
    else{ 
      hist1("et")->Fill((getCluster_et(onl)-getCluster_et(off)));
      hist1("eta")->Fill((onl->eta()-off->eta()));
      hist1("phi")->Fill((onl->phi()-off->phi()));

      hist2("res_etVsEta")->Fill(onl->eta(),
				 (getCluster_et(onl)-getCluster_et(off)));
      hist2("res_etVsEt")->Fill( getCluster_et(onl)/1e3,
				 (getCluster_et(onl)-getCluster_et(off)));
      float feta = fabs(onl->eta());
      if( feta < 1.37 )
	hist1("res_etInEta0")->Fill((getCluster_et(onl)-getCluster_et(off)));
      else if( feta >=1.37 && feta <= 1.52 )
	hist1("res_etInEta1")->Fill((getCluster_et(onl)-getCluster_et(off)));
      else if( feta >= 1.55 && feta < 1.8 )
	hist1("res_etInEta2")->Fill((getCluster_et(onl)-getCluster_et(off)));
      else if( feta >= 1.8 && feta < 2.45 )
	hist1("res_etInEta3")->Fill((getCluster_et(onl)-getCluster_et(off)));
    }
    
    hist1("e011")->Fill((getShowerShape_e011(onl)-getShowerShape_e011(off)));
    hist1("e132")->Fill((getShowerShape_e132(onl)-getShowerShape_e132(off)));
    hist1("e237")->Fill((getShowerShape_e237(onl)-getShowerShape_e237(off)));
    hist1("e277")->Fill((getShowerShape_e277(onl)-getShowerShape_e277(off)));
    hist1("ethad")->Fill((getShowerShape_ethad(onl)-getShowerShape_ethad(off)));
    hist1("ethad1")->Fill((getShowerShape_ethad1(onl)-getShowerShape_ethad1(off)));
    hist1("Rhad")->Fill((getShowerShape_Rhad(onl)-getShowerShape_Rhad(off)));
    hist1("Rhad1")->Fill((getShowerShape_Rhad1(onl)-getShowerShape_Rhad1(off)));
    hist1("Reta")->Fill((getShowerShape_Reta(onl)-getShowerShape_Reta(off)));
    hist1("Rphi")->Fill((getShowerShape_Rphi(onl)-getShowerShape_Rphi(off)));
    hist1("weta1")->Fill((getShowerShape_weta1(onl)-getShowerShape_weta1(off)));
    hist1("weta2")->Fill((getShowerShape_weta2(onl)-getShowerShape_weta2(off)));
    hist1("wtots1")->Fill((getShowerShape_wtots1(onl)-getShowerShape_wtots1(off)));
    hist1("f1")->Fill((getShowerShape_f1(onl)-getShowerShape_f1(off)));
    hist1("f3")->Fill((getShowerShape_f3(onl)-getShowerShape_f3(off)));
    hist1("eratio")->Fill((getShowerShape_Eratio(onl)-getShowerShape_Eratio(off)));
}

void TrigEgammaAnalysisBaseTool::fillL2CaloResolution(const std::string dir,const xAOD::TrigEMCluster *onl, const xAOD::Egamma *off){

    cd(dir);
    ATH_MSG_DEBUG("Fill Resolution");
    if(off->type()==xAOD::Type::Electron){
        const xAOD::TrigEMCluster* elonl =onl;
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        float deltaR = 999.0;
        deltaR = dR(elonl->eta(),elonl->phi(), eloff->caloCluster()->eta(),eloff->caloCluster()->phi() );


        hist1("et")->Fill(((elonl->et())-getEt(eloff))/getEt(eloff));
        hist1("eta")->Fill((elonl->eta()-eloff->caloCluster()->eta())/eloff-> caloCluster()->eta());
        hist1("phi")->Fill((elonl->phi()-eloff->caloCluster()->phi())/eloff->caloCluster()->phi());

        hist2("res_etVsEta")->Fill(elonl->eta(),
                                   ((elonl->et())-getEt(eloff))/getEt(eloff)
                                   );
        hist2("res_etVsEt")->Fill( (elonl->et())/1e3,
                                   ((elonl->et())-getEt(eloff))/getEt(eloff)
                                   );

    float elonl_ethad = elonl->energy( CaloSampling::HEC0 ); elonl_ethad += elonl->energy( CaloSampling::HEC1 );
    elonl_ethad += elonl->energy( CaloSampling::HEC2 ); elonl_ethad += elonl->energy( CaloSampling::HEC3 );
    elonl_ethad += elonl->energy( CaloSampling::TileBar0 ); elonl_ethad += elonl->energy( CaloSampling::TileExt0 ); 
    elonl_ethad += elonl->energy( CaloSampling::TileBar1 ); elonl_ethad += elonl->energy( CaloSampling::TileExt1 ); 
    elonl_ethad += elonl->energy( CaloSampling::TileBar2 ); elonl_ethad += elonl->energy( CaloSampling::TileExt2 ); 
    elonl_ethad /= TMath::CosH(elonl->eta() );
    hist1("ethad")->Fill((elonl_ethad-getShowerShape_ethad(off))/getShowerShape_ethad(off));
    if (m_detailedHists ) {
        hist2("res_ethadVsEta")->Fill(elonl->eta(),
                                   (elonl_ethad-getShowerShape_ethad(eloff))/getShowerShape_ethad(eloff)
                                   );
        hist2("res_ethadVsEt")->Fill( elonl->et()/1e3,
                                   (elonl_ethad-getShowerShape_ethad(eloff))/getShowerShape_ethad(eloff)
                                   );
    }
    hist1("ethad1")->Fill(( (onl->ehad1()/TMath::Abs(onl->eta()) )-getShowerShape_ethad1(off))/getShowerShape_ethad1(off));
    if (m_detailedHists ) {
        hist2("res_ethad1VsEta")->Fill(elonl->eta(),
                                   ( (elonl->ehad1()/TMath::Abs(onl->eta()) )-getShowerShape_ethad1(eloff))/getShowerShape_ethad1(eloff)
                                   );
        hist2("res_ethad1VsEt")->Fill( elonl->et()/1e3,
                                   ( (elonl->ehad1()/TMath::Abs(onl->eta()) )-getShowerShape_ethad1(eloff))/getShowerShape_ethad1(eloff)
                                   );
    }
    float elonl_Rhad = elonl_ethad / onl->energy() ;
    hist1("Rhad")->Fill(( elonl_Rhad-getShowerShape_Rhad(off))/getShowerShape_Rhad(off));
    if (m_detailedHists ) {
        hist2("res_RhadVsEta")->Fill(elonl->eta(),
                                   ( elonl_Rhad-getShowerShape_Rhad(eloff))/getShowerShape_Rhad(eloff)
                                   );
        hist2("res_RhadVsEt")->Fill( elonl->et()/1e3,
                                   ( elonl_Rhad-getShowerShape_Rhad(eloff))/getShowerShape_Rhad(eloff)
                                   );
    }
    float elonl_Rhad1 = onl->ehad1() / onl->energy() ;
    hist1("Rhad1")->Fill(( elonl_Rhad1-getShowerShape_Rhad1(off))/getShowerShape_Rhad1(off));
    if (m_detailedHists ) {
        hist2("res_Rhad1VsEta")->Fill(elonl->eta(),
                                   ( elonl_Rhad1-getShowerShape_Rhad1(eloff))/getShowerShape_Rhad1(eloff)
                                   );
        hist2("res_Rhad1VsEt")->Fill( elonl->et()/1e3,
                                   ( elonl_Rhad1-getShowerShape_Rhad1(eloff))/getShowerShape_Rhad1(eloff)
                                   );
    }
    float onl_reta= 999.0;
    if ( fabsf ( onl->e277() ) > 0.01 ) onl_reta = onl->e237() / onl->e277();
    hist1("Reta")->Fill( (onl_reta -getShowerShape_Reta(off))/getShowerShape_Reta(off));
    if (m_detailedHists ) {
        hist2("res_RetaVsEta")->Fill(elonl->eta(),
                                   ( onl_reta-getShowerShape_Reta(eloff))/getShowerShape_Reta(eloff)
                                   );
        hist2("res_RetaVsEt")->Fill( elonl->et()/1e3,
                                   ( onl_reta-getShowerShape_Reta(eloff))/getShowerShape_Reta(eloff)
                                   );
    }
    hist1("weta2")->Fill(( (onl->weta2())-getShowerShape_weta2(off))/getShowerShape_weta2(off));
    if (m_detailedHists ) {
        hist2("res_weta2VsEta")->Fill(elonl->eta(),
                                   ( (elonl->weta2())-getShowerShape_weta2(eloff))/getShowerShape_weta2(eloff)
                                   );
        hist2("res_weta2VsEt")->Fill( elonl->et()/1e3,
                                   ( (elonl->weta2())-getShowerShape_weta2(eloff))/getShowerShape_weta2(eloff)
                                   );
    }

    float onl_f1 = onl->energy(CaloSampling::EMB1)+onl->energy(CaloSampling::EME1);
    onl_f1 /= onl->energy();
    hist1("f1")->Fill(( (onl_f1)-getShowerShape_f1(off))/getShowerShape_f1(off));
    if (m_detailedHists ) {
        hist2("res_f1VsEta")->Fill(elonl->eta(),
                                   ( (onl_f1)-getShowerShape_f1(eloff))/getShowerShape_f1(eloff)
                                   );
        hist2("res_f1VsEt")->Fill( elonl->et()/1e3,
                                   ( (onl_f1)-getShowerShape_f1(eloff))/getShowerShape_f1(eloff)
                                   );
    }
    float onl_f3 = onl->energy(CaloSampling::EMB3)+onl->energy(CaloSampling::EME3);
    onl_f3 /= onl->energy();
    hist1("f3")->Fill(( (onl_f3)-getShowerShape_f3(off))/getShowerShape_f3(off));
    if (m_detailedHists ) {
        hist2("res_f3VsEta")->Fill(elonl->eta(),
                                   ( (onl_f3)-getShowerShape_f3(eloff))/getShowerShape_f3(eloff)
                                   );
        hist2("res_f3VsEt")->Fill( elonl->et()/1e3,
                                   ( (onl_f3)-getShowerShape_f3(eloff))/getShowerShape_f3(eloff)
                                   );
    }
    float onl_eratio = 999.0;
    if ( fabsf(onl->emaxs1() + onl->e2tsts1()) > 0.01 ) 
	onl_eratio = (onl->emaxs1() - onl->e2tsts1()) / (onl->emaxs1() + onl->e2tsts1());
    hist1("eratio")->Fill(( (onl_eratio)-getShowerShape_Eratio(off))/getShowerShape_Eratio(off));
    if (m_detailedHists ) {
        hist2("res_eratioVsEta")->Fill(elonl->eta(),
                                   ( (onl_eratio)-getShowerShape_Eratio(eloff))/getShowerShape_Eratio(eloff)
                                   );
        hist2("res_eratioVsEt")->Fill( elonl->et()/1e3,
                                   ( (onl_eratio)-getShowerShape_Eratio(eloff))/getShowerShape_Eratio(eloff)
                                   );
    }
    }

}

void TrigEgammaAnalysisBaseTool::fillInefficiency(const std::string dir,const xAOD::Electron *selEF,const xAOD::Photon *selPh,const xAOD::CaloCluster *clus,const xAOD::TrackParticle *trk){
    cd(dir);
    ATH_MSG_DEBUG("REGTEST::Inefficiency");
    // Currently check the PID on the xAOD
    // xAOD PID for trigger needs validation!
    // First check for the nullptr
    // 36 bins for isEM
    // 11 bins for isEMLH

    float lastbinIsEM=hist1("IsEmFailTight")->GetNbinsX()-1;
    float lastbinIsEMLH=hist1("IsEmLHFailTight")->GetNbinsX()-1;

    hist1("IsEmFailTight")->Fill(lastbinIsEM+0.5);
    hist1("IsEmFailMedium")->Fill(lastbinIsEM+0.5);
    hist1("IsEmFailLoose")->Fill(lastbinIsEM+0.5);
    hist1("IsEmLHFailTight")->Fill(lastbinIsEMLH+0.5);
    hist1("IsEmLHFailMedium")->Fill(lastbinIsEMLH+0.5);
    hist1("IsEmLHFailLoose")->Fill(lastbinIsEMLH+0.5);
  
    if(selPh==NULL) ATH_MSG_DEBUG("fillIneffiency::No photon found!");
    if(selEF!=NULL){
        ATH_MSG_DEBUG("REGTEST::Inefficiency Electron pt, eta, phi "<< selEF->pt() << " " << selEF->eta() << " " << selEF->phi());

        unsigned int loose = -99;
        unsigned int medium= -99;
        unsigned int tight = -99;
        unsigned int lhloose = -99;
        unsigned int lhmedium= -99;
        unsigned int lhtight = -99;
        selEF->selectionisEM(loose,"isEMLoose");
        selEF->selectionisEM(medium,"isEMMedium");
        selEF->selectionisEM(tight,"isEMTight");
        selEF->selectionisEM(lhloose,"isEMLHLoose");
        selEF->selectionisEM(lhmedium,"isEMLHMedium");
        selEF->selectionisEM(lhtight,"isEMLHTight");

        for(int ii=0;ii<32;ii++){
            if ( (tight>>ii) & 0x1 ){
                hist1("IsEmFailTight")->Fill(ii+0.5);
            }
            if ( (medium>>ii) & 0x1 ){
                hist1("IsEmFailMedium")->Fill(ii+0.5);
            }
            if ( (loose>>ii) & 0x1 ){
                hist1("IsEmFailLoose")->Fill(ii+0.5);
            }
        }
        for(int ii=0;ii<8;ii++){
            if ( (lhtight>>ii) & 0x1 ){
                hist1("IsEmLHFailTight")->Fill(ii+0.5);
            }
            if ( (lhmedium>>ii) & 0x1 ){
                hist1("IsEmLHFailMedium")->Fill(ii+0.5);
            }
            if ( (lhloose>>ii) & 0x1 ){
                hist1("IsEmLHFailLoose")->Fill(ii+0.5);
            }
        }
        
    }
    else {
        if(trk==NULL && clus!=NULL){
            ATH_MSG_DEBUG("fillInefficiency::No Electron, nearby cluster"); 
            // No electron candidate but we have photon
            // Do something for hasCluster
            hist1("IsEmFailTight")->Fill( (lastbinIsEM-3) + 0.5);
            hist1("IsEmFailMedium")->Fill( (lastbinIsEM-3) + 0.5);
            hist1("IsEmFailLoose")->Fill( (lastbinIsEM-3) + 0.5);
            hist1("IsEmLHFailTight")->Fill( (lastbinIsEMLH-3) + 0.5);
            hist1("IsEmLHFailMedium")->Fill( (lastbinIsEMLH-3) + 0.5);
            hist1("IsEmLHFailLoose")->Fill( (lastbinIsEMLH-3) + 0.5);
        }
        if(clus==NULL && trk!=NULL){
            ATH_MSG_DEBUG("fillInefficiency::No Electron, no cluster"); 
            // No electron candidate but we have photon
            // Do something for hasCluster
            hist1("IsEmFailTight")->Fill( (lastbinIsEM-2) + 0.5);
            hist1("IsEmFailMedium")->Fill( (lastbinIsEM-2) + 0.5);
            hist1("IsEmFailLoose")->Fill( (lastbinIsEM-2) + 0.5);
            hist1("IsEmLHFailTight")->Fill( (lastbinIsEMLH-2) + 0.5);
            hist1("IsEmLHFailMedium")->Fill( (lastbinIsEMLH-2) + 0.5);
            hist1("IsEmLHFailLoose")->Fill( (lastbinIsEMLH-2) + 0.5);
        }
        if(clus==NULL && trk==NULL){
            ATH_MSG_DEBUG("fillInefficiency::No Electron, no cluster"); 
            // Unknown failure
            hist1("IsEmFailTight")->Fill( (lastbinIsEM-1) + 0.5);
            hist1("IsEmFailMedium")->Fill( (lastbinIsEM-1) + 0.5);
            hist1("IsEmFailLoose")->Fill( (lastbinIsEM-1) + 0.5);
            hist1("IsEmLHFailTight")->Fill( (lastbinIsEMLH-1) + 0.5);
            hist1("IsEmLHFailMedium")->Fill( (lastbinIsEMLH-1) + 0.5);
            hist1("IsEmLHFailLoose")->Fill( (lastbinIsEMLH-1) + 0.5);
        }
    }
    if(clus!=NULL) ATH_MSG_DEBUG("REGTEST::Inefficiency Cluster " << clus->et() << " " << clus->eta() << " " << clus->phi());
    if(trk!=NULL) ATH_MSG_DEBUG("REGTEST::Inefficiency Track " << trk->pt() << " " << trk->eta() << " " << trk->phi());
    else ATH_MSG_DEBUG("REGTEST::Inefficiency No track");
}

void TrigEgammaAnalysisBaseTool::inefficiency(const std::string basePath,
        const unsigned int runNumber, const unsigned int eventNumber, const float etthr, 
        std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    ATH_MSG_DEBUG("Start Inefficiency Analysis ======================= " << basePath);
    // Inefficiency analysis
    float et=0.;
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    if(pairObj.first->type()==xAOD::Type::Electron){
        ATH_MSG_DEBUG("Offline Electron");
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->eta();
    float phi = eg->phi();
    ATH_MSG_DEBUG("Offline et, eta, phi " << et << " " << eta << " " << phi);
    const xAOD::Electron* selEF = NULL;
    const xAOD::Photon* selPh = NULL;
    const xAOD::CaloCluster* selClus = NULL;
    const xAOD::TrackParticle* selTrk = NULL;
    
    // Can we acquire L1 information 
    //
    //auto initRois = fc.get<TrigRoiDescriptor>();
    //if ( initRois.size() < 1 ) ATH_MSG_DEBUG("No L1 RoI"); 
    //auto itEmTau = m_trigDecTool->ancestor<xAOD::EmTauRoI>(initRois[0]);
    ATH_MSG_DEBUG("Retrieve L1");
    const auto* EmTauRoI = getFeature<xAOD::EmTauRoI>(feat);
    ATH_MSG_DEBUG("Retrieve EF Electron");
    const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
    ATH_MSG_DEBUG("Retrieve EF Photons");
    const auto* EFPh = getFeature<xAOD::PhotonContainer>(feat);
    ATH_MSG_DEBUG("Retrieve EF Cluster");
    const auto* EFClus = getFeature<xAOD::CaloClusterContainer>(feat);
    ATH_MSG_DEBUG("Retrieve EF Trk");
    const auto* EFTrk = getFeature<xAOD::TrackParticleContainer>(feat);
    float dRmax=0.5;
    bool passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
    bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
    if(EmTauRoI==NULL) ATH_MSG_DEBUG("L1 EmTauRoI NULL pointer");
    if(!passedEF && passedEFCalo){
        ATH_MSG_DEBUG("REGEST::Fails EF Electron, passes EFCalo Hypo Run " << runNumber << " Event " << eventNumber);
        if ( EFEl != NULL ){
            ATH_MSG_DEBUG("Retrieved ElectronContainer for inefficiency " << EFEl->size());
            for(const auto& el : *EFEl){
                float dr=dR(eta,phi,el->eta(),el->phi());
                if ( dr<dRmax){
                    dRmax=dr;
                    selEF = el;
                } // dR
            } // loop over EFEl
            ATH_MSG_DEBUG("Closest electron dR " << dRmax);
        } //FC exists
        else ATH_MSG_DEBUG("Electron Container NULL");
        dRmax=0.5;
        if ( EFPh != NULL ){
            ATH_MSG_DEBUG("Retrieved PhotonnContainer for inefficiency " << EFPh->size());
            for(const auto& ph : *EFPh){
                float dr=dR(eta,phi,ph->eta(),ph->phi());
                if ( dr<dRmax){
                    dRmax=dr;
                    selPh = ph;
                } // dR
            } // loop over EFEl
            ATH_MSG_DEBUG("Closest electron dR " << dRmax);
        } //FC exists
        else ATH_MSG_DEBUG("Photon Container NULL");
        dRmax=0.5;
        if ( EFClus != NULL ){
            ATH_MSG_DEBUG("Retrieved ClusterContainer for inefficiency " << EFClus->size());
            for(const auto& clus : *EFClus){
                float dr=dR(eta,phi,clus->eta(),clus->phi());
                if(dr<dRmax){
                    dRmax=dr;
                    selClus = clus;
                } // dR
            } // loop over EFPh
            ATH_MSG_DEBUG("Closest cluster dR " << dRmax);
        }
        else ATH_MSG_DEBUG("CaloCluster Container NULL");
        dRmax=0.5;
        if ( EFTrk != NULL ){
            ATH_MSG_DEBUG("Retrieved TrackContainer for inefficiency " << EFTrk->size());
            for(const auto& trk : *EFTrk){
                float dr=dR(eta,phi,trk->eta(),trk->phi());
                if(dr<dRmax){
                    dRmax=dr;
                    selTrk = trk;
                } // dR
            } // loop over EFPh
            ATH_MSG_DEBUG("Closest track dR " << dRmax);
        } //FC exists
        else ATH_MSG_DEBUG("TrackParticle Container NULL");

        fillInefficiency(basePath,selEF,selPh,selClus,selTrk);
    }
    ATH_MSG_DEBUG("End Inefficiency Analysis ======================= " << basePath);
}

void TrigEgammaAnalysisBaseTool::resolutionL2Photon(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    const xAOD::Photon* phOff =static_cast<const xAOD::Photon*> (pairObj.first);
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedL2Ph = ancestorPassed<xAOD::TrigPhotonContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::TrigPhoton *phL2 = NULL;

    const auto* L2Ph = getFeature<xAOD::TrigPhotonContainer>(feat);
    dRMax=100.;
    if(L2Ph != NULL){
        for(const auto& ph : *L2Ph){
            if(ph == NULL) {
                ATH_MSG_DEBUG("TrigPhoton from TE NULL");
                continue;
            }
            deltaR = dR(phOff->caloCluster()->eta(),phOff->caloCluster()->phi(), ph->eta(),ph->phi());
            if (deltaR < dRMax) {
                dRMax = deltaR;
                phL2 =ph;
            } 
        } //Loop over EF photons
        if(dRMax < 0.05) { 
            if(passedL2Ph && phL2!=NULL){
                //fillRes(trigger,phEF,phOff);
                //fillShowerShapes(trigger,phEF,phOff);           
            } // Is EF Photon
        } // Found closest photon match
    } // Feature Container
    else ATH_MSG_DEBUG("Feature Container NULL");
}

void TrigEgammaAnalysisBaseTool::resolutionL2Electron(const std::string dir,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    cd(dir);
    const xAOD::Egamma *eg = pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedL2Electron = ancestorPassed<xAOD::TrigElectronContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::TrigElectron *trigEl = NULL;

    const auto* L2El = getFeature<xAOD::TrigElectronContainer>(feat);

    // Get the pass bits also
    dRMax=100.;
    if(L2El!=NULL){
        for(const auto& el : *L2El){
            if(el==NULL) {
                ATH_MSG_DEBUG("TrigElectron from TE NULL");
                continue;
            }
            deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), el->eta(),el->phi());
            if (deltaR < dRMax) {
                dRMax = deltaR;
                trigEl=el;
            } 
        } //Loop over EF photons
        if(dRMax < 0.05) { 
            if(passedL2Electron && trigEl!=NULL){
                // Do something, fill resolutions and distributions
                //fillRes(trigger,phEF,phOff);
                //fillShowerShapes(trigger,phEF,phOff);           
            } // Is EF Photon
        } // Found closest photon match
    } // Feature Container
    else ATH_MSG_DEBUG("Feature Container NULL");
}

void TrigEgammaAnalysisBaseTool::resolutionEFCalo(const std::string dir,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    cd(dir);
    const xAOD::Egamma *eg = pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::CaloCluster *clusEF = NULL;

    const auto* EFCalo = getFeature<xAOD::CaloClusterContainer>(feat);
    dRMax=100.;
    if(EFCalo!=NULL){
        for(const auto& clus : *EFCalo){
            if(clus==NULL) {
                ATH_MSG_DEBUG("CaloCluster from TE NULL");
                continue;
            }
            deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), clus->eta(),clus->phi());
            if (deltaR < dRMax) {
                dRMax = deltaR;
                clusEF=clus;
            } 
        } //Loop over EF photons
        if(dRMax < 0.05) { 
            if(passedEFCalo && clusEF!=NULL){
                //fillRes(trigger,phEF,phOff);
                //fillShowerShapes(trigger,phEF,phOff);           
            } // Is EF Photon
        } // Found closest photon match
    } // Feature Container
    else ATH_MSG_DEBUG("Feature Container NULL");
}

void TrigEgammaAnalysisBaseTool::resolutionPhoton(const std::string basePath,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    ATH_MSG_DEBUG("Resolution photon "<< basePath);
    std::string dir1 = basePath + "/Resolutions/HLT";
    std::string dir2 = basePath + "/AbsResolutions/HLT";
    std::string dir3 = basePath + "/Distributions/HLT";
    std::string dir4 = basePath + "/Distributions/Offline";

    const xAOD::Photon* phOff =static_cast<const xAOD::Photon*> (pairObj.first);
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedEFPh = ancestorPassed<xAOD::PhotonContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::Photon *phEF = NULL;

    const auto* EFPh = getFeature<xAOD::PhotonContainer>(feat);
    const TrigPassBits *EFbits = getFeature<TrigPassBits>(feat);
    if(EFbits==NULL) ATH_MSG_DEBUG("PassBits NULL");
    if(EFPh != NULL){
        if(passedEFPh){
            for(const auto& ph : *EFPh){
                if( HLT::isPassing(EFbits,ph,EFPh)) ATH_MSG_DEBUG("Found passing Hypo object");
                else {
                    ATH_MSG_DEBUG("Failed Hypo Selection");
                    continue;
                }
                if(ph == NULL) {
                    ATH_MSG_DEBUG("Photon from TE NULL");
                    continue;
                }
                deltaR = dR(phOff->caloCluster()->eta(),phOff->caloCluster()->phi(), ph->caloCluster()->eta(),ph->caloCluster()->phi());
                if (deltaR < dRMax) {
                    dRMax = deltaR;
                    phEF =ph;
                } 
            } //Loop over EF photons
        } // Passed Hypo
        if(dRMax < 0.05){
            fillHLTResolution(dir1,phEF,phOff);
            fillHLTAbsResolution(dir2,phEF,phOff);
            fillShowerShapes(dir3,phEF); // Fill HLT shower shapes
            fillShowerShapes(dir4,phOff); // Fill Offline shower shapes
        }
    } // Feature Container
    else ATH_MSG_DEBUG("Feature Container NULL");
}
void TrigEgammaAnalysisBaseTool::resolutionElectron(const std::string basePath,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    ATH_MSG_DEBUG("Resolution Electron " << basePath);
    std::string dir1 = basePath + "/Resolutions/HLT";
    std::string dir2 = basePath + "/AbsResolutions/HLT";
    std::string dir3 = basePath + "/Distributions/HLT";
    std::string dir4 = basePath + "/Distributions/Offline";
    std::string dir5 = basePath + "/Resolutions/L2Calo";
    std::string dir6 = basePath + "/Resolutions/L2Calo_vs_HLT";
    std::string dir7 = basePath + "/Resolutions/L1Calo";
    std::string dir8 = basePath + "/AbsResolutions/L1Calo";

    const xAOD::Electron* elOff =static_cast<const xAOD::Electron*> (pairObj.first);
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedEFEl = ancestorPassed<xAOD::ElectronContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::Electron *elEF = NULL;

    const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
    const TrigPassBits *EFbits = getFeature<TrigPassBits>(feat);
    if(EFbits==NULL) ATH_MSG_DEBUG("PassBits NULL");
    // Require passing hypo and passing object
    if(EFEl != NULL) {
        ATH_MSG_DEBUG("Retrieve Electron FC");
        if(passedEFEl){
            for(const auto& el : *EFEl){
                // Only consider passing objects
                if( HLT::isPassing(EFbits,el,EFEl)) ATH_MSG_DEBUG("Found passing Hypo object");
                else {
                    ATH_MSG_DEBUG("Failed Hypo Selection");
                    continue;
                }
                if(el == NULL) {
                    ATH_MSG_DEBUG("Electron from TE NULL");
                    continue;
                }
                deltaR = dR(elOff->trackParticle()->eta(),elOff->trackParticle()->phi(), el->trackParticle()->eta(),el->trackParticle()->phi());
                if (deltaR < dRMax) {
                    dRMax = deltaR;
                    elEF =el;
                }
            } // Loop over EF container
        } // Passed Hypo
        if(dRMax < 0.05){
            fillHLTResolution(dir1,elEF,elOff);
            fillHLTAbsResolution(dir2,elEF,elOff);
            fillShowerShapes(dir3,elEF); // Fill HLT shower shapes
            fillShowerShapes(dir4,elOff); // Fill Offline shower shapes
            fillTracking(dir3,elEF); // Fill HLT shower shapes
            fillTracking(dir4,elOff); // Fill Offline shower shapes
	    const xAOD::TrigEMCluster* clus = getFeature<xAOD::TrigEMCluster>(feat);
	    if ( clus != NULL ) {
		fillL2CaloResolution(dir5,clus, elOff );
		fillL2CaloResolution(dir6,clus, elEF );
	    }
        }
    } // EFEl Feature
    else ATH_MSG_DEBUG("NULL EFEl Feature");
    // L1 resolutions
    if (feat) {
        auto itEmTau = m_trigdec->ancestor<xAOD::EmTauRoI>(feat);
        const xAOD::EmTauRoI *l1 = itEmTau.cptr();
        if (l1) {
            fillL1CaloResolution(dir7, l1, elOff);
            fillL1CaloAbsResolution(dir8, l1, elOff);
        }
    }
    else ATH_MSG_DEBUG("NULL L1 Feature");
}

void TrigEgammaAnalysisBaseTool::resolution(const std::string basePath,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
   
    ATH_MSG_DEBUG("Executing resolution for " << basePath);
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    
    if ( feat!=NULL ) {
        if(eg->type()==xAOD::Type::Electron){
            resolutionElectron(basePath,pairObj);
        } // Offline object Electron
        else if(eg->type()==xAOD::Type::Photon){
            resolutionPhoton(basePath,pairObj);
        } // Offline photon
    }
}

bool TrigEgammaAnalysisBaseTool::isIsolated(const xAOD::Electron *eg, const std::string isolation){
  ATH_MSG_DEBUG("Apply Isolation " << isolation);
  float ptcone20;
  eg->isolationValue(ptcone20, xAOD::Iso::ptcone20);
  ATH_MSG_DEBUG("ptcone20 " << ptcone20);
  if (!(fabs(eg->pt()) > 0)) {
    ATH_MSG_DEBUG("Electron pt is zero, can't calculate relative isolation");
    return false;
  }
  float ptcone20_rel = ptcone20/eg->pt();
  ATH_MSG_DEBUG("Relative isolation value " << ptcone20_rel);
  if (isolation == "Loose"){
    if (ptcone20_rel > 0.1) {
      ATH_MSG_DEBUG("Probe failing isolation");
      return false;
    } else {
      ATH_MSG_DEBUG("Probe passing isolation");
      return true;
    }
  }
  else {
    ATH_MSG_DEBUG("No valid working point defined for " << isolation << " continue without isolation");
  }
  return false;
}

float TrigEgammaAnalysisBaseTool::dR(const float eta1, const float phi1, const float eta2, const float phi2){
    float deta = fabs(eta1 - eta2);
    float dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath:: \
                 Pi() - fabs(phi1 - phi2);
    return sqrt(deta*deta + dphi*dphi);
}
      
float TrigEgammaAnalysisBaseTool::getEta2(const xAOD::Egamma* eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return fabs(cluster->etaBE(2)); 
    }
    else return -99.;
}
float TrigEgammaAnalysisBaseTool::getEt(const xAOD::Electron* eg){
    if(eg && (eg->caloCluster()) && (eg->trackParticle())){
        float eta   = fabs(eg->trackParticle()->eta()); 
        return eg->caloCluster()->e()/cosh(eta);      
    }
    else return -99.;
}
float TrigEgammaAnalysisBaseTool::getEtCluster37(const xAOD::Egamma* eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        float eta2   = fabs(cluster->etaBE(2)); 
        return cluster->e()/cosh(eta2);      
    }
    else return -99.;
}
float TrigEgammaAnalysisBaseTool::getDEmaxs1(const xAOD::Egamma *eg){
    if(eg){
        float emax2=0.;
        eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
        float emax=0.;
        eg->showerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
        float val = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;
        return val;
    }
    else return -99.;
}
float TrigEgammaAnalysisBaseTool::rTRT  (const xAOD::Electron* eg) {
    if(eg && eg->trackParticle()){ 
        uint8_t trtHits   = 0;
        eg->trackParticleSummaryValue(trtHits,xAOD::numberOfTRTHits);
        uint8_t trtHTHits = 0; 
        eg->trackParticleSummaryValue(trtHTHits,xAOD::numberOfTRTHighThresholdHits);
        if(trtHits!=0) {
            return ( (double)trtHTHits / (double)trtHits ); 
        }
        else return -99.;
    }
    else return -99.;
}

float TrigEgammaAnalysisBaseTool::getSigmaD0(const xAOD::Electron *eg){
    const xAOD::TrackParticle* t = eg->trackParticle();   
    float d0sigma=0.;
    if (t)
    {
        float vard0 = t->definingParametersCovMatrix()(0,0);
        if (vard0 > 0) {
            d0sigma=sqrtf(vard0);
        }
        else return -99.;
        return d0sigma;
    }
    else return -99.;
}

float TrigEgammaAnalysisBaseTool::getD0sig(const xAOD::Electron *eg){
    const xAOD::TrackParticle* t = eg->trackParticle();   
    float d0sigma=0.;
    if (t)
    {

        float vard0 = t->definingParametersCovMatrix()(0,0);
        if (vard0 > 0) {
            d0sigma=sqrtf(vard0);
        }
        else return -99.;
        return t->d0()/d0sigma;
    }
    else return -99.;
}

float TrigEgammaAnalysisBaseTool::getEnergyBE0(const xAOD::Egamma *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(0); 
    }
    else return 0;
}

float TrigEgammaAnalysisBaseTool::getEnergyBE1(const xAOD::Egamma *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(1); 
    }
    else return 0.;
}

float TrigEgammaAnalysisBaseTool::getEnergyBE2(const xAOD::Egamma *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(2); 
    }
    else return 0.;
}

float TrigEgammaAnalysisBaseTool::getEnergyBE3(const xAOD::Egamma *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(3); 
    }
    else return 0.;
}

float TrigEgammaAnalysisBaseTool::getEaccordion(const xAOD::Egamma *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        float ebe1 = cluster->energyBE(1); 
        float ebe2 = cluster->energyBE(2); 
        float ebe3 = cluster->energyBE(3);
        return (ebe1+ebe2+ebe3);
    }
    else return 0.;
}

float TrigEgammaAnalysisBaseTool::getE0Eaccordion(const xAOD::Egamma *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        float ebe0 = cluster->energyBE(0); 
        float ebe1 = cluster->energyBE(1); 
        float ebe2 = cluster->energyBE(2); 
        float ebe3 = cluster->energyBE(3);
        float eacc = ebe1+ebe2+ebe3;
        if(eacc==0.) return 0.;
        return (ebe0/eacc);
    }
    else return 0.;
}

float TrigEgammaAnalysisBaseTool::getAvgMu() {
  return m_lumiBlockMuTool->averageInteractionsPerCrossing();
}

/*! Macros for plotting */  
#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getShowerShape_##_name_(const xAOD::Egamma* eg) \
{ float val{-99}; \
    eg->showerShapeValue(val,xAOD::EgammaParameters::_name_); \
    return val; } 
    GETTER(e011)
    GETTER(e132)    
    GETTER(e237)
    GETTER(e277)
    GETTER(ethad)       
    GETTER(ethad1)    
    GETTER(weta1)
    GETTER(weta2)
    GETTER(f1)
    GETTER(e2tsts1) 
    GETTER(emins1)
    GETTER(emaxs1)
    GETTER(wtots1)
    GETTER(fracs1)   
    GETTER(Reta)  
    GETTER(Rphi)    
    GETTER(f3)  
    GETTER(f3core)
    GETTER(Eratio)
    GETTER(Rhad)
    GETTER(Rhad1)  
GETTER(DeltaE)    
#undef GETTER

    // GETTER for Isolation monitoring
#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getIsolation_##_name_(const xAOD::Electron* eg) \
{ float val{-99}; \
    eg->isolationValue(val,xAOD::Iso::_name_); \
    return val; } 
    GETTER(etcone20)
    GETTER(etcone30)
    GETTER(etcone40)    
    GETTER(topoetcone20)
    GETTER(topoetcone30)
    GETTER(topoetcone40)    
    GETTER(ptcone20)
    GETTER(ptcone30)
GETTER(ptcone40)    
#undef GETTER    

    // GETTERs for CaloCluster monitoring   
#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getCluster_##_name_(const xAOD::Egamma* eg) \
{    if(eg && eg->caloCluster()) \
    return eg->caloCluster()->_name_(); \
    else return -99.;}
    GETTER(et)
    GETTER(phi)
GETTER(eta)   
#undef GETTER

    // GETTERs for Track monitoring   
#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getTrack_##_name_(const xAOD::Electron* eg) \
{    if(eg && eg->trackParticle()) \
    return eg->trackParticle()->_name_(); \
    else return -99.;}
    GETTER(pt)
    GETTER(phi)
    GETTER(eta)   
    GETTER(d0)  
GETTER(z0)    
#undef GETTER
    // GETTERs for Track details monitoring    
#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getTrackSummary_##_name_(const xAOD::Electron* eg) \
{ uint8_t val_uint8{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_uint8,xAOD::_name_); \
        return val_uint8; } \
    else return -99; }
    GETTER(numberOfInnermostPixelLayerHits) 
    GETTER(numberOfInnermostPixelLayerOutliers) 
    GETTER(numberOfPixelHits)
    GETTER(numberOfPixelOutliers)
    GETTER(numberOfSCTHits)    
    GETTER(numberOfSCTOutliers)    
    GETTER(numberOfTRTHits)
    GETTER(numberOfTRTHighThresholdHits)
    GETTER(numberOfTRTHighThresholdOutliers)
    GETTER(numberOfTRTOutliers)
    GETTER(expectInnermostPixelLayerHit)
    GETTER(numberOfPixelDeadSensors)
    GETTER(numberOfSCTDeadSensors)
GETTER(numberOfTRTXenonHits)

#undef GETTER

#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getTrackSummaryFloat_##_name_(const xAOD::Electron* eg) \
{ float val_float{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_float,xAOD::_name_); \
        return val_float; } \
    else return -99; }
    GETTER(eProbabilityComb)
    GETTER(eProbabilityHT)
    GETTER(eProbabilityToT)
    GETTER(eProbabilityBrem)
GETTER(pixeldEdx)    

#undef GETTER
    // GETTERs for Calo-Track monitoring
#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getCaloTrackMatch_##_name_(const xAOD::Electron* eg) \
{ float val={-99.}; \
    if(eg && eg->trackParticle()){ \
        eg->trackCaloMatchValue(val,xAOD::EgammaParameters::_name_);} \
    return val; }
    GETTER(deltaEta0)
    GETTER(deltaPhi0)
    GETTER(deltaPhiRescaled0)    
    GETTER(deltaEta1)
    GETTER(deltaPhi1)
    GETTER(deltaPhiRescaled1)    
    GETTER(deltaEta2)
    GETTER(deltaPhi2)
    GETTER(deltaPhiRescaled2)    
    GETTER(deltaEta3)
    GETTER(deltaPhi3)
GETTER(deltaPhiRescaled3) 
#undef GETTER    

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



bool TrigEgammaAnalysisBaseTool::getTrigCaloRings( const xAOD::TrigEMCluster *emCluster, std::vector<float> &ringsE){
  ringsE.clear();
  if(!emCluster)  return false;

  Trig::FeatureContainer fc = (m_trigdec->features("HLT_.*",TrigDefs::alsoDeactivateTEs));
  const std::vector< Trig::Feature<xAOD::TrigRingerRings > > vec_featRinger = fc.get< xAOD::TrigRingerRings >("",TrigDefs::alsoDeactivateTEs);
  ATH_MSG_DEBUG("L2Calo Rings FC Size " << vec_featRinger.size());
  for( Trig::Feature<xAOD::TrigRingerRings > featRinger : vec_featRinger ){
    const xAOD::TrigRingerRings *ringer = featRinger.cptr();
    if(emCluster->RoIword() ==  (getFeature<xAOD::TrigEMCluster>(featRinger.te()))->RoIword() ){
      ATH_MSG_DEBUG("L2Calo Rings matched with TrigEMCluster object.");
      for(unsigned i = 0; i < ringer->size();++i){
        ringsE.push_back(ringer->rings()[i]);
      } // loop over rings
      return true;
    }
  }
  return false;
}


bool TrigEgammaAnalysisBaseTool::getCaloRings( const xAOD::Electron *el, std::vector<float> &ringsE ){
  ringsE.clear();
  /*auto m_ringsELReader = xAOD::getCaloRingsReader();

  // First, check if we can retrieve decoration: 
  const xAOD::CaloRingsELVec *caloRingsELVec(nullptr); 
  try { 
    caloRingsELVec = &(m_ringsELReader->operator()(*el)); 
  } catch ( const std::exception &e) { 
    ATH_MSG_WARNING("Couldn't retrieve CaloRingsELVec. Reason: " << e.what()); 
  } 

  if ( caloRingsELVec->empty() ){ 
    ATH_MSG_WARNING("Particle does not have CaloRings decoratorion.");
    return false;
  }


  // For now, we are using only the first cluster 
  const xAOD::CaloRings *clrings = *(caloRingsELVec->at(0));
  // For now, we are using only the first cluster 
  
  if(clrings) clrings->exportRingsTo(*ringsE);
  else{
    ATH_MSG_WARNING("There is a problem when try to attack the rings vector using exportRigsTo() method.");
    return false;
  }
  */
  return true;
}

void TrigEgammaAnalysisBaseTool::fillL1CaloResolution(const std::string dir,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off){
    cd(dir);
    ATH_MSG_DEBUG("Fill L1CaloResolution");
    if(off->type()==xAOD::Type::Electron){
      const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
      hist2("res_etVsEta")->Fill(l1->eta(),
                                 (l1->emClus()-getEt(eloff))/getEt(eloff));
    }
}

void TrigEgammaAnalysisBaseTool::fillL1CaloAbsResolution(const std::string dir,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off){
    cd(dir);
    ATH_MSG_DEBUG("Fill L1CaloAbsResolution");
    if(off->type()==xAOD::Type::Electron){
      const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
      hist2("res_etVsEta")->Fill(l1->eta(),
                                 0.001*(l1->emClus()-getEt(eloff)));
    }
}

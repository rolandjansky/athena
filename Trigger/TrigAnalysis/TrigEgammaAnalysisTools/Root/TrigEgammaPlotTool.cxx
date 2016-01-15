/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Plot Tool -- for all your plotting needs

// 
#include "TrigEgammaAnalysisTools/TrigEgammaPlotTool.h"
#include "TrigEgammaAnalysisTools/ValidationException.h"

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
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

TrigEgammaPlotTool::
TrigEgammaPlotTool( const std::string& myname )
    : AsgTool(myname) 
{

    declareProperty("DetailedHistograms", m_detailedHists=false);
    declareProperty("doJpsiee",m_doJpsiee=false);
    m_storeGate = nullptr;
    m_histsvc = nullptr;
    m_parent = nullptr;
}

StatusCode TrigEgammaPlotTool::initialize() {

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

    return sc;
}

StatusCode TrigEgammaPlotTool::book(const std::string dir){
    bookAnalysisHistos(dir);
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaPlotTool::execute(){
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaPlotTool::finalize(){
    return StatusCode::SUCCESS;
}

void TrigEgammaPlotTool::cd(const std::string &dir) {
    // Check if it exists!
    m_currentDir = dir;
}

void TrigEgammaPlotTool::addDirectory(const std::string &dir) {
    m_dir.push_back(dir);
    std::stringstream ss;
    // This is not needed, get this from the file path from histobase
    // Basically set via the m_dir property in all the tool classes
    //ss << "HLT/Egamma/" << dir; 
    //
    cd(dir);
}


void TrigEgammaPlotTool::addHistogram(TH1 *h, const std::string &dir) {
    ATH_MSG_VERBOSE("Adding Histogram");
    if (!h)
        throw ValidationException("TrigEgammaPlotTool::addHistogram(TH1 *h == 0, ...)!");


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


    if(m_parent){
        std::set<std::string>::const_iterator itr = m_mongroups.find( theDir );
        if ( itr==m_mongroups.end() ) { 
            m_mongroups.insert( theDir );
            /// create actual mongroup 
            m_parent->addMonGroup(new ManagedMonitorToolBase::MonGroup(m_parent,theDir,ManagedMonitorToolBase::run)); //Can be per run or per lumi block
        }
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

void TrigEgammaPlotTool::addHistogram(TH2 *h, const std::string &dir) {
    if (!h)
        throw ValidationException("TrigEgammaPlotTool::addHistogram(TH2 *h == 0, ...)!");

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

void TrigEgammaPlotTool::addTree(TTree *t, const std::string &dir) {
    if (!t)
        throw ValidationException("TrigEgammaPlotTool::addTree(TTree *t == 0, ...)!");


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

void TrigEgammaPlotTool::setLabels(TH1* histo, const std::vector<std::string>& labels) {
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
void TrigEgammaPlotTool::getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret) {
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

std::string TrigEgammaPlotTool::getPath(const std::string &histName, const std::string &dir) {
    std::string theDir;

    std::vector<std::string>::iterator dirItr;
    if (dir == "") {
        theDir = m_currentDir;
    } else {
        theDir = dir;
    }

    //ATH_MSG_DEBUG("getPath " << m_file << " " << theDir << " " << histName);
    dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
    if (dirItr == m_dir.end())
        throw ValidationException(std::string(std::string("Getting object ") + histName) + std::string(" failed: no directory named ") + theDir);

    std::stringstream ss;
    ss << "/" << m_file << "/" << theDir << "/" << histName;
    return ss.str();
}

TH1 *TrigEgammaPlotTool::hist1(const std::string &histName, const std::string &dir) {
    std::string fullPath = getPath(histName, dir);
    std::map<std::string, TH1 *>::iterator histItr = m_hist1.find(fullPath);
    if (histItr == m_hist1.end())
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!histItr->second)
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

    return histItr->second;
}

TH2 *TrigEgammaPlotTool::hist2(const std::string &histName, const std::string &dir) {
    std::string fullPath = getPath(histName, dir);
    std::map<std::string, TH2 *>::iterator histItr = m_hist2.find(fullPath);
    if (histItr == m_hist2.end())
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!histItr->second)
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

    return histItr->second;
}
TTree *TrigEgammaPlotTool::tree(const std::string &treeName, const std::string &dir) {
    std::string fullPath = getPath(treeName, dir);
    std::map<std::string, TTree *>::iterator treeItr = m_tree.find(fullPath);
    if (treeItr == m_tree.end())
        throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!treeItr->second)
        throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

    return treeItr->second;
}

void TrigEgammaPlotTool::parseCaloRingsLayers( unsigned layer, unsigned &minRing, 
                                                                       unsigned &maxRing, std::string &caloLayerName)
{
  if(layer == 0){minRing = 0;   maxRing = 7 ; caloLayerName = "PreSampler";}
  if(layer == 1){minRing = 8;   maxRing = 71; caloLayerName = "EM1";}
  if(layer == 2){minRing = 72;  maxRing = 79; caloLayerName = "EM2";}
  if(layer == 3){minRing = 80;  maxRing = 87; caloLayerName = "EM3";}
  if(layer == 4){minRing = 88;  maxRing = 91; caloLayerName = "HAD1";}
  if(layer == 5){minRing = 92;  maxRing = 95; caloLayerName = "HAD2";}
  if(layer == 6){minRing = 96;  maxRing = 99; caloLayerName = "HAD3";}
}

void TrigEgammaPlotTool::bookAnalysisHistos(const std::string basePath){
    std::vector <std::string> dirnames;
    ATH_MSG_VERBOSE("Booking Path " << basePath);
 
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
    float etbins_Zee[31]={0.,2.,4.,6.,8.,10.,
                          12.,14.,16.,18.,20.,22.,24.,26.,28.,
                          30.,32.,34.,36.,38.,40.,42.,44.,46.,48.,50.,55.,60.,65.,70.,100.};
    
    float etbins_Jpsiee[52]={ 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5,
                              5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5,
                              10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,
                              15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,
                              20,20.5,21,21.5,22,22.5,23,23.5,24,24.5,
                              25,25.5};
    

    float *etbins;
    int netbins=0;


    if(m_doJpsiee){
      etbins=etbins_Jpsiee;
      netbins=51;
    }
    else {
      etbins=etbins_Zee;
      netbins=30;
    }


    // Strings for inefficiency labels
    std::vector<std::string> label_trigstep;
    label_trigstep.push_back("L2CaloCont");
    label_trigstep.push_back("L2Calo");
    label_trigstep.push_back("L2Cont"); 
    label_trigstep.push_back("L2");
    label_trigstep.push_back("L2TrackCont");
    label_trigstep.push_back("EFCaloCont");   
    label_trigstep.push_back("EFCalo");
    label_trigstep.push_back("EFTrackCont");    
    label_trigstep.push_back("EFTrack");
    label_trigstep.push_back("HLTCont");
    label_trigstep.push_back("HLT");
    
    
    std::vector<std::string> label_hltobj;
    label_hltobj.push_back("ClusterCont");
    label_hltobj.push_back("Cluster");
    label_hltobj.push_back("ClusterMatch");
    label_hltobj.push_back("TrackCont");
    label_hltobj.push_back("Track");
    label_hltobj.push_back("TrackMatch");
    label_hltobj.push_back("PhotonCont");
    label_hltobj.push_back("Photon");
    label_hltobj.push_back("PhotonMatch");
    label_hltobj.push_back("ElectronCont");
    label_hltobj.push_back("Electron");
    label_hltobj.push_back("ElectronMatch");

    std::vector<std::string> label_hltte;
    label_hltte.push_back("L1Calo");
    label_hltte.push_back("L2Calo");
    label_hltte.push_back("L2");
    label_hltte.push_back("EFCalo");
    label_hltte.push_back("EFTrack");
    label_hltte.push_back("HLT");

    

    dirnames.push_back(basePath + "/Efficiency/L1Calo");
    dirnames.push_back(basePath + "/Efficiency/L2Calo");
    dirnames.push_back(basePath + "/Efficiency/L2");
    dirnames.push_back(basePath + "/Efficiency/EFCalo");

    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        
        addHistogram(new TH1F("match_pt", "Trigger Matched Offline p_{T}; p_{T} [GeV] ; Count", netbins,etbins)); 
        addHistogram(new TH1F("match_et", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", netbins,etbins));
        addHistogram(new TH1F("match_highet", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 100, 0., 2000.));
        addHistogram(new TH1F("match_eta", "Trigger Matched Offline #eta; #eta ; Count",20,etabins));
        addHistogram(new TH1F("match_phi", "Trigger Matched #phi; #phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TH1F("match_mu", "Trigger Matched <#mu>; <#mu> ; Count", 50, 0, 100));
        addHistogram(new TH2D("match_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TH2D("match_coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 
        
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count",netbins,etbins)); 
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", netbins, etbins)); 
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eta", "Offline #eta; #eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "Offline #phi; #phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TH1F("mu", "<#mu>; <#mu> ; Count", 50, 0, 100));
        
        addHistogram(new TH2D("et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TH2D("coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 
        
        addHistogram(new TProfile("eff_pt", "#epsilon(p_T); p_{T} ; #epsilon",netbins,etbins)); 
        addHistogram(new TProfile("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Count", netbins,etbins)); 
        addHistogram(new TProfile("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TProfile("eff_eta", "#epsilon(#eta); #eta ; Count", 20, etabins));
        addHistogram(new TProfile("eff_phi", "#epsilon(#phi); #phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TProfile("eff_mu", "#epsilon(<#mu>; <#mu> ; Count", 50, 0, 100));
        addHistogram(new TProfile2D("eff_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TProfile2D("eff_coarse_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 
    }
    
    dirnames.clear();
    dirnames.push_back(basePath + "/Efficiency/HLT");
    
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("match_pt", "Trigger Matched Offline p_{T}; p_{T} [GeV] ; Count", netbins,etbins)); 
        addHistogram(new TH1F("match_et", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", netbins,etbins));
        addHistogram(new TH1F("match_highet", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 100, 0., 2000.));
        addHistogram(new TH1F("match_eta", "Trigger Matched Offline #eta; #eta ; Count",20,etabins));
        addHistogram(new TH1F("match_phi", "Trigger Matched #phi; #phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TH1F("match_mu", "Trigger Matched <#mu>; <#mu> ; Count", 50, 0, 100));
        addHistogram(new TH2D("match_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TH2D("match_coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 
        
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count",netbins,etbins)); 
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", netbins, etbins)); 
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eta", "Offline #eta; #eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "Offline #phi; #phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TH1F("mu", "<#mu>; <#mu> ; Count", 50, 0, 100));
        
        addHistogram(new TH2D("et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TH2D("coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 
        
        addHistogram(new TProfile("eff_pt", "#epsilon(p_T); p_{T} ; #epsilon",netbins,etbins)); 
        addHistogram(new TProfile("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Count", netbins,etbins)); 
        addHistogram(new TProfile("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TProfile("eff_eta", "#epsilon(#eta); #eta ; Count", 20, etabins));
        addHistogram(new TProfile("eff_phi", "#epsilon(#phi); #phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TProfile("eff_mu", "#epsilon(<#mu>; <#mu> ; Count", 50, 0, 100));
        addHistogram(new TProfile2D("eff_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
        addHistogram(new TProfile2D("eff_coarse_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 

        addHistogram(new TProfile("eff_triggerstep","eff_triggerstep",11,0,11));
        addHistogram(new TProfile("eff_hltreco","eff_hltreco",12,0,12));

        addHistogram(new TH1F("IsEmFailLoose","IsEmFailLoose",36,0,36));
        addHistogram(new TH1F("IsEmFailMedium","IsEmFailMedium",36,0,36));
        addHistogram(new TH1F("IsEmFailTight","IsEmFailTight",36,0,36));
        addHistogram(new TH1F("IneffIsEmLoose","IsEmLoose",36,0,36));
        addHistogram(new TH1F("IneffIsEmMedium","IsEmMedium",36,0,36));
        addHistogram(new TH1F("IneffIsEmTight","IsEmTight",36,0,36));
        setLabels(hist1("eff_triggerstep"),label_trigstep);
        setLabels(hist1("eff_hltreco"),label_hltobj);
        /*setLabels(hist1("IsEmFailLoose"),m_labels);
        setLabels(hist1("IsEmFailMedium"),m_labels);
        setLabels(hist1("IsEmFailTight"),m_labels);
        setLabels(hist1("IneffIsEmLoose"),m_labels);
        setLabels(hist1("IneffIsEmMedium"),m_labels);
        setLabels(hist1("IneffIsEmTight"),m_labels);*/
        addHistogram(new TH1F("IsEmLHFailLoose","IsEmLHFailLoose",11,0,11));
        addHistogram(new TH1F("IsEmLHFailMedium","IsEmLHFailMedium",11,0,11));
        addHistogram(new TH1F("IsEmLHFailTight","IsEmLHFailTight",11,0,11));

        if ( m_detailedHists ) {
            std::vector<std::string> effdirs;
            effdirs.push_back(basePath + "/Efficiency/HLT/Loose");
            effdirs.push_back(basePath + "/Efficiency/HLT/Medium");
            effdirs.push_back(basePath + "/Efficiency/HLT/Tight");
            effdirs.push_back(basePath + "/Efficiency/HLT/LHLoose");
            effdirs.push_back(basePath + "/Efficiency/HLT/LHMedium");
            effdirs.push_back(basePath + "/Efficiency/HLT/LHTight");
            effdirs.push_back(basePath + "/Efficiency/HLT/LooseIso");
            effdirs.push_back(basePath + "/Efficiency/HLT/MediumIso");
            effdirs.push_back(basePath + "/Efficiency/HLT/TightIso");
            effdirs.push_back(basePath + "/Efficiency/HLT/LHLooseIso");
            effdirs.push_back(basePath + "/Efficiency/HLT/LHMediumIso");
            effdirs.push_back(basePath + "/Efficiency/HLT/LHTightIso");
            for (int i = 0; i < (int) effdirs.size(); i++) {
                ATH_MSG_VERBOSE(effdirs[i]);
                addDirectory(effdirs[i]);
                addHistogram(new TH1F("match_pt", "Trigger Matched Offline p_{T}; p_{T} [GeV] ; Count", netbins,etbins)); 
                addHistogram(new TH1F("match_et", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", netbins,etbins));
                addHistogram(new TH1F("match_highet", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 100, 0., 2000.));
                addHistogram(new TH1F("match_eta", "Trigger Matched Offline #eta; #eta ; Count",20,etabins));
                addHistogram(new TH1F("match_phi", "Trigger Matched #phi; #phi ; Count", 20, -3.2, 3.2));
                addHistogram(new TH1F("match_mu", "Trigger Matched <#mu>; <#mu> ; Count", 50, 0, 100));
                addHistogram(new TH2D("match_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
                addHistogram(new TH2D("match_coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 

                addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count",netbins,etbins)); 
                addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", netbins, etbins)); 
                addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
                addHistogram(new TH1F("eta", "Offline #eta; #eta ; Count", 20,etabins)); 
                addHistogram(new TH1F("phi", "Offline #phi; #phi ; Count", 20, -3.2, 3.2));
                addHistogram(new TH1F("mu", "<#mu>; <#mu> ; Count", 50, 0, 100));

                addHistogram(new TH2D("et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
                addHistogram(new TH2D("coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins)); 

                addHistogram(new TProfile("eff_pt", "#epsilon(p_T); p_{T} ; #epsilon",netbins,etbins)); 
                addHistogram(new TProfile("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Count", netbins,etbins)); 
                addHistogram(new TProfile("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Count", 100, 0., 2000.));
                addHistogram(new TProfile("eff_eta", "#epsilon(#eta); #eta ; Count", 20, etabins));
                addHistogram(new TProfile("eff_phi", "#epsilon(#phi); #phi ; Count", 20, -3.2, 3.2));
                addHistogram(new TProfile("eff_mu", "#epsilon(<#mu>; <#mu> ; Count", 50, 0, 100));
                addHistogram(new TProfile2D("eff_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",13, default_et_bins, 20, default_eta_bins)); 
                addHistogram(new TProfile2D("eff_coarse_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",6, coarse_et_bins, 8, coarse_eta_bins));
            }
        }
    }
    
    
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/Offline");
    dirnames.push_back(basePath + "/Distributions/HLT");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        
        addHistogram(new TH1F("rejection","N_{TE}; #Step",6,0,6));
        setLabels(hist1("rejection"),label_hltte);
        
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count", 100,0.,200.)); 
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", 100,0.,200.)); 
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2));
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
        //addHistogram(new TH1F("e2tsts1", "e2tsts1; e2tsts1 ; Count", 50, 0., 100.));
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
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("energyBE0", "Cluster Energy BE0; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energyBE1", "Cluster Energy BE1; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energyBE2", "Cluster Energy BE2; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energyBE3", "Cluster Energy BE3; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("energy", "Cluster Energy BE3; E [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("et", "ET; ET [GeV] ; Count", 100, 0., 200.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TH1F("eta_calo", "eta_calo; eta_calo ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("phi_calo", "phi_calo; phi_calo ; Count", 50, -3.14, 3.14));
    }
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L2Photon"); 
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("et", "E_{T}; E_{T} [GeV] ; Count", 100,0.,200.)); 
        addHistogram(new TH1F("eta", "eta; eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2));
    } 
    
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L2Electron");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("trkClusDeta", "Trk Clus Deta; deta ; Count", 50, -0.5, 0.5));
        addHistogram(new TH1F("trkClusDphi", "Trk Clus Dphi; dphi ; Count", 50, -0.5, 0.5)); 
        addHistogram(new TH1F("et", "E_{T}; E_{T} [GeV] ; Count", 100,0.,200.)); 
        addHistogram(new TH1F("eta", "eta; eta ; Count", 20,etabins)); 
        addHistogram(new TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2));
    }

    
    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L2Calo");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_DEBUG(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("et", "HLT E_{T}; E_{T} [GeV]; Count", 100, 0., 200.));
        addHistogram(new TH1F("eta", "#eta; #eta ; Count", 20, etabins));
        addHistogram(new TH1F("phi", "#phi; #phi ; Count", 20, -3.2, 3.2));
        addHistogram(new TH1F("ringer_nnOutput", "Discriminator distribution; nnOutput ; Count", 100, -1, 1));
        addHistogram(new TH2F("ringer_etVsEta", "ringer count as function of #eta and E_{t}; #eta; E_{t} [GeV]; Count",
                              20,etabins, netbins, etbins ));
        
        for(unsigned layer =0; layer < 7; ++layer){
          unsigned minRing, maxRing;  std::string strLayer;
          parseCaloRingsLayers( layer, minRing, maxRing, strLayer );
          addDirectory(dirnames[i]+"/rings_"+strLayer);
          for(unsigned r=minRing; r<=maxRing; ++r){
              std::stringstream ss_title, ss;
            ss_title << "ringer_ring#" << r;  ss << "L2Calo ringer ("<< strLayer <<"); ring#" << r << " E [MeV]; Count";
            addHistogram(new TH1F(ss_title.str().c_str(), ss.str().c_str(), 200, -20, 50000.));
          }
        }///Loop for each calo layers    */
    }///L2Calo monitoring
    

    dirnames.clear();
    dirnames.push_back(basePath + "/Distributions/L1Calo");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("energy", "Cluster Energy; E [GeV] ; Count", 100, 0., 200.));
        addHistogram(new TH1F("roi_et", "RoI word Cluster Energy; E [GeV] ; Count", 100, 0., 200.));
        addHistogram(new TH1F("emIso", "EM Isolation; E [GeV] ; Count", 50, -1., 20.));
        addHistogram(new TH1F("hadCore", "Hadronic Isolation; E [GeV] ; Count", 50, -1., 20.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 50, -2.5, 2.5));
        addHistogram(new TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2));
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
        ATH_MSG_VERBOSE(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("roi_eta", "RoI #eta; #eta ; Count", 51, -2.55, 2.55));
        addHistogram(new TH1F("roi_phi", "RoI #phi; #phi ; Count", 20, -3.2, 3.2));
    }

    dirnames.clear();
    dirnames.push_back(basePath + "/Resolutions/HLT");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_VERBOSE(dirnames[i]);
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
              //addHistogram(new TH1F("e2tsts1", "e2tsts1 resolution; e2tsts1 ; Count", 50, -10, 10.));
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
        ATH_MSG_VERBOSE(dirnames[i]);
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
        //addHistogram(new TH1F("e2tsts1", "e2tsts1 resolution; e2tsts1 ; Count", 50, -10, 10.));
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
        ATH_MSG_VERBOSE(dirnames[i]);
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
    ATH_MSG_VERBOSE("Creating L1Calo resolution hists");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        addDirectory(dirnames[i]);
        addHistogram(new TH2F("res_etVsEta", "L1 cluster Energy resolution as function of L1 #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    50, -2.55, 2.55,
                    200, -1., 1.));
    }
    dirnames.clear();
    dirnames.push_back(basePath + "/AbsResolutions/L1Calo");
    ATH_MSG_VERBOSE("Creating L1Calo Abs resolution hists");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        addDirectory(dirnames[i]);
        addHistogram(new TH2F("res_etVsEta", "L1 cluster Energy resolution as function of L1 #eta; #eta; E_{T}(on)-E_{T}(off); Count",
                    51, -2.55, 2.55,
                    200, -100., 100.));
    }
}


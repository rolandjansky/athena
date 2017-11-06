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
    declareProperty("DirectoryPath",m_baseDir="/HLT/Egamma");
    declareProperty("doJpsiee",m_doJpsiee=false);
    declareProperty("TPTrigger",m_tp=false);
    declareProperty("MaM",m_mam);
    declareProperty("Efficiency",m_effplots);
    declareProperty("Distribution",m_distplots);
    declareProperty("Resolution",m_resplots);

    // Coverity
    m_storeGate = nullptr;
    m_histsvc = nullptr;
    m_parent = nullptr;
    m_nEtbins=0;
    m_nEtabins=0;
    m_ndefaultEtbins=0; 
    m_ndefaultEtabins=0; 
    m_ncoarseEtbins=0; 
    m_ncoarseEtabins=0;
    m_detailedHists=false;
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

    // Initialize private member
    if ( m_detailedHists ) {
        TH1::SetDefaultSumw2();
        TH2::SetDefaultSumw2();
    }
    
    m_label_trigstep.push_back("L2CaloCont");
    m_label_trigstep.push_back("L2Calo");
    m_label_trigstep.push_back("L2Cont"); 
    m_label_trigstep.push_back("L2");
    m_label_trigstep.push_back("L2TrackCont");
    m_label_trigstep.push_back("EFCaloCont");   
    m_label_trigstep.push_back("EFCalo");
    m_label_trigstep.push_back("EFTrackCont");    
    m_label_trigstep.push_back("EFTrack");
    m_label_trigstep.push_back("HLTCont");
    m_label_trigstep.push_back("HLT");

   
    m_label_hltobj.push_back("ClusterCont");
    m_label_hltobj.push_back("Cluster");
    m_label_hltobj.push_back("ClusterMatch");
    m_label_hltobj.push_back("TrackCont");
    m_label_hltobj.push_back("Track");
    m_label_hltobj.push_back("TrackMatch");
    m_label_hltobj.push_back("PhotonCont");
    m_label_hltobj.push_back("Photon");
    m_label_hltobj.push_back("PhotonMatch");
    m_label_hltobj.push_back("ElectronCont");
    m_label_hltobj.push_back("Electron");
    m_label_hltobj.push_back("ElectronMatch");

   
    m_label_hltte.push_back("L1Calo");
    m_label_hltte.push_back("L2Calo");
    m_label_hltte.push_back("L2");
    m_label_hltte.push_back("EFCalo");
    m_label_hltte.push_back("EFTrack");
    m_label_hltte.push_back("HLT");

    return sc;
}

void TrigEgammaPlotTool::copyArrayToVector(std::vector<float> vec,float arr[], int n) {vec.insert(vec.end(), &arr[0], &arr[n]);}
void TrigEgammaPlotTool::copyArrayToVector(std::vector<double> vec,double arr[], int n) {vec.insert(vec.end(), &arr[0], &arr[n]);}

void TrigEgammaPlotTool::setBinning(){


    // Method to define all binning required that is variable
    // Arrays are copied to vectors that are class members

    //Ensure vectors are cleared since book method called from tools
    m_etbins.clear();
    m_etabins.clear();
    m_defaultEtbins.clear();
    m_defaultEtabins.clear();
    m_coarseEtbins.clear();
    m_coarseEtabins.clear();
    m_ringerEtbins.clear();
    m_ringerEtabins.clear();

    // Binning as defined in TP framework
    double coarse_eta_bins[9] ={-2.47,-1.52,-1.37,-0.60,0.00,0.60,1.37,1.52,2.47};

    double coarse_et_bins[7] = {4.,20.,30.,40.,50.,60.,150.};

    double default_eta_bins[21] = {-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.80,-0.60,-0.10,0.00,
        0.10, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47};

    double default_et_bins[14] = {4.,7.,10.,15.,20.,25.,30.,35.,40.,45.,50.,60.,80.,150.};

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

    //float ringer_eta_bins[10]={0.00, 0.60, 0.80, 1.15, 1.37, 1.54, 1.81, 2.01, 2.37, 2.47};
    //float ringer_et_bins[9]  ={4.00,7.00,10.0,15.0,20.0,30.0,40.0,50.0,50000.};
    float ringer_thres_et_bins[6]={15.,20.,30.,40.,50.,50000.};
    float ringer_thres_eta_bins[6]={0.,0.8,1.37,1.54,2.37,2.5};

    if(m_doJpsiee){
        m_nEtbins=51;
        //copyArrayToVector(m_etbins,etbins_Jpsiee,m_nEtbins+1);
        //data.assign(newData, newData + count);
        m_etbins.insert(m_etbins.end(), &etbins_Jpsiee[0], &etbins_Jpsiee[m_nEtbins+1]);
    }
    else {
        m_nEtbins=30;
        //copyArrayToVector(m_etbins,etbins_Zee,m_nEtbins+1);
        m_etbins.insert(m_etbins.end(), &etbins_Zee[0], &etbins_Zee[m_nEtbins+1]);
    }

    // Define the binning
    m_nEtabins=20;
    m_ndefaultEtbins=13;
    m_ndefaultEtabins=20;
    m_ncoarseEtbins=6;
    m_ncoarseEtabins=8;

    //Fill the arrays
    m_etabins.insert(m_etabins.end(), &etabins[0], &etabins[m_nEtabins+1]);
    m_defaultEtbins.insert(m_defaultEtbins.end(), &default_et_bins[0], &default_et_bins[m_ndefaultEtbins+1]);
    m_defaultEtabins.insert(m_defaultEtabins.end(), &default_eta_bins[0], &default_eta_bins[m_ndefaultEtabins+1]);
    m_coarseEtbins.insert(m_coarseEtbins.end(), &coarse_et_bins[0], &coarse_et_bins[m_ncoarseEtbins+1]);
    m_coarseEtabins.insert(m_coarseEtabins.end(), &coarse_eta_bins[0], &coarse_eta_bins[m_ncoarseEtabins+1]);
    m_ringerEtbins.insert(m_ringerEtbins.end(), &ringer_thres_et_bins[0], &ringer_thres_et_bins[6]);
    m_ringerEtabins.insert(m_ringerEtabins.end(), &ringer_thres_eta_bins[0], &ringer_thres_eta_bins[6]);
}

StatusCode TrigEgammaPlotTool::book(std::map<std::string,TrigInfo> trigInfo){

    setBinning();
    bool badconfig=false;
    for(const auto info:trigInfo){
        // Check for existing trigger in list
        if(m_trigInfo.count(info.first) != 0) {
            ATH_MSG_ERROR("Trigger booked! Check list of triggers in tools");
            badconfig=true;
            continue;
        }
        m_trigInfo.insert(info);
        ATH_MSG_INFO("Book " << info.first);
        if(info.second.trigL1){
            bookL1Histos(info.second);
        }else if(m_tp) { //For TP triggers just monitor the distributions
            const std::string basePath=m_baseDir+"/Expert/"+info.first;
            std::vector<std::string> dirnames;
            dirnames.push_back(basePath + "/Distributions/Offline");
            dirnames.push_back(basePath + "/Distributions/HLT");
            for (const auto dir:dirnames){
                ATH_MSG_VERBOSE(dir);
                addDirectory(dir);
                bookDistributionHistos(dir);
                bookEgammaDistributionHistos(dir);
                if(info.second.trigType=="electron")
                    bookElectronDistributionHistos(dir);
                addHistogram(new TH1F("rejection","N_{TE}; #Step",6,0,6));
                setLabels(hist1("rejection"),m_label_hltte);
            }
        }
        else 
            bookExpertHistos(info.second);
    }
    bookShifterHistos();
    if(badconfig) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

void TrigEgammaPlotTool::bookShifterHistos(){
    std::string dirmam;
    std::string dirtrig;
    std::string fullPath;
    std::string category="";
    std::vector<std::string>::iterator dirItr;
    std::map<std::string, TH1 *>::iterator histItr; 
    // Book histograms from existing expert plots
    for(const auto info:m_trigInfo){
        ATH_MSG_INFO(info.first << " ");
        if(getCategoryFromTrigger(info.first,category)){
            dirtrig=m_baseDir+"/Expert/"+info.first;
            dirmam=m_baseDir+"/Shifter/"+category;
            dirItr = std::find(m_dir.begin(), m_dir.end(), dirmam);
            if (dirItr == m_dir.end()){
                ATH_MSG_DEBUG("MaM Booking " << category << " " << info.first);
                addDirectory(dirmam);
                //Add contents of the histograms
                if(m_tp){
                    for(const auto plot:m_distplots){
                        fullPath = getPath(plot, dirtrig+"/Distributions/HLT");
                        histItr = m_hist1.find(fullPath);
                        if (histItr == m_hist1.end())
                            ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                        else
                            addHistogram((TH1*)hist1(plot,dirtrig+"/Distributions/HLT")->Clone());
                    }
                }
                else {
                    for(const auto plot:m_effplots){
                        fullPath = getPath(plot, dirtrig+"/Efficiency/HLT");
                        histItr = m_hist1.find(fullPath);
                        if (histItr == m_hist1.end())
                            ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                        else
                        addHistogram((TH1*)hist1(plot,dirtrig+"/Efficiency/HLT")->Clone());
                    }
                    for(const auto plot:m_distplots){
                        fullPath = getPath(plot, dirtrig+"/Distributions/HLT");
                        histItr = m_hist1.find(fullPath);
                        if (histItr == m_hist1.end())
                            ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                        else
                            addHistogram((TH1*)hist1(plot,dirtrig+"/Distributions/HLT")->Clone());
                    }
                    for(const auto plot:m_resplots){
                        fullPath = getPath(plot, dirtrig+"/Resolutions/HLT");
                        histItr = m_hist1.find(fullPath);
                        if (histItr == m_hist1.end())
                            ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                        else 
                            addHistogram((TH1*)hist1(plot,dirtrig+"/Resolutions/HLT")->Clone());
                    }
                }
            }
        }
    }
}

StatusCode TrigEgammaPlotTool::execute(){
    return StatusCode::SUCCESS;
}

bool TrigEgammaPlotTool::getCategoryFromTrigger(const std::string trigger,std::string &category){
    for(const auto mam:m_mam){
        if(mam.second == trigger){
            category=mam.first;
            return true;
        }
    }
    return false;
}

// Copy contents to shifter plots
StatusCode TrigEgammaPlotTool::finalizeShifterHistos(std::map<std::string,TrigInfo> trigInfo){
    ATH_MSG_INFO("Finalize " << name() << " " << m_tp );
    std::string dirmam;
    std::string dirtrig;
    std::string category="";
    std::string fullPath;
    std::map<std::string, TH1 *>::iterator histItr; 
    for(const auto info:trigInfo){
        if(getCategoryFromTrigger(info.first,category)){
            dirtrig=m_baseDir+"/Expert/"+info.first;
            dirmam=m_baseDir+"/Shifter/"+category;
            if(m_tp){
                ATH_MSG_INFO("Finalize " << name() << " " << info.first << " " << m_tp );
                for(const auto plot:m_distplots){
                    fullPath = getPath(plot, dirtrig+"/Distributions/HLT");
                    histItr = m_hist1.find(fullPath);
                    if (histItr == m_hist1.end())
                        ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                    else
                        hist1(plot,dirmam)->Add(hist1(plot,dirtrig+"/Distributions/HLT"));
                }

            }
            else {
                ATH_MSG_INFO("Finalize " << name() << " " << info.first << " " << m_tp );
                for(const auto plot:m_effplots){
                    fullPath = getPath(plot, dirtrig+"/Efficiency/HLT");
                    histItr = m_hist1.find(fullPath);
                    if (histItr == m_hist1.end())
                        ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                    else
                        hist1(plot,dirmam)->Add(hist1(plot,dirtrig+"/Efficiency/HLT"));
                }
                for(const auto plot:m_distplots){
                    fullPath = getPath(plot, dirtrig+"/Distributions/HLT");
                    histItr = m_hist1.find(fullPath);
                    if (histItr == m_hist1.end())
                        ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                    else
                        hist1(plot,dirmam)->Add(hist1(plot,dirtrig+"/Distributions/HLT"));
                }
                for(const auto plot:m_resplots){
                    fullPath = getPath(plot, dirtrig+"/Resolutions/HLT");
                    histItr = m_hist1.find(fullPath);
                    if (histItr == m_hist1.end())
                        ATH_MSG_DEBUG("MaM skip " << plot << " " << fullPath);
                    else 
                        hist1(plot,dirmam)->Add(hist1(plot,dirtrig+"/Resolutions/HLT"));
                }
            }           
        }
    }
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
    // Set via the m_dir property in all the tool classes
    // ss << "HLT/Egamma/" << dir; 
    //
    cd(dir);
}


void TrigEgammaPlotTool::addHistogram(TH1 *h, const std::string &dir) {
    ATH_MSG_VERBOSE("Adding Histogram");
    if (!h)
        throw ValidationException("TrigEgammaPlotTool::addHistogram(TH1 *h == 0, ...)!");

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
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a nullptr pointer!"));

    return histItr->second;
}

TH2 *TrigEgammaPlotTool::hist2(const std::string &histName, const std::string &dir) {
    std::string fullPath = getPath(histName, dir);
    std::map<std::string, TH2 *>::iterator histItr = m_hist2.find(fullPath);
    if (histItr == m_hist2.end())
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!histItr->second)
        throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a nullptr pointer!"));

    return histItr->second;
}
TTree *TrigEgammaPlotTool::tree(const std::string &treeName, const std::string &dir) {
    std::string fullPath = getPath(treeName, dir);
    std::map<std::string, TTree *>::iterator treeItr = m_tree.find(fullPath);
    if (treeItr == m_tree.end())
        throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! No such object!"));
    if (!treeItr->second)
        throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! It is a nullptr pointer!"));

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

void TrigEgammaPlotTool::bookAbsResolutionHistos(const std::string directory){
    cd(directory);    
    addHistogram(new TH1F("res_pt", "HLT p_{T} resolution; (p_{T}(on)-p_{T}(off)) ; Count", 200, -1.5, 1.5));
    addHistogram(new TH1F("res_et", "HLT E_{T} resolution; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.5, 0.5));
    addHistogram(new TH1F("res_eta", "#eta resolution; (#eta(on)-#eta(off)) ; Count", 40, -0.2, 0.2));
    addHistogram(new TH1F("res_phi", "#phi resolution; (#phi(on)-#phi(off)) ; Count", 40, -0.2, 0.2));

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
    addHistogram(new TH2F("res_ptcone20_onVsOff", "online ptcone20 vs offline ptcone20; offline [MeV]; online [MeV]; Count",
                200, 0.0, 10000.0,
                200, 0.0, 10000.0));
    addHistogram(new TH2F("res_ptcone20_rel_onVsOff", "online ptcone20/pt vs offline ptcone20/pt; offline; online; Count",
                200, 0.0, 0.2,
                200, 0.0, 0.2));

    addHistogram(new TH1F("res_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1));

    // Relative resolutions
    //addHistogram(new TH1F("res_e011", "e011 resolution; (e011(on)-e011(off)) ; Count", 50, -0.05, 0.05));
    //addHistogram(new TH1F("res_e132", "e132 resolution; (e132(on)-e132(off)) ; Count", 50, -0.05, 0.05));
    //addHistogram(new TH1F("res_e237", "e237 resolution; (e237(on)-e237(off)) ; Count", 50, -0.05, 0.05));
    //addHistogram(new TH1F("res_e277", "e277 resolution; (e277(on)-e277(off)) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_ethad", "ethad resolution; (ethad(on)-ethad(off)) ; Count", 100, -10, 10));
    addHistogram(new TH1F("res_ethad1", "ethad1 resolution; (ethad1(on)-ethad1(off)) ; Count", 100, -10, 10));
    addHistogram(new TH1F("res_Rhad", "Rhad resolution; (Rhad(on)-Rhad(off)) ; Count", 50, -10., 10.));
    addHistogram(new TH1F("res_Rhad1", "Rhad1; Rhad1 resolution; (Rhad1(on)-Rhad1(off));Count", 50, -10., 10.));
    addHistogram(new TH1F("res_Reta", "Reta resolution; (Reta(on)-Reta(off)) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_Rphi", "Rphi resolution; (Rphi(on)-Rphi(off)) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_weta1", "weta1 resolution; (weta1(on)-weta1(off)) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_weta2", "weta2 resolution; (weta2(on)-weta2(off)) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_f1", "f1 resolution; (f1(on)-f1(off)) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_f3", "f3 resolution; (f3(on)-f3(off)) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_eratio", "eratio resolution; (eratio(on)-eratio(off)) ; Count", 200, -0.001, 0.001));
    addHistogram(new TH1F("res_deta1", "deta1; deta1 ; (deta1(on)-deta1(off))", 100, -1., 1.));
    addHistogram(new TH1F("res_deta2", "deta2; deta2 ; (deta2(on)-deta2(off))", 100, -1., 1.));
    addHistogram(new TH1F("res_dphi2", "dphi2; dphi2 ; (dphi2(on)-dphi2(off))", 100, -1., 1.));
    addHistogram(new TH1F("res_dphiresc", "dphiresc; (dphires(on)-dphires(off)) ; Count", 100, -1., 1.));
    addHistogram(new TH1F("res_d0", "resolution d0; (d0(on)-d0(off)) ; Count", 100, -0.5, 0.5));
    addHistogram(new TH1F("res_d0sig", "resolution d0sig; (d0sig(on)-d0sig(off)) ; Count", 50, -10, 10));
    addHistogram(new TH1F("res_eprobht","resolution eProbHT; (eProbHT(on)-eProbHT(off)); Count",50, -1, 1));
    // TRT
    addHistogram(new TH2F("res_eprobhtVsPt", "eProbHT resolution as function of p_{T}; p_{T} [GeV]; (eprobHT(on)-eprobHT(off)); Count",
                50, 0., 100.,
                50, -1., 1.));
    addHistogram(new TH2F("res_eprobht_onVsOff", "online eprobHT vs offline eprobHT; offline ; online ; Count",
                50, 0., 1.,
                50, 0., 1.));
    addHistogram(new TH1F("res_nscthits","resolution nSCTHit; (nSCTHits(on)-nSCTHits(off); Count",20, -10, 10));
    addHistogram(new TH1F("res_npixhits","resolution nPixHit; (nPixHits(on)-nPixHits(off)); Count",10, -5, 5));
    addHistogram(new TH1F("res_ptcone20", "resolution ptcone20; ptcone20 (on-off); Count", 200, -20, 20));
    addHistogram(new TH1F("res_ptcone20_rel", "resolution ptcone20/pt; ptcone20/pt (on-off); Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_wtots1", "wtots1 resolution; (wtots1(on)-wtots1(off)) ; Count", 50, -0.05, 0.05));

}

void TrigEgammaPlotTool::bookResolutionHistos(const std::string directory){
    cd(directory);
    addHistogram(new TH1F("res_et", "E_{T} resolution; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 100, -0.1, 0.1));
    addHistogram(new TH1F("res_eta", "#eta resolution; (#eta(on)-#eta(off))/#eta(off) ; Count", 40, -0.2, 0.2));
    addHistogram(new TH1F("res_phi", "#phi resolution; (#phi(on)-#phi(off))/#phi(off) ; Count", 40, -0.2, 0.2));
    addHistogram(new TH2F("res_etVsEta", "E_{T} resolution as function of #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                25, -2.5, 2.5,
                50, -0.1, 0.1));
    addHistogram(new TH2F("res_etVsEt", "E_{T} resolution as function of E_{T}; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                25, 0., 100.,
                50, -0.1, 0.1));
    addHistogram(new TH1F("res_ethad", "ethad resolution; (ethad(on)-ethad(off))/ethad(off) ; Count", 20, -5, 5));
    addHistogram(new TH1F("res_ethad1", "ethad1 resolution; (ethad1(on)-ethad1(off))/ethad1(off) ; Count", 20, -5, 5));
    addHistogram(new TH1F("res_Rhad", "Rhad resolution; (Rhad(on)-Rhad(off))/Rhad(off) ; Count", 20, -10., 10.));
    addHistogram(new TH1F("res_Rhad1", "Rhad1; Rhad1 resolution; (Rhad1(on)-Rhad1(off))/Rhad1(off)", 20, -10., 10.));
    addHistogram(new TH1F("res_Reta", "Reta resolution; (Reta(on)-Reta(off))/Reta(off) ; Count", 20, -0.01, 0.01));
    addHistogram(new TH1F("res_Rphi", "Rphi resolution; (Rphi(on)-Rphi(off))/Rphi(off) ; Count", 20, -0.01, 0.01));
    addHistogram(new TH1F("res_weta1", "weta1 resolution; (weta1(on)-weta1(off))/weta1(off) ; Count", 20, -0.05, 0.05));
    addHistogram(new TH1F("res_weta2", "weta2 resolution; (weta2(on)-weta2(off))/weta2(off) ; Count", 20, -0.05, 0.05));
    addHistogram(new TH1F("res_f1", "f1 resolution; (f1(on)-f1(off))/f1(off) ; Count", 20, -0.05, 0.05));
    addHistogram(new TH1F("res_f3", "f3 resolution; (f3(on)-f3(off))/f3(off) ; Count", 20, -0.05, 0.05));
    addHistogram(new TH1F("res_eratio", "eratio resolution; (eratio(on)-eratio(off))/eratio(off) ; Count", 20, -0.001, 0.001));

}

void TrigEgammaPlotTool::bookElectronResolutionHistos(const std::string directory){
    cd(directory);    
    //Electron
    addHistogram(new TH1F("res_pt", "p_{T} resolution; (p_{T}(on)-p_{T}(off))/p_{T}(off) ; Count", 120, -1.5, 1.5));
    addHistogram(new TH1F("res_deta1", "deta1; deta1 ; (deta1(on)-deta1(off))/deta1(off)", 100, -1., 1.));
    addHistogram(new TH1F("res_deta2", "deta2; deta2 ; (deta2(on)-deta2(off))/deta2(off)", 100, -1., 1.));
    addHistogram(new TH1F("res_dphi2", "dphi2; dphi2 ; (dphi2(on)-dphi2(off))/dphi2(off)", 100, -1., 1.));
    addHistogram(new TH1F("res_dphiresc", "dphiresc; (dphires(on)-dphires(off))/dphires(off) ; Count", 100, -1., 1.));
    addHistogram(new TH1F("res_d0", "resolution d0; (d0(on)-d0(off)) ; Count", 100, -0.5, 0.5));
    addHistogram(new TH1F("res_d0sig", "resolution d0sig; (d0sig(on)-d0sig(off)) ; Count", 50, -10, 10));
    addHistogram(new TH1F("res_eprobht","resolution eProbHT; (eProbHT(on)-eProbHT(off)); Count",50, -1, 1));
    addHistogram(new TH1F("res_nscthits","resolution nSCTHit; (nSCTHits(on)-nSCTHits(off); Count",20, -10, 10));
    addHistogram(new TH1F("res_npixhits","resolution nPixHit; (nPixHits(on)-nPixHits(off)); Count",10, -5, 5));
    // TRT
    addHistogram(new TH2F("res_eprobhtVsPt", "eProbHT resolution as function of p_{T}; p_{T} [GeV]; (eprobHT(on)-eprobHT(off)); Count",
                50, 0., 100.,
                50, -1., 1.));
    addHistogram(new TH2F("res_eprobht_onVsOff", "online eprobHT vs offline eprobHT; offline ; online ; Count",
                50, 0., 1.,
                50, 0., 1.));
}

void TrigEgammaPlotTool::bookElectronIsoResolutionHistos(const std::string directory){
    cd(directory);
    addHistogram(new TH1F("res_ptcone20", "resolution ptcone20; ptcone20 (on-off)/off; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_ptcone20_rel", "resolution ptcone20/pt; ptcone20/pt (on-off)/off; Count", 100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptcone20_relVsEta", "HLT ptcone20/pt resolution as function of #eta; #eta; (on-off)/off; Count",
                50, -2.47, 2.47,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptcone20_relVsEt", "HLT ptcone20/pt resolution as function of E_{T}; E_{T} [GeV]; (on-off)/off; Count",
                50, 0., 100.,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptcone20VsMu", "HLT ptcone20 resolution as function of avg #mu; #mu; (on-off)/off; Count",
                50, 0, 100,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptcone20_relVsMu", "HLT ptcone20/pt resolution as function of avg #mu; #mu; (on-off)/off; Count",
                50, 0, 100,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptcone20_onVsOff", "online ptcone20 vs offline ptcone20; offline [MeV]; online [MeV]; Count",
                100, 0.0, 10000.0,
                100, 0.0, 10000.0));
    addHistogram(new TH2F("res_ptcone20_rel_onVsOff", "online ptcone20/pt vs offline ptcone20/pt; offline; online; Count",
                100, 0.0, 0.2,
                100, 0.0, 0.2));
    
    addHistogram(new TH1F("res_ptvarcone20", "resolution ptvarcone20; ptvarcone20 (on-off)/off; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_ptvarcone20_rel", "resolution ptvarcone20/pt; ptvarcone20/pt (on-off)/off; Count", 100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptvarcone20_relVsEta", "HLT ptvarcone20/pt resolution as function of #eta; #eta; (on-off)/off; Count",
                50, -2.47, 2.47,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptvarcone20_relVsEt", "HLT ptvarcone20/pt resolution as function of E_{T}; E_{T} [GeV]; (on-off)/off; Count",
                50, 0., 100.,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptvarcone20VsMu", "HLT ptvarcone20 resolution as function of avg #mu; #mu; (on-off)/off; Count",
                50, 0, 100,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptvarcone20_relVsMu", "HLT ptvarcone20/pt resolution as function of avg #mu; #mu; (on-off)/off; Count",
                50, 0, 100,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_ptvarcone20_onVsOff", "online ptvarcone20 vs offline ptvarcone20; offline [MeV]; online [MeV]; Count",
                100, 0.0, 10000.0,
                100, 0.0, 10000.0));
    addHistogram(new TH2F("res_ptvarcone20_rel_onVsOff", "online ptvarcone20/pt vs offline ptvarcone20/pt; offline; online; Count",
                100, 0.0, 0.2,
                100, 0.0, 0.2));

}

void TrigEgammaPlotTool::bookPhotonResolutionHistos(const std::string directory){
    cd(directory);    
    addHistogram(new TH1F("res_et_cnv", "HLT E_{T} resolution for converted Photons; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_et_uncnv", "HLT E_{T} resolution for unconverted Photons; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));

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
    addHistogram(new TH1F("res_cnv_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_cnv_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_cnv_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_cnv_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));

    addHistogram(new TH1F("res_uncnv_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_uncnv_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_uncnv_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_uncnv_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));

}
void TrigEgammaPlotTool::bookPhotonIsoResolutionHistos(const std::string directory){
    cd(directory);
    addHistogram(new TH1F("res_topoetcone20", "resolution topoetcone20; ptcone20 (on-off)/off; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_topoetcone20_rel", "resolution topoetcone20/pt; ptcone20/pt (on-off)/off; Count", 100, -0.1, 0.1));
    addHistogram(new TH2F("res_topoetcone20_relVsEta", "HLT topoetcone20/pt resolution as function of #eta; #eta; (on-off)/off; Count",
                50, -2.47, 2.47,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_topoetcone20_relVsEt", "HLT topoetcone20/pt resolution as function of E_{T}; E_{T} [GeV]; (on-off)/off; Count",
                50, 0., 100.,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_topoetcone20VsMu", "HLT topoetcone20 resolution as function of avg #mu; #mu; (on-off)/off; Count",
                50, 0, 100,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_topoetcone20_relVsMu", "HLT topoetcone20/pt resolution as function of avg #mu; #mu; (on-off)/off; Count",
                50, 0, 100,
                100, -0.1, 0.1));
    addHistogram(new TH2F("res_topoetcone20_onVsOff", "online topoetcone20 vs offline topoetcone20; offline [GeV]; online [GeV]; Count",
                100, -10.0, 10.0,
                100, -10.0, 10.0));
    addHistogram(new TH2F("res_topoetcone20_rel_onVsOff", "online topoetcone20/pt vs offline topoetcone20/pt; offline; online; Count",
                100, -0.5, 0.5,
                100, -0.5, 0.5));
    addHistogram(new TH2F("res_topoetcone40_shift_onVsOff", "online topoetcone40-2.45 GeV vs offline topoetcone40-2.45 GeV; offline [GeV]; online [GeV]; Count",
                100, -10.0, 10.0,
                100, -10.0, 10.0));
    addHistogram(new TH2F("res_topoetcone40_shift_rel_onVsOff", "online (topoetcone40-2.45 GeV)/pt vs offline (topoetcone40-2.45 GeV)/pt; offline; online; Count",
                100, -0.5, 0.5,
                100, -0.5, 0.5));
}

void TrigEgammaPlotTool::bookExpertResolutionHistos(const std::string directory){
    cd(directory);    
    
    addHistogram(new TH1F("res_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
    addHistogram(new TH1F("res_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));


    // Relative resolutions
    //addHistogram(new TH1F("res_e011", "e011 resolution; (e011(on)-e011(off))/e011(off) ; Count", 50, -0.05, 0.05));
    //addHistogram(new TH1F("res_e132", "e132 resolution; (e132(on)-e132(off))/e132(off) ; Count", 50, -0.05, 0.05));
    //addHistogram(new TH1F("res_e237", "e237 resolution; (e237(on)-e237(off))/e237(off) ; Count", 50, -0.05, 0.05));
    //addHistogram(new TH1F("res_e277", "e277 resolution; (e277(on)-e277(off))/e277(off) ; Count", 50, -0.05, 0.05));
    addHistogram(new TH1F("res_wtots1", "wtots1 resolution; (wtots1(on)-wtots1(off))/wtots1(off) ; Count", 50, -0.05, 0.05));


    //Detailed resolution plots
    if ( m_detailedHists ) {
        addHistogram(new TH2F("res_ethadVsEta", "HLT E_{T} Had resolution as function of #eta; #eta; (ethad(on)-ethad(off))/ethad(off); Count",
                    50, -2.47, 2.47,
                    50, -0.5, 0.5));
        addHistogram(new TH2F("res_ethadVsEt", "HLT E_{T} Had resolution as function of E_{T}; E_{T} [GeV]; (ethad(on)-ethad(off))/ethad(off); Count",
                    50, 0., 100.,
                    50, -0.5, 0.5));
        addHistogram(new TH2F("res_ethad1VsEta", "HLT E_{T} Had1 resolution as function of #eta; #eta; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                    50, -2.47, 2.47,
                    50, -0.5, 0.5));
        addHistogram(new TH2F("res_ethad1VsEt", "HLT E_{T} Had1 resolution as function of E_{T}; E_{T} [GeV]; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                    50, 0., 100.,
                    50, -0.5, 0.5));
        addHistogram(new TH2F("res_RhadVsEta", "HLT E_{T} Rhad resolution as function of #eta; #eta; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                    50, -2.47, 2.47,
                    50, -10, 10));
        addHistogram(new TH2F("res_RhadVsEt", "HLT E_{T} RHad resolution as function of E_{T}; E_{T} [GeV]; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                    50, 0., 100.,
                    50, -10, 10));
        addHistogram(new TH2F("res_Rhad1VsEta", "HLT E_{T} Rhad1 resolution as function of #eta; #eta; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                    50, -2.47, 2.47,
                    50, -10, 10));
        addHistogram(new TH2F("res_Rhad1VsEt", "HLT E_{T} RHad1 resolution as function of E_{T}; E_{T} [GeV]; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                    50, 0., 100.,
                    50, -10, 10));
        addHistogram(new TH2F("res_RetaVsEta", "HLT Reta resolution as function of #eta; #eta; (Reta(on)-Reta(off))/Reta(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_RetaVsEt", "HLT Reta resolution as function of E_{T}; E_{T} [GeV]; (Reta(on)-Reta(off))/Reta(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_RphiVsEta", "HLT Rphi resolution as function of #eta; #eta; (Rphi(on)-Rphi(off))/Rphi(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_RphiVsEt", "HLT Rphi resolution as function of E_{T}; E_{T} [GeV]; (Rphi(on)-Rphi(off))/Rphi(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_weta1VsEta", "HLT weta1 resolution as function of #eta; #eta; (weta1(on)-weta1(off))/weta1(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_weta1VsEt", "HLT weta1 resolution as function of E_{T}; E_{T} [GeV]; (weta1(on)-weta1(off))/weta1(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_weta2VsEta", "HLT weta2 resolution as function of #eta; #eta; (weta2(on)-weta2(off))/weta2(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_weta2VsEt", "HLT weta2 resolution as function of E_{T}; E_{T} [GeV]; (weta2(on)-weta2(off))/weta2(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_wtots1VsEta", "HLT wtots1 resolution as function of #eta; #eta; (wtots1(on)-wtots1off))/wtots1(off); Count",
                    50,-2.47,2.47,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_wtots1VsEt", "HLT wtots1 resolution as function of E_{T}; E_{T} [GeV]; (wtots1(on)-wtots1(off))/wtots1(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_f1VsEta", "HLT f1 resolution as function of #eta; #eta; (f1(on)-f1(off))/f1(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_f1VsEt", "HLT f1 resolution as function of E_{T}; E_{T} [GeV]; (f1(on)-f1(off))/f1(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_f3VsEta", "HLT f3 resolution as function of #eta; #eta; (f3(on)-f3(off))/f3(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_f3VsEt", "HLT f3 resolution as function of E_{T}; E_{T} [GeV]; (f3(on)-f3(off))/f3(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05));
        addHistogram(new TH2F("res_e2tsts1VsEta", "HLT e2tsts1 resolution as function of #eta; #eta; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                    50, -2.47, 2.47,
                    50, -10, 10));
        addHistogram(new TH2F("res_e2tsts1VsEt", "HLT e2tsts1 resolution as function of E_{T}; E_{T} [GeV]; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                    50, 0., 100.,
                    50, -10, 10));
        addHistogram(new TH2F("res_eratioVsEta", "HLT eratio resolution as function of #eta; #eta; (eratio(on)-eratio(off))/eratio(off); Count",
                    50, -2.47, 2.47,
                    50, -0.001, 0.001));
        addHistogram(new TH2F("res_eratioVsEt", "HLT eratio resolution as function of E_{T}; E_{T} [GeV]; (eratio(on)-eratio(off))/eratio(off); Count",
                    50, 0., 100.,
                    50, -0.001, 0.001));

    }
}

void TrigEgammaPlotTool::bookExpertL2CaloResolutionHistos(const std::string directory){
    cd(directory);
    addHistogram(new TH2F("res_f3VsEta", "L2Calo f3 resolution as function of #eta; #eta; (f3(on)-f3(off))/f3(off); Count",
                50, -2.47, 2.47,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_f3VsEt", "L2Calo f3 resolution as function of E_{T}; E_{T} [GeV]; (f3(on)-f3(off))/f3(off); Count",
                50, 0., 100.,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_f1VsEta", "L2Calo f1 resolution as function of #eta; #eta; (f1(on)-f1(off))/f1(off); Count",
                50, -2.47, 2.47,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_f1VsEt", "L2Calo f1 resolution as function of E_{T}; E_{T} [GeV]; (f1(on)-f1(off))/f1(off); Count",
                50, 0., 100.,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_weta2VsEta", "L2Calo weta2 resolution as function of #eta; #eta; (weta2(on)-weta2(off))/weta2(off); Count",
                50, -2.47, 2.47,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_weta2VsEt", "L2Calo weta2 resolution as function of E_{T}; E_{T} [GeV]; (weta2(on)-weta2(off))/weta2(off); Count",
                50, 0., 100.,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_weta1VsEta", "L2Calo weta1 resolution as function of #eta; #eta; (weta1(on)-weta1(off))/weta1(off); Count",
                50, -2.47, 2.47,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_weta1VsEt", "L2Calo weta1 resolution as function of E_{T}; E_{T} [GeV]; (weta1(on)-weta1(off))/weta1(off); Count",
                50, 0., 100.,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_RetaVsEta", "L2Calo Reta resolution as function of #eta; #eta; (Reta(on)-Reta(off))/Reta(off); Count",
                50, -2.47, 2.47,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_RetaVsEt", "L2Calo Reta resolution as function of E_{T}; E_{T} [GeV]; (Reta(on)-Reta(off))/Reta(off); Count",
                50, 0., 100.,
                50, -0.05, 0.05));
    addHistogram(new TH2F("res_Rhad1VsEta", "L2Calo E_{T} Rhad1 resolution as function of #eta; #eta; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                50, -2.47, 2.47,
                50, -10, 10));
    addHistogram(new TH2F("res_Rhad1VsEt", "L2Calo E_{T} RHad1 resolution as function of E_{T}; E_{T} [GeV]; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                50, 0., 100.,
                50, -10, 10));
    addHistogram(new TH2F("res_RhadVsEta", "L2Calo E_{T} Rhad resolution as function of #eta; #eta; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                50, -2.47, 2.47,
                50, -10, 10));
    addHistogram(new TH2F("res_RhadVsEt", "L2Calo E_{T} RHad resolution as function of E_{T}; E_{T} [GeV]; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                50, 0., 100.,
                50, -10, 10));
    addHistogram(new TH2F("res_ethad1VsEta", "L2Calo E_{T} Had1 resolution as function of #eta; #eta; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                50, -2.47, 2.47,
                50, -0.5, 0.5));
    addHistogram(new TH2F("res_ethad1VsEt", "L2Calo E_{T} Had1 resolution as function of E_{T}; E_{T} [GeV]; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                50, 0., 100.,
                50, -0.5, 0.5));
    addHistogram(new TH2F("res_ethadVsEta", "L2Calo E_{T} Had resolution as function of #eta; #eta; (ethad(on)-ethad(off))/ethad(off); Count",
                50, -2.47, 2.47,
                50, -0.5, 0.5));
    addHistogram(new TH2F("res_ethadVsEt", "L2Calo E_{T} Had resolution as function of E_{T}; E_{T} [GeV]; (ethad(on)-ethad(off))/ethad(off); Count",
                50, 0., 100.,
                50, -0.5, 0.5));
    addHistogram(new TH2F("res_eratioVsEta", "L2Calo eratio resolution as function of #eta; #eta; (eratio(on)-eratio(off))/eratio(off); Count",
                50, -2.47, 2.47,
                50, -0.001, 0.001));
    addHistogram(new TH2F("res_eratioVsEt", "L2Calo eratio resolution as function of E_{T}; E_{T} [GeV]; (eratio(on)-eratio(off))/eratio(off); Count",
                50, 0., 100.,
                50, -0.001, 0.001));
}

void TrigEgammaPlotTool::bookDistributionHistos(const std::string directory){
    cd(directory);
    addHistogram(new TH1F("et", "ET; ET [GeV] ; Count", 100, 0., 100.));
    addHistogram(new TH1F("eta", "eta; eta ; Count", m_nEtabins,m_etabins.data())); 
    addHistogram(new TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2));
}

void TrigEgammaPlotTool::bookEgammaDistributionHistos(const std::string directory){
    cd(directory);
    addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
    //addHistogram(new TH1F("e011", "e011; e011 ; Count", 165, -15., 150.));
    //addHistogram(new TH1F("e132", "e132; e132 ; Count", 165, -15., 150.));
    //addHistogram(new TH1F("e237", "e237; e237 ; Count", 215, -15., 200.));
    //addHistogram(new TH1F("e277", "e277; e277 ; Count", 215, -15., 200.));
    addHistogram(new TH1F("ethad", "ethad; ethad ; Count", 20, -10, 10));
    addHistogram(new TH1F("ethad1", "ethad1; ehad1 ; Count", 20, -10, 10));
    addHistogram(new TH1F("weta1", "weta1; weta1 ; Count", 12, 0.4, 1.));
    addHistogram(new TH1F("weta2", "weta2; weta2 ; Count", 20, 0., 0.02));
    //addHistogram(new TH1F("wtots1", "wtots1; wtots1 ; Count", 50, 0., 0.05));
    addHistogram(new TH1F("f1", "f1; f1 ; Count", 11, -0.1, 1.));
    addHistogram(new TH1F("f3", "f3; f3 ; Count", 21, -0.1, 0.2));
    addHistogram(new TH1F("Reta", "Reta; Reta ; Count", 15, 0., 1.5));
    addHistogram(new TH1F("Rphi", "Rphi; Rphi ; Count", 15, 0., 1.5));
    addHistogram(new TH1F("Rhad", "Rhad; Rhad ; Count", 35, -0.3, 0.3));
    addHistogram(new TH1F("Rhad1", "Rhad1; Rhad1 ; Count", 30, -0.3, 0.3));
    addHistogram(new TH1F("eratio","eratio; eratio; Count",20, 0, 2));
    addHistogram(new TH1F("topoetcone20", "topoetcone20; topoetcone20 [GeV] ; Count", 100, -10.0, 10.0));
    addHistogram(new TH1F("topoetcone20_rel", "topoetcone20/pt; topoetcone20/pt ; Count", 100, -0.5, 0.5));
    addHistogram(new TH1F("topoetcone40_shift", "topoetcone40-2.45 GeV; topoetcone40-2.45 GeV [GeV] ; Count", 100, -10.0, 10.0));
    addHistogram(new TH1F("topoetcone40_shift_rel", "(topoetcone40-2.45 GeV)/pt; (topoetcone40-2.45 GeV)/pt ; Count", 100, -0.5, 0.5));
}

void TrigEgammaPlotTool::bookElectronDistributionHistos(const std::string directory){
    cd(directory);
    addHistogram(new TH1F("pt", "p_{T}; p_{T} [GeV] ; Count", 100,0.,100.)); 
    addHistogram(new TH1F("deta1", "deta1; deta1 ; Count", 40, -0.01, 0.01));
    addHistogram(new TH1F("deta1_EMECA", "deta1 EMEC-A; deta1 ; Count", 40, -0.01, 0.01));
    addHistogram(new TH1F("deta1_EMECC", "deta1 EMEC-C; deta1 ; Count", 40, -0.01, 0.01));
    addHistogram(new TH1F("deta1_EMEBA", "deta1 EMEB-A; deta1 ; Count", 40, -0.01, 0.01));
    addHistogram(new TH1F("deta1_EMEBC", "deta1 EMEB-A; deta1 ; Count", 40, -0.01, 0.01));
    addHistogram(new TH1F("deta2", "deta2; deta2 ; Count", 40, -0.01, 0.01));
    addHistogram(new TH1F("dphi2", "dphi2; dphi2 ; Count", 40, -0.1, 0.1));
    addHistogram(new TH1F("dphiresc", "dphiresc; dphiresc ; Count", 40, -0.1, 0.1));
    addHistogram(new TH1F("d0", "d0; d0 ; Count", 40, -1, 1));
    addHistogram(new TH1F("d0sig", "d0sig; d0sig ; Count", 40, -10, 10));
    addHistogram(new TH1F("eprobht","eProbHT; eProbHT; Count",20, 0, 1.));
    addHistogram(new TH1F("nscthits","nSCTHit; nSCTHits; Count",20, 0, 20));
    addHistogram(new TH1F("npixhits","nPixHit; nPixHits; Count",10, 0, 10));
    addHistogram(new TH1F("charge","charge; charge; Count", 4,-2,2));
    addHistogram(new TH1F("ptcone20", "ptcone20; ptcone20; Count", 50, 0.0, 5.0));
    addHistogram(new TH1F("ptcone20_rel", "ptcone20/pt; ptcone20/pt; Count", 50, 0.0, 1.0));
    addHistogram(new TH1F("ptvarcone20", "ptcone20; ptcone20; Count", 50, 0.0, 5.0));
    addHistogram(new TH1F("ptvarcone20_rel", "ptcone20/pt; ptcone20/pt; Count", 50, 0.0, 1.0));
    
    if(m_detailedHists){
        addHistogram(new TH2F("deta1_vs_clusterEta", "HLT deta1 as function of cluster #eta; #eta; deta1; Count",
                    50, -2.47, 2.47,
                    90, -0.03, 0.03));
    }

}

void TrigEgammaPlotTool::bookEfficiency2DHistos(const std::string directory){
    cd(directory);
    
    addHistogram(new TH2F("match_coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                m_ncoarseEtbins, m_coarseEtbins.data(), m_ncoarseEtabins, m_coarseEtabins.data())); 
    addHistogram(new TH2F("coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                m_ncoarseEtbins, m_coarseEtbins.data(), m_ncoarseEtabins, m_coarseEtabins.data())); 
    addHistogram(new TProfile2D("eff_coarse_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",
                m_ncoarseEtbins, m_coarseEtbins.data(), m_ncoarseEtabins, m_coarseEtabins.data())); 
    
    if(m_detailedHists){
        addHistogram(new TH2F("match_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                    m_ndefaultEtbins, m_defaultEtbins.data(), m_ndefaultEtabins, m_defaultEtabins.data())); 
        addHistogram(new TH2F("et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                    m_ndefaultEtbins, m_defaultEtbins.data(), m_ndefaultEtabins, m_defaultEtabins.data())); 
        addHistogram(new TProfile2D("eff_et_eta","#epsilon(#eta,E_{T}); E_{T} GeV ;#eta; Count",
                    m_ndefaultEtbins, m_defaultEtbins.data(), m_ndefaultEtabins, m_defaultEtabins.data())); 
    }

}

void TrigEgammaPlotTool::bookEfficiencyTProfile(const std::string directory){
    cd(directory);
    addHistogram(new TProfile("eff_pt", "#epsilon(p_T); p_{T} ; Efficiency",m_nEtbins,m_etbins.data())); 
    addHistogram(new TProfile("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Efficiency", m_nEtbins,m_etbins.data())); 
    addHistogram(new TProfile("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Efficiency", 40, 0., 1000.));
    addHistogram(new TProfile("eff_eta", "#epsilon(#eta); #eta ; Efficiency", m_nEtabins, m_etabins.data()));
    addHistogram(new TProfile("eff_phi", "#epsilon(#phi); #phi ; Efficiency", 20, -3.2, 3.2));
    addHistogram(new TProfile("eff_mu", "#epsilon(<#mu>); <#mu> ; Efficiency", 16, 0, 80));

    if(m_detailedHists)
      addHistogram(new TProfile("eff_npvtx", "#epsilon(npvtx); npvtx ; Efficiency", 16, 0, 80));
}

void TrigEgammaPlotTool::bookEfficiencyHistos(const std::string directory){ 
    cd(directory);
    bookEfficiencyTProfile(directory);
    // Numerator
    addHistogram(new TH1F("match_pt", "Trigger Matched Offline p_{T}; p_{T} [GeV] ; Count", m_nEtbins,m_etbins.data())); 
    addHistogram(new TH1F("match_et", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", m_nEtbins,m_etbins.data()));
    addHistogram(new TH1F("match_highet", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 40, 0., 1000.));
    addHistogram(new TH1F("match_eta", "Trigger Matched Offline #eta; #eta ; Count",m_nEtabins,m_etabins.data()));
    addHistogram(new TH1F("match_phi", "Trigger Matched #phi; #phi ; Count", 20, -3.2, 3.2));
    addHistogram(new TH1F("match_mu", "Trigger Matched <#mu>; <#mu> ; Count", 16, 0, 80));
    if(m_detailedHists)
      addHistogram(new TH1F("match_npvtx", "Trigger Matched npvtx; npvtx ; Count", 16, 0, 80));

    // Denominator
    addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count",m_nEtbins,m_etbins.data())); 
    addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", m_nEtbins,m_etbins.data())); 
    addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 40, 0., 1000.));
    addHistogram(new TH1F("eta", "Offline #eta; #eta ; Count", m_nEtabins,m_etabins.data())); 
    addHistogram(new TH1F("phi", "Offline #phi; #phi ; Count", 20, -3.2, 3.2));
    addHistogram(new TH1F("mu", "<#mu>; <#mu> ; Count", 16, 0, 80));
    if(m_detailedHists)
      addHistogram(new TH1F("npvtx", "npvtx; npvtx ; Count", 16, 0, 80));
}

void TrigEgammaPlotTool::bookL1Histos(TrigInfo trigInfo){
    const std::string basePath=m_baseDir+"/Expert/"+trigInfo.trigName;
    std::vector <std::string> dirnames;
    
    std::string dirname=basePath + "/Efficiency/L1Calo";
    addDirectory(dirname);
    bookEfficiencyHistos(dirname);
    bookEfficiency2DHistos(dirname);
    
    if (m_doEmulation){
      dirname=basePath + "/Emulation/L1Calo";
      addDirectory(dirname);
      bookEfficiencyHistos(dirname);
      bookEfficiency2DHistos(dirname);
    }//book emulation

    dirname=basePath + "/Distributions/L1Calo";
    addDirectory(dirname);
    addHistogram(new TH1F("energy", "Cluster Energy; E [GeV] ; Count", 100, 0., 200.));
    addHistogram(new TH1F("roi_et", "RoI word Cluster Energy; E [GeV] ; Count", 100, 0., 200.));
    addHistogram(new TH1F("emIso", "EM Isolation; E [GeV] ; Count", 50, -1., 20.));
    addHistogram(new TH1F("hadCore", "Hadronic Isolation; E [GeV] ; Count", 50, -1., 20.));
    addHistogram(new TH1F("eta", "eta; eta ; Count", 50, -2.5, 2.5));
    addHistogram(new TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2));
    addHistogram(new TH2F("emClusVsEta", "L1 Cluster Energy vs L1 #eta; #eta; E [GeV]; Count",
                51, -2.55, 2.55,
                60, 0, 60));
    addHistogram(new TH2F("emClusVsEmIsol", "L1 Cluster Energy vs emIsol; emIsol [GeV]; E [GeV]; Count",
                20, -0.1, 9.9,
                60, 0, 60));
    addHistogram(new TH2F("emClusVsHadCore", "L1 Cluster Energy vs hadCore; hadCore [GeV]; E [GeV]; Count",
                10, -0.1, 4.9,
                60, 0, 60));

    dirname=basePath + "/Distributions/RoI";
    addDirectory(dirname);
    addHistogram(new TH1F("roi_eta", "RoI #eta; #eta ; Count", 51, -2.55, 2.55));
    addHistogram(new TH1F("roi_phi", "RoI #phi; #phi ; Count", 20, -3.2, 3.2));
    
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

void TrigEgammaPlotTool::bookRnnDistributionHistos(const std::string dir)
{
    cd(dir);
    //addHistogram(new TH2F("RingerShapes","RingerShapes;E_{t};#rings;Count",21000,-1000,20000,100,0,100));
    addHistogram(new TH1F("discriminant","discriminant(integrated);discriminant(ringer);Count",95,-12,7));
    addHistogram(new TH2F("discriminantVsMu","discriminantVsMu(integrated);discriminant(ringer);avgmu;Count",95,-12,7,16,0,70));
    addDirectory(dir+"/discriminant_binned");
    for(unsigned etBinIdx=0; etBinIdx<m_ringerEtbins.size()-1; ++etBinIdx){
      for(unsigned etaBinIdx=0; etaBinIdx<m_ringerEtabins.size()-1; ++etaBinIdx){
        std::stringstream ss1,ss2,ss3,ss4;
        ss1 << "discriminant_et_"<<etBinIdx<<"_eta_"<<etaBinIdx;
        ss2       << m_ringerEtabins[etaBinIdx]<<"<=|#eta|<"<< m_ringerEtabins[etaBinIdx+1] <<
          " and " << m_ringerEtbins[etBinIdx]<< "<=E_{t}<"<< m_ringerEtbins[etBinIdx+1] <<
          ";discriminant(ringer);Count";
        
        addHistogram(new TH1F(ss1.str().c_str(),ss2.str().c_str(),95,-12.,7.));
        ss3 << "discriminantVsMu_et_"<<etBinIdx<<"_eta_"<<etaBinIdx;
        ss4       << m_ringerEtabins[etaBinIdx]<<"<=|#eta|<"<< m_ringerEtabins[etaBinIdx+1] <<
          " and " << m_ringerEtbins[etBinIdx]<< "<=E_{t}<"<< m_ringerEtbins[etBinIdx+1] <<
          ";discriminant(ringer);Avgmu;Count";
        addHistogram(new TH2F(ss3.str().c_str(),ss4.str().c_str(),95,-12.,7.,16,0.,70.));
      }
    }

}

void TrigEgammaPlotTool::bookExpertHistos(TrigInfo trigInfo){
    
    const std::string basePath=m_baseDir+"/Expert/"+trigInfo.trigName;
    std::string dirname;
    std::vector <std::string> dirnames;

    bookL1Histos(trigInfo);
    
    std::vector <std::string> algnames;
    algnames.push_back("Efficiency"); // Default
    if(m_doEmulation)  algnames.push_back("Emulation");

    // Loop over sub directorys
    for ( auto algname : algnames){

      dirname=basePath + "/"+algname+"/HLT";
      addDirectory(dirname);
      bookEfficiencyHistos(dirname);
      bookEfficiency2DHistos(dirname);
      dirnames.push_back(basePath + "/"+algname+"/L2Calo");
      dirnames.push_back(basePath + "/"+algname+"/L2");
      dirnames.push_back(basePath + "/"+algname+"/EFCalo");

      for (const auto dir:dirnames) {
          addDirectory(dir);
          bookEfficiencyHistos(dir);
          if ( m_detailedHists ) 
              bookEfficiency2DHistos(dir);
      }

      dirnames.clear();
      dirname=basePath + "/"+algname+"/HLT";
      cd(dirname);
      if(trigInfo.trigType=="electron"){
          addHistogram(new TProfile("eff_triggerstep","eff_triggerstep",11,0,11));
          addHistogram(new TProfile("eff_hltreco","eff_hltreco",12,0,12));
          setLabels(hist1("eff_triggerstep"),m_label_trigstep);
          setLabels(hist1("eff_hltreco"),m_label_hltobj);

          // Labels
          std::vector<std::string> label_failisem {"ClusterEtaRange","ConversionMatch",
              "ClusterHadronicLeakage","ClusterMiddleEnergy","ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth",
              "f3","ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE","ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c",
              "empty14","ClusterStripsDEmaxs1",
              "TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","TrackTRTeProbabilityHT_Electron",
              "TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight","Isolation","ClusterIsolation","TrackIsolation",
              "No Track","No Cluster","No Object","Total"};

          std::vector<std::string> label_failisemlh {"Kinematic","NSi","NPix","NBlayer","Conversion","LH","A0","dEta","dPhiRes","WstotHighET","EoverPHighEt",
              "No Track","No Cluster","No Object","Total"};

          std::vector<std::string> label_ineffisem {"L2Calo", "L2", "EFCalo",/* "EFTrack",*/ "Shower shape", "TrkClus", "Track", "TRT", "Track & TRT", "TrkClus & Trk & TRT", "IsEM", "Isolation", "isEMLH and Iso", "Track & Cluster", "No cluster", "No track", "No object", "Some object", "Unknown w/o electron", "Unknown w/ electron", "Sum", "Total"};

          std::vector<std::string> label_ineffisemlh {"L2Calo", "L2", "EFCalo",/* "EFTrack",*/ "Kinematic", "NSi", "NPix", "NBlayer", "Conversion", "LH", "A0", "dEta", "dPhiRes", "WstotHighET", "EoverPHighEt", "isEMLH", "Isolation", "isEMLH & Iso", "Track & Cluster", "No cluster", "No track", "No object", "Some object", "Unknown w/o electron", "Unknown w/ electron", "Sum", "Total"};

          std::vector<std::string> label_isem_trk {"TrackBlayer_Electron","TrackPixel_Electron","TrackSi_Electron","TrackA0Tight_Electron","Total"};
          std::vector<std::string> label_isem_trkclus {"#eta match","#phi match","E/p match","TrackMatchEtaTight_Electron","Total"};
          std::vector<std::string> label_isem_trt {"TrackTRTeProbabilityHT_Electron","TrackTRThits_Electron","TrackTRTratio_Electron","TrackTRTratio90_Electron","TrackA0Tight_Electron","Total"};

          std::string pid = trigInfo.trigPidDecorator.substr(2);
          const std::string fail = "FailisEM" + pid;
          const std::string ineffisEM = "IneffisEM" + pid;

          if (boost::contains(pid, "LH")) {
              addHistogram(new TH1F(fail.c_str(), fail.c_str(), 15, 0, 15));
              addHistogram(new TProfile(ineffisEM.c_str(), ineffisEM.c_str(), 26, 0, 26));
              setLabels(hist1(fail), label_failisemlh);
              setLabels(hist1(ineffisEM), label_ineffisemlh);
          }
          else {
              addHistogram(new TH1F(fail.c_str(), fail.c_str(), 36, 0, 36));
              addHistogram(new TProfile(ineffisEM.c_str(), ineffisEM.c_str(), 22, 0, 22));
              setLabels(hist1(fail), label_ineffisem);
              setLabels(hist1(ineffisEM), label_failisem);

              const std::string ineffTrk = ineffisEM + "Trk";
              const std::string ineffTrkClus = ineffisEM + "TrkClus";
              const std::string ineffTRT = ineffisEM + "TRT";
              addHistogram(new TProfile(ineffTrk.c_str(), ineffTrk.c_str(), 6, 0, 6));
              addHistogram(new TProfile(ineffTrkClus.c_str(), ineffTrkClus.c_str(), 5, 0, 5));
              addHistogram(new TProfile(ineffTRT.c_str(), ineffTRT.c_str(), 5, 0, 5));
              setLabels(hist1(ineffTrk), label_isem_trk);
              setLabels(hist1(ineffTrkClus), label_isem_trkclus);
              setLabels(hist1(ineffTRT), label_isem_trt);
          }
      }

      if ( m_detailedHists ) {
          std::vector<std::string> effdirs;
          effdirs.push_back(basePath + "/"+algname+"/HLT/Loose");
          effdirs.push_back(basePath + "/"+algname+"/HLT/Medium");
          effdirs.push_back(basePath + "/"+algname+"/HLT/Tight");
          effdirs.push_back(basePath + "/"+algname+"/HLT/LHLoose");
          effdirs.push_back(basePath + "/"+algname+"/HLT/LHMedium");
          effdirs.push_back(basePath + "/"+algname+"/HLT/LHTight");
          effdirs.push_back(basePath + "/"+algname+"/HLT/LooseIso");
          effdirs.push_back(basePath + "/"+algname+"/HLT/MediumIso");
          effdirs.push_back(basePath + "/"+algname+"/HLT/TightIso");
          effdirs.push_back(basePath + "/"+algname+"/HLT/LHLooseIso");
          effdirs.push_back(basePath + "/"+algname+"/HLT/LHMediumIso");
          effdirs.push_back(basePath + "/"+algname+"/HLT/LHTightIso");
          for (const auto effdir:effdirs) {
              addDirectory(effdir);
              bookEfficiencyHistos(effdir);
              bookEfficiency2DHistos(effdir);
          }
      }// detailerhistos
      
      dirnames.clear();
    }// loop over Efficiency (and or) Emulation


    dirnames.push_back(basePath + "/Distributions/Offline");
    dirnames.push_back(basePath + "/Distributions/HLT");
    for (const auto dir:dirnames){
        ATH_MSG_VERBOSE(dir);
        addDirectory(dir);
        bookEgammaDistributionHistos(dir);
        if(trigInfo.trigType=="electron")
            bookElectronDistributionHistos(dir);
        addHistogram(new TH1F("rejection","N_{TE}; #Step",6,0,6));
        setLabels(hist1("rejection"),m_label_hltte);
    }

    dirname=basePath + "/Distributions/EFCalo";
    dirnames.push_back(dirname);
    addDirectory(dirname);
    addHistogram(new TH1F("energyBE0", "Cluster Energy BE0; E [GeV] ; Count", 50, 0., 100.));
    addHistogram(new TH1F("energyBE1", "Cluster Energy BE1; E [GeV] ; Count", 50, 0., 100.));
    addHistogram(new TH1F("energyBE2", "Cluster Energy BE2; E [GeV] ; Count", 50, 0., 100.));
    addHistogram(new TH1F("energyBE3", "Cluster Energy BE3; E [GeV] ; Count", 50, 0., 100.));
    addHistogram(new TH1F("energy", "Cluster Energy; E [GeV] ; Count", 50, 0., 100.));
    addHistogram(new TH1F("eta_calo", "eta_calo; eta_calo ; Count", 50, -2.47, 2.47));
    addHistogram(new TH1F("phi_calo", "phi_calo; phi_calo ; Count", 50, -3.14, 3.14));

    if(trigInfo.trigType=="photon"){
        dirname=basePath + "/Distributions/L2Photon"; 
        addDirectory(dirname);
        bookDistributionHistos(dirname);
    }

    if(trigInfo.trigType=="electron"){
        dirname=basePath + "/Distributions/L2Electron";
        addDirectory(dirname);
        bookDistributionHistos(dirname);
        addHistogram(new TH1F("trkClusDeta", "Trk Clus Deta; deta ; Count", 50, -0.5, 0.5));
        addHistogram(new TH1F("trkClusDphi", "Trk Clus Dphi; dphi ; Count", 50, -0.5, 0.5)); 
    }

    dirname=basePath + "/Distributions/L2Calo";
    dirnames.push_back(dirname);
    addDirectory(dirname);
    bookRnnDistributionHistos(dirname);

    //Book the kinematic plots for each trigger level
    for(const auto dir:dirnames) 
        bookDistributionHistos(dir);

    // Resolution
    dirname=basePath + "/Resolutions/HLT";
    addDirectory(dirname);
    bookResolutionHistos(dirname);
    if(trigInfo.trigType=="electron"){
        bookElectronResolutionHistos(dirname);
        if(boost::contains(trigInfo.trigName,"iloose") || boost::contains(trigInfo.trigName,"ivarloose"))
            bookElectronIsoResolutionHistos(dirname);
    }
    if(trigInfo.trigType=="photon"){
        bookPhotonResolutionHistos(dirname);
        if(boost::contains(trigInfo.trigName,"icaloloose") ||boost::contains(trigInfo.trigName,"icalovloose") || boost::contains(trigInfo.trigName,"icalotight"))
            bookPhotonIsoResolutionHistos(dirname);
    }
    
    bookExpertResolutionHistos(dirname);

    if(m_detailedHists){
        dirname=basePath + "/AbsResolutions/HLT";
        addDirectory(dirname);
        bookAbsResolutionHistos(dirname);
    }

    if(m_detailedHists){
        dirname=basePath + "/Resolutions/L2Calo";
        addDirectory(dirname);
        bookResolutionHistos(dirname);
        bookExpertL2CaloResolutionHistos(dirname);
    }
    
    dirname=basePath + "/Resolutions/L2Calo_vs_HLT";
    addDirectory(dirname);
    bookResolutionHistos(dirname);
    if(m_detailedHists) bookExpertL2CaloResolutionHistos(dirname);
}


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
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

using namespace std;
//**********************************************************************

TrigEgammaAnalysisBaseTool::
TrigEgammaAnalysisBaseTool( const std::string& myname )
    : AsgTool(myname),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool",this),
    m_plot("TrigEgammaPlotTool/TrigEgammaPlotTool"),
    m_lumiTool("LuminosityTool"),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
{
    declareProperty("MatchTool",m_matchTool);
    declareProperty("Tools", m_tools);
    declareProperty("ElectronKey",m_offElContKey="Electrons");
    declareProperty("PhotonKey",m_offPhContKey="Photons");
    declareProperty("File",m_file="");
    declareProperty("UseLuminosityTool", m_useLumiTool=true);
    declareProperty("LuminosityTool", m_lumiTool, "Luminosity Tool");
    declareProperty("LuminosityToolOnline", m_lumiBlockMuTool, "Luminosity Tool Online");
    declareProperty("DetailedHistograms", m_detailedHists=false)->declareUpdateHandler(&TrigEgammaAnalysisBaseTool::updateDetail,this);
    declareProperty("DefaultProbePid", m_defaultProbePid="Loose");
    declareProperty("doJpsiee",m_doJpsiee=false)->declareUpdateHandler(&TrigEgammaAnalysisBaseTool::updateAltBinning,this);
    declareProperty("isEMResultNames",m_isemname,"isEM");
    declareProperty("LHResultNames",m_lhname,"LH");
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

void TrigEgammaAnalysisBaseTool::updateDetail(Property& /*p*/){
    plot()->setDetail(m_detailedHists);
    // Detail for tools needs to be reset for every call in Analysis
    for( const auto& tool : m_tools) {
        tool->setDetail(m_detailedHists); 
    }
}

void TrigEgammaAnalysisBaseTool::updateAltBinning(Property& /*p*/){
    plot()->setAltBinning(m_doJpsiee);
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
    if ( (m_trigdec.retrieve()).isFailure() ){
        ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
        return StatusCode::FAILURE;
    }
    //Enable expert methods
    m_trigdec->ExperimentalAndExpertMethods()->enable();

    //TrigEgammaPlotTool
    if(m_parent) m_plot->setParent(m_parent);
    /*sc = m_plot->initialize();
    if( sc.isFailure() ) {
        ATH_MSG_ERROR("Unable to locate TrigEgammaPlotTool");
        return sc;
    }*/
    
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

    for( const auto& tool : m_tools) {
        try {
            ATH_MSG_DEBUG("child Initialize " << tool->name());
            sc = tool->childInitialize();
        } catch(const ValidationException &e) {
            ATH_MSG_ERROR("Exception thrown: " << e.msg());
            return StatusCode::FAILURE;
        } catch(...) {
            ATH_MSG_ERROR("Unknown exception caught, while initializing");
            return StatusCode::FAILURE;
        }
    
    }

    return sc;
}

StatusCode TrigEgammaAnalysisBaseTool::book() {
    StatusCode sc = StatusCode::FAILURE;
    for( const auto& tool : m_tools) {
        tool->setPlotTool(m_plot);
    }

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
    for( const auto& tool : m_tools) {
        try {
            ATH_MSG_DEBUG("child Finalize " << tool->name());
            sc = tool->childFinalize();
        } catch(const ValidationException &e) {
            ATH_MSG_ERROR("Exception thrown: " << e.msg());
            return StatusCode::FAILURE;
        } catch(...) {
            ATH_MSG_ERROR("Unknown exception caught, while initializing");
            return StatusCode::FAILURE;
        }
    
    }

    try {
        sc = childFinalize();
    } catch(const ValidationException &e) {
        ATH_MSG_ERROR("Exception thrown: " << e.msg() );
        return StatusCode::FAILURE;
    } catch(...) {
        ATH_MSG_ERROR("Unknown exception caught, while processing histograms");
        return StatusCode::FAILURE;
    }
    m_trigInfo.clear();
    return sc;
}

void TrigEgammaAnalysisBaseTool::print() const {
    AsgTool::print();
    ATH_MSG_INFO("   Message: \"" << m_msg << "\"");
    ATH_MSG_INFO("  MsgLevel: " << MSG::name( msg().currentLevel() ) );
    ATH_MSG_ALWAYS("This message always appears.");
}

void TrigEgammaAnalysisBaseTool::cd(const std::string &dir) {
    plot()->cd(dir);
}

void TrigEgammaAnalysisBaseTool::addDirectory(const std::string &dir) {
    plot()->addDirectory(dir);
}


void TrigEgammaAnalysisBaseTool::addHistogram(TH1 *h, const std::string &dir) {
    plot()->addHistogram(h,dir);
}

void TrigEgammaAnalysisBaseTool::addHistogram(TH2 *h, const std::string &dir) {
    plot()->addHistogram(h,dir);
}

void TrigEgammaAnalysisBaseTool::addTree(TTree *t, const std::string &dir) {
    plot()->addTree(t,dir);
}

void TrigEgammaAnalysisBaseTool::setLabels(TH1* histo, const std::vector<std::string>& labels) {
    plot()->setLabels(histo,labels);
}

TH1 *TrigEgammaAnalysisBaseTool::hist1(const std::string &histName, const std::string &dir) {
    return plot()->hist1(histName,dir);
}

TH2 *TrigEgammaAnalysisBaseTool::hist2(const std::string &histName, const std::string &dir) {
    return plot()->hist2(histName,dir);
}

TTree *TrigEgammaAnalysisBaseTool::tree(const std::string &treeName, const std::string &dir) {
    return plot()->tree(treeName,dir);
}

// adds entry in TrigInfo map to retrieve later via trigger name
void TrigEgammaAnalysisBaseTool::setTrigInfo(const std::string trigger){

    /********************************************
      std::string trigName; //Trigger Name
      std::string trigType; //Electron or Photon
      std::string trigL1Item; //L1 item for HLT
      std::string trigL1Type; //VHI
      std::string trigPidType; //Loose, Medium, Tight, etc...
      bool trigL1; // Level1 Trigger
      bool trigPerf; // Performance chain
      bool trigEtcut; // Et cut only chain
      float trigThrHLT; // HLT Et threshold
      float trigThrL1; // L1 Et threshold
     *******************************************/

    std::string type="";
    bool isL1=false;
    float etthr=0;
    float l1thr=0;
    std::string l1type="";
    std::string pidname="";
    bool perf=false;
    bool etcut=false;
    parseTriggerName(trigger,m_defaultProbePid,isL1,type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger

    std::string l1item = "";
    if(isL1) l1item=trigger;
    else getL1Item(trigger);
    std::string decorator="is"+pidname;

    if(isL1) etthr=l1thr; // Should be handled elsewhere 

    TrigInfo info{trigger,type,l1item,l1type,pidname,decorator,isL1,perf,etcut,etthr,l1thr};
    m_trigInfo[trigger] = info;
}

// return the TrigInfo from trigger name
TrigInfo TrigEgammaAnalysisBaseTool::getTrigInfo(const std::string trigger) { return m_trigInfo[trigger]; }

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
    ATH_MSG_VERBOSE("Booking Path " << basePath);
    if(plot()->book(basePath).isFailure()) ATH_MSG_ERROR("Unable to book histos for " << basePath); 
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
        const xAOD::TrackParticle *trk=eg->trackParticle();
        const xAOD::CaloCluster *clus=eg->caloCluster();
        float eta   = fabs(trk->eta()); 
        return clus->e()/cosh(eta);      
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
    if(eg){ \
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
    if(eg){ \
        eg->trackParticleSummaryValue(val_float,xAOD::_name_); \
        return val_float; } \
    else return -99; }
    GETTER(eProbabilityComb)
    GETTER(eProbabilityHT)
GETTER(pixeldEdx)    

#undef GETTER
    // GETTERs for Calo-Track monitoring
#define GETTER(_name_) float TrigEgammaAnalysisBaseTool::getCaloTrackMatch_##_name_(const xAOD::Electron* eg) \
{ float val={-99.}; \
    if(eg){ \
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
    if(boost::contains(trigger,"HLT")) trigger.erase(0,4);
    auto trig_conf = m_trigdec->ExperimentalAndExpertMethods()->getChainConfigurationDetails("HLT_"+trigger);
    std::string L1_seed = "";
    if(trig_conf != NULL){
        ATH_MSG_DEBUG("TrigConf available");
        L1_seed = trig_conf->lower_chain_name(); //L1 trigger seed             
    }
    return L1_seed; 
}

void TrigEgammaAnalysisBaseTool::parseCaloRingsLayers( unsigned layer, unsigned &minRing, 
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
  if(!el) return false;
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
  
  if(clrings) clrings->exportRingsTo(ringsE);
  else{
    ATH_MSG_WARNING("There is a problem when try to attack the rings vector using exportRigsTo() method.");
    return false;
  }
  */
  return true;
}

const xAOD::TruthParticle* TrigEgammaAnalysisBaseTool::matchTruth(const xAOD::TruthParticleContainer *truthContainer, 
                                                                  const xAOD::Egamma *eg,
                                                                  bool &Zfound,
                                                                  bool &Wfound)
{ 
  // find MC particle
  if(truthContainer){
    TLorentzVector elp; elp.SetPtEtaPhiE(eg->pt(),eg->eta(),eg->phi(),eg->e());
    for(const auto& mc : *truthContainer ){
      Zfound = false;
      Wfound = false;
      if(mc->isElectron()){
        size_t nParents = mc->nParents();
        for(size_t iparent = 0; iparent < nParents; ++iparent){
          if((mc->parent(iparent))->isZ()){
            Zfound = true;     
          }
          if((mc->parent(iparent))->isW()){
            Wfound = true;     
          }
        }
      }
      TLorentzVector mcp;
      mcp.SetPtEtaPhiE(mc->pt(), mc->eta(), mc->phi(), mc->e() );
      if(mcp.DeltaR(elp) < 0.07){
        return mc;
      }
    }
  }
  return nullptr;
}


void TrigEgammaAnalysisBaseTool::calculatePileupPrimaryVertex(){
  // pileup calculation
  m_nGoodVtx = 0; m_nPileupPrimaryVtx = 0;
  if( m_storeGate->contains<xAOD::VertexContainer>("PrimaryVertices")) {
     const xAOD::VertexContainer* vxContainer(0);
     if ( m_storeGate->retrieve(vxContainer, "PrimaryVertices").isFailure() ) {
       ATH_MSG_WARNING ("Could not retrieve xAOD::VertexContainer 'PrimaryVertices'.");
     }else{
       for(unsigned ivx = 0; ivx < vxContainer->size(); ++ivx){
         int nTrackParticles = vxContainer->at(ivx)->nTrackParticles();
         if (nTrackParticles >= 4) m_nGoodVtx++;
         if ( (nTrackParticles >= 4 && vxContainer->at(ivx)->vertexType() == xAOD::VxType::PriVtx) ||
            (nTrackParticles >= 2 && vxContainer->at(ivx)->vertexType() == xAOD::VxType::PileUp) )
           m_nPileupPrimaryVtx++;
       }// loop over vertexs
     } 
   }// protection
   ATH_MSG_DEBUG("calculatePileupPrimaryVertex(): nPileupPrimaryVtx = " << m_nPileupPrimaryVtx);

}



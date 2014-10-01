/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************* 
// Algorithm: NavZeeTPxAODValidation 
// Author: Ryan Mackenzie White <ryan.white@cern.ch>
// Description: 
// Athena-based T&P efficiency algorithm using TDT
// *************************************************************

#include "TrigEgammaValidation/NavZeeTPxAODValidation.h"
#include "TrigEgammaValidation/FEXValidationAlgo.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"

//Trigger Element
#include "TrigNavigation/NavigationCore.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"

#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TH1F.h"
#include "TMath.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "TLorentzVector.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace HLT;
using namespace std;

//Macros for egamma variables
// GETTER for Shower Shape monitoring
#define GETTER(_name_) float getShowerShape_##_name_(const xAOD::Egamma* eg) \
{ float val{-99}; \
    eg->showerShapeValue(val,xAOD::EgammaParameters::_name_); \
    return val; }
GETTER(e237)
GETTER(e277)
GETTER(ethad1)   
GETTER(weta1)
GETTER(weta2)
GETTER(f1)
GETTER(e2tsts1)
GETTER(emins1)
GETTER(wtots1)
GETTER(fracs1)   
#undef GETTER

// GETTERs for CaloCluster monitoring   
#define GETTER(_name_) float getCluster_##_name_(const xAOD::Egamma* eg) \
{    if(eg && eg->caloCluster()) \
    return eg->caloCluster()->_name_(); \
    else return -99.;}
GETTER(et)
GETTER(phi)
GETTER(eta)   
#undef GETTER

// GETTERs for Track monitoring   
#define GETTER(_name_) float getTrack_##_name_(const xAOD::Electron* eg) \
{    if(eg && eg->trackParticle()) \
    return eg->trackParticle()->_name_(); \
    else return -99.;}
GETTER(pt)
GETTER(phi)
GETTER(eta)   
#undef GETTER
    
// GETTERs for Track details monitoring   
#define GETTER(_name_) float getTrackSummary_##_name_(const xAOD::Electron* eg) \
{ uint8_t val_uint8{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_uint8,xAOD::_name_); \
        return val_uint8; } \
    else return -99; }
GETTER(numberOfBLayerHits)
GETTER(numberOfPixelHits)
GETTER(numberOfSCTHits)   
GETTER(numberOfTRTHits)
GETTER(numberOfTRTHighThresholdHits)
GETTER(numberOfTRTHighThresholdOutliers)
GETTER(numberOfTRTOutliers)
#undef GETTER

// GETTERs for Calo-Track monitoring
#define GETTER(_name_) float getCaloTrackMatch_##_name_(const xAOD::Electron* eg) \
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

NavZeeTPxAODValidation::NavZeeTPxAODValidation(const std::string & name, ISvcLocator * pSvcLocator) :
    NavAuxValidation(name, pSvcLocator) , m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    fHLTFrame(0)
{
	declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
	declareProperty("Chains", vec_chain);
        declareProperty("CheckPhotons", m_checkPhotons = true);
	declareProperty("CheckElectrons", m_checkElectrons = true);

        declareProperty("ElectronTightPPSelector", m_electronTightPPCutIDTool);
        declareProperty("ElectronMediumPPSelector", m_electronMediumPPCutIDTool);
        declareProperty("ElectronLoosePPSelector", m_electronLoosePPCutIDTool);

        //declareProperty("ElectronTight1Selector", m_electronTight1CutIDTool);
        //declareProperty("ElectronMedium1Selector", m_electronMedium1CutIDTool);
        //declareProperty("ElectronLoose1Selector", m_electronLoose1CutIDTool);
        
        //declareProperty("ElectronTightSelector", m_electronTightCutIDTool);
        //declareProperty("ElectronMediumSelector", m_electronMediumCutIDTool);
        //declareProperty("ElectronLooseSelector", m_electronLooseCutIDTool);
        
        // isEM masks 
        declareProperty("IsEMloose", m_isEMloose = 0xF2);
	declareProperty("IsEMloose1", m_isEMloose1 = 0xF2);
	declareProperty("IsEMloosePP",m_isEMloosePP = 0xF2);
        
        declareProperty("IsEMmedium", m_isEMmedium = 0xF2);
	declareProperty("IsEMmedium1", m_isEMmedium1 = 0xF2);
	declareProperty("IsEMmediumPP",m_isEMmediumPP = 0xF2);
        
        declareProperty("IsEMtight", m_isEMtight = 0xF2);
	declareProperty("IsEMtight1", m_isEMtight1 = 0xF2);
	declareProperty("IsEMtightPP",m_isEMtightPP = 0xF2);
}

NavZeeTPxAODValidation::~NavZeeTPxAODValidation()
{
}

StatusCode NavZeeTPxAODValidation::childInitialize() {
   StatusCode sc;
   sc = m_trigdec.retrieve();
   if ( sc.isFailure() ) {
      (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
      return StatusCode::FAILURE;
   }
   //Execute for all chains
   m_trigdec->ExperimentalAndExpertMethods()->enable();
   m_nav = m_trigdec->ExperimentalAndExpertMethods()->getNavigation();
   if (!m_nav)
   { 
       (*m_log) << MSG::ERROR << "Navigation is pointer to NULL!" << endreq;;
       return StatusCode::FAILURE;
   }

   sc = service("THistSvc",m_thistSvc);
   //check if the status is successful
   if (sc.isFailure()) {
       (*m_log) << MSG::DEBUG << "No THistSvc!!!!!!!" << endreq;
       return StatusCode::FAILURE;
   }
   
   sc = m_electronTightPPCutIDTool.retrieve();
   if(sc.isFailure()) {
       (*m_log) << MSG::ERROR << "Unable to retrieve " << m_electronTightPPCutIDTool << endreq;
       return StatusCode::FAILURE;
   }
   else {
       (*m_log) << MSG::DEBUG << "Tool " << m_electronTightPPCutIDTool << " retrieved" << endreq;
   }
  
   sc = m_electronMediumPPCutIDTool.retrieve();
   if(sc.isFailure()) {
       (*m_log) << MSG::ERROR << "Unable to retrieve " << m_electronMediumPPCutIDTool << "tool" << endreq;
       return StatusCode::FAILURE;
   }
   else {
       (*m_log) << MSG::DEBUG << "Tool " << m_electronMediumPPCutIDTool << " retrieved" << endreq;
   }
   
   sc = m_electronLoosePPCutIDTool.retrieve();
   if(sc.isFailure()) {
       (*m_log) << MSG::ERROR << "Unable to retrieve " << m_electronLoosePPCutIDTool << "tool" << endreq;
       return StatusCode::FAILURE;
   }
   else {
       (*m_log) << MSG::DEBUG << "Tool " << m_electronLoosePPCutIDTool << " retrieved" << endreq;
   }
   
   (*m_log) << MSG::DEBUG << "Book the histograms" << endreq; 
   const char * pidnames[32] ={"ClusterEtaRange","ConversionMatch","ClusterHadronicLeakage","ClusterMiddleEnergy",
       "ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth","f3",
       "ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE",
       "ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c","empty14",
       "ClusterStripsDEmaxs1","TrackBlayer","TrackPixel","TrackSi",
       "TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","empty23",
       "TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight",
       "Isolation","ClusterIsolation","TrackIsolation"};

       
   std::vector <std::string> dirnames;
   for (int i = 0; i < (int) vec_chain.size(); i++) {
       dirnames.push_back(name()+"/" + vec_chain[i] + "/L2CaloEfficiencies");
       dirnames.push_back(name()+"/" + vec_chain[i] + "/L2Efficiencies");
       dirnames.push_back(name()+"/" + vec_chain[i] + "/EFEfficiencies");
   }
   
   for (int i = 0; i < (int) dirnames.size(); i++) {
       addDirectory(dirnames[i]);
       addHistogram(new TH1F("ZeeProbePt", "Probe P_{T} for Z->e+e; p_{T} [GeV] ; Count", 50, 0., 100.));
       addHistogram(new TH1F("ZeeMatchProbePt", "Match Probe P_{T} for Z->e+e; p_{T} [GeV] ; Count", 50, 0., 100.));
       addHistogram(new TH1F("ZeeProbeEffPt", "Probe Efficiency P_{T} for Z->e+e; p_{T} [GeV] ; #epsilon", 50, 0., 100.));
       addHistogram(new TH1F("ZeeProbeEta", "Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
       addHistogram(new TH1F("ZeeMatchProbeEta", "Match Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
       addHistogram(new TH1F("ZeeProbeEffEta", "Probe #eta for Z->e+e; #eta ; #epsilon", 50, -2.5, 2.5));
   }

   for (int i = 0; i < (int) vec_chain.size(); i++) {
       (*m_log) << MSG::DEBUG << "Chain " << vec_chain[i] << endreq;;
       
       addDirectory(name()+"/" + vec_chain[i] + "/Efficiencies");
       addHistogram(new TH1F("ZeeTP", "Mass for Z->e+e; M [GeV] ; Count", 80, 80, 100));
       addHistogram(new TH1F("ZeeProbePt", "Probe P_{T} for Z->e+e; p_{T} [GeV] ; Count", 50, 0., 100.));
       addHistogram(new TH1F("ZeeMatchProbePt", "Match Probe P_{T} for Z->e+e; p_{T} [GeV] ; Count", 50, 0., 100.));
       addHistogram(new TH1F("ZeeProbeEffPt", "Probe Efficiency P_{T} for Z->e+e; p_{T} [GeV] ; #epsilon", 50, 0., 100.));
       addHistogram(new TH1F("ZeeProbeEta", "Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
       addHistogram(new TH1F("ZeeMatchProbeEta", "Match Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
       addHistogram(new TH1F("ZeeProbeEffEta", "Probe #eta for Z->e+e; #eta ; #epsilon", 50, -2.5, 2.5));
       addHistogram(new TH1F("ZeeTagPt", "Probe P_{T} for Z->e+e; P_{T} [GeV] ; Count", 50, 0., 100.));
       addHistogram(new TH1F("ZeeMatchTagPt", "Match Probe P_{T} for Z->e+e; P_{T} [GeV] ; Count", 50, 0., 100.));
       
       addDirectory(name()+"/" + vec_chain[i] + "/Inefficiencies");
       addHistogram(new TH1F("ZeeProbeLoose", "Probe IsEM Loose Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeClosestLoose", "Closest Object IsEM Loose Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeLooseRatio", "Ratio IsEM Loose Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeProbeMedium", "Probe IsEM Medium Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeClosestMedium", "Closest Object IsEM Medium Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeMediumRatio", "Ratio IsEM Medium Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeProbeTight", "Probe IsEM Tight Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeClosestTight", "Closest Object IsEM Tight Z->e+e; IsEM ; Counts", 32, 0., 32.));
       addHistogram(new TH1F("ZeeTightRatio", "Ratio IsEM Tight Z->e+e; IsEM ; Counts", 32, 0., 32.));
       
       for(int j=0;j<32;j++){
           hist1("ZeeProbeLoose")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeClosestLoose")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeLooseRatio")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeProbeMedium")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeClosestMedium")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeMediumRatio")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeProbeTight")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeClosestTight")->GetXaxis()->SetBinLabel(j+1,pidnames[j]);
           hist1("ZeeTightRatio")->GetXaxis()->SetBinLabel(i+1,pidnames[j]);
       }
       
           
       addDirectory(name()+"/" + vec_chain[i] + "/AbsoluteResolutions");
       addHistogram(new TH1F("eta", "eta;  ; Count", 80, -0.2, 0.2));
       addHistogram(new TH1F("phi", "phi;  ; Count", 80, -0.2, 0.2));
       addHistogram(new TH1F("etaCalo", "etaCalo;  ; Count", 80, -0.2, 0.2));
       addHistogram(new TH1F("phiCalo", "phiCalo;  ; Count", 80, -0.2, 0.2));
       addHistogram(new TH1F("Energy", "energy [MeV];  ; Count", 100, -10, 10));
       addHistogram(new TH1F("pt", "p_T [MeV];  ; Count", 100, -10, 10));
       addHistogram(new TH1F("pt_track", "p_T(track) [MeV];  ; Count", 100, -10, 10));
       addHistogram(new TH1F("EBE0", "energy BE0 [MeV];  ; Count", 100, -10, 10));
       addHistogram(new TH1F("EBE1", "energy BE1 [MeV];  ; Count", 100, -10, 10));
       addHistogram(new TH1F("EBE2", "energy BE2 [MeV];  ; Count", 100, -10, 10));
       addHistogram(new TH1F("EBE3", "energy BE3 [MeV];  ; Count", 100, -10, 10));
   }
   (*m_log) << MSG::DEBUG << "End of NavZeeValidation::initialize()"  << endreq;

   //m_NProbesIsEM.assign(32,0);
   //m_NFailedIsEM.assign(32,0);
   return StatusCode::SUCCESS;
}

StatusCode NavZeeTPxAODValidation::childFinalize()
{
    (*m_log) << MSG::DEBUG << "End of NavZeeValidation::finalize()" << endreq;
    (*m_log) << MSG::DEBUG << "Finalize Efficiencies()" << endreq;
    for (int i = 0; i < (int)vec_chain.size(); i++)
    {
        (*m_log) << MSG::DEBUG << " Executing chain " << m_chain << endreq;
        m_chain = vec_chain[i];
        cd(name()+"/" + m_chain + "/Efficiencies");
        hist1("ZeeMatchProbePt")->Sumw2();
        hist1("ZeeProbePt")->Sumw2();
        hist1("ZeeProbeEffPt")->Divide(hist1("ZeeMatchProbePt"),hist1("ZeeProbePt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        cd(name()+"/" + m_chain + "/L2CaloEfficiencies");
        hist1("ZeeMatchProbePt")->Sumw2();
        hist1("ZeeProbePt")->Sumw2();
        hist1("ZeeProbeEffPt")->Divide(hist1("ZeeMatchProbePt"),hist1("ZeeProbePt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        cd(name()+"/" + m_chain + "/L2Efficiencies");
        hist1("ZeeMatchProbePt")->Sumw2();
        hist1("ZeeProbePt")->Sumw2();
        hist1("ZeeProbeEffPt")->Divide(hist1("ZeeMatchProbePt"),hist1("ZeeProbePt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        cd(name()+"/" + m_chain + "/EFEfficiencies");
        hist1("ZeeMatchProbePt")->Sumw2();
        hist1("ZeeProbePt")->Sumw2();
        hist1("ZeeProbeEffPt")->Divide(hist1("ZeeMatchProbePt"),hist1("ZeeProbePt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        
        cd(name()+"/" + m_chain + "/Inefficiencies");
        hist1("ZeeLooseRatio")->Divide(hist1("ZeeClosestLoose"),hist1("ZeeProbeLoose"));
        hist1("ZeeMediumRatio")->Divide(hist1("ZeeClosestMedium"),hist1("ZeeProbeMedium"));
        hist1("ZeeTightRatio")->Divide(hist1("ZeeClosestTight"),hist1("ZeeProbeTight"));
    } 
    return StatusCode::SUCCESS;
}

StatusCode NavZeeTPxAODValidation::childExecute()
{

    if (vec_chain.size() == 0)
    {
        (*m_log) << MSG::ERROR << "Chain vector is empty, there is nothing to be done" << endreq;
        return StatusCode::FAILURE;
    }

    StatusCode sc;
    const xAOD::ElectronContainer* electrons = 0;
    sc=m_storeGate->retrieve(electrons,"ElectronCollection");
    if (sc.isFailure()) {
        //ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
 
    
    m_Probes.clear();
    m_ProbesLV.clear();
    //for(unsigned int i=0;i<32;i++) m_NProbesIsEM[i]=0;
    //for(unsigned int i=0;i<32;i++) m_NFailedIsEM[i]=0;
    for(const auto& elTag : *electrons){
        if(isTagElectron(elTag).isFailure()) continue; 
        //All offline electron and online electron efficiency
        for (int i = 0; i < (int)vec_chain.size(); i++)
        {
            (*m_log) << MSG::DEBUG << " Executing chain " << m_chain << endreq;
            m_chain = vec_chain[i];
            cd(name()+"/" + m_chain + "/Efficiencies");
            
            hist1("ZeeTagPt")->Fill(elTag->pt()/1.e3);
            if(!isMatchEF(elTag,m_chain).isFailure()) hist1("ZeeMatchTagPt")->Fill(elTag->pt()/1.e3);
        }
        for(const auto& elProbe : *electrons){
            if(elProbe==elTag) continue;
            //ATH_MSG_DEBUG("Found different T&P pair");
            if(elProbe->charge() == elTag->charge()) continue;
            //ATH_MSG_DEBUG("Found opposite charge");

            //Must be an easy way with IParticle
            TLorentzVector el1;
            TLorentzVector el2;
            el1.SetPtEtaPhiE(elTag->pt(), elTag->trackParticle()->eta(), elTag->trackParticle()->phi(), elTag->e());
            el2.SetPtEtaPhiE(elProbe->pt(), elProbe->trackParticle()->eta(), elProbe->trackParticle()->phi(), elProbe->e());
            float tpPairMass = (el1 + el2).M();
            if( !((tpPairMass > 80.0*1.e3) && (tpPairMass < 100.0*1.e3))){ 
                (*m_log) << MSG::DEBUG << "tag and probe pair not in Z mass window" << endreq;
                continue; 
            }
            //ATH_MSG_DEBUG("Found Z candidate");
            if(isProbeElectron(elProbe).isFailure()) continue;
            TLorentzVector elProbeLV;
            elProbeLV.SetPtEtaPhiE(elProbe->pt(), elProbe->trackParticle()->eta(), elProbe->trackParticle()->phi(), elProbe->e());
            m_Probes.push_back(elProbe);
            m_ProbesLV.push_back(elProbeLV);
            (*m_log) << MSG::DEBUG << "Found T&P pair mass " << tpPairMass << endreq;
            for (int i = 0; i < (int)vec_chain.size(); i++)
            {
                (*m_log) << MSG::DEBUG << " Executing chain " << m_chain << endreq;
                m_chain = vec_chain[i];
                
                // Get the features
                const Trig::ChainGroup* HLT_Chains  = m_trigdec->getChainGroup("HLT_"+m_chain);
                if(!HLT_Chains) continue;
                m_HLTFeatures = m_trigdec->features(HLT_Chains, TrigDefs::alsoDeactivateTEs);
                
                cd(name()+"/" + m_chain + "/L2CaloEfficiencies");
                hist1("ZeeProbePt")->Fill(elProbe->pt()/1.e3);
                hist1("ZeeProbeEta")->Fill(elProbe->eta());
                
                cd(name()+"/" + m_chain + "/L2Efficiencies");
                hist1("ZeeProbePt")->Fill(elProbe->pt()/1.e3);
                hist1("ZeeProbeEta")->Fill(elProbe->eta());
                
                cd(name()+"/" + m_chain + "/EFEfficiencies");
                hist1("ZeeProbePt")->Fill(elProbe->pt()/1.e3);
                hist1("ZeeProbeEta")->Fill(elProbe->eta());
                
                cd(name()+"/" + m_chain + "/Efficiencies");
                hist1("ZeeTP")->Fill(tpPairMass/1.e3);
                hist1("ZeeProbePt")->Fill(elProbe->pt()/1.e3);
                hist1("ZeeProbeEta")->Fill(elProbe->eta());
                
                //Find nearest probe -- useful for matches and failures
                const xAOD::Electron *nearestEl = ClosestEFObject(elProbe);
                if (isMatchEF(elProbe,m_chain).isFailure()){
                    (*m_log) << MSG::DEBUG << "No matched trigger object" << endreq;
                    // Look for isEM of Closest object with pt > 25 GeV
                    if(nearestEl){
                        (*m_log) << MSG::DEBUG << "Nearest object E " << nearestEl->e() << endreq;
                        (*m_log) << MSG::DEBUG << "Nearest object pt " << nearestEl->pt() << endreq;
                        (*m_log) << MSG::DEBUG << "Nearest object eta " << nearestEl->eta() << endreq;
                        (*m_log) << MSG::DEBUG << "Nearest object cosh(eta)" << cosh(nearestEl->eta()) << endreq;
                        if(fillIsEM(elProbe,nearestEl).isFailure()) (*m_log) << MSG::DEBUG << "Problems in fillIsEM" << endreq;    
                    }
                }
                else{
                    //ATH_MSG_DEBUG("Found trigger match");
                    cd(name()+"/" + m_chain + "/Efficiencies");
                    hist1("ZeeMatchProbePt")->Fill(elProbe->pt()/1.e3);
                    hist1("ZeeMatchProbeEta")->Fill(elProbe->eta());
                    //Fill the resolution plots
                    if(nearestEl)
                        if(AbsoluteResolutions(elProbe,nearestEl).isFailure()) (*m_log) << MSG::DEBUG << "Problems in Resolutions" << endreq;    
                }
                if(!combinedL2CaloExecute(elProbe).isFailure()){
                    cd(name()+"/" + m_chain + "/L2CaloEfficiencies");
                    hist1("ZeeMatchProbePt")->Fill(elProbe->pt()/1.e3);
                    hist1("ZeeMatchProbeEta")->Fill(elProbe->eta());
                }
                if(!combinedL2Execute(elProbe).isFailure()){
                    cd(name()+"/" + m_chain + "/L2Efficiencies");
                    hist1("ZeeMatchProbePt")->Fill(elProbe->pt()/1.e3);
                    hist1("ZeeMatchProbeEta")->Fill(elProbe->eta());
                }
                if(!combinedEFExecute(elProbe).isFailure()){
                    cd(name()+"/" + m_chain + "/EFEfficiencies");
                    hist1("ZeeMatchProbePt")->Fill(elProbe->pt()/1.e3);
                    hist1("ZeeMatchProbeEta")->Fill(elProbe->eta());
                }
            }
        }
    }

    (*m_log) << MSG::DEBUG << "End of NavZeeValidation::Execute()" << endreq;
    return StatusCode::SUCCESS;
}

StatusCode NavZeeTPxAODValidation::chainExecute()
{
    return StatusCode::SUCCESS;
}

StatusCode NavZeeTPxAODValidation::fillIsEM(const xAOD::Electron *egoff, const xAOD::Electron *eghlt){
    (*m_log) << MSG::DEBUG << "In fillIsEM" << endreq;    
    cd(name()+"/" + m_chain + "/Inefficiencies");
    unsigned int isEMoff;
    unsigned int isEMhlt;
    if (m_chain.find("loose") != string::npos){
        (*m_log) << MSG::DEBUG << "In fillIsEM Loose" << endreq;    
        if(m_electronLoosePPCutIDTool->execute(egoff, 20000, false).isFailure()) (*m_log) << MSG::DEBUG << "REGTEST::fillIsEM Probe Must Pass " << endreq;
        isEMoff = m_electronLoosePPCutIDTool->IsemValue();
        (*m_log) << MSG::DEBUG << "REGTEST::Probe IsEM " << std::hex << isEMoff << endreq;

        if(m_electronLoosePPCutIDTool->execute(eghlt, 20000, false).isFailure()) (*m_log) << MSG::DEBUG << "REGTEST::fillIsEM Loose HLT Object Fails " << endreq;
        isEMhlt = m_electronLoosePPCutIDTool->IsemValue();
        (*m_log) << MSG::DEBUG << "REGTEST::Failed IsEM " << std::hex << isEMhlt << endreq;
        
        for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
            hist1("ZeeProbeLoose")->AddBinContent(i+1,(isEMoff & (0x1<<i)) && 1); 
            hist1("ZeeClosestLoose")->AddBinContent(i+1,(isEMhlt & (0x1<<i)) && 1); 
        }
    }
    else if (m_chain.find("medium") != string::npos){
        (*m_log) << MSG::DEBUG << "In fillIsEM Medium" << endreq;    
        if(m_electronMediumPPCutIDTool->execute(egoff, 20000, false).isFailure()) (*m_log) << MSG::DEBUG << "REGTEST::fillIsEM Probe Must Pass " << endreq;
        isEMoff = m_electronMediumPPCutIDTool->IsemValue();
        (*m_log) << MSG::DEBUG << "REGTEST::Probe IsEM " << std::hex << isEMoff << endreq;
        if(m_electronMediumPPCutIDTool->execute(eghlt, 20000, false).isFailure()) (*m_log) << MSG::DEBUG << "REGTEST::fillIsEMM Medium HLT Object Fails " << endreq;
        isEMhlt = m_electronMediumPPCutIDTool->IsemValue();
        (*m_log) << MSG::DEBUG << "REGTEST::Failed IsEM " << std::hex << isEMhlt << endreq;
        for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
            hist1("ZeeProbeMedium")->AddBinContent(i+1,(isEMoff & (0x1<<i)) && 1); 
            hist1("ZeeClosestMedium")->AddBinContent(i+1,(isEMhlt & (0x1<<i)) && 1); 
        }
    }
    else if (m_chain.find("tight") != string::npos){
        (*m_log) << MSG::DEBUG << "In fillIsEM Tight" << endreq;    
        if(m_electronTightPPCutIDTool->execute(egoff, 20000, false).isFailure()) (*m_log) << MSG::DEBUG << "REGTEST::fillIsEM Probe Must Pass " << endreq;
        isEMoff = m_electronTightPPCutIDTool->IsemValue();
        (*m_log) << MSG::DEBUG << "REGTEST::Probe IsEM " << std::hex << isEMoff << endreq;
        if(m_electronTightPPCutIDTool->execute(eghlt, 20000, false).isFailure()) (*m_log) << MSG::DEBUG << "REGTEST::fillIsEM Tight HLT Object Fails " << endreq;
        isEMhlt = m_electronTightPPCutIDTool->IsemValue();
        (*m_log) << MSG::DEBUG << "REGTEST::Failed IsEM " << std::hex << isEMhlt << endreq;
        for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
            hist1("ZeeProbeTight")->AddBinContent(i+1,(isEMoff & (0x1<<i)) && 1); 
            hist1("ZeeClosestTight")->AddBinContent(i+1,(isEMhlt & (0x1<<i)) && 1); 
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode NavZeeTPxAODValidation::combinedL2CaloExecute(const xAOD::Electron *eloff){
    Trig::FeatureContainer::combination_const_iterator L2_cIt = m_HLTFeatures.getCombinations().begin();
    
    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(eloff->pt(), eloff->trackParticle()->eta(), eloff->trackParticle()->phi(), eloff->e());
    bool isMatch = false;
    for ( ; L2_cIt != m_HLTFeatures.getCombinations().end(); ++L2_cIt){
        //Retrieve feature of ElectronContainers from combination
        vector< Trig::Feature<xAOD::TrigEMCluster> > L2eg = (*L2_cIt).get<xAOD::TrigEMCluster>("",TrigDefs::alsoDeactivateTEs);
        if (!L2eg.size()) continue; 
        //Loop over features
        TLorentzVector elHLT;
        for(auto elfeat : L2eg){
            if(!elfeat.te()->getActiveState()) continue;
            //(*m_log) << MSG::DEBUG << "REGTEST:: LABEL " << elfeat.label() << endreq;
            //Retrieve ElectronContainer for each feature
            const xAOD::TrigEMCluster *clus = elfeat.cptr();
            elHLT.SetPtEtaPhiE(clus->energy()/clus->eta(), clus->eta(), clus->phi(), clus->energy());
            if(elHLT.DeltaR(eloffLV) < 0.1) isMatch = true;
        }
    }
    
    if(!isMatch) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

StatusCode NavZeeTPxAODValidation::combinedL2Execute(const xAOD::Electron *eloff){
    Trig::FeatureContainer::combination_const_iterator L2_cIt = m_HLTFeatures.getCombinations().begin();
    
    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(eloff->pt(), eloff->trackParticle()->eta(), eloff->trackParticle()->phi(), eloff->e());
    bool isMatch = false;
    for ( ; L2_cIt != m_HLTFeatures.getCombinations().end(); ++L2_cIt){
        //Retrieve feature of ElectronContainers from combination
        vector< Trig::Feature<xAOD::TrigElectronContainer> > L2eg = (*L2_cIt).get<xAOD::TrigElectronContainer>("",TrigDefs::alsoDeactivateTEs);
        
        if (!L2eg.size()) continue; 
        //Loop over features
        TLorentzVector elHLT;
        for(auto elfeat : L2eg){
            if(!elfeat.te()->getActiveState()) continue;
            //(*m_log) << MSG::DEBUG << "REGTEST:: LABEL " << elfeat.label() << endreq;
            //Retrieve ElectronContainer for each feature
            const xAOD::TrigElectronContainer *elCont = elfeat.cptr();
            //loop over EF egammaContainers
            for(const auto& el : *elCont){ 
                elHLT.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());
                if(elHLT.DeltaR(eloffLV) < 0.1) isMatch = true;
            }
        }
    }
    
    if(!isMatch) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;

}

// Here we obtain a feature container of the HLT item
StatusCode NavZeeTPxAODValidation::isMatchEF(const xAOD::Electron *eloff,std::string trigItem) { 
    if(!m_trigdec->isPassed("HLT_"+trigItem) ) {
        (*m_log) << MSG::DEBUG << "Failed trigger " << trigItem << endreq;
        return StatusCode::FAILURE;
    }

    (*m_log) << MSG::DEBUG << "Finding trigger probes " << trigItem << endreq;
    Trig::FeatureContainer fc = m_trigdec->features("HLT_"+trigItem);
    const std::vector< Trig::Feature<xAOD::ElectronContainer> > vec_el = fc.get<xAOD::ElectronContainer>();
    //ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::ElectronContainer> > = " << vec_el.size());

    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(eloff->pt(), eloff->trackParticle()->eta(), eloff->trackParticle()->phi(), eloff->e());

    bool matched=false;
    for(auto elfeat : vec_el){
        //ATH_MSG_INFO("REGTEST: Got electron container, size = " << elfeat.cptr()->size());
        const xAOD::ElectronContainer *elCont = elfeat.cptr();
        for(const auto& eg : *elCont){
            TLorentzVector elLV;
            elLV.SetPtEtaPhiE(eg->pt(), eg->trackParticle()->eta(), eg->trackParticle()->phi(), eg->e());
            if(elLV.DeltaR(eloffLV) < 0.1) matched = true;
        }
    }
    if(!matched) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

// Here we obtain the combined features of a chain
// Then obtain the ElectronContainer features
StatusCode NavZeeTPxAODValidation::combinedEFExecute(const xAOD::Electron *eloff){

    //const Trig::ChainGroup* EF_Chains  = m_trigdec->getChainGroup("HLT_"+m_chain);
    //if(!EF_Chains) return StatusCode::FAILURE;
    //Trig::FeatureContainer EF_features = m_trigdec->features(EF_Chains, TrigDefs::Physics);
    Trig::FeatureContainer::combination_const_iterator EF_cIt = m_HLTFeatures.getCombinations().begin();
    
    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(eloff->pt(), eloff->trackParticle()->eta(), eloff->trackParticle()->phi(), eloff->e());
    bool isMatch = false;
    for ( ; EF_cIt != m_HLTFeatures.getCombinations().end(); ++EF_cIt){
        //Retrieve feature of ElectronContainers from combination
        vector< Trig::Feature<xAOD::ElectronContainer> > EFeg = (*EF_cIt).get<xAOD::ElectronContainer>("",TrigDefs::Physics);
        if (!EFeg.size()) continue; 
        //Loop over features
        TLorentzVector elHLT;
        for(auto elfeat : EFeg){
            // Ensure the feature is Active
            if(!elfeat.te()->getActiveState()) continue;
            //Retrieve ElectronContainer for each feature
            const xAOD::ElectronContainer *elCont = elfeat.cptr();
            //loop over EF ElectronContainer
            for(const auto& el : *elCont){ 
                elHLT.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());
                if(elHLT.DeltaR(eloffLV) < 0.1) isMatch = true;
            }
        }
    }
    
    if(!isMatch) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}

StatusCode NavZeeTPxAODValidation::isTagElectron(const xAOD::Electron *el){

    // Tag the event
    // Require offline tight electron
    // Match to e24_tight1_L1EM20V
    //ATH_MSG_DEBUG("Selecting Tag Electron");
     double GeV = 1000.;

     //Check constituents
     if(!el->trackParticle()){
         //ATH_MSG_DEBUG("No track Particle");
         return StatusCode::FAILURE;
     }
     if(!el->caloCluster()){
         //ATH_MSG_DEBUG("No caloCluster");
         return StatusCode::FAILURE;
     }

     //Author no longer required (no soft-e)

    //el_OQ&1446 == 0
     /*if(!el->isGoodOQ(1446)){
         ATH_MSG_DEBUG("Tag Electron Fails GoodOQ");
         return StatusCode::FAILURE;
     }*/

     //Require offline Tight++
     const Root::TAccept& acc = m_electronTightPPCutIDTool->accept(el);
     if(! (bool) (acc) ) {
         //ATH_MSG_DEBUG("Tag electron fails isEMTight");
         return StatusCode::FAILURE;
     }
     
     //Require Et > 25 GeV
     if( !(el->e()/cosh(el->trackParticle()->eta())  > 25.0*GeV) ){ 
       //ATH_MSG_DEBUG( "failed Tag ET "<< el->e()/cosh(el->trackParticle()->eta()) << " , E = " << el->e());
       return StatusCode::FAILURE;
     }
     
     //fiducial detector acceptance region    
     if ( (fabs(el->eta())>1.37 && fabs(el->eta())<1.52) || fabs(el->eta())>2.47 ){
      //ATH_MSG_DEBUG("Failed tag eta " << fabs(el->eta())); 
       return StatusCode::FAILURE;
     }
     if(isMatchEF(el,"e24_tight1_L1EM20V").isFailure()) { 
         //ATH_MSG_DEBUG("No matched trigger object");
         return StatusCode::FAILURE;
     }
     
     //ATH_MSG_DEBUG("Found a tag electron");
    return StatusCode::SUCCESS; 
}

StatusCode NavZeeTPxAODValidation::isProbeElectron(const xAOD::Electron *el){

    //ATH_MSG_DEBUG("Selecting Probe Electron");
     double GeV = 1000.;

     //Check constituents
     if(!el->trackParticle()){
         //ATH_MSG_DEBUG("No track Particle");
         return StatusCode::FAILURE;
     }
     if(!el->caloCluster()){
         //ATH_MSG_DEBUG("No caloCluster");
         return StatusCode::FAILURE;
     }
     // get jet container of interest
     const xAOD::JetContainer* jets = 0;
     StatusCode sc=m_storeGate->retrieve(jets,"AntiKt4EMTopoJets" );
     if (sc.isFailure()) {
         //ATH_MSG_ERROR("Failed to retrieve AntiKt4EMTopoJets with key ");
         return StatusCode::FAILURE;
     };

     //Author no longer required (no soft-e)

    //el_OQ&1446 == 0
     /*if(!el->isGoodOQ(1446)){
         ATH_MSG_DEBUG("Tag Electron Fails GoodOQ");
         return StatusCode::FAILURE;
     }*/

     //Require offline PID type corresponding to trigger
     if (m_chain.find("loose") != string::npos){
         (*m_log) << MSG::DEBUG << "USing loose offline probe selection" << endreq;
         const Root::TAccept& acc = m_electronLoosePPCutIDTool->accept(el);
         if(! (bool) (acc) ) {
             //ATH_MSG_DEBUG("Probe electron fails isEMLoose");
             return StatusCode::FAILURE;
         }
     }
     else if (m_chain.find("medium") != string::npos){
         (*m_log) << MSG::DEBUG << "USing loose offline probe selection" << endreq;
         const Root::TAccept& acc = m_electronMediumPPCutIDTool->accept(el);
         if(! (bool) (acc) ) {
             //ATH_MSG_DEBUG("Probe electron fails isEMMedium");
             return StatusCode::FAILURE;
         }
     }
     else if (m_chain.find("tight") != string::npos){
         (*m_log) << MSG::DEBUG << "USing tight offline probe selection" << endreq;
         const Root::TAccept& acc = m_electronTightPPCutIDTool->accept(el);
         if(! (bool) (acc) ) {
             //ATH_MSG_DEBUG("Probe electron fails isEMMedium");
             return StatusCode::FAILURE;
         }
     }
     
     //Require Et > 25 GeV
     if( !(el->e()/cosh(el->trackParticle()->eta())  > 25.0*GeV) ){ 
       //ATH_MSG_DEBUG( "Failed Probe ET "<< el->e()/cosh(el->trackParticle()->eta()) << " , E = " << el->e());
       return StatusCode::FAILURE;
     }
     
     //fiducial detector acceptance region    
     if ( (fabs(el->eta())>1.37 && fabs(el->eta())<1.52) || fabs(el->eta())>2.47 ){
      //ATH_MSG_DEBUG("Failed Probe eta " << fabs(el->eta())); 
       return StatusCode::FAILURE;
     }

     TLorentzVector probeCandidate;
     probeCandidate.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());
     Int_t jetsAroundProbeElectron = 0;  
     for(const auto &i_jet : *jets){
         TLorentzVector jet;
         jet.SetPtEtaPhiE(i_jet->pt(), i_jet->eta(), i_jet->phi(), i_jet->e());
         if( (jet.Et() > 20*GeV) && (jet.DeltaR(probeCandidate) < 0.4)) jetsAroundProbeElectron++;
     }
     //reject if more than 1 jet close to the probe electron
     if ( jetsAroundProbeElectron >= 2 ) {

         //ATH_MSG_DEBUG("too many jets around object");
         return StatusCode::FAILURE;
     }
     
     //ATH_MSG_DEBUG("Found a probe electron");
     return StatusCode::SUCCESS; 
}

StatusCode NavZeeTPxAODValidation::isMatchL2(const xAOD::Electron *){ return StatusCode::SUCCESS; }

StatusCode NavZeeTPxAODValidation::isMatchL1(const xAOD::Electron *){ 
    // Need the ROI descriptor
    //const Trig::ChainGroup* L1_Chains  = getTDT()->getChainGroup("L1_EM.*");
    return StatusCode::SUCCESS;

}

const xAOD::Electron * NavZeeTPxAODValidation::ClosestEFObject(const xAOD::Electron *eloff){
    
    // Find the closest EF object with minimum pt > 25
    const xAOD::Electron *elEF = 0;
    double dRMax = 100;

    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(eloff->pt(), eloff->trackParticle()->eta(), eloff->trackParticle()->phi(), eloff->e());
    
    const xAOD::ElectronContainer* elCont=0;
    StatusCode sc = m_storeGate->retrieve(elCont,"HLT_xAOD__ElectronContainer_egamma_Electrons");
    if (sc.isFailure()) {
        (*m_log) << MSG::DEBUG << ("REGTEST No Electron container HLT_xAOD__ElectronContainer_egamma_Electrons") << endreq;
        return 0;
    }

    for(const auto& eg : *elCont){
        if( !(eg->e()/cosh(eg->trackParticle()->eta())  > 20.0*1.e3) ) continue; 
        TLorentzVector elLV;
        elLV.SetPtEtaPhiE(eg->pt(), eg->trackParticle()->eta(), eg->trackParticle()->phi(), eg->e());
        double deltaR = elLV.DeltaR(eloffLV); 
        //find closest trigger object
        if (deltaR < dRMax) {
            dRMax = deltaR;
            elEF = eg;
        }
    }
    
    double dRcut = 0.1;
    if(dRMax <= dRcut) {
        return elEF;
    }
    else return 0;
    return 0; 
}

StatusCode NavZeeTPxAODValidation::AbsoluteResolutions(const xAOD::Electron *elProbe, const xAOD::Electron *nearestEl){
    if(!elProbe->caloCluster()) return StatusCode::FAILURE;
    if(!nearestEl->caloCluster()) return StatusCode::FAILURE;
    if(!elProbe->trackParticle()) return StatusCode::FAILURE;
    if(!nearestEl->trackParticle()) return StatusCode::FAILURE;

    cd(name()+"/" + m_chain + "/AbsoluteResolutions");
    hist1("etaCalo")->Fill( (elProbe->caloCluster()->auxdata<float>("etaCalo") - nearestEl->caloCluster()->auxdata<float>("etaCalo") )); 
    hist1("phiCalo")->Fill( (elProbe->caloCluster()->auxdata<float>("phiCalo") - nearestEl->caloCluster()->auxdata<float>("phiCalo") ));
    hist1("eta")->Fill( elProbe->eta() - nearestEl->eta());
    hist1("phi")->Fill( elProbe->phi() - nearestEl->phi());
    
    hist1("Energy")->Fill( (elProbe->caloCluster()->e() - nearestEl->caloCluster()->e() ));
    hist1("pt")->Fill( (elProbe->pt() - nearestEl->pt()));
    hist1("pt_track")->Fill( (getTrack_pt(elProbe) - getTrack_pt(nearestEl)) );

    hist1("EBE0")->Fill( (elProbe->caloCluster()->energyBE(0) - nearestEl->caloCluster()->energyBE(0) ));
    hist1("EBE1")->Fill( (elProbe->caloCluster()->energyBE(1) - nearestEl->caloCluster()->energyBE(1) ));
    hist1("EBE2")->Fill( (elProbe->caloCluster()->energyBE(2) - nearestEl->caloCluster()->energyBE(2) ));
    hist1("EBE3")->Fill( (elProbe->caloCluster()->energyBE(3) - nearestEl->caloCluster()->energyBE(3) ));
    //float ss = getShowerShape_e237(elProbe);
    
    double layer0diff = (elProbe->caloCluster()->energyBE(0) - nearestEl->caloCluster()->energyBE(0) );
    if(layer0diff > 2 ){
        (*m_log) << MSG::DEBUG << " CALOINFORMATION " << endreq; 
        (*m_log) << MSG::DEBUG << " Large energy layer difference " << endreq;
        (*m_log) << MSG::DEBUG << " Offline cluster information " << endreq;
        (*m_log) << MSG::DEBUG << " eta " << elProbe->caloCluster()->eta() << endreq; 
        (*m_log) << MSG::DEBUG << " phi " << elProbe->caloCluster()->phi() << endreq; 
        (*m_log) << MSG::DEBUG << " etaCalo " << elProbe->caloCluster()->auxdata<float>("etaCalo") << endreq; 
        (*m_log) << MSG::DEBUG << " phiCalo " << elProbe->caloCluster()->auxdata<float>("phiCalo") << endreq; 
        (*m_log) << MSG::DEBUG << " EB0 " << elProbe->caloCluster()->energyBE(0) << endreq;
        (*m_log) << MSG::DEBUG << " EB1 " << elProbe->caloCluster()->energyBE(1) << endreq;
        (*m_log) << MSG::DEBUG << " EB2 " << elProbe->caloCluster()->energyBE(2) << endreq;
        (*m_log) << MSG::DEBUG << " EB3 " << elProbe->caloCluster()->energyBE(3) << endreq; 
        (*m_log) << MSG::DEBUG << " E " << elProbe->caloCluster()->e() << endreq; 

        (*m_log) << MSG::DEBUG << " eta " << nearestEl->caloCluster()->eta() << endreq; 
        (*m_log) << MSG::DEBUG << " phi " << nearestEl->caloCluster()->phi() << endreq; 
        (*m_log) << MSG::DEBUG << " etaCalo " << nearestEl->caloCluster()->auxdata<float>("etaCalo") << endreq; 
        (*m_log) << MSG::DEBUG << " phiCalo " << nearestEl->caloCluster()->auxdata<float>("phiCalo") << endreq; 
        (*m_log) << MSG::DEBUG << " EB0 " << nearestEl->caloCluster()->energyBE(0) << endreq;
        (*m_log) << MSG::DEBUG << " EB1 " << nearestEl->caloCluster()->energyBE(1) << endreq;
        (*m_log) << MSG::DEBUG << " EB2 " << nearestEl->caloCluster()->energyBE(2) << endreq;
        (*m_log) << MSG::DEBUG << " EB3 " << nearestEl->caloCluster()->energyBE(3) << endreq; 
        (*m_log) << MSG::DEBUG << " E " << nearestEl->caloCluster()->e() << endreq; 
    }


    return StatusCode::SUCCESS;
}



/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTTauMonTool.cxx
 *   
 *    Artur Kalinowski (Artur.Kalinowski@cern.ch)       
 *    Based on HLTMonTool example by Christiane Risler and Martin zur Nedden
 *    Maintained by ccuenca, jengbou
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "AnalysisUtils/AnalysisMisc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>

#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"

#include "VxVertex/VxContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"

#include <vector>
#include <iostream>
#include <fstream>
//#define _USE_MATH_DEFINES 
#include <math.h>


#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "HLTTauMonTool.h"


using namespace std;
using namespace AnalysisUtils;
const float PI=2.0*acos(0.);
const float TWOPI=2.0*PI;


HLTTauMonTool::HLTTauMonTool(const std::string & type, const std::string & n, const IInterface* parent)
  : IHLTMonTool(type, n, parent){
  
  ATH_MSG_DEBUG("HLTTauMonTool::HLTTauMonTool()");

  declareProperty("primary_tau", m_primary_tau);
  declareProperty("monitoring_tau", m_monitoring_tau);
  declareProperty("prescaled_tau", m_prescaled_tau);
  declareProperty("FakeJetEff", m_fakeJetEff);
  declareProperty("OfflineEff", m_offlineEff);
  declareProperty("OfflineEffBDTMedium", m_offlineEffBDTMedium);
  declareProperty("TracksInfo", m_TracksInfo);
  declareProperty("EffOffTauPtCut", m_effOffTauPtCut);
  //  declareProperty("tauChain", m_tauChain);

}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
HLTTauMonTool::~HLTTauMonTool() {
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::init() {

  // put all trigger names into one arry
  for(std::vector<std::string>::iterator it = m_monitoring_tau.begin(); it != m_monitoring_tau.end(); ++it) {
    m_trigItems.push_back(*it);
  }
  for(std::vector<std::string>::iterator it = m_primary_tau.begin(); it != m_primary_tau.end(); ++it) {
    m_trigItems.push_back(*it);
  }
  for(std::vector<std::string>::iterator it = m_prescaled_tau.begin(); it != m_prescaled_tau.end(); ++it) {
    m_trigItems.push_back(*it);
  }


  return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTTauMonTool::book()
#else
StatusCode HLTTauMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun)
#endif
{

  for(unsigned int i=0;i<m_trigItems.size(); ++i) addMonGroup(new MonGroup(this,"HLT/TauMon/"+m_trigItems[i],run));

  ATH_MSG_DEBUG("initialize being called");
  
  if (newRun) for(unsigned int i=0;i<m_trigItems.size(); ++i) bookHistogramsForItem(m_trigItems[i]);
  else if ( newEventsBlock || newLumiBlock ) {
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fill() {
  
  ATH_MSG_DEBUG(" ====== Begin fillHists() ====== ");
  StatusCode sc;
  
  for(unsigned int j=0;j<m_trigItems.size();++j) 
    {
      // std::string trig_item = "EF_"+m_trigItems[j];
      std::string trig_item = "HLT_"+m_trigItems[j];
      if (m_trigItems[j] != "Dump" && m_trigItems[j] != "EF_tauNoCut_cosmic"){

 	if (getTDT()->isPassed(trig_item)) sc = fillHistogramsForItem(m_trigItems[j]); 

      } else {
	sc = fillHistogramsForItem(m_trigItems[j]);
      }

    }
  
  
  return sc;
  //ATH_MSG_DEBUG(" ====== End fillHists() ====== ");
  
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTTauMonTool::proc()
#else
StatusCode  HLTTauMonTool::proc(bool endOfEventsBlock, bool  endOfLumiBlock, bool  endOfRun)
#endif
{

  if(endOfEventsBlock || endOfLumiBlock) {}//Get rid of compilation warnings
  
  if(endOfRun){
 
   for(unsigned int i=0;i<m_trigItems.size();++i){

     if(m_trigItems[i]=="Dump") continue;

     ///EF vs L1
     divide(hist("hEFRoIEtaNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	    hist("hL1RoIEtaDenom", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	    hist("hEFvsL1EtaEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );
     
     divide(hist("hEFRoIPhiNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	    hist("hL1RoIPhiDenom", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	    hist("hEFvsL1PhiEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );
     
     divide(hist("hEFRoIEtNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	    hist("hL1RoIEtDenom", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	    hist("hEFvsL1EtEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );

      if(m_offlineEff) {
	// Eta
	divide(hist("hTauEtaL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauEta", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioEta" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauEtaEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauEta", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioEta" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	// Phi
	divide(hist("hTauPhiL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPhi", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioPhi" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );
      
	divide(hist("hTauPhiEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPhi", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioPhi" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	// Pt      
	divide(hist("hTauPtL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPt", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioPt" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauPtEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPt", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioPt" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	// Nvtx
	divide(hist("hTauNvtxL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauNvtx", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioNvtx" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauNvtxEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauNvtx", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioNvtx" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

      }

      if(m_offlineEffBDTMedium) {

	// Eta
	divide(hist("hTauEtaL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );
	
	divide(hist("hTauEtaEFBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hEFOfflineRatioEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	// Phi
	divide(hist("hTauPhiL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );
	
	divide(hist("hTauPhiEFBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hEFOfflineRatioPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );
	
	// Pt
	divide(hist("hTauPtL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	divide(hist("hTauPtEFBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hEFOfflineRatioPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	// Nvtx
	divide(hist("hTauNvtxL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauNvtxBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioNvtxBDTMedium" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	divide(hist("hTauNvtxEFBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauNvtxBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hEFOfflineRatioNvtxBDTMedium" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

      }
      
    }    
    
  }  

  return StatusCode::SUCCESS;

}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void HLTTauMonTool::bookHistogramsForItem(const std::string & trigItem){

  // define here all histograms
  //L1 Roi
  addMonGroup(new MonGroup(this,"HLT/TauMon/"+trigItem+"/L1RoI",run));
  addHistogram(new TH1F("hL1RoIEta","L1 RoI Eta ; #eta; N RoI",26,-2.6,2.6));
  addHistogram(new TH1F("hL1RoIPhi","L1 RoI Phi ; #phi; N RoI",32,-3.2,3.2));

  addHistogram(new TH2F("hL1EtaVsPhi","L1 RoI Eta vs Phi; #eta; #phi",26,-2.6,2.6,32,-3.2,3.2));

  addHistogram(new TH1F("hL1RoITauClus","L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI",40,0.,100.));
  addHistogram(new TH1F("hL1RoIEMIso","L1 RoI EM Isol ; E_{T}^{EM Isol}[GeV]; N RoI",16,-2,30));
  addHistogram(new TH1F("hL1RoIHadCore","L1 RoI HAD Core ; E_{T}^{HAD}[GeV]; N RoI",16,-2,30));
  addHistogram(new TH1F("hL1RoIHadIsol","L1 RoI HAD Isol ; E_{T}^{HAD Isol}[GeV]; N RoI",16,-2,30));

  
  //--------------------
  //EF
  //--------------------
  addMonGroup(new MonGroup(this,"HLT/TauMon/"+trigItem+"/EFTau",run));

  addHistogram(new TH1F("hEFEt","EF Et;E_{T}[GeV];Nevents",40,0.0,100.0));
  addHistogram(new TH1F("hEFnTrack","EF number of tracks;number of tracks;Nevents",10,0,10));

  addHistogram(new TH1F("hEta","EF TrigCaloCluster Eta; #eta ; Nevents",26,-2.6,2.6));
  addHistogram(new TH1F("hPhi","EF TrigCaloCluster Phi; #phi ; Nevents",32,-3.2,3.2));

  addHistogram(new TH2F("hEFEtaVsPhi","EF TrigCaloCluster Eta vs Phi; #eta ; #phi ; Nevents",
			26,-2.6,2.6,32,-3.2,3.2));
  addHistogram(new TH2F("hEtVsEta","Et from tau Jet vs #eta; #eta^{EF}; Raw E_{T}[GeV]",
			26,-2.6,2.6,40,0.0,100.0));
  addHistogram(new TH2F("hEtVsPhi","Et from tau Jet vs #phi; #phi^{EF}; Raw E_{T} [GeV]",
			32,-3.2,3.2,40,0.0,100.0));

  addHistogram(new TH1F("hEFnWideTrack","EF number of wide tracks;number of tracks;Nevents",10,0,10));
  addHistogram(new TH1F("hEFEtRaw","EF Et Raw;Uncalibrated E_{T}[GeV];Nevents",40,0.,100.));

  addHistogram(new TH1F("hEFEMRadius","EF EMRadius;EM Radius;Clusters",21,-0.01,0.2));
  addHistogram(new TH1F("hEFHADRadius","EF HADRadius;HAD Radius;Clusters",30,-0.05,0.25));

  addHistogram(new TH1F("hEFTrkAvgDist", "Track Average Distance at EF; trkAvgDist at EF; Candidates",41,-0.01,0.4));
  addHistogram(new TH1F("hEFEtovPtLd", "Et over Lead Track Pt at EF; etovPtLd at EF; Candidates",41,-0.5,20.0));
  addHistogram(new TH1F("hEFCentFrac", "Centrality Fraction at EF; centFrac at EF; Candidates",44,-0.05,1.05));
  addHistogram(new TH1F("hEFIsoFrac", "Iso Fraction at EF; isoFrac at EF; Candidates",44,-0.05,1.05));
  addHistogram(new TH1F("hEFPSSFraction", "PSS Fraction at EF; PSS at EF; Candidates",44,-0.05,1.05));
  addHistogram(new TH1F("hEFDRmax", "Max dR of associated tracks at EF; dRmax at EF; Candidates",21,-0.005,0.205));
  addHistogram(new TH1F("hEFEMFraction", "Em Fraction at EF; EM Fraction at EF; Candidates",44,-0.05,1.05));
  addHistogram(new TH1F("hEFEMPOverTrkSysP", "EMPOverTrkSysP at EF; EMPOverTrkSysP at EF; Candidates", 40,0.0,40.0));
  addHistogram(new TH1F("hEFChPiEMEOverCaloEME", "ChPiEMEOverCaloEME at EF; ChPiEMEOverCaloEME at EF; Candidates", 40,-20.0,20.0));
  addHistogram(new TH1F("hEFTopoInvMass", "TopoInvMass at EF; TopoInvMass at EF [GeV]; Candidates",51,-0.2,20.2));
  addHistogram(new TH1F("hEFMassTrkSys", "MassTrkSys at EF; MassTrkSys at EF [GeV]; Candidates",26,-0.2,5.0));
  addHistogram(new TH1F("hEFIpSigLeadTrk", "IpSigLeadTrk at EF; IpSigLeadTrk at EF; Candidates",40,-10.0,10.0));
  addHistogram(new TH1F("hEFTrFlightPathSig", "TrFlightPathSig at EF; TrFlightPathSig at EF; Candidates",40,-10.0,10.0));


  //--------------------
  //EF vs Offline
  //--------------------
  addMonGroup(new MonGroup(this,"HLT/TauMon/"+trigItem+"/EFVsOffline",run));

  addHistogram(new TH2F("hMatchdRVsEta","#Delta R for Offline tau matching EF vs #eta^{EF}; #eta^{EF}; #Delta R(EF,Offline)",
			26,-2.6,2.6,21,-0.01,0.2));
  addHistogram(new TH2F("hMatchdEtaVsEta","#Delta #eta for Offline tau matching EF vs #eta^{EF}; #eta^{EF}; #Delta #eta(EF,Offline)",
			26,-2.6,2.6,30,-0.15,0.15));
  addHistogram(new TH2F("hMatchdPhiVsEta","#Delta #phi for Offline tau matching EF vs #eta^{EF}; #eta^{EF}; #Delta #phi(EF,Offline)",
			26,-2.6,2.6,30,-0.15,0.15));
  addHistogram(new TH2F("hOffEFEMDiffVsEta","Relative difference in EM energy (EF-Offline)/Offline vs #eta^{EF}; #eta^{EF}; EM energy relative diff",
			26,-2.6,2.6, 50,-0.5,0.5));
  addHistogram(new TH2F("hOffEFEMDiffVsPhi","Relative difference in EM energy (EF-Offline)/Offline vs #phi^{EF}; #phi^{EF}; EM energy relative diff",
                        32,-3.2,3.2, 50,-0.5,0.5));
  addHistogram(new TH2F("hOffEFHADDiffVsEta","Relative difference in HAD energy (EF-Offline)/Offline vs #eta^{EF}; #eta^{EF}; HAD energy relative diff",
			26,-2.6,2.6, 50,-0.5,0.5));
  addHistogram(new TH2F("hOffEFHADDiffVsPhi","Relative difference in HAD energy (EF-Offline)/Offline vs #phi^{EF}; #phi^{EF}; HAD energy relative diff",
                        32,-3.2,3.2, 50,-0.5,0.5));
  addHistogram(new TH2F("hEMRadiusRatioVsEta",
			"Relative difference in tauRec EMRadius (EF-Offline)/Offline vs #eta^{EF}; #eta^{EF}; EMRadius relative diff",
			26,-2.6,2.6,50,-0.5,0.5));
  addHistogram(new TH2F("hIsoFracRatioVsEta",
			"Relative difference in tauRec IsoFrac (EF-Offline)/Offline vs #eta^{EF}; #eta^{EF}; IsoFrac relative diff",
			26,-2.6,2.6,50,-0.5,0.5));
  addHistogram(new TH2F("hEtOverPtLeadTrkRatioVsEta",
			"Relative difference in EtOvLdPt (EF-Offline)/offline vs #eta^{EF}; #eta^{EF}; EtOvLeadPtTrack relative diff",
			26,-2.6,2.6,50,-0.5,0.5));

  addHistogram(new TH2F("hEFvsOffnTrks","nTrks at EF vs Off; nTrks off; nTrks ef",15,0,15,15,0,15));

  addHistogram(new TH1F("hOffEFEMDiff","Relative difference in EM energy (EF-Offline)/Offline; EM energy relative diff; Candidates", 50,-0.5,0.5));
  addHistogram(new TH1F("hOffEFHADDiff","Relative difference in HAD energy (EF-Offline)/Offline; HAD energy relative diff; Candidates", 50,-0.5,0.5));

  addHistogram(new TH1F("hEMRadiusRatio","Relative difference in EMRadius ;EMRadius relative diff; Clusters",50,-0.5,0.5));
  addHistogram(new TH1F("hHadRadiusRatio","Relative difference in HadRadius ;HadRadius relative diff; Clusters",50,-0.5,0.5));
  addHistogram(new TH1F("hIsoFracRatio","Relative difference in IsoFrac;IsoFrac relative diff; Candidates",50,-0.5,0.5));
  addHistogram(new TH1F("hPSSFracRatio","Relative difference in PSSFrac;PSSFrac relative diff; Candidates",50,-0.5,0.5));
  addHistogram(new TH1F("hEtOverPtLeadTrkRatio","Relative difference in EtOvLdPt ;EtOvLeadPtTrack relative diff; Candidates",50,-0.5,0.5));
  addHistogram(new TH1F("hEFEMPOverTrkSysPRatio","Relative difference in EFEMPOverTrkSysP;EFEMPOverTrkSysP relative diff; Candidates",50,-0.5,0.5));

  addHistogram(new TH1F("hCentFracRatio","Relative difference in Centrality Fraction (EF-Offline)/Offline; CentFrac relative diff; Candidates",50,-0.5,0.5));
  addHistogram(new TH1F("hNWideTrackRatio","Relative difference in number of wide tracks (EF-Offline)/Offline; NWideTrack relative diff; Candidates", 50,-2.5,2.5));
  addHistogram(new TH1F("hNTrackRatio","Relative difference in number of tracks (EF-Offline)/Offline; NTrack relative diff; Candidates", 20,-1.0,1.0));
  addHistogram(new TH1F("hIpSigLeadTrkRatio","Relative difference in IpSigLeadTrk (EF-Offline)/Offline; IpSigLeadTrk relative diff; Candidates",80,-4.0,4.0));
  addHistogram(new TH1F("hTrFlightPathSigRatio","Relative difference in TrFlightPathSig (EF-Offline)/Offline; TrFlightPathSig relative diff; Candidates", 80,-4.0,4.0));
  addHistogram(new TH1F("hMassTrkSysRatio","Relative difference in MassTrkSys (EF-Offline)/Offline; MassTrkSys relative diff; Candidates", 40,-2.0,2.0));
  addHistogram(new TH1F("hTopoInvMassRatio","Relative difference in TopoInvMass (EF-Offline)/Offline; TopoInvMass relative diff; Candidates", 40,-2.0,2.0));
  addHistogram(new TH1F("hDRmaxRatio","Relative difference in DRmax (EF-Offline)/Offline; DRmax relative diff; Candidates", 40,-2.0,2.0));
  addHistogram(new TH1F("hTrkAvgDistRatio","Relative difference in trkAvgDist (EF-Offline)/Offline; TrkAvgDist relative diff; Candidates",40,-2.0,2.0));


  //--------------------
  //Efficiency
  //--------------------
  if(trigItem!="Dump"){

    addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/RelativeEfficiency", run, ATTRIB_MANAGED, "") );

    addHistogram(new TH1F("hL1RoIEtaDenom","L1 ROI, #eta; #eta ; Nevents",26,-2.6,2.6));
    addHistogram(new TH1F("hL1RoIPhiDenom","L1 ROI, #phi; #phi ; Nevents",32,-3.2,3.2));
    addHistogram(new TH1F("hL1RoIEtDenom","L1 ROI, E_{T}; E_{T} [GeV] ; Nevents",40,0,100));

    addHistogram(new TH1F("hEFRoIEtaNum","L1 ROI for items passing EF, #eta; #eta ; Nevents",26,-2.6,2.6));
    addHistogram(new TH1F("hEFRoIPhiNum","L1 ROI for items passing EF, #phi; #phi ; Nevents",32,-3.2,3.2));
    addHistogram(new TH1F("hEFRoIEtNum","L1 ROI for items passing EF, E_{T}; E_{T} [GeV] ; Nevents",40,0,100));


    addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/RelativeEfficiency/Efficiency", run, ATTRIB_MANAGED, "", "effAsPerCent") );

    addHistogram(new TH1F("hEFvsL1EtaEfficiency","Relative Efficiency of EF w.r.t.L1; #eta^{L1 RoI}; Efficiency",26,-2.6,2.6));
    addHistogram(new TH1F("hEFvsL1PhiEfficiency","Relative Efficiency of EF w.r.t.L1; #phi^{L1 RoI}; Efficiency",32,-3.2,3.2));
    addHistogram(new TH1F("hEFvsL1EtEfficiency","Relative Efficiency of EF w.r.t.L1; E_{T}^{L1 RoI} [GeV]; Efficiency",40,0,100));


    //--------------------
    //EF vs L1
    //--------------------
    addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/EFvsL1", run, ATTRIB_MANAGED, "") );

    addHistogram(new TH2F("hEtEFVsL1","EF vs L1: E_{T};EF E_{T} [GeV];L1 E_{T} [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtaEFVsL1","EF vs L1: #eta;EF #eta;L1 #eta",26,-2.6,2.6,26,-2.6,2.6));
    addHistogram(new TH2F("hPhiEFVsL1","EF vs L1: #phi;EF #phi;L1 #phi",32,-3.2,3.2,32,-3.2,3.2));

    addHistogram(new TH2F("hEtEFUncalibVsL1","EF vs L1: E_{T};Uncalibrated EF E_{T} [GeV];L1 E_{T} [GeV]",40,0.0,100.0,40,0.0,100.0));

    
    //--------------------
    // offline eff
    //--------------------

    if(m_offlineEff) {

      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio", run, ATTRIB_MANAGED, "") );
	
      addHistogram(new TH1F("hTauEta","Offline Tau Eta ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaL1","Eta of Offline Tau matched to L1 ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaEF","Eta of Offline Tau matched to EF ; #eta; Nevents",26,-2.6,2.6));

      addHistogram(new TH1F("hTauPhi","Offline Tau Phi ; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiL1","phi of Offline Tau matched to L1; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiEF","phi of Offline Tau matched to EF ; #phi; Nevents",32,-3.2,3.2));

      addHistogram(new TH1F("hTauPt","Offline Tau pt ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtL1","pT of Offline Tau matched to L1 ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtEF","pT of Offline Tau matched to EF ; p_{T}[GeV]; Nevents",21,-5,100));

      addHistogram(new TH1F("hTauNvtx","Offline number of Tau; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxL1","number of Tau matched to L1; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxEF","number of Tau matched to EF; nVtx; Candidates",30,0.5,30.5));

      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio/Ratio", run, ATTRIB_MANAGED, "", "effAsPerCent") );
      std::string tmpstr = boost::lexical_cast<std::string>(m_effOffTauPtCut);
      
      addHistogram(new TH1F("hL1OfflineRatioEta",
			    TString("Ratio of L1 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));
      addHistogram(new TH1F("hEFOfflineRatioEta",
			    TString("Ratio of EF w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));

      addHistogram(new TH1F("hL1OfflineRatioPhi",
			    TString("Ratio of L1 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));
      addHistogram(new TH1F("hEFOfflineRatioPhi",
			    TString("Ratio of EF w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));

      addHistogram(new TH1F("hL1OfflineRatioPt",TString("Ratio of L1 w.r.t. Offline ;Offline p_{T}[GeV]; Ratio"),21,-5,100));
      addHistogram(new TH1F("hEFOfflineRatioPt",TString("Ratio of EF w.r.t. Offline ;Offline p_{T}[GeV]; Ratio"),21,-5,100));

      addHistogram(new TH1F("hL1OfflineRatioNvtx",
			    TString("Ratio of L1 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
      addHistogram(new TH1F("hEFOfflineRatioNvtx",
			    TString("Ratio of EF w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
    }
    
    if(m_offlineEffBDTMedium) {

      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio/BDTMedium", run, ATTRIB_MANAGED, "") );
      
      addHistogram(new TH1F("hTauEtaBDTMedium","Offline BDTMedium Tau Eta ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaL1BDTMedium","Eta of Offline BDTMedium Tau matched to L1 ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaEFBDTMedium","Eta of Offline BDTMedium Tau matched to EF ; #eta; Nevents",26,-2.6,2.6));

      addHistogram(new TH1F("hTauPhiBDTMedium","Offline BDTMedium Tau Phi ; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiL1BDTMedium","phi of Offline BDTMedium Tau matched to L1; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiEFBDTMedium","phi of Offline BDTMedium Tau matched to EF ; #phi; Nevents",32,-3.2,3.2));

      addHistogram(new TH1F("hTauPtBDTMedium","Offline BDTMedium Tau pt ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtL1BDTMedium","pT of Offline BDTMedium Tau matched to L1 ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtEFBDTMedium","pT of Offline BDTMedium Tau matched to EF ; p_{T}[GeV]; Nevents",21,-5,100));
      
      addHistogram(new TH1F("hTauNvtxBDTMedium","Offline number of BDTMedium Tau; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxL1BDTMedium","number of BDTMedium Tau matched to L1; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxEFBDTMedium","number of BDTMedium Tau matched to EF; nVtx; Candidates",30,0.5,30.5));
      
      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio/Ratio/BDTMedium", run, ATTRIB_MANAGED, "", "effAsPerCent") );
      std::string tmpstr = boost::lexical_cast<std::string>(m_effOffTauPtCut);
      
      addHistogram(new TH1F("hL1OfflineRatioEtaBDTMedium",
			    TString("Ratio of L1 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));
      addHistogram(new TH1F("hEFOfflineRatioEtaBDTMedium",
			    TString("Ratio of EF w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));

      addHistogram(new TH1F("hL1OfflineRatioPhiBDTMedium",
			    TString("Ratio of L1 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));
      addHistogram(new TH1F("hEFOfflineRatioPhiBDTMedium",
			    TString("Ratio of EF w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));

      addHistogram(new TH1F("hL1OfflineRatioPtBDTMedium","Ratio of L1 w.r.t. BDTMedium Offline ;Offline p_{T}[GeV]; Ratio",21,-5,100));
      addHistogram(new TH1F("hEFOfflineRatioPtBDTMedium","Ratio of EF w.r.t. BDTMedium Offline ;Offline p_{T}[GeV]; Ratio",21,-5,100));

      addHistogram(new TH1F("hL1OfflineRatioNvtxBDTMedium",
			    TString("Ratio of L1 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
      addHistogram(new TH1F("hEFOfflineRatioNvtxBDTMedium",
			    TString("Ratio of EF w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));

    }

  }

}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fillHistogramsForItem(const std::string & trigItem){

  ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem " << trigItem);

  StatusCode sc;
  
  ///////////////////////// L1
  //MzN: causes an Fatal Error 
  sc = fillL1ForItem(trigItem);
  
  // std::string trig_item_EF = "EF_"+trigItem;
  std::string trig_item_EF = "HLT_"+trigItem;

  ///////////////////////// EF
  if(trigItem=="Dump"){

    const xAOD::TauJetContainer * tauJetCont = 0;

    if ( (evtStore()->retrieve(tauJetCont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged")).isSuccess() ) {

      xAOD::TauJetContainer::const_iterator CI,tauJetCont_end = tauJetCont->end();

      setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFTau");
      for(CI=tauJetCont->begin(); CI!=tauJetCont_end; ++CI) fillEFTau(*CI);

      setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFVsOffline");
      for(CI=tauJetCont->begin(); CI!=tauJetCont_end; ++CI) fillEFTauVsOffline(*CI);

    }

  } else {

    if (getTDT()->isPassed(trig_item_EF)) {

      ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem passed " << trig_item_EF);

      // const std::vector<Trig::Feature<xAOD::TauJetContainer> > vec_tauEFClust = (getTDT()->features("EF_"+trigItem)).get<xAOD::TauJetContainer>();
      const std::vector<Trig::Feature<xAOD::TauJetContainer> > vec_tauEFClust = (getTDT()->features("HLT_"+trigItem)).get<xAOD::TauJetContainer>();
      std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator CI = vec_tauEFClust.begin();
      
      for(; CI != vec_tauEFClust.end(); ++CI)
     	if(CI->cptr()){
     	  xAOD::TauJetContainer::const_iterator tauItr = CI->cptr()->begin();
     	  xAOD::TauJetContainer::const_iterator tauEnd = CI->cptr()->end();
    	  
     	  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFTau");
     	  for(tauItr = CI->cptr()->begin(); tauItr != tauEnd; ++tauItr) {if(*tauItr) fillEFTau(*tauItr);}
    	  
     	  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFVsOffline");
     	  for(tauItr = CI->cptr()->begin(); tauItr != tauEnd; ++tauItr) {if(*tauItr) fillEFTauVsOffline(*tauItr);}
     	}
    }
    
  }

  if(trigItem!="Dump"){
    sc = fillEfficiency(trigItem);
    // if(m_offlineEff) sc = OfflineEfficiency(trigItem);
    // if(m_offlineEffBDTMedium) sc = OfflineEfficiencyBDTMedium(trigItem);
  }
  
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not fill histograms for given item.");
  }

  return sc;

}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fillL1ForItem(const std::string & trigItem){

  ATH_MSG_DEBUG ("HLTTauMonTool::fillL1ForItem " << trigItem);

  //////////////////////// L1

  const xAOD::EmTauRoIContainer*  l1Tau_cont = 0;
  if ( !m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isSuccess() ){ // retrieve arguments: container type, container key                                
    ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_VERBOSE("found LVL1EmTauRoI in SG");
  }

  xAOD::EmTauRoIContainer::const_iterator itEMTau = l1Tau_cont->begin();
  xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();
  xAOD::EmTauRoIContainer::const_iterator matchedL1;
  
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/L1RoI");
  
  if( trigItem=="Dump"){

    for( xAOD::EmTauRoIContainer::const_iterator it=itEMTau; it!=itEMTau_e; ++it){

      hist("hL1RoIEta")->Fill((*it)->eta());
      hist("hL1RoIPhi")->Fill((*it)->phi());
      hist2("hL1EtaVsPhi")->Fill((*it)->eta(),(*it)->phi());
     
      hist("hL1RoITauClus")->Fill((*it)->tauClus()/CLHEP::GeV);
      hist("hL1RoIEMIso")  ->Fill((*it)->emIsol()/CLHEP::GeV);
      hist("hL1RoIHadCore")->Fill((*it)->hadCore()/CLHEP::GeV);
      hist("hL1RoIHadIsol")->Fill((*it)->hadIsol()/CLHEP::GeV);
 
   }

  } else {

    // std::string trig_item_EF = "EF_"+trigItem;
    std::string trig_item_EF = "HLT_"+trigItem;
    
    if (getTDT()->isPassed(trig_item_EF)){
      
      Trig::FeatureContainer f = getTDT()->features( trig_item_EF );
      Trig::FeatureContainer::combination_const_iterator cIt;   
      
      for( cIt = f.getCombinations().begin(); cIt != f.getCombinations().end(); ++cIt ) {
	
	std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = cIt->get<TrigRoiDescriptor>("initialRoI");
	std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = initRois.begin();
	
	for(; CI!= initRois.end(); ++CI){
	  if(!(CI->cptr())) continue;
	  hist("hL1RoIEta")->Fill(CI->cptr()->eta());
	  hist("hL1RoIPhi")->Fill(CI->cptr()->phi());
	  hist2("hL1EtaVsPhi")->Fill(CI->cptr()->eta(), CI->cptr()->phi());
	  unsigned int id =   CI->cptr()->roiWord();
	  //bool matched = false;
	  for( xAOD::EmTauRoIContainer::const_iterator it=itEMTau; it!=itEMTau_e;++it){
	    if(id==(*it)->roiWord()){ 
 	      //matched = true;
 	      
 	      hist("hL1RoITauClus")->Fill((*it)->tauClus()/CLHEP::GeV);
 	      hist("hL1RoIEMIso")  ->Fill((*it)->emIsol()/CLHEP::GeV);
 	      hist("hL1RoIHadCore")->Fill((*it)->hadCore()/CLHEP::GeV);
 	      hist("hL1RoIHadIsol")->Fill((*it)->hadIsol()/CLHEP::GeV); 	     
 	    }
 	  }
	  
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}

void HLTTauMonTool::fillEFTau(const xAOD::TauJet *aEFTau){

  ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTau");

  if(!aEFTau) return;

  float trkAvgDist = 0;
  float etOverPtLeadTrk = 0;
  float emRadius = 0;
  float hadRadius = 0;
  float isoFrac = 0;
  float centFrac = 0;
  float ipSigLeadTrk = 0;
  float trFlightPathSig = 0;
  float dRmax = 0;
  float massTrkSys = 0;
  float PSSFraction = 0;
  float EMPOverTrkSysP = 0;
  float chPiEMEOverCaloEME = 0;
  float etEM = 0;
  float etHad = 0;
  float topoInvMass = 0;

  float EFEMFrac = -1.0;
  
  hist("hEFEt")->Fill(aEFTau->pt()/CLHEP::GeV);
  hist("hEFnTrack")->Fill(aEFTau->nTracks());
  hist("hEta")->Fill(aEFTau->eta());
  hist("hPhi")->Fill(aEFTau->phi());

  hist2("hEFEtaVsPhi")->Fill(aEFTau->eta(),aEFTau->phi());
  hist2("hEtVsEta")->Fill(aEFTau->eta(),aEFTau->pt()/CLHEP::GeV);
  hist2("hEtVsPhi")->Fill(aEFTau->phi(),aEFTau->pt()/CLHEP::GeV);
  
  hist("hEFnWideTrack")->Fill(aEFTau->nWideTracks());

  if ( aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEM)
       && aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHad) ){
    hist("hEFEtRaw")->Fill( (etEM + etHad)/CLHEP::GeV);   

    EFEMFrac = etEM / (etEM + etHad);
    hist("hEFEMFraction")->Fill(EFEMFrac);
    
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::EMRadius, emRadius) ){
    hist("hEFEMRadius")->Fill(emRadius);  
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::hadRadius, hadRadius) ){
    hist("hEFHADRadius")->Fill(hadRadius);
  }  
 
  if ( aEFTau->detail(xAOD::TauJetParameters::trkAvgDist, trkAvgDist) ){
    hist("hEFTrkAvgDist")->Fill(trkAvgDist);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk) ){
    hist("hEFEtovPtLd")->Fill(etOverPtLeadTrk);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac) ){
    hist("hEFCentFrac")->Fill(centFrac);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::isolFrac, isoFrac) ){
    hist("hEFIsoFrac")->Fill(isoFrac);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction) ){
    hist("hEFPSSFraction")->Fill(PSSFraction);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::dRmax, dRmax) ){
    hist("hEFDRmax")->Fill(dRmax);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP) ){
    hist("hEFEMPOverTrkSysP")->Fill(EMPOverTrkSysP);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, chPiEMEOverCaloEME) ){
    hist("hEFChPiEMEOverCaloEME")->Fill(chPiEMEOverCaloEME);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::topoInvMass, topoInvMass) ){
    hist("hEFTopoInvMass")->Fill(topoInvMass/CLHEP::GeV);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys) ){
    hist("hEFMassTrkSys")->Fill(massTrkSys/CLHEP::GeV);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk) ){
    hist("hEFIpSigLeadTrk")->Fill(ipSigLeadTrk);
  }

  if ( aEFTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig) ){
    hist("hEFTrFlightPathSig")->Fill(trFlightPathSig);
  }

  
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::fillEFTauVsOffline(const xAOD::TauJet *aEFTau){

  ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTauVsOffline");

  if(!aEFTau) return;


  //match offline - EF
  const xAOD::TauJet *aOfflineTau = 0;
  float tmpR = 0.3;

  const xAOD::TauJetContainer* tauCont = 0;

  if (  (m_storeGate->retrieve(tauCont, "TauRecContainer") ).isSuccess() ) {

    xAOD::TauJetContainer::const_iterator CI,offlinetau_end = tauCont->end();
    for(CI=tauCont->begin();CI!=offlinetau_end; ++CI){
      if((*CI)){

	if( (*CI)->p4().DeltaR(aEFTau->p4()) < tmpR ){
	  tmpR = (*CI)->p4().DeltaR(aEFTau->p4());
	  aOfflineTau = (*CI);
	}

      }
    }

  }

  if(aOfflineTau){
    ATH_MSG_DEBUG("Found an offline TauJet matching EF tauJet, dR = "<<tmpR);
  }else {  
    ATH_MSG_DEBUG("Not found an offline TauJet matching EF tauJet");
    return;
  }


  //fill everything
  //matching plots
  hist2("hMatchdRVsEta")->Fill(aEFTau->eta(), tmpR);
  hist2("hMatchdEtaVsEta")->Fill(aEFTau->eta(), aEFTau->eta() - aOfflineTau->eta());
  hist2("hMatchdPhiVsEta")->Fill(aEFTau->eta(), aEFTau->phi() - aOfflineTau->phi());


  //2D ratio style plots
  
  float etEM = 0;
  float etHad = 0;
  float emRadius = 0;
  float hadRadius = 0;
  float isoFrac = 0;
  float etOverPtLeadTrk = 0;
  float etEMAtEMScale = 0;
  float etHadAtEMScale = 0;
  float etEMDiffRel = 0;
  float etHadDiffRel = 0;
  float trkAvgDist = 0;
  float centFrac = 0;
  float PSSFraction = 0;
  float EMPOverTrkSysP = 0;
  float dRMax = 0;
  float topoInvMass = 0;
  float massTrkSys = 0;
  float ipSigLeadTrk = 0;
  float trFlightPathSig = 0;

  float emRadiusOff = 0;
  float hadRadiusOff = 0;
  float isoFracOff = 0;
  float etOverPtLeadTrkOff = 0;
  float etEMAtEMScaleOff = 0;
  float etHadAtEMScaleOff = 0;
  float trkAvgDistOff = 0;
  float centFracOff = 0;
  float PSSFractionOff = 0;
  float EMPOverTrkSysPOff = 0;
  float dRMaxOff = 0;
  float topoInvMassOff = 0;
  float massTrkSysOff = 0;
  float ipSigLeadTrkOff = 0;
  float trFlightPathSigOff = 0;
  

  FillRelDiffHist(hist("hNWideTrackRatio"), aOfflineTau->nWideTracks(), aEFTau->nWideTracks(), 0, 1);

  FillRelDiffHist(hist("hNTrackRatio"), aOfflineTau->nTracks(), aEFTau->nTracks(), 0, 1);

  /////////////////////////////////// here



   if ( aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScale)
        && aOfflineTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScaleOff) ){

     FillRelDiffHist(hist("hOffEFEMDiff"), etEMAtEMScaleOff, etEMAtEMScale, 0.1, 1);

     if ( etEMAtEMScaleOff > 0.1 ) {
       etEMDiffRel = (etEMAtEMScale - etEMAtEMScaleOff) / etEMAtEMScaleOff;
       hist2("hOffEFEMDiffVsEta")->Fill(aEFTau->eta(), etEMDiffRel);
       hist2("hOffEFEMDiffVsPhi")->Fill(aEFTau->phi(), etEMDiffRel);
     }

   }
  
   if ( aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScale)
        && aOfflineTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScaleOff) ){

     FillRelDiffHist(hist("hOffEFHADDiff"), etHadAtEMScaleOff, etHadAtEMScale, 0.1, 1);

     if ( etHadAtEMScaleOff > 0.1 ) {
       etHadDiffRel = (etHadAtEMScale - etHadAtEMScaleOff) / etHadAtEMScaleOff;
       hist2("hOffEFHADDiffVsEta")->Fill(aEFTau->eta(), etHadDiffRel);
       hist2("hOffEFHADDiffVsPhi")->Fill(aEFTau->phi(), etHadDiffRel);
     }

   }
  
   if ( aEFTau->detail(xAOD::TauJetParameters::EMRadius, emRadius)
        && aOfflineTau->detail(xAOD::TauJetParameters::EMRadius, emRadiusOff) ){

     FillRelDiffHist(hist("hEMRadiusRatio"), emRadiusOff, emRadius, 0.001, 1);

     if ( emRadiusOff > 0.001 ) {
       emRadiusOff = (emRadius - emRadiusOff) / emRadiusOff;
       hist2("hEMRadiusRatioVsEta")->Fill(aEFTau->eta(), emRadiusOff);
     }

   }

   if ( aEFTau->detail(xAOD::TauJetParameters::hadRadius, hadRadius)
        && aOfflineTau->detail(xAOD::TauJetParameters::hadRadius, hadRadiusOff) ){

     FillRelDiffHist(hist("hHadRadiusRatio"), hadRadiusOff, hadRadius, 0.001, 1);

   }

   if ( aEFTau->detail(xAOD::TauJetParameters::trkAvgDist, trkAvgDist)
        && aOfflineTau->detail(xAOD::TauJetParameters::trkAvgDist, trkAvgDistOff) ){

     FillRelDiffHist(hist("hTrkAvgDistRatio"), trkAvgDistOff, trkAvgDist, 0.001, 1);

   }

   if ( aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk)
        && aOfflineTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrkOff) ){

     FillRelDiffHist(hist("hEtOverPtLeadTrkRatio"), etOverPtLeadTrkOff, etOverPtLeadTrk, 0.001, 1);

     if ( etOverPtLeadTrkOff > 0.001) {
       etOverPtLeadTrkOff = ( etOverPtLeadTrk - etOverPtLeadTrkOff) / etOverPtLeadTrkOff;
       hist2("hEtOverPtLeadTrkRatioVsEta")->Fill(aEFTau->eta(), etOverPtLeadTrkOff);
     }

   }

  if ( aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac)
       && aOfflineTau->detail(xAOD::TauJetParameters::centFrac, centFracOff) ){

    FillRelDiffHist(hist("hCentFracRatio"), centFracOff, centFrac, 0.001, 1);

  }

  if ( aEFTau->detail(xAOD::TauJetParameters::isolFrac, isoFrac)
       && aOfflineTau->detail(xAOD::TauJetParameters::isolFrac, isoFracOff) ){

    FillRelDiffHist(hist("hIsoFracRatio"), isoFracOff, isoFrac, 0.001, 1);

    if ( isoFracOff > 0.001 ) {
      isoFracOff = (isoFrac - isoFracOff) / isoFracOff;
      hist2("hIsoFracRatioVsEta")->Fill(aEFTau->eta(), isoFracOff);
    }

  }


  if ( aEFTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction)
       && aOfflineTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFractionOff) ){

    FillRelDiffHist(hist("hPSSFracRatio"), PSSFractionOff, PSSFraction, 0.001, 1);

  }

  if ( aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP)
       && aOfflineTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysPOff)){

    FillRelDiffHist(hist("hEFEMPOverTrkSysPRatio"), EMPOverTrkSysPOff, EMPOverTrkSysP, 0.001, 1);

  }

  if ( aEFTau->detail(xAOD::TauJetParameters::dRmax, dRMax)
       && aOfflineTau->detail(xAOD::TauJetParameters::dRmax, dRMaxOff) ){

    FillRelDiffHist(hist("hDRmaxRatio"), dRMaxOff, dRMax, 0.001, 1);

  }

  if ( aEFTau->detail(xAOD::TauJetParameters::topoInvMass, topoInvMass)
       && aOfflineTau->detail(xAOD::TauJetParameters::topoInvMass, topoInvMassOff)){

    FillRelDiffHist(hist("hTopoInvMassRatio"), topoInvMassOff/CLHEP::GeV, topoInvMass/CLHEP::GeV, 0.1, 1);

  }

  if ( aEFTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys)
       && aOfflineTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSysOff) ){

    FillRelDiffHist(hist("hMassTrkSysRatio"), massTrkSysOff/CLHEP::GeV, massTrkSys/CLHEP::GeV, 0.1, 1);

  }

  if ( aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk)
       && aOfflineTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrkOff) ){

    FillRelDiffHist(hist("hIpSigLeadTrkRatio"), ipSigLeadTrkOff, ipSigLeadTrk, 0.1, 2);

  }

  if ( aEFTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig)
       && aOfflineTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSigOff) ){

    FillRelDiffHist(hist("hTrFlightPathSigRatio"), trFlightPathSigOff, trFlightPathSig, 0.1, 2);

  }



  //2D plots
  hist2("hEFvsOffnTrks")->Fill(aOfflineTau->nTracks(), aEFTau->nTracks());



  return;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fillEfficiency(const std::string & trigItem){

  ATH_MSG_DEBUG ("HLTTauMonTool::fillEfficiency " << trigItem);

  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/RelativeEfficiency");

  // const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("EF_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("HLT_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);

  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFall.begin();
  for (; CI!=vec_L1roi_EFall.end();++CI){
    if(!(CI->cptr())) continue;
    hist("hL1RoIEtaDenom")->Fill(CI->cptr()->eta());
    hist("hL1RoIPhiDenom")->Fill(CI->cptr()->phi());
    const xAOD::EmTauRoI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(aEmTau_ROI) {
      hist("hL1RoIEtDenom")->Fill(aEmTau_ROI->tauClus()/CLHEP::GeV);
      ATH_MSG_DEBUG(" TrigRoiDescriptor (eta,phi): ("<<CI->cptr()->eta()<<", "<<CI->cptr()->phi()<<")"
		    <<" matched EmTau_ROI (eta,phi,et): "
		    <<aEmTau_ROI->eta()<<", "<<aEmTau_ROI->phi()<<", "<<aEmTau_ROI->tauClus()<<")");
    }
  }


  ///////////////Get passed L1 RoIs

  // const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("EF_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("HLT_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");

  for(CI = vec_L1roi_EFpass.begin(); CI!=vec_L1roi_EFpass.end(); ++CI){
    if(!(CI->cptr())) continue;
    hist("hEFRoIEtaNum")->Fill(CI->cptr()->eta());
    hist("hEFRoIPhiNum")->Fill(CI->cptr()->phi());
    const xAOD::EmTauRoI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(aEmTau_ROI) hist("hEFRoIEtNum")->Fill(aEmTau_ROI->tauClus()/CLHEP::GeV);
  }

  ATH_MSG_DEBUG(" number of PassL1RoIs at EF: "<<vec_L1roi_EFpass.size());

  return StatusCode::SUCCESS;

}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
const xAOD::EmTauRoI * HLTTauMonTool::findLVL1_ROI(const TrigRoiDescriptor * roiDesc){

  const xAOD::EmTauRoIContainer*  l1Tau_cont = 0;
  if ( !m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isSuccess() ){ // retrieve arguments: container type, container key                                
    ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
    return 0;
  } else{
    ATH_MSG_VERBOSE("found LVL1EmTauRoI in SG");
  }

  xAOD::EmTauRoIContainer::const_iterator itEMTau = l1Tau_cont->begin();
  xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();
 
  unsigned int id = roiDesc->roiWord();
  for( xAOD::EmTauRoIContainer::const_iterator it=itEMTau; it!=itEMTau_e;++it){
    if(id==(*it)->roiWord()) return (*it);
  }
  return 0;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {

  quo->Divide(num, den, 1., 1., "b");
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
float  HLTTauMonTool::deltaPhi(float phi1, float phi2)
{
  float PHI=fabs(phi1-phi2);
  return (PHI<=PI)? PHI : TWOPI-PHI;
}

float HLTTauMonTool::deltaR(float eta1, float eta2, float phi1, float phi2)
{

  float dEta = eta1 - eta2;
  float dPhi = deltaPhi(phi1, phi2);

  return sqrt(dEta*dEta + dPhi*dPhi);

}

float HLTTauMonTool::deltaR(const xAOD::TauJet* aEFTau, const xAOD::EmTauRoI* l1Tau)
{

  return deltaR(aEFTau->eta(), l1Tau->eta(), aEFTau->phi(), l1Tau->phi());

}

bool HLTTauMonTool::Match_Offline_L1(const xAOD::TauJet *aOfflineTau, const std::string & trigItem){
  
  bool matchedL1=false;
  float tmpR = 5.0;   

  // const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("EF_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("HLT_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);

  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFall.begin();

  for (; CI!=vec_L1roi_EFall.end();++CI){
    if(!(CI->cptr())) continue;
    const xAOD::EmTauRoI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(!aEmTau_ROI) continue;
    if ( deltaR(aOfflineTau, aEmTau_ROI ) < tmpR) {
      tmpR = deltaR(aOfflineTau, aEmTau_ROI);
    }   
  }  


  if (tmpR <0.2) matchedL1=true;
  return matchedL1;
  
}


bool HLTTauMonTool::Match_Offline_EF(const xAOD::TauJet *aOfflineTau, const std::string & trigItem){
  
  bool matchedEF=false;
  float tmpR = 5.0;

  // const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("EF_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("HLT_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFpass.begin();

  for(; CI!=vec_L1roi_EFpass.end(); ++CI){
    if(!(CI->cptr())) continue;      
    const xAOD::EmTauRoI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(!aEmTau_ROI) continue;
    if ( deltaR(aOfflineTau, aEmTau_ROI) < tmpR) {
      tmpR = deltaR(aOfflineTau, aEmTau_ROI);
    }      

  } 

  if (tmpR < 0.2) matchedEF = true;
  
  return matchedEF;
  
}



// StatusCode HLTTauMonTool::OfflineEfficiency(const std::string & trigItem){
  
//   // setCurrentMonGroup("HLT/TauMon/"+trigItem+"/OfflineRatio");
  
//   // const Analysis::TauJet *aOfflineTau = 0;
  
//   // std::vector<int> nTaus (4,0);
//   // const DataHandle<Analysis::TauJetContainer> offlinetau;
//   // if (  (m_storeGate->retrieve(offlinetau, "TauRecContainer") ).isSuccess() ) {
//   //   Analysis::TauJetContainer::const_iterator CI,offlinetau_end = offlinetau->end();
//   //   for(CI=offlinetau->begin();CI!=offlinetau_end; ++CI){
//   //     if(!(*CI)) continue;
//   //     if (((*CI)->pt()/CLHEP::GeV) < 15.0 || abs((*CI)->eta()) > 2.5) continue;      
//   //     if ( ((*CI)->hasAuthor( TauJetParameters::tau1P3P)) && ! ((*CI)->hasAuthor( TauJetParameters::tauRec)) ) continue;
//   //     hist("hTauPt")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   //     if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 	nTaus.at(0) += 1;
//   // 	hist("hTauEta")  -> Fill((*CI)->eta());
//   // 	hist("hTauPhi")  -> Fill((*CI)->phi());
//   //     }
//   //     aOfflineTau = (*CI);
      
//   //     if(Match_Offline_L1(aOfflineTau, trigItem)) {
//   // 	hist("hTauPtL1")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   // 	if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 	  nTaus.at(1) += 1;
//   // 	  hist("hTauEtaL1")  -> Fill((*CI)->eta());
//   // 	  hist("hTauPhiL1")  -> Fill((*CI)->phi());
//   // 	}
//   // 	// if(Match_Offline_L2(aOfflineTau, trigItem)) {
//   // 	//   hist("hTauPtL2")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   // 	//   if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 	//     nTaus.at(2) += 1;
//   // 	//     hist("hTauEtaL2")  -> Fill((*CI)->eta());   
//   // 	//     hist("hTauPhiL2")  -> Fill((*CI)->phi()); 
//   // 	//   }
//   // 	  if(Match_Offline_EF(aOfflineTau, trigItem)) {
//   // 	    hist("hTauPtEF")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   // 	    if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 	      nTaus.at(3) += 1;
//   // 	      hist("hTauEtaEF")  -> Fill((*CI)->eta());
//   // 	      hist("hTauPhiEF")  -> Fill((*CI)->phi());
//   // 	    }
//   // 	  }
//   // 	}
//   //     }
//   //   }
//   // }

//   // const VxContainer* vxContainer(0);
//   // if (evtStore()->contains<VxContainer>("VxPrimaryCandidate")) {
//   //   if (evtStore()->retrieve(vxContainer,"VxPrimaryCandidate").isFailure() ) {
//   //     ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key VxPrimaryCandidate");
//   //     return StatusCode::SUCCESS;
//   //   }
//   // } else {
//   //   ATH_MSG_DEBUG ("StoreGate doesn't contain primary vertex container with key VxPrimaryCandidate");
//   //   return StatusCode::SUCCESS;
//   // }

//   // int nVtx = vxContainer->size();
//   // int nbins = hist("hTauNvtx")->GetNbinsX();
//   // if (nVtx > 0){
//   //   if (nVtx <= nbins) {
//   //     if (nTaus.at(0) > 0) hist("hTauNvtx")  ->SetBinContent(nVtx, (nTaus.at(0) + hist("hTauNvtx")  ->GetBinContent(nVtx)));
//   //     if (nTaus.at(1) > 0) hist("hTauNvtxL1")->SetBinContent(nVtx, (nTaus.at(1) + hist("hTauNvtxL1")->GetBinContent(nVtx)));
//   //     // if (nTaus.at(2) > 0) hist("hTauNvtxL2")->SetBinContent(nVtx, (nTaus.at(2) + hist("hTauNvtxL2")->GetBinContent(nVtx)));
//   //     if (nTaus.at(3) > 0) hist("hTauNvtxEF")->SetBinContent(nVtx, (nTaus.at(3) + hist("hTauNvtxEF")->GetBinContent(nVtx)));
//   //   }
//   //   else {
//   //     if (nTaus.at(0) > 0) hist("hTauNvtx")  ->SetBinContent(nbins+1, (nTaus.at(0) + hist("hTauNvtx")  ->GetBinContent(nbins+1)));
//   //     if (nTaus.at(1) > 0) hist("hTauNvtxL1")->SetBinContent(nbins+1, (nTaus.at(1) + hist("hTauNvtxL1")->GetBinContent(nbins+1)));
//   //     // if (nTaus.at(2) > 0) hist("hTauNvtxL2")->SetBinContent(nbins+1, (nTaus.at(2) + hist("hTauNvtxL2")->GetBinContent(nbins+1)));
//   //     if (nTaus.at(3) > 0) hist("hTauNvtxEF")->SetBinContent(nbins+1, (nTaus.at(3) + hist("hTauNvtxEF")->GetBinContent(nbins+1)));
//   //   }
//   // }

//   return StatusCode::SUCCESS;
// }



// StatusCode HLTTauMonTool::OfflineEfficiencyBDTMedium(const std::string & trigItem){
  
//   // setCurrentMonGroup("HLT/TauMon/"+trigItem+"/OfflineRatio/BDTMedium");
  
//   // const Analysis::TauJet *aOfflineTau = 0;
  
//   // std::vector<int> nTaus (4,0);
//   // const DataHandle<Analysis::TauJetContainer> offlinetau;
//   // if (  (m_storeGate->retrieve(offlinetau, "TauRecContainer") ).isSuccess() ) {
//   //   Analysis::TauJetContainer::const_iterator CI,offlinetau_end = offlinetau->end();
//   //   for(CI=offlinetau->begin();CI!=offlinetau_end; ++CI){
//   //     if(!(*CI)) continue;
//   //     if (((*CI)->pt()/CLHEP::GeV) < 15.0 || abs((*CI)->eta()) > 2.5) continue;      
//   //     if ( ((*CI)->hasAuthor( TauJetParameters::tau1P3P)) && ! ((*CI)->hasAuthor( TauJetParameters::tauRec)) ) continue;
      
//   //     const Analysis::TauPID *p_tauid = (*CI)->tauID();
      
//   //     if (p_tauid && p_tauid->isTau( TauJetParameters::JetBDTSigMedium)){
//   // 	hist("hTauPtBDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   // 	if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 	  nTaus.at(0) += 1;
//   // 	  hist("hTauEtaBDTMedium")  -> Fill((*CI)->eta());
//   // 	  hist("hTauPhiBDTMedium")  -> Fill((*CI)->phi());
//   // 	}	
//   //     	aOfflineTau = (*CI);
	
//   // 	if(Match_Offline_L1(aOfflineTau, trigItem)) {
//   // 	  hist("hTauPtL1BDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   // 	  if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 	    nTaus.at(1) += 1;
//   // 	    hist("hTauEtaL1BDTMedium")  -> Fill((*CI)->eta());   
//   // 	    hist("hTauPhiL1BDTMedium")  -> Fill((*CI)->phi()); 
//   // 	  }
//   // 	  // if(Match_Offline_L2(aOfflineTau, trigItem)) {
//   // 	  //   hist("hTauPtL2BDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   // 	  //   if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 	  //     nTaus.at(2) += 1;
//   // 	  //     hist("hTauEtaL2BDTMedium")  -> Fill((*CI)->eta());
//   // 	  //     hist("hTauPhiL2BDTMedium")  -> Fill((*CI)->phi());
//   // 	  //   }
//   // 	    if(Match_Offline_EF(aOfflineTau, trigItem)) {
//   // 	      hist("hTauPtEFBDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
//   // 	      if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
//   // 		nTaus.at(3) += 1;
//   // 		hist("hTauEtaEFBDTMedium")  -> Fill((*CI)->eta());
//   // 		hist("hTauPhiEFBDTMedium")  -> Fill((*CI)->phi());
//   // 	      }
//   // 	    }
//   // 	  }
//   // 	}	
//   //     }      
//   //   }
//   // }

//   // const VxContainer* vxContainer(0);
//   // if (evtStore()->contains<VxContainer>("VxPrimaryCandidate")) {
//   //   if (evtStore()->retrieve(vxContainer,"VxPrimaryCandidate").isFailure() ) {
//   //     ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key VxPrimaryCandidate");
//   //     return StatusCode::SUCCESS;
//   //   }
//   // } else {
//   //   ATH_MSG_DEBUG ("StoreGate doesn't contain primary vertex container with key VxPrimaryCandidate");
//   //   return StatusCode::SUCCESS;
//   // }

//   // int nVtx = vxContainer->size();
//   // int nbins = hist("hTauNvtxBDTMedium")->GetNbinsX();
//   // if (nVtx > 0){
//   //   if (nVtx <= nbins) {
//   //     if (nTaus.at(0) > 0) hist("hTauNvtxBDTMedium")  ->SetBinContent(nVtx, (nTaus.at(0) + hist("hTauNvtxBDTMedium")  ->GetBinContent(nVtx)));
//   //     if (nTaus.at(1) > 0) hist("hTauNvtxL1BDTMedium")->SetBinContent(nVtx, (nTaus.at(1) + hist("hTauNvtxL1BDTMedium")->GetBinContent(nVtx)));
//   //     // if (nTaus.at(2) > 0) hist("hTauNvtxL2BDTMedium")->SetBinContent(nVtx, (nTaus.at(2) + hist("hTauNvtxL2BDTMedium")->GetBinContent(nVtx)));
//   //     if (nTaus.at(3) > 0) hist("hTauNvtxEFBDTMedium")->SetBinContent(nVtx, (nTaus.at(3) + hist("hTauNvtxEFBDTMedium")->GetBinContent(nVtx)));
//   //   }
//   //   else {
//   //     if (nTaus.at(0) > 0) hist("hTauNvtxBDTMedium")  ->SetBinContent(nbins+1, (nTaus.at(0) + hist("hTauNvtxBDTMedium")  ->GetBinContent(nbins+1)));
//   //     if (nTaus.at(1) > 0) hist("hTauNvtxL1BDTMedium")->SetBinContent(nbins+1, (nTaus.at(1) + hist("hTauNvtxL1BDTMedium")->GetBinContent(nbins+1)));
//   //     // if (nTaus.at(2) > 0) hist("hTauNvtxL2BDTMedium")->SetBinContent(nbins+1, (nTaus.at(2) + hist("hTauNvtxL2BDTMedium")->GetBinContent(nbins+1)));
//   //     if (nTaus.at(3) > 0) hist("hTauNvtxEFBDTMedium")->SetBinContent(nbins+1, (nTaus.at(3) + hist("hTauNvtxEFBDTMedium")->GetBinContent(nbins+1)));
//   //   }
//   // }

//   return StatusCode::SUCCESS;
// }


///////////////////////////////////////////////////////////
// Function to fill relative difference histograms
///////////////////////////////////////////////////////////
void HLTTauMonTool::FillRelDiffHist(TH1 * hist, float val1, float val2, float checkVal, int checkMode){
  if (val1 != Analysis::TauDetails::DEFAULT) {
    if (val2 != Analysis::TauDetails::DEFAULT) {
      if ( (checkMode == 1 && val1 > checkVal) || (checkMode == 2 && fabs(val1) > checkVal) )
	val1 = (val2 - val1) / val1;

      if (val1 >= hist->GetXaxis()->GetXmax()) {
	int nbins = hist->GetNbinsX();
	val1 = hist->GetBinLowEdge(nbins)+0.5*hist->GetBinWidth(nbins);
      }
      else if (val1 < hist->GetXaxis()->GetXmin())
	val1 = hist->GetXaxis()->GetXmin();

      hist->Fill(val1);
    }
    //else hist->Fill(hist->GetXaxis()->GetXmin()); // fill invalid values in the first visible bin instead of the underflow bin
  }
  //else hist->Fill(hist->GetXaxis()->GetXmin()); // fill invalid values in the first visible bin instead of the underflow bin

}

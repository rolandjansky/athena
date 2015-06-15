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
#include "xAODEventInfo/EventInfo.h"

#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

//#include "TrigTauEmulation/Level1EmulationTool.h"
//#include "TrigTauEmulation/HltEmulationTool.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "VxVertex/VxContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TEfficiency.h"
#include "TProfile.h"

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
  : IHLTMonTool(type, n, parent)
//  : IHLTMonTool(type, n, parent),
//    m_emulationTool(this),
//    m_hltemulationTool(this)
  {
    
    ATH_MSG_DEBUG("HLTTauMonTool::HLTTauMonTool()");
    
    declareProperty("primary_tau", 		m_primary_tau);
    declareProperty("monitoring_tau", 		m_monitoring_tau);
    declareProperty("prescaled_tau", 		m_prescaled_tau);
    declareProperty("EffOffTauPtCut", 		m_effOffTauPtCut=20000.);
    declareProperty("TurnOnCurves", 		m_turnOnCurves=true);
    declareProperty("TurnOnCurvesDenom",        m_turnOnCurvesDenom="RecoID");
    declareProperty("doTruth", 			m_truth=false);
    declareProperty("doRealZtautauEff",		m_RealZtautauEff=false);
    declareProperty("doEmulation",         	m_emulation=false);
    declareProperty("emulation_l1_tau",         m_emulation_l1_tau);    
    declareProperty("emulation_hlt_tau",        m_emulation_hlt_tau);
//    declareProperty("HltEmulationTool", 	m_hltemulationTool);
//    declareProperty("EmulationTool",            m_emulationTool);
    declareProperty("doTestTracking", 		m_doTestTracking=false);
    declareProperty("LowestSingleTau", 		m_lowest_singletau="");
    declareProperty("doIncludeL1deactivateTE", 	m_doIncludeL1deactivateTE=false);
    declareProperty("doIncludePreseldeactivateTE", m_doIncludePreseldeactivateTE=false);
    declareProperty("doIncludeHLTdeactivateTE", m_doIncludeHLTdeactivateTE=false);
    declareProperty("nTrkMax",			m_selection_nTrkMax=-1);
    declareProperty("nTrkMin",			m_selection_nTrkMin=-1);
    declareProperty("PtMax",			m_selection_ptMax=-1.);
    declareProperty("PtMin",			m_selection_ptMin=-1.);
    declareProperty("AbsEtaMax",		m_selection_absEtaMax=-1.);
    declareProperty("AbsEtaMin",		m_selection_absEtaMin=-1.);
    declareProperty("AbsPhiMax",		m_selection_absPhiMax=-1.);
    declareProperty("AbsPhiMin",		m_selection_absPhiMin=-1.);

}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
HLTTauMonTool::~HLTTauMonTool() { 
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::init() {
   
    // TrigTauEmul::Level1EmulationTool emulationTool("TauTriggerEmulator");
  if(m_emulation){
//    ATH_MSG_INFO("Yo, Initializing " << m_emulationTool->name());
//    ATH_CHECK(m_emulationTool.retrieve());
//    ATH_MSG_INFO("Yo, Initializing " << m_hltemulationTool->name());
//    ATH_CHECK(m_hltemulationTool.retrieve());
  }
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

    m_L1flag = TrigDefs::Physics;
    m_Preselectionflag = TrigDefs::Physics;
    m_HLTflag = TrigDefs::Physics;
    if(m_doIncludeL1deactivateTE) 	m_L1flag = TrigDefs::alsoDeactivateTEs;
    if(m_doIncludePreseldeactivateTE) 	m_Preselectionflag = TrigDefs::alsoDeactivateTEs;
    if(m_doIncludeHLTdeactivateTE) 	m_HLTflag = TrigDefs::alsoDeactivateTEs;

//   ATH_MSG_WARNING("Retrieving HLT tau chains");
//   const Trig::ChainGroup* m_allHLTTauItems = getTDT()->getChainGroup(".*");
//   std::vector<std::string> TauItems = m_allHLTTauItems->getListOfTriggers();
//   ATH_MSG_WARNING("Retrieved "<< TauItems.size() << " chains");
//   for(int i=0;i<TauItems.size();i++){
//	ATH_MSG_WARNING("ChainGroup "<< TauItems.at(i).c_str());
//   }
    
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
    
    for(unsigned int i=0;i<m_trigItems.size(); ++i) addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+m_trigItems[i],run));
    
    ATH_MSG_DEBUG("initialize being called");
    
    if (newRun){ 
      for(unsigned int i=0;i<m_trigItems.size(); ++i) bookHistogramsForItem(m_trigItems[i]);
      bookHistogramsAllItem();
    } else if ( newEventsBlock || newLumiBlock ) {
        return StatusCode::SUCCESS;
    }
    return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fill() {
    
    ATH_MSG_DEBUG(" ====== Begin fillHists() ====== ");
    StatusCode sc;
 
    if(!m_trigItemsAll.size()){ 
      ATH_MSG_DEBUG("Retrieving HLT tau chains");
      const Trig::ChainGroup* m_allHLTTauItems = getTDT()->getChainGroup("HLT_.*tau.*");
      m_trigItemsAll = m_allHLTTauItems->getListOfTriggers();
      ATH_MSG_DEBUG("Retrieved "<< m_trigItemsAll.size() << " chains");
      for(unsigned int i=0;i<m_trigItemsAll.size();++i){
        ATH_MSG_DEBUG("ChainGroup "<< m_trigItemsAll.at(i).c_str());
       }
//      for(unsigned int j=0;j<m_trigItems.size();++j){
//        bool activeChain(false);
//	for(unsigned int i=0;i<m_trigItemsAll.size();++i) if("HLT_"+m_trigItems.at(j) == m_trigItemsAll.at(i)) activeChain = true;
//	if(!activeChain && m_trigItems.at(j)!="Dump") ATH_MSG_WARNING("trigger chain " << m_trigItems.at(j) << " not active! skipping");
//      }
    }

 
    for(unsigned int j=0;j<m_trigItems.size();++j)
    {

	// test Trigger Before Prescale (TBP) decision:
	std::string trig_item_EF = "HLT_"+m_trigItems.at(j);
        std::string trig_item_L1(LowerChain( trig_item_EF ) );
        const unsigned int TBP_bit = getTDT()->isPassedBits(trig_item_L1);
        setCurrentMonGroup("HLT/TauMon/Expert");
	if(TBP_bit & TrigDefs::L1_isPassedBeforePrescale) hist("hL1TBPCounts")->Fill(m_trigItems.at(j).c_str(),1.);

	bool activeChain(false);
	for(unsigned int i=0;i<m_trigItemsAll.size();++i) if("HLT_"+m_trigItems.at(j) == m_trigItemsAll.at(i)) activeChain = true;
	if(!activeChain && m_trigItems.at(j)!="Dump"){ATH_MSG_DEBUG("trigger chain " << m_trigItems.at(j) << " not active! skipping");}
	else {
		std::string trig_item_EF = "HLT_"+m_trigItems.at(j);
		std::string trig_item_L1(LowerChain( trig_item_EF ) );
		setCurrentMonGroup("HLT/TauMon/Expert");
		if ( getTDT()->isPassed(trig_item_L1) ) hist("hL1Counts")->Fill(m_trigItems.at(j).c_str(),1.);
		if ( getTDT()->isPassed(trig_item_EF) ) hist("hHLTCounts")->Fill(m_trigItems.at(j).c_str(),1.);  
		//testL1TopoNavigation(m_trigItems[j]);
		sc = fillHistogramsForItem(m_trigItems[j]);
		if(sc.isFailure()){ ATH_MSG_WARNING("Failed at fillHistogramsForItem. Exiting!"); return StatusCode::FAILURE;}       	
    	}	 
    }

    if(m_emulation) {
        sc = Emulation();
        if(sc.isFailure()){ ATH_MSG_WARNING("Failed emulation. Exiting!"); return StatusCode::FAILURE;}
    }

    
    if(m_doTestTracking){ sc = test2StepTracking();
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed at test2Steptracking. Exiting!"); return StatusCode::FAILURE;}
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
  if(endOfRun)
    {
      for(unsigned int i=0;i<m_trigItems.size();++i)
	{  
          if(m_truth && m_turnOnCurves)
	    {  
	      setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      plotUnderOverFlow(hist("hTrueTauPtDenom"));  
	      plotUnderOverFlow(hist("hTrueTauPt1PDenom"));  
	      plotUnderOverFlow(hist("hTrueTauPt3PDenom"));  
	      plotUnderOverFlow(hist("hTrueTauEtaDenom")); 
	      plotUnderOverFlow(hist("hTrueTauPhiDenom"));
	      plotUnderOverFlow(hist("hTrueTauNTrackDenom"));
	      plotUnderOverFlow(hist("hTrueTauNVtxDenom"));
	      plotUnderOverFlow(hist("hTrueTauMuDenom")); 
	      plotUnderOverFlow(hist("hTrueL1PtNum"));
	      plotUnderOverFlow(hist("hTrueL1Pt1PNum"));
	      plotUnderOverFlow(hist("hTrueL1Pt3PNum"));
	      plotUnderOverFlow(hist("hTrueL1EtaNum"));
	      plotUnderOverFlow(hist("hTrueL1PhiNum"));
	      plotUnderOverFlow(hist("hTrueL1NTrackNum"));
	      plotUnderOverFlow(hist("hTrueL1NVtxNum"));   
	      plotUnderOverFlow(hist("hTrueL1MuNum"));	
	      plotUnderOverFlow(hist("hTrueHLTPtNum"));
	      plotUnderOverFlow(hist("hTrueHLTPt1PNum"));
	      plotUnderOverFlow(hist("hTrueHLTPt3PNum"));
	      plotUnderOverFlow(hist("hTrueHLTEtaNum"));
	      plotUnderOverFlow(hist("hTrueHLTPhiNum"));
	      plotUnderOverFlow(hist("hTrueHLTNTrackNum"));
	      plotUnderOverFlow(hist("hTrueHLTNVtxNum"));
	      plotUnderOverFlow(hist("hTrueHLTMuNum"));
	      setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");  
	      plotUnderOverFlow(hist("hTruthRecoTauPtDenom"));     
	      plotUnderOverFlow(hist("hTruthRecoTauPt1PDenom"));     
	      plotUnderOverFlow(hist("hTruthRecoTauPt3PDenom"));     
	      plotUnderOverFlow(hist("hTruthRecoTauEtaDenom"));     
	      plotUnderOverFlow(hist("hTruthRecoTauPhiDenom"));
	      plotUnderOverFlow(hist("hTruthRecoTauNTrackDenom"));
	      plotUnderOverFlow(hist("hTruthRecoTauNVtxDenom"));    
	      plotUnderOverFlow(hist("hTruthRecoTauMuDenom"));     
	      plotUnderOverFlow(hist("hTruthRecoL1PtNum"));
	      plotUnderOverFlow(hist("hTruthRecoL1Pt1PNum"));
	      plotUnderOverFlow(hist("hTruthRecoL1Pt3PNum"));
	      plotUnderOverFlow(hist("hTruthRecoL1EtaNum"));
	      plotUnderOverFlow(hist("hTruthRecoL1PhiNum"));
	      plotUnderOverFlow(hist("hTruthRecoL1NTrackNum"));
	      plotUnderOverFlow(hist("hTruthRecoL1NVtxNum"));
	      plotUnderOverFlow(hist("hTruthRecoL1MuNum"));    
	      plotUnderOverFlow(hist("hTruthRecoHLTPtNum"));
	      plotUnderOverFlow(hist("hTruthRecoHLTPt1PNum"));
	      plotUnderOverFlow(hist("hTruthRecoHLTPt3PNum"));
	      plotUnderOverFlow(hist("hTruthRecoHLTEtaNum"));
	      plotUnderOverFlow(hist("hTruthRecoHLTPhiNum"));
	      plotUnderOverFlow(hist("hTruthRecoHLTNTrackNum"));
	      plotUnderOverFlow(hist("hTruthRecoHLTNVtxNum"));
	      plotUnderOverFlow(hist("hTruthRecoHLTMuNum"));
	      setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      plotUnderOverFlow(hist("hCombTauPtDenom"));
	      plotUnderOverFlow(hist("hCombL1TauPtNum"));
	      plotUnderOverFlow(hist("hCombHLTTauPtNum"));
	      plotUnderOverFlow(hist("hCombelPtDenom"));
	      plotUnderOverFlow(hist("hCombL1elPtNum"));
	      plotUnderOverFlow(hist("hCombHLTelPtNum"));
	      plotUnderOverFlow(hist("hCombmuPtDenom"));
	      plotUnderOverFlow(hist("hCombL1muPtNum"));
	      plotUnderOverFlow(hist("hCombHLTmuPtNum"));
	      plotUnderOverFlow(hist("hCombOffjetPtDenom"));
	      plotUnderOverFlow(hist("hCombL1OffjetPtNum"));
	      plotUnderOverFlow(hist("hCombHLTOffjetPtNum"));
	      plotUnderOverFlow(hist("hCombMETDenom"));
	      plotUnderOverFlow(hist("hCombL1METNum"));
	      plotUnderOverFlow(hist("hCombHLTMETNum"));
	      plotUnderOverFlow(hist("hCombdRDenom"));
	      plotUnderOverFlow(hist("hCombL1dRNum"));
	      plotUnderOverFlow(hist("hCombHLTdRNum"));
	      plotUnderOverFlow(hist("hCombdEtaDenom"));
	      plotUnderOverFlow(hist("hCombL1dEtaNum"));
	      plotUnderOverFlow(hist("hCombHLTdEtaNum"));
	      plotUnderOverFlow(hist("hCombdPhiDenom"));
	      plotUnderOverFlow(hist("hCombL1dPhiNum"));
	      plotUnderOverFlow(hist("hCombHLTdPhiNum"));
	    }
	  if(m_turnOnCurves)
	    {
	      setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");  
	      plotUnderOverFlow(hist("hRecoTauPtDenom"));     
	      plotUnderOverFlow(hist("hRecoTauPt1PDenom"));     
	      plotUnderOverFlow(hist("hRecoTauPt3PDenom"));     
	      plotUnderOverFlow(hist("hRecoTauEtaDenom"));     
	      plotUnderOverFlow(hist("hRecoTauPhiDenom"));
	      plotUnderOverFlow(hist("hRecoTauNTrackDenom"));
	      plotUnderOverFlow(hist("hRecoTauNVtxDenom"));    
	      plotUnderOverFlow(hist("hRecoTauMuDenom"));     
	      plotUnderOverFlow(hist("hRecoL1PtNum"));
	      plotUnderOverFlow(hist("hRecoL1Pt1PNum"));
	      plotUnderOverFlow(hist("hRecoL1Pt3PNum"));
	      plotUnderOverFlow(hist("hRecoL1EtaNum"));
	      plotUnderOverFlow(hist("hRecoL1PhiNum"));
	      plotUnderOverFlow(hist("hRecoL1NTrackNum"));
	      plotUnderOverFlow(hist("hRecoL1NVtxNum"));
	      plotUnderOverFlow(hist("hRecoL1MuNum"));    
	      plotUnderOverFlow(hist("hRecoHLTPtNum"));
	      plotUnderOverFlow(hist("hRecoHLTPt1PNum"));
	      plotUnderOverFlow(hist("hRecoHLTPt3PNum"));
	      plotUnderOverFlow(hist("hRecoHLTEtaNum"));
	      plotUnderOverFlow(hist("hRecoHLTPhiNum"));
	      plotUnderOverFlow(hist("hRecoHLTNTrackNum"));
	      plotUnderOverFlow(hist("hRecoHLTNVtxNum"));
	      plotUnderOverFlow(hist("hRecoHLTMuNum"));
	    }
	  if(m_RealZtautauEff)
	    {
	      setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/RealZtautauEff");
	      plotUnderOverFlow(hist("hRealZttPtDenom"));
	      plotUnderOverFlow(hist("hRealZttL1PtNum"));
	      plotUnderOverFlow(hist("hRealZttHLTPtNum"));
	    }

	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/L1RoI");
	  plotUnderOverFlow(hist("hL1RoIEta"));
	  plotUnderOverFlow(hist("hL1RoIPhi"));
	  plotUnderOverFlow(hist("hL1RoIeT"));
	  plotUnderOverFlow(hist("hL1RoIisol")); 	   
	  plotUnderOverFlow(hist("hL1RoITauClus"));
	  plotUnderOverFlow(hist("hL1RoIEMIso"));
	  plotUnderOverFlow(hist("hL1RoIHadCore"));
	  plotUnderOverFlow(hist("hL1RoIHadIsol"));
	  
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/PreselectionTau");
	  plotUnderOverFlow(hist("hEFEt"));
	  plotUnderOverFlow(hist("hFTFnTrack"));
	  plotUnderOverFlow(hist("hEta"));
	  plotUnderOverFlow(hist("hPhi"));
	  plotUnderOverFlow(hist("hFTFnWideTrack"));
	  plotUnderOverFlow(hist("hdRmax"));	  

	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau");
	  plotUnderOverFlow(hist("hEFEt"));
	  plotUnderOverFlow(hist("hEFEta"));
	  plotUnderOverFlow(hist("hEFPhi"));
	  plotUnderOverFlow(hist("hEFnTrack"));
	  plotUnderOverFlow(hist("hEFEtRaw"));           
	  plotUnderOverFlow(hist("hEFnWideTrack"));
	  plotUnderOverFlow(hist("hEFEMRadius"));
	  plotUnderOverFlow(hist("hEFHADRadius"));
	  plotUnderOverFlow(hist("hEFIsoFrac"));           
	  plotUnderOverFlow(hist("hEFPSSFraction"));
	  plotUnderOverFlow(hist("hEFEMFraction"));
 	  plotUnderOverFlow(hist("hScore1p"));
	  plotUnderOverFlow(hist("hScoremp"));
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/1p_nonCorrected");
	  plotUnderOverFlow(hist("hEFinnerTrkAvgDist1PNCorr"));
	  plotUnderOverFlow(hist("hEFetOverPtLeadTrk1PNCorr"));
	  plotUnderOverFlow(hist("hEFipSigLeadTrk1PNCorr"));
	  plotUnderOverFlow(hist("hEFSumPtTrkFrac1PNCorr"));
	  plotUnderOverFlow(hist("hEFChPiEMEOverCaloEME1PNCorr"));
	  plotUnderOverFlow(hist("hEFEMPOverTrkSysP1PNCorr"));
	  plotUnderOverFlow(hist("hEFcentFrac1PNCorr"));
	  plotUnderOverFlow(hist("hEFptRatioEflowApprox1PNCorr"));
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/mp_nonCorrected");
	  plotUnderOverFlow(hist("hEFinnerTrkAvgDistMPNCorr"));
	  plotUnderOverFlow(hist("hEFetOverPtLeadTrkMPNCorr"));
	  plotUnderOverFlow(hist("hEFChPiEMEOverCaloEMEMPNCorr"));
	  plotUnderOverFlow(hist("hEFEMPOverTrkSysPMPNCorr"));
	  plotUnderOverFlow(hist("hEFcentFracMPNCorr"));
	  plotUnderOverFlow(hist("hEFptRatioEflowApproxMPNCorr"));
	  plotUnderOverFlow(hist("hEFdRmaxMPNCorr"));
	  plotUnderOverFlow(hist("hEFtrFlightPathSigMPNCorr"));
	  plotUnderOverFlow(hist("hEFmassTrkSysMPNCorr"));
	  plotUnderOverFlow(hist("hEFmEflowApproxMPNCorr"));
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/1p_Corrected");
	  plotUnderOverFlow(hist("hEFinnerTrkAvgDist1PCorr"));
	  plotUnderOverFlow(hist("hEFetOverPtLeadTrk1PCorr"));
	  plotUnderOverFlow(hist("hEFipSigLeadTrk1PCorr"));
	  plotUnderOverFlow(hist("hEFSumPtTrkFrac1PCorr"));
	  plotUnderOverFlow(hist("hEFChPiEMEOverCaloEME1PCorr"));
	  plotUnderOverFlow(hist("hEFEMPOverTrkSysP1PCorr"));
	  plotUnderOverFlow(hist("hEFcentFrac1PCorr"));
	  plotUnderOverFlow(hist("hEFptRatioEflowApprox1PCorr"));
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/mp_Corrected");
	  plotUnderOverFlow(hist("hEFinnerTrkAvgDistMPCorr"));
	  plotUnderOverFlow(hist("hEFetOverPtLeadTrkMPCorr"));
	  plotUnderOverFlow(hist("hEFChPiEMEOverCaloEMEMPCorr"));
	  plotUnderOverFlow(hist("hEFEMPOverTrkSysPMPCorr"));
	  plotUnderOverFlow(hist("hEFcentFracMPCorr"));
	  plotUnderOverFlow(hist("hEFptRatioEflowApproxMPCorr"));
	  plotUnderOverFlow(hist("hEFdRmaxMPCorr"));
	  plotUnderOverFlow(hist("hEFtrFlightPathSigMPCorr"));
	  plotUnderOverFlow(hist("hEFmassTrkSysMPCorr"));
	  plotUnderOverFlow(hist("hEFmEflowApproxMPCorr"));

	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/L1VsOffline");
	  //...
	  
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/PreselectionVsOffline");
	  plotUnderOverFlow(hist("hEFEtRatio"));
	  plotUnderOverFlow(hist("hEtaRatio"));
	  plotUnderOverFlow(hist("hPhiRatio"));
	  
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFVsOffline");
	  plotUnderOverFlow(hist("hptRatio"));
	  plotUnderOverFlow(hist("hetaRatio"));
	  plotUnderOverFlow(hist("hphiRatio"));
	  plotUnderOverFlow(hist("hEMRadiusRatio"));
	  plotUnderOverFlow(hist("hHadRadiusRatio"));
	  plotUnderOverFlow(hist("hIsoFracRatio"));
	  plotUnderOverFlow(hist("hPSSFracRatio"));
	  plotUnderOverFlow(hist("hEMFracRatio"));
	  plotUnderOverFlow(hist("hEtRawRatio"));
	  plotUnderOverFlow(hist("hOffEFEMDiff"));
	  plotUnderOverFlow(hist("hOffEFHADDiff"));
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFVsOffline/BDT/1p_nonCorrected");
	  plotUnderOverFlow(hist("hInnerTrkAvgDistRatio1P"));
	  plotUnderOverFlow(hist("hEtOverPtLeadTrkRatio1P"));
	  plotUnderOverFlow(hist("hIpSigLeadTrkRatio1P"));
	  plotUnderOverFlow(hist("hSumPtTrkFracRatio1P"));
	  plotUnderOverFlow(hist("hChPiEMEOverCaloEMERatio1P"));
	  plotUnderOverFlow(hist("hEMPOverTrkSysPRatio1P"));
	  plotUnderOverFlow(hist("hCentFracRatio1P"));
	  plotUnderOverFlow(hist("hPtRatioEflowApproxRatio1P"));
	  plotUnderOverFlow(hist("hDRmaxRatio1P"));          
	  plotUnderOverFlow(hist("hTopoInvMassRatio1P"));
	  setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFVsOffline/BDT/mp_nonCorrected");
	  plotUnderOverFlow(hist("hInnerTrkAvgDistRatioMP"));
	  plotUnderOverFlow(hist("hEtOverPtLeadTrkRatioMP"));
	  plotUnderOverFlow(hist("hChPiEMEOverCaloEMERatioMP"));
	  plotUnderOverFlow(hist("hEMPOverTrkSysPRatioMP"));
	  plotUnderOverFlow(hist("hCentFracRatioMP"));
	  plotUnderOverFlow(hist("hPtRatioEflowApproxRatioMP"));
	  plotUnderOverFlow(hist("hDRmaxRatioMP"));          
	  plotUnderOverFlow(hist("hTrFlightPathSigRatioMP"));
	  plotUnderOverFlow(hist("hMassTrkSysRatioMP"));
	  plotUnderOverFlow(hist("hMEflowApproxRatioMP"));
	  if(m_truth && m_turnOnCurves)
            {
              //Truth  
	      divide("hTrueL1PtNum","hTrueTauPtDenom","hTrueL1PtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueL1Pt1PNum","hTrueTauPt1PDenom","hTrueL1Pt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueL1Pt3PNum","hTrueTauPt3PDenom","hTrueL1Pt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueL1EtaNum","hTrueTauEtaDenom","hTrueL1EtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueL1PhiNum","hTrueTauPhiDenom","hTrueL1PhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueL1NTrackNum","hTrueTauNTrackDenom","hTrueL1NTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
              divide("hTrueL1NVtxNum","hTrueTauNVtxDenom","hTrueL1NVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
              divide("hTrueL1MuNum","hTrueTauMuDenom","hTrueL1MuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide2("hTrueL1EtaVsPhiNum","hTrueTauEtaVsPhiDenom","hTrueL1EtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency"); 
	      divide("hTrueHLTPtNum","hTrueTauPtDenom","hTrueHLTPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueHLTPt1PNum","hTrueTauPt1PDenom","hTrueHLTPt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueHLTPt3PNum","hTrueTauPt3PDenom","hTrueHLTPt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueHLTEtaNum","hTrueTauEtaDenom","hTrueHLTEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueHLTPhiNum","hTrueTauPhiDenom","hTrueHLTPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide("hTrueHLTNTrackNum","hTrueTauNTrackDenom","hTrueHLTNTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
              divide("hTrueHLTNVtxNum","hTrueTauNVtxDenom","hTrueHLTNVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
              divide("hTrueHLTMuNum","hTrueTauMuDenom","hTrueHLTMuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      divide2("hTrueHLTEtaVsPhiNum","hTrueTauEtaVsPhiDenom","hTrueHLTEtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //Truth+Reco
	      divide("hTruthRecoL1PtNum","hTruthRecoTauPtDenom","hTruthRecoL1PtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoL1Pt1PNum","hTruthRecoTauPt1PDenom","hTruthRecoL1Pt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoL1Pt3PNum","hTruthRecoTauPt3PDenom","hTruthRecoL1Pt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoL1EtaNum","hTruthRecoTauEtaDenom","hTruthRecoL1EtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoL1PhiNum","hTruthRecoTauPhiDenom","hTruthRecoL1PhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoL1NTrackNum","hTruthRecoTauNTrackDenom","hTruthRecoL1NTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
              divide("hTruthRecoL1NVtxNum","hTruthRecoTauNVtxDenom","hTruthRecoL1NVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
              divide("hTruthRecoL1MuNum","hTruthRecoTauMuDenom","hTruthRecoL1MuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide2("hTruthRecoL1EtaVsPhiNum","hTruthRecoTauEtaVsPhiDenom","hTruthRecoL1EtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoHLTPtNum","hTruthRecoTauPtDenom","hTruthRecoHLTPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoHLTPt1PNum","hTruthRecoTauPt1PDenom","hTruthRecoHLTPt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoHLTPt3PNum","hTruthRecoTauPt3PDenom","hTruthRecoHLTPt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoHLTEtaNum","hTruthRecoTauEtaDenom","hTruthRecoHLTEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoHLTPhiNum","hTruthRecoTauPhiDenom","hTruthRecoHLTPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide("hTruthRecoHLTNTrackNum","hTruthRecoTauNTrackDenom","hTruthRecoHLTNTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
              divide("hTruthRecoHLTNVtxNum","hTruthRecoTauNVtxDenom","hTruthRecoHLTNVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
              divide("hTruthRecoHLTMuNum","hTruthRecoTauMuDenom","hTruthRecoHLTMuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      divide2("hTruthRecoHLTEtaVsPhiNum","hTruthRecoTauEtaVsPhiDenom","hTruthRecoHLTEtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      
	      //Combined trig eff
	      divide("hCombL1TauPtNum","hCombTauPtDenom","hCombL1TauPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTTauPtNum","hCombTauPtDenom","hCombHLTTauPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombL1elPtNum","hCombelPtDenom","hCombL1elPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTelPtNum","hCombelPtDenom","hCombHLTelPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombL1muPtNum","hCombmuPtDenom","hCombL1muPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTmuPtNum","hCombmuPtDenom","hCombHLTmuPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombL1OffjetPtNum","hCombOffjetPtDenom","hCombL1OffjetPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTOffjetPtNum","hCombOffjetPtDenom","hCombHLTOffjetPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombL1METNum","hCombMETDenom","hCombL1METEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTMETNum","hCombMETDenom","hCombHLTMETEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombL1dRNum","hCombdRDenom","hCombL1dREfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTdRNum","hCombdRDenom","hCombHLTdREfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombL1dEtaNum","hCombdEtaDenom","hCombL1dEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTdEtaNum","hCombdEtaDenom","hCombHLTdEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombL1dPhiNum","hCombdPhiDenom","hCombL1dPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
	      divide("hCombHLTdPhiNum","hCombdPhiDenom","hCombHLTdPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
            }
	  if(m_turnOnCurves)
	    {
	      //Reco Only
	      divide("hRecoL1PtNum","hRecoTauPtDenom","hRecoL1PtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoL1Pt1PNum","hRecoTauPt1PDenom","hRecoL1Pt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoL1Pt3PNum","hRecoTauPt3PDenom","hRecoL1Pt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoL1EtaNum","hRecoTauEtaDenom","hRecoL1EtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoL1PhiNum","hRecoTauPhiDenom","hRecoL1PhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoL1NTrackNum","hRecoTauNTrackDenom","hRecoL1NTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
              divide("hRecoL1NVtxNum","hRecoTauNVtxDenom","hRecoL1NVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
              divide("hRecoL1MuNum","hRecoTauMuDenom","hRecoL1MuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide2("hRecoL1EtaVsPhiNum","hRecoTauEtaVsPhiDenom","hRecoL1EtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoHLTPtNum","hRecoTauPtDenom","hRecoHLTPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoHLTPt1PNum","hRecoTauPt1PDenom","hRecoHLTPt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoHLTPt3PNum","hRecoTauPt3PDenom","hRecoHLTPt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoHLTEtaNum","hRecoTauEtaDenom","hRecoHLTEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoHLTPhiNum","hRecoTauPhiDenom","hRecoHLTPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide("hRecoHLTNTrackNum","hRecoTauNTrackDenom","hRecoHLTNTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
              divide("hRecoHLTNVtxNum","hRecoTauNVtxDenom","hRecoHLTNVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
              divide("hRecoHLTMuNum","hRecoTauMuDenom","hRecoHLTMuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      divide2("hRecoHLTEtaVsPhiNum","hRecoTauEtaVsPhiDenom","hRecoHLTEtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	    }
	  if(m_RealZtautauEff)
	    {
	      divide("hRealZttL1PtNum","hRealZttPtDenom","hRealZttL1PtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/RealZtautauEff");
	      divide("hRealZttHLTPtNum","hRealZttPtDenom","hRealZttHLTPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/RealZtautauEff");
	    }
	}//End of trigItem loop

  // clone shifter histograms
  std::vector<string> lowest_names, lowest_trigger_names; 
  lowest_names.push_back("lowest_singletau"); 	lowest_trigger_names.push_back(m_lowest_singletau);
  //lowest_names.push_back("lowest_ditau"); 	lowest_trigger_names.push_back(m_lowest_ditau);
  //lowest_names.push_back("lowest_etau"); 	lowest_trigger_names.push_back(m_lowest_etau);
  //lowest_names.push_back("lowest_mutau");	lowest_trigger_names.push_back(m_lowest_mutau);
  //lowest_names.push_back("lowest_mettau");	lowest_trigger_names.push_back(m_lowest_mettau); 
  //lowest_names.push_back("cosmic_chain");       lowest_trigger_names.push_back(m_cosmic_chain);
 
  for(unsigned int i=0;i<lowest_names.size();++i)
    {
      bool good_item(false);
      for(unsigned int j=0;j<m_trigItems.size();++j)
	{
	  if(m_trigItems.at(j)==lowest_trigger_names.at(i)) good_item = true;
	}
      if(!good_item) continue;
      
      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoIEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoIPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoITauClus","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoIeT","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoIisol","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoIEMIso","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoIHadCore","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram("hL1RoIHadIsol","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram2("hL1EtaVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
      cloneHistogram2("hL1RoITauClusEMIso","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");     

      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/PreselectionTau");
      cloneHistogram("hEFEt","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau"); 
      cloneHistogram("hFTFnTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
      cloneHistogram("hFTFnWideTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
      cloneHistogram("hEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
      cloneHistogram("hPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
      cloneHistogram("hdRmax","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
      cloneHistogram2("hEFEtaVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");

      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau");
      cloneHistogram("hEFEt","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFnTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFnWideTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFEtRaw","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFEMRadius","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFHADRadius","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFIsoFrac","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFPSSFraction","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hEFEMFraction","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hScore1p","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram("hScoremp","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram2("hEFEtaVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram2("hEFEtVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      cloneHistogram2("hEFEtVsEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
      //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      cloneHistogram("hEFinnerTrkAvgDist1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      cloneHistogram("hEFetOverPtLeadTrk1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      cloneHistogram("hEFipSigLeadTrk1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      cloneHistogram("hEFSumPtTrkFrac1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      //      cloneHistogram("hEFChPiEMEOverCaloEME1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      cloneHistogram("hEFEMPOverTrkSysP1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      cloneHistogram("hEFcentFrac1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
      //      cloneHistogram("hEFptRatioEflowApprox1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected"); 
      //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      cloneHistogram("hEFinnerTrkAvgDistMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      cloneHistogram("hEFetOverPtLeadTrkMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      //      cloneHistogram("hEFChPiEMEOverCaloEMEMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      cloneHistogram("hEFEMPOverTrkSysPMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      cloneHistogram("hEFcentFracMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      //      cloneHistogram("hEFptRatioEflowApproxMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      cloneHistogram("hEFdRmaxMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      cloneHistogram("hEFtrFlightPathSigMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      //      cloneHistogram("hEFmassTrkSysMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      //      cloneHistogram("hEFmEflowApproxMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
      //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/1p_Corrected");
      cloneHistogram("hEFinnerTrkAvgDist1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      cloneHistogram("hEFetOverPtLeadTrk1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      cloneHistogram("hEFipSigLeadTrk1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      cloneHistogram("hEFSumPtTrkFrac1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      //      cloneHistogram("hEFChPiEMEOverCaloEME1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      cloneHistogram("hEFEMPOverTrkSysP1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      cloneHistogram("hEFcentFrac1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      //      cloneHistogram("hEFptRatioEflowApprox1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
      //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/mp_Corrected");
      cloneHistogram("hEFinnerTrkAvgDistMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      cloneHistogram("hEFetOverPtLeadTrkMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      //      cloneHistogram("hEFChPiEMEOverCaloEMEMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      cloneHistogram("hEFEMPOverTrkSysPMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      cloneHistogram("hEFcentFracMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      //      cloneHistogram("hEFptRatioEflowApproxMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      cloneHistogram("hEFdRmaxMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      cloneHistogram("hEFtrFlightPathSigMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      //      cloneHistogram("hEFmassTrkSysMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      //      cloneHistogram("hEFmEflowApproxMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
      
      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline");
      cloneHistogram("hptRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
      cloneHistogram("hetaRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
      cloneHistogram("hphiRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
      cloneHistogram("hEMRadiusRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
      cloneHistogram("hHadRadiusRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
//      cloneHistogram("hIsoFracRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
//      cloneHistogram("hPSSFracRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
      cloneHistogram("hEMFracRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
      cloneHistogram("hEtRawRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
//      cloneHistogram("hOffEFEMDiff","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
//      cloneHistogram("hOffEFHADDiff","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
//      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hInnerTrkAvgDistRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hEtOverPtLeadTrkRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hIpSigLeadTrkRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hSumPtTrkFracRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hChPiEMEOverCaloEMERatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hEMPOverTrkSysPRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hCentFracRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hPtRatioEflowApproxRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hDRmaxRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      cloneHistogram("hTopoInvMassRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
//      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hInnerTrkAvgDistRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hEtOverPtLeadTrkRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hChPiEMEOverCaloEMERatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hEMPOverTrkSysPRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hCentFracRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hPtRatioEflowApproxRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hDRmaxRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hTrFlightPathSigRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hMassTrkSysRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
//      cloneHistogram("hMEflowApproxRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");

       if(m_turnOnCurves){
         setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/TurnOnCurves");
         cloneHistogram("hRecoL1PtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
         cloneHistogram("hRecoHLTPtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
         cloneHistogram("hRecoL1PhiEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
         cloneHistogram("hRecoHLTPhiEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
         cloneHistogram("hRecoL1EtaEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
         cloneHistogram("hRecoHLTEtaEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
         cloneHistogram("hRecoL1NVtxEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
         cloneHistogram("hRecoHLTNVtxEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
       }

    }//End of lowest name loop

   }//End of Run  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void HLTTauMonTool::bookHistogramsForItem(const std::string & trigItem){
    
    const int nbin_pt = 13;
    double bins_pt[nbin_pt] = {10.,20.,25.,30.,35.,40.,45.,50.,60.,70.,100.,150.,200.};
    const int nbin_leppt = 32;
    double bins_leppt[nbin_leppt] = {10.,11.,12.,13.,14.,15.,16.,17.,18.,19.,20.,21.,22.,23.,24.,25.,26.,27.,28.,29.,30.,32.,34.,36.,38.,40.,45.,50.,60.,70.,80.,100.};
    const int nbin_eta = 9;
    double bins_eta[nbin_eta] = {-2.47,-1.52,-1.37,-0.69,0.,0.69,1.37,1.52,2.47};
    const int nbin_nvtx = 6;
    double bins_nvtx[nbin_nvtx] = {0.,5.,10.,15.,20.,25.};
    const int nbin_mu = 21;
    float bins_mu[nbin_mu] = {0.,2.,4.,6.,8.,10.,12.,14.,16.,18.,20.,22.,24.,26.,28.,30.,32.,34.,36.,38.,40.};
    const int nbin_met = 14;
    double bins_met[nbin_met] = {0.,5.,10.,20.,25.,30.,35.,40.,45.,50.,60.,70.,100.,150.};
    const int nbin_dr = 18;
    double bins_dr[nbin_dr] = {0.,0.5,1.,1.5,1.8,2.,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,3.0,3.1,3.2,3.4};

    // define here all histograms
    //L1 Roi
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/L1RoI",run));
    addHistogram(new TH1F("hL1RoIEta","L1 RoI Eta ; #eta; N RoI",100,-2.6,2.6));
    addHistogram(new TH1F("hL1RoIPhi","L1 RoI Phi ; #phi; N RoI",100,-3.2,3.2));
    
    addHistogram(new TH2F("hL1EtaVsPhi","L1 RoI Eta vs Phi; #eta; #phi",100,-2.6,2.6,100,-3.2,3.2));
    addHistogram(new TH1F("hL1RoIisol","L1 RoI Isolation; RoI Isolation Bit; N RoI",10,0.5,9.5));    
    addHistogram(new TH1F("hL1RoIeT","L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI",200,0.,100.));
    addHistogram(new TH1F("hL1RoITauClus","L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI",200,0.,100.));
    addHistogram(new TH1F("hL1RoIEMIso","L1 RoI EM Isol ; E_{T}^{EM Isol}[GeV]; N RoI",16,-2,30));
    addHistogram(new TH1F("hL1RoIHadCore","L1 RoI HAD Core ; E_{T}^{HAD}[GeV]; N RoI",16,-2,30));
    addHistogram(new TH1F("hL1RoIHadIsol","L1 RoI HAD Isol ; E_{T}^{HAD Isol}[GeV]; N RoI",16,-2,30));
    addHistogram(new TH2F("hL1RoITauClusEMIso","L1 RoI TauClus vs EMiso ; E_{T}[GeV]; E_{T}^{EM Isol}[GeV]",25,0.,100.,16,-2,30));
    
    //--------------------
    //Pre-selection Tau
    //--------------------
    
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/PreselectionTau",run));
    
    addHistogram(new TH1F("hEFEt","EF Et;E_{T}[GeV];Nevents",40,0.0,100.0));
    addHistogram(new TH1F("hFTFnTrack","EF number of tracks;number of tracks;Nevents",10,0,10));
    addHistogram(new TH1F("hEta","EF TrigCaloCluster Eta; #eta ; Nevents",26,-2.6,2.6));
    addHistogram(new TH1F("hPhi","EF TrigCaloCluster Phi; #phi ; Nevents",32,-3.2,3.2));
    addHistogram(new TH1F("hdRmax","EF deltaR max; dRmax ; Nevents",52,-0.02,0.5));
    addHistogram(new TH2F("hEFEtaVsPhi","EF TrigCaloCluster Eta vs Phi; #eta ; #phi ; Nevents",
                          26,-2.6,2.6,32,-3.2,3.2));
    addHistogram(new TH2F("hEtVsEta","Et from tau Jet vs #eta; #eta^{EF}; Raw E_{T}[GeV]",
                          26,-2.6,2.6,40,0.0,100.0));
    addHistogram(new TH2F("hEtVsPhi","Et from tau Jet vs #phi; #phi^{EF}; Raw E_{T} [GeV]",
                          32,-3.2,3.2,40,0.0,100.0));    
    addHistogram(new TH1F("hFTFnWideTrack","EF number of wide tracks;number of tracks;Nevents",10,0,10));

    //--------------------
    // EF
    //--------------------                                                                                                                                                                                                                   
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFTau",run, ATTRIB_MANAGED, ""));
    //Basic kinematic variables
    addHistogram(new TH1F("hEFEt","EF Et;E_{T}[GeV];Nevents",50,0.0,100.0));
    addHistogram(new TH1F("hEFEta","EF TrigCaloCluster Eta; #eta ; Nevents",26,-2.6,2.6));
    addHistogram(new TH1F("hEFPhi","EF TrigCaloCluster Phi; #phi ; Nevents",16,-3.2,3.2));
    addHistogram(new TH1F("hEFnTrack","EF number of tracks;number of tracks;Nevents",10,0,10));
    addHistogram(new TH2F("hEFEtaVsPhi","EF TrigCaloCluster Eta vs Phi; #eta ; #phi ; Nevents",26,-2.6,2.6,16,-3.2,3.2));
    addHistogram(new TH2F("hEFEtVsPhi","Et from tau Jet vs #phi; #phi^{EF}; Raw E_{T} [GeV]",16,-3.2,3.2,50,0.0,100.0));
    addHistogram(new TH2F("hEFEtVsEta","Et from tau Jet vs #eta; #eta^{EF}; Raw E_{T}[GeV]",26,-2.6,2.6,50,0.0,100.0));
    addHistogram(new TH1F("hEFEtRaw","EF Et Raw;Uncalibrated E_{T}[GeV];Nevents",50,0.,100.));
    addHistogram(new TH1F("hEFnWideTrack","EF number of wide tracks;number of tracks;Nevents",10,0,10));
    //other variables not used in BDT
    addHistogram(new TH1F("hEFEMRadius","EF EMRadius;EM Radius;Clusters",50,-0.1,0.5));
    addHistogram(new TH1F("hEFHADRadius","EF HADRadius;HAD Radius;Clusters",50,-0.1,0.5));
    addHistogram(new TH1F("hEFIsoFrac", "Iso Fraction at EF; isoFrac at EF; Candidates",50,-0.1,1.1));
    addHistogram(new TH1F("hEFPSSFraction", "PSS Fraction at EF; PSS at EF; Candidates",50,-0.05,1.1));
    addHistogram(new TH1F("hEFEMFraction", "Em Fraction at EF; EM Fraction at EF; Candidates",50,-0.05,1.1));
    addHistogram(new TH1F("hScore1p", "1p BDT Score; HLT BDT Score; Candidates",50,0.,1.));
    addHistogram(new TH1F("hScoremp", "mp BDT Score; HLT BDT Score; Candidates",50,0.,1.));
    //BDT inputs for 1-prong Non-Corrected
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/1p_nonCorrected",run, ATTRIB_MANAGED, ""));
    addHistogram(new TH1F("hEFinnerTrkAvgDist1PNCorr", "Inner Track Average Distance at EF 1-prong non-corrected; innertrkAvgDist at EF; Candidates",50,-0.05,0.5));
    addHistogram(new TH1F("hEFetOverPtLeadTrk1PNCorr", "Et over Lead Track Pt at EF 1-prong non-corrected; etOverPtLeadTrk at EF; Candidates",51,-0.1,25.0));
    addHistogram(new TH1F("hEFipSigLeadTrk1PNCorr", "IpSigLeadTrk at EF 1-prong non-corrected; ipSigLeadTrk at EF; Candidates",50,-20.0,20.0));
    addHistogram(new TH1F("hEFSumPtTrkFrac1PNCorr", "SumPtTrkFrac at EF 1-prong non-corrected; SumPtTrkFrac at EF; Candidates",50,-0.5,1.1));
    addHistogram(new TH1F("hEFChPiEMEOverCaloEME1PNCorr", "ChPiEMEOverCaloEME at EF 1-prong non-corrected; ChPiEMEOverCaloEME at EF; Candidates",51,-20.0,20.0));
    addHistogram(new TH1F("hEFEMPOverTrkSysP1PNCorr", "EMPOverTrkSysP at EF 1-prong non-corrected; EMPOverTrkSysP at EF; Candidates", 41,0.0,40.0));
    addHistogram(new TH1F("hEFcentFrac1PNCorr", "Centrality Fraction at EF 1-prong non-corrected; centFrac at EF; Candidates",50,-0.05,1.2));
    addHistogram(new TH1F("hEFptRatioEflowApprox1PNCorr", "ptRatioEflowApprox at EF 1-prong non-corrected; ptRatioEflowApprox at EF; Candidates",50,0.0,2.0));
    addProfile(new TProfile("hEFinnerTrkAvgDist1PNCmu", "InnerTrkAvgDist at EF vs mu 1-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFetOverPtLeadTrk1PNCmu", "EtOverPtLeadTrk at EF vs mu 1-prong non-corrected;Average interactions per bunch crossing ;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFipSigLeadTrk1PNCmu", "IpSigLeadTrk at EF vs mu 1-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFSumPtTrkFrac1PNCmu", "SumPtTrkFrac at EF vs mu 1-prong non-corrected;Average interactions per bunch crossing ;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFChPiEMEOverCaloEME1PNCmu", "ChPiEMEOverCaloEME at EF vs mu 1p non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFEMPOverTrkSysP1PNCmu", "EMPOverTrkSysP at EF vs mu 1-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFcentFrac1PNCmu", "Centrality Fraction at EF vs mu 1-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFptRatioEflowApprox1PNCmu", "ptRatioEflowApprox at EF vs mu 1p non-corrected;Average interactions per bunch crossing ;",nbin_mu-1,bins_mu));
    //BDT inputs for 3-prong Non-Corrected
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/mp_nonCorrected",run, ATTRIB_MANAGED, ""));
    addHistogram(new TH1F("hEFinnerTrkAvgDistMPNCorr", "Inner Track Average Distance at EF m-prong non-corrected; innertrkAvgDist at EF; Candidates",50,-0.05,0.5));
    addHistogram(new TH1F("hEFetOverPtLeadTrkMPNCorr", "Et over Lead Track Pt at EF m-prong non-corrected; etOverPtLeadTrk at EF; Candidates",51,-0.1,25.0));
    addHistogram(new TH1F("hEFChPiEMEOverCaloEMEMPNCorr", "ChPiEMEOverCaloEME at EF m-prong non-corrected; ChPiEMEOverCaloEME at EF; Candidates",51,-20.0,20.0));
    addHistogram(new TH1F("hEFEMPOverTrkSysPMPNCorr", "EMPOverTrkSysP at EF m-prong non-corrected; EMPOverTrkSysP at EF; Candidates", 41,0.0,40.0));
    addHistogram(new TH1F("hEFcentFracMPNCorr", "Centrality Fraction at EF m-prong non-corrected; centFrac at EF; Candidates",50,-0.05,1.2));
    addHistogram(new TH1F("hEFptRatioEflowApproxMPNCorr", "ptRatioEflowApprox at EF m-prong non-corrected; ptRatioEflowApprox at EF; Candidates",50,0.0,2.0));
    addHistogram(new TH1F("hEFdRmaxMPNCorr", "Max dR of associated tracks at EF m-prong non-corrected; dRmax at EF; Candidates",50,-0.1,0.3));
    addHistogram(new TH1F("hEFtrFlightPathSigMPNCorr", "TrFlightPathSig at EF m-prong non-corrected; trFlightPathSig at EF; Candidates",50,-20.0,20.0));
    addHistogram(new TH1F("hEFmassTrkSysMPNCorr", "MassTrkSys at EF m-prong non-corrected; MassTrkSys at EF [GeV]; Candidates",50,-0.1,15.0));
    addHistogram(new TH1F("hEFmEflowApproxMPNCorr", "mEflowApprox at EF m-prong non-corrected;  mEflowApprox at EF ; Candidates",61,-0.2,60.2));
    addProfile(new TProfile("hEFinnerTrkAvgDistMPNCmu", "InnerTrkAvgDist at EF vs mu m-prong non-corrected; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFetOverPtLeadTrkMPNCmu", "EtOverPtLeadTrk at EF vs mu m-prong non-corrected; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFChPiEMEOverCaloEMEMPNCmu", "ChPiEMEOverCaloEME at EF vs mu mp non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFEMPOverTrkSysPMPNCmu", "EMPOverTrkSysP at EF vs mu m-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFcentFracMPNCmu", "Centrality Fraction at EF vs mu m-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFptRatioEflowApproxMPNCmu", "ptRatioEflowApprox at EF vs mu mp non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFdRmaxMPNCmu", "Max dR of associated tracks at EF vs mu m-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFtrFlightPathSigMPNCmu", "TrFlightPathSig at EF vs mu m-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFmassTrkSysMPNCmu", "MassTrkSys at EF vs mu m-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFmEflowApproxMPNCmu", "mEflowApprox at EF vs mu m-prong non-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    //BDT inputs for 1-prong mu-Corrected
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/1p_Corrected",run, ATTRIB_MANAGED, ""));
    addHistogram(new TH1F("hEFinnerTrkAvgDist1PCorr", "Inner Track Average Distance at EF 1-prong mu-corrected; innertrkAvgDist at EF; Candidates",50,-0.05,0.5));
    addHistogram(new TH1F("hEFetOverPtLeadTrk1PCorr", "Et over Lead Track Pt at EF 1-prong mu-corrected; etOverPtLeadTrk at EF; Candidates",51,-0.1,25.0));
    addHistogram(new TH1F("hEFipSigLeadTrk1PCorr", "IpSigLeadTrk at EF 1-prong mu-corrected; IpSigLeadTrk at EF; Candidates",50,-20.0,20.0));
    addHistogram(new TH1F("hEFSumPtTrkFrac1PCorr", "SumPtTrkFrac at EF 1-prong mu-corrected; SumPtTrkFrac at EF; Candidates",50,-0.5,1.1));
    addHistogram(new TH1F("hEFChPiEMEOverCaloEME1PCorr", "ChPiEMEOverCaloEME at EF 1-prong mu-corrected; ChPiEMEOverCaloEME at EF; Candidates", 51,-20.0,20.0));
    addHistogram(new TH1F("hEFEMPOverTrkSysP1PCorr", "EMPOverTrkSysP at EF 1-prong mu-corrected; EMPOverTrkSysP at EF; Candidates", 41,0.0,40.0));
    addHistogram(new TH1F("hEFcentFrac1PCorr", "Centrality Fraction at EF 1-prong mu-corrected; centFrac at EF; Candidates",50,-0.05,1.2));
    addHistogram(new TH1F("hEFptRatioEflowApprox1PCorr", "ptRatioEflowApprox at EF 1-prong mu-corrected; ptRatioEflowApprox at EF; Candidates",50,0.0,2.0));
    addProfile(new TProfile("hEFinnerTrkAvgDist1PCmu", "InnerTrkAvgDist at EF vs mu 1-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFetOverPtLeadTrk1PCmu", "EtOverPtLeadTrk at EF vs mu 1-prong mu-corrected;Average interactions per bunch crossing ;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFipSigLeadTrk1PCmu", "IpSigLeadTrk at EF vs mu 1-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFSumPtTrkFrac1PCmu", "SumPtTrkFrac at EF vs mu 1-prong mu-corrected;Average interactions per bunch crossing ;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFChPiEMEOverCaloEME1PCmu", "ChPiEMEOverCaloEME at EF vs mu 1p mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFEMPOverTrkSysP1PCmu", "EMPOverTrkSysP at EF vs mu 1-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFcentFrac1PCmu", "Centrality Fraction at EF vs mu 1-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFptRatioEflowApprox1PCmu", "ptRatioEflowApprox at EF vs mu 1p mu-corrected;Average interactions per bunch crossing ;",nbin_mu-1,bins_mu));
    //BDT inputs for 3-prong mu-Corrected
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/mp_Corrected",run, ATTRIB_MANAGED, ""));
    addHistogram(new TH1F("hEFinnerTrkAvgDistMPCorr", "Inner Track Average Distance at EF m-prong mu-corrected; innertrkAvgDist at EF; Candidates",50,-0.05,0.5));
    addHistogram(new TH1F("hEFetOverPtLeadTrkMPCorr", "Et over Lead Track Pt at EF m-prong mu-corrected; etOverPtLeadTrk at EF; Candidates",51,-0.1,25.0));
    addHistogram(new TH1F("hEFChPiEMEOverCaloEMEMPCorr", "ChPiEMEOverCaloEME at EF m-prong mu-corrected; ChPiEMEOverCaloEME at EF; Candidates", 51,-20.0,20.0));
    addHistogram(new TH1F("hEFEMPOverTrkSysPMPCorr", "EMPOverTrkSysP at EF m-prong mu-corrected; EMPOverTrkSysP at EF; Candidates", 41,0.0,40.0));
    addHistogram(new TH1F("hEFcentFracMPCorr", "Centrality Fraction at EF m-prong mu-corrected; centFrac at EF; Candidates",50,-0.05,1.2));
    addHistogram(new TH1F("hEFptRatioEflowApproxMPCorr", "ptRatioEflowApprox at EF m-prong mu-corrected; ptRatioEflowApprox at EF; Candidates",50,0.0,2.0));
    addHistogram(new TH1F("hEFdRmaxMPCorr", "Max dR of associated tracks at EF m-prong mu-corrected; dRmax at EF; Candidates",50,-0.1,0.3));
    addHistogram(new TH1F("hEFtrFlightPathSigMPCorr", "TrFlightPathSig at EF m-prong mu-corrected; trFlightPathSig at EF; Candidates",50,-20.0,20.0));
    addHistogram(new TH1F("hEFmassTrkSysMPCorr", "MassTrkSys at EF m-prong mu-corrected; massTrkSys at EF [GeV]; Candidates",50,-0.1,15.0));
    addHistogram(new TH1F("hEFmEflowApproxMPCorr", "mEflowApprox at EF m-prong mu-corrected;  mEflowApprox at EF ; Candidates",61,-0.2,60.2));
    addProfile(new TProfile("hEFinnerTrkAvgDistMPCmu", "InnerTrkAvgDist at EF vs mu m-prong mu-corrected; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFetOverPtLeadTrkMPCmu", "EtOverPtLeadTrk at EF vs mu m-prong mu-corrected; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFChPiEMEOverCaloEMEMPCmu", "ChPiEMEOverCaloEME at EF vs mu mp mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFEMPOverTrkSysPMPCmu", "EMPOverTrkSysP at EF vs mu m-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFcentFracMPCmu", "Centrality Fraction at EF vs mu m-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFptRatioEflowApproxMPCmu", "ptRatioEflowApprox at EF vs mu mp mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFdRmaxMPCmu", "Max dR of associated tracks at EF vs mu m-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFtrFlightPathSigMPCmu", "TrFlightPathSig at EF vs mu m-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFmassTrkSysMPCmu", "MassTrkSys at EF vs mu m-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEFmEflowApproxMPCmu", "mEflowApprox at EF vs mu m-prong mu-corrected;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    
    //--------------------
    // L1 vs Offline
    //--------------------

    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/L1VsOffline",run));
    // add plots here ....
 
    //--------------------
    //Pre selection vs Offline
    //--------------------
    
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/PreselectionVsOffline",run));
    addHistogram(new TH2F("hPreselvsOffnTrks","nTrks at FTF vs Off; nTrks off; nTrks FTF",10,0,10,10,0,10));
    addHistogram(new TH2F("hPreselvsOffnWideTrks","nWideTrks at FTF vs Off; nWideTrks off; nWideTrks FTF",10,0,10,10,0,10));
    addHistogram(new TH1F("hEFEtRatio","FTF-Offline Et Relative difference; Et relative diff; Et relative diff",50,-0.3,0.3));
    addHistogram(new TH1F("hEtaRatio","FTF-Offline Eta Relative difference; Eta relative diff; Eta relative diff",50,-0.3,0.3));
    addHistogram(new TH1F("hPhiRatio","FTF-Offline Phi Relative difference; Phi relative diff; Phi relative diff",50,-0.05,0.05));

    if(m_truth){
      //----------------
      // EF vs Truth
      //----------------
      addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFVsTruth",run, ATTRIB_MANAGED, ""));
      addProfile(new TProfile("hEtRatiovspt","Relative difference in Et (EF-Truth)/Truth vs Truth Tau pT;Truth Tau pT;",nbin_pt-1,bins_pt));
      addProfile(new TProfile("hEtRatiovseta","Relative difference in Et (EF-Truth)/Truth vs Truth Tau eta;Truth Tau #eta;",nbin_eta-1,bins_eta));
      addProfile(new TProfile("hEtRatiovsphi","Relative difference in Et (EF-Truth)/Truth vs Truth Tau phi;Truth Tau #phi;",40,-3.2,3.2));
      addProfile(new TProfile("hEtRatiovsmu","Relative difference in Et (EF-Truth)/Truth vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    }
    //-------------------
    //EF vs Offline
    //-------------------
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFVsOffline",run, ATTRIB_MANAGED, ""));
    //Basic Kinematic Vars
    addHistogram(new TH1F("hptRatio","Relative difference in pt (EF-Offline)/Offline; pt relative diff;", 100,-0.3,0.3));
    addHistogram(new TH1F("hetaRatio","Relative difference in eta (EF-Offline)/Offline; eta relative diff;", 100,-0.3,0.3));
    addHistogram(new TH1F("hphiRatio","Relative difference in phi (EF-Offline)/Offline; phi relative diff;", 100,-0.2,0.2));
    //Other Vars
    addHistogram(new TH1F("hEMRadiusRatio","Relative difference in EMRadius ;EMRadius relative diff;",50,-0.3,0.3));
    addHistogram(new TH1F("hHadRadiusRatio","Relative difference in HadRadius ;HadRadius relative diff;",50,-0.3,0.3));
    addHistogram(new TH1F("hIsoFracRatio","Relative difference in IsoFrac;IsoFrac relative diff;",50,-0.3,0.3));
    addHistogram(new TH1F("hPSSFracRatio","Relative difference in PSSFrac;PSSFrac relative diff;",50,-0.3,0.3));
    addHistogram(new TH1F("hEMFracRatio","Relative difference in EMFrac;EMFrac relative diff;",50,-0.3,0.3));
    addHistogram(new TH1F("hEtRawRatio","Relative difference in EtRaw;EtRaw relative diff;",50,-0.3,0.3));
    addHistogram(new TH1F("hOffEFEMDiff","Relative difference in EM energy (EF-Offline)/Offline; EM energy relative diff;", 50,-0.1,0.1));
    addHistogram(new TH1F("hOffEFHADDiff","Relative difference in HAD energy (EF-Offline)/Offline; HAD energy relative diff;", 50,-0.1,0.1));
    //TProfile
    addProfile(new TProfile("hEtRawRatiovspt","Relative difference in EtRaw (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEtRawRatiovseta","Relative difference in EtRaw (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hEtRawRatiovsphi","Relative difference in EtRaw (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hEtRawRatiovsmu","Relative difference in EtRaw (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));

    addProfile(new TProfile("hEtRatiovspt","Relative difference in Et (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEtRatiovseta","Relative difference in Et (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hEtRatiovsphi","Relative difference in Et (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hEtRatiovsmu","Relative difference in Et (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));

    addProfile(new TProfile("hEMRadiusRatiovspt","Relative difference in EMRadius (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEMRadiusRatiovseta","Relative difference in EMRadius (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hEMRadiusRatiovsphi","Relative difference in EMRadius (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hEMRadiusRatiovsmu","Relative difference in EMRadius (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));

    addProfile(new TProfile("hHADRadiusRatiovspt","Relative difference in HADRadius (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hHADRadiusRatiovseta","Relative difference in HADRadius (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hHADRadiusRatiovsphi","Relative difference in HADRadius (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hHADRadiusRatiovsmu","Relative difference in HADRadius (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));

    addProfile(new TProfile("hIsoFracRatiovspt","Relative difference in IsoFrac (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hIsoFracRatiovseta","Relative difference in IsoFrac (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hIsoFracRatiovsphi","Relative difference in IsoFrac (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hIsoFracRatiovsmu","Relative difference in IsoFrac (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));

    addProfile(new TProfile("hCentFracRatiovspt","Relative difference in CentFrac (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hCentFracRatiovseta","Relative difference in CentFrac (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hCentFracRatiovsphi","Relative difference in CentFrac (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hCentFracRatiovsmu","Relative difference in CentFrac (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
   
    addProfile(new TProfile("hEMFracRatiovspt","Relative difference in EMFrac (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEMFracRatiovseta","Relative difference in EMFrac (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hEMFracRatiovsphi","Relative difference in EMFrac (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hEMFracRatiovsmu","Relative difference in EMFrac (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
 
    addProfile(new TProfile("hPSSFracRatiovspt","Relative difference in PSSFrac (EF-Offline)/Offline vs Offline Tau pT;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hPSSFracRatiovseta","Relative difference in PSSFrac (EF-Offline)/Offline vs Offline Tau eta;Offline Tau #eta;",nbin_eta-1,bins_eta));
    addProfile(new TProfile("hPSSFracRatiovsphi","Relative difference in PSSFrac (EF-Offline)/Offline vs Offline Tau phi;Offline Tau #phi;",40,-3.2,3.2));
    addProfile(new TProfile("hPSSFracRatiovsmu","Relative difference in PSSFrac (EF-Offline)/Offline vs mu;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));

    //BDT inputs 1p non corrected 
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFVsOffline/BDT/1p_nonCorrected",run, ATTRIB_MANAGED, ""));
    addHistogram(new TH1F("hInnerTrkAvgDistRatio1P","Relative diff in innertrkAvgDist (EF-Offline)/Offline; InnerTrkAvgDist 1-prong relative diff;",40,-2.0,2.0)); 
    addHistogram(new TH1F("hEtOverPtLeadTrkRatio1P","Relative diff in EtOverPtLeadTrk ;EtOverPtLeadTrack 1-prong relative diff;",40,-0.2,0.2));
    addHistogram(new TH1F("hIpSigLeadTrkRatio1P","Relative diff in IpSigLeadTrk (EF-Offline)/Offline; IpSigLeadTrk 1-prong relative diff;",40,-2.0,2.0));
    addHistogram(new TH1F("hSumPtTrkFracRatio1P","Relative diff in SumPtTrkFrac (EF-Offline)/Offline; SumPtTrkFrac 1-prong relative diff;",40,-2.0,2.0));
    addHistogram(new TH1F("hChPiEMEOverCaloEMERatio1P","Relative diff in ChPiEMEOverCaloEME (EF-Offline)/Offline; ChPiEMEOverCaloEME 1-prong relative diff;",40,-1,1));
    addHistogram(new TH1F("hEMPOverTrkSysPRatio1P","Relative diff in EMPOverTrkSysP (EF-Offline)/Offline; EMPOverTrkSysP 1-prong relative diff;",40,-0.2,0.2));
    addHistogram(new TH1F("hCentFracRatio1P","Relative diff in Centrality Fraction (EF-Offline)/Offline; CentFrac 1-prong relative diff;",40,-0.1,0.1));
    addHistogram(new TH1F("hPtRatioEflowApproxRatio1P","Relative diff in ptRatioEflowApprox (EF-Offline)/Offline; ptRatioEflowApprox 1-prong rel diff;", 40,-0.2,0.2));
    addHistogram(new TH1F("hDRmaxRatio1P","Relative diff in DRmax (EF-Offline)/Offline; DRmax 1-prong relative diff;", 40,-2.0,2.0));
    addHistogram(new TH1F("hTopoInvMassRatio1P","Relative diff in TopoInvMass (EF-Offline)/Offline; TopoInvMass 1-prong relative diff;", 40,-1.0,1.0));
    addProfile(new TProfile("hInnerTrkAvgDistVspt1P","Relative diff in innertrkAvgDist (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hInnerTrkAvgDistVsmu1P","Rel diff in innertrkAvgDist (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEtOverPtLeadTrkVspt1P","Relative diff in EtOverPtLeadTrk (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEtOverPtLeadTrkVsmu1P","Rel diff in EtOverPtLeadTrk (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hIpSigLeadTrkVspt1P","Relative diff in IpSigLeadTrk (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hIpSigLeadTrkVsmu1P","Relative diff in IpSigLeadTrk (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hSumPtTrkFracVspt1P","Relative diff in SumPtTrkFrac (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hSumPtTrkFracVsmu1P","Relative diff in SumPtTrkFrac (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hChPiEMEOvCaloEMEVspt1P","Relative diff in ChPiEMEOverCaloEME (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hChPiEMEOvCaloEMEVsmu1P","Rel diff in ChPiEMEOvCaloEME(EF-Off)/Off vs mu 1p;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEMPOverTrkSysPVspt1P","Relative diff in EMPOverTrkSysP (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEMPOverTrkSysPVsmu1P","Rel diff in EMPOverTrkSysP (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hCentFracVspt1P","Relative diff in CentFrac (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hCentFracVsmu1P","Relative diff in CentFrac (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hPtRatioEflowApproxVspt1P","Relative diff in PtRatioEflowApprox (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hPtRatioEflowApproxVsmu1P","Rel diff in PtRatioEflowApprox(EF-Off)/Off vs mu 1p;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hDRmaxVspt1P","Relative diff in DRmax (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hDRmaxVsmu1P","Relative diff in DRmax (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hTopoInvMassVspt1P","Relative diff in TopoInvMass (EF-Off)/Off vs Off Tau pT 1-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hTopoInvMassVsmu1P","Relative diff in TopoInvMass (EF-Off)/Off vs mu 1-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    //BDT inputs mp non corrected 
    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+trigItem+"/EFVsOffline/BDT/mp_nonCorrected",run, ATTRIB_MANAGED, ""));
    addHistogram(new TH1F("hInnerTrkAvgDistRatioMP","Relative diff in innertrkAvgDist (EF-Offline)/Offline; InnerTrkAvgDist m-prong relative diff;",40,-2.0,2.0)); 
    addHistogram(new TH1F("hEtOverPtLeadTrkRatioMP","Relative diff in etOverPtLeadTrk (EF-Offline)/Offline; etOverPtLeadTrk m-prong relative diff;",40,-0.2,0.2));
    addHistogram(new TH1F("hChPiEMEOverCaloEMERatioMP","Relative diff in ChPiEMEOverCaloEME (EF-Offline)/Offline; ChPiEMEOverCaloEME m-prong relative diff;",40,-1,1));
    addHistogram(new TH1F("hEMPOverTrkSysPRatioMP","Relative diff in EMPOverTrkSysP (EF-Offline)/Offline; EMPOverTrkSysP m-prong relative diff;",40,-0.2,0.2));
    addHistogram(new TH1F("hCentFracRatioMP","Relative diff in Centrality Fraction (EF-Offline)/Offline; centFrac m-prong relative diff;",40,-0.1,0.1));
    addHistogram(new TH1F("hPtRatioEflowApproxRatioMP","Relative diff in ptRatioEflowApprox (EF-Offline)/Offline; ptRatioEflowApprox m-prong rel diff;", 40,-0.2,0.2));
    addHistogram(new TH1F("hDRmaxRatioMP","Relative diff in DRmax (EF-Offline)/Offline; dRmax m-prong relative diff;", 40,-1.0,1.0));
    addHistogram(new TH1F("hTrFlightPathSigRatioMP","Relative diff in TrFlightPathSig (EF-Offline)/Offline; trFlightPathSig m-prong relative diff;", 40,-1.0,1.0));
    addHistogram(new TH1F("hMassTrkSysRatioMP","Relative diff in MassTrkSys (EF-Offline)/Offline; MassTrkSys m-prong relative diff;", 40,-0.5,0.5));
    addHistogram(new TH1F("hMEflowApproxRatioMP","Relative diff in mEflowApprox (EF-Offline)/Offline; mEflowApprox m-prong rel diff;", 40,-0.5,0.5));
    addProfile(new TProfile("hInnerTrkAvgDistVsptMP","Relative diff in innertrkAvgDist (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hInnerTrkAvgDistVsmuMP","Rel diff in innertrkAvgDist (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEtOverPtLeadTrkVsptMP","Relative diff in EtOverPtLeadTrk (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEtOverPtLeadTrkVsmuMP","Rel diff in EtOverPtLeadTrk (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hChPiEMEOvCaloEMEVsptMP","Relative diff in ChPiEMEOverCaloEME (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hChPiEMEOvCaloEMEVsmuMP","Rel diff in ChPiEMEOvCaloEME(EF-Off)/Off vs mu mp;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hEMPOverTrkSysPVsptMP","Relative diff in EMPOverTrkSysP (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hEMPOverTrkSysPVsmuMP","Rel diff in EMPOverTrkSysP (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hCentFracVsptMP","Relative diff in CentFrac (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hCentFracVsmuMP","Relative diff in CentFrac (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hPtRatioEflowApproxVsptMP","Relative diff in PtRatioEflowApprox (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hPtRatioEflowApproxVsmuMP","Rel diff in PtRatioEflowApprox(EF-Off)/Off vs mu mp;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hDRmaxVsptMP","Relative diff in DRmax (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hDRmaxVsmuMP","Relative diff in DRmax (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hTrFlightPathSigVsptMP","Relative diff in TrFlightPathSig (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hTrFlightPathSigVsmuMP","Rel diff in TrFlightPathSig (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hMassTrkSysVsptMP","Relative diff in MassTrkSys (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hMassTrkSysVsmuMP","Relative diff in MassTrkSys (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
    addProfile(new TProfile("hMEflowApproxVsptMP","Relative diff in mEflowApprox (EF-Off)/Off vs Off Tau pT m-prong;Offline Tau pT;",nbin_pt-1,bins_pt));
    addProfile(new TProfile("hMEflowApproxVsmuMP","Relative diff in mEflowApprox (EF-Off)/Off vs mu m-prong;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));

    //--------------------
    //Turn On Curves
    //--------------------
    
    if(m_turnOnCurves)
    {
       
       if(m_truth){ 
	//Truth
        addMonGroup( new MonGroup(this, "HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/TruthEfficiency",run, ATTRIB_MANAGED, "") );

        addHistogram(new TH1F("hTrueTauPtDenom",";Truth p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hTrueTauPt1PDenom",";Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hTrueTauPt3PDenom",";Truth 3 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueTauEtaDenom",";#eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTrueTauPhiDenom",";#phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hTrueTauNTrackDenom",";Number of tracks;",10,0,10));
        addHistogram(new TH1F("hTrueTauNVtxDenom",";Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTrueTauMuDenom",";Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTrueTauEtaVsPhiDenom",";#eta; #phi;",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTrueL1PtNum","L1 vs Truth;Truth p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hTrueL1Pt1PNum","L1 vs Truth;Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hTrueL1Pt3PNum","L1 vs Truth;Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueL1EtaNum","L1 vs Truth; #eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTrueL1PhiNum","L1 vs Truth;#phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hTrueL1NTrackNum","L1 vs Truth;Number of tracks;",10,0,10));
        addHistogram(new TH1F("hTrueL1NVtxNum","L1 vs Truth;Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTrueL1MuNum","L1 vs Truth;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTrueL1EtaVsPhiNum","L1 vs Truth;#eta;#phi;",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTrueHLTPtNum","HLT vs Truth;Truth p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueHLTPt1PNum","HLT vs Truth;Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueHLTPt3PNum","HLT vs Truth;Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueHLTEtaNum","HLT vs Truth; #eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTrueHLTPhiNum","HLT vs Truth;#phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hTrueHLTNTrackNum","HLT vs Truth;Number of tracks;",10,0,10));
        addHistogram(new TH1F("hTrueHLTNVtxNum","HLT vs Truth;Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTrueHLTMuNum","HLT vs Truth;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTrueHLTEtaVsPhiNum","HLT vs Truth;#eta;#phi;",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTrueL1PtEfficiency","L1 vs Truth Efficiency; Truth p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueL1Pt1PEfficiency","L1 vs Truth Efficiency; Truth 1 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueL1Pt3PEfficiency","L1 vs Truth Efficiency; Truth 3 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueL1EtaEfficiency","L1 vs Truth Efficiency; Truth #eta; Efficiency",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTrueL1PhiEfficiency","L1 vs Truth Efficiency; Truth #phi; Efficiency",16,-3.2,3.2));
	addHistogram(new TH1F("hTrueL1NTrackEfficiency","L1 vs Truth Efficiency; Number of tracks; Efficiency",10,0,10));
        addHistogram(new TH1F("hTrueL1NVtxEfficiency","L1 vs Truth Efficiency; Number of primary vertices; Efficiency",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTrueL1MuEfficiency","L1 vs Truth Efficiency; Average interactions per bunch crossing; Efficiency",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTrueL1EtaVsPhiEfficiency","L1 vs Truth in Eta-Phi; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTrueHLTPtEfficiency","HLT vs Truth Efficiency; Truth p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueHLTPt1PEfficiency","HLT vs Truth Efficiency; Truth 1 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueHLTPt3PEfficiency","HLT vs Truth Efficiency; Truth 3 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTrueHLTEtaEfficiency","HLT vs Truth Efficiency; Truth #eta; Efficiency",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTrueHLTPhiEfficiency","HLT vs Truth Efficiency; Truth #phi; Efficiency",16,-3.2,3.2));
	addHistogram(new TH1F("hTrueHLTNTrackEfficiency","HLT vs Truth Efficiency; Number of tracks; Efficiency",10,0,10));
        addHistogram(new TH1F("hTrueHLTNVtxEfficiency","HLT vs Truth Efficiency; Number of primary vertices; Efficiency",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTrueHLTMuEfficiency","HLT vs Truth Efficiency; Average interactions per bunch crossing; Efficiency",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTrueHLTEtaVsPhiEfficiency","HLT vs truth in Eta-Phi; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

	//Truth+Reco
        addMonGroup( new MonGroup(this, "HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/Truth+RecoEfficiency",run, ATTRIB_MANAGED, "") );

        addHistogram(new TH1F("hTruthRecoTauPtDenom","Truth p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hTruthRecoTauPt1PDenom",";Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hTruthRecoTauPt3PDenom",";Truth 3 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoTauEtaDenom",";#eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTruthRecoTauPhiDenom",";#phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hTruthRecoTauNTrackDenom",";Number of tracks;",10,0,10));
        addHistogram(new TH1F("hTruthRecoTauNVtxDenom",";Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTruthRecoTauMuDenom",";Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTruthRecoTauEtaVsPhiDenom",";#eta;#phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTruthRecoL1PtNum","L1 vs Truth+Reco; Truth p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoL1Pt1PNum","L1 vs Truth+Reco; Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoL1Pt3PNum","L1 vs Truth+Reco; Truth 3 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoL1EtaNum","L1 vs Truth+Reco; #eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTruthRecoL1PhiNum","L1 vs Truth+Reco #phi;;",16,-3.2,3.2));
	addHistogram(new TH1F("hTruthRecoL1NTrackNum","L1 vs Truth+Reco; Number of tracks;",10,0,10));
        addHistogram(new TH1F("hTruthRecoL1NVtxNum","L1 vs Truth+Reco; Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTruthRecoL1MuNum","L1 vs Truth+Reco;Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTruthRecoL1EtaVsPhiNum","L1 vs Truth+Reco;#eta;#phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTruthRecoHLTPtNum","HLT vs Truth+Reco; Truth p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoHLTPt1PNum","HLT vs Truth+Reco; Truth 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoHLTPt3PNum","HLT vs Truth+Reco; Truth 3 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoHLTEtaNum","HLT vs Truth+Reco; #eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTruthRecoHLTPhiNum","HLT vs Truth+Reco; #phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hTruthRecoHLTNTrackNum","HLT vs Truth+Reco; Number of tracks;",10,0,10));
        addHistogram(new TH1F("hTruthRecoHLTNVtxNum","HLT vs Truth+Reco; Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTruthRecoHLTMuNum","HLT vs Truth+Reco; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTruthRecoHLTEtaVsPhiNum","HLT vs Truth+Reco; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTruthRecoL1PtEfficiency","L1 vs Truth+Reco Efficiency; Truth p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoL1Pt1PEfficiency","L1 vs Truth+Reco Efficiency; Truth 1 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoL1Pt3PEfficiency","L1 vs Truth+Reco Efficiency; Truth 3 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoL1EtaEfficiency","L1 vs Truth+Reco Efficiency; Truth #eta; Efficiency",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTruthRecoL1PhiEfficiency","L1 vs Truth+Reco Efficiency; Truth #phi; Efficiency",16,-3.2,3.2));
	addHistogram(new TH1F("hTruthRecoL1NTrackEfficiency","L1 vs Truth+Reco Efficiency; Number of tracks; Efficiency",10,0,10));
        addHistogram(new TH1F("hTruthRecoL1NVtxEfficiency","L1 vs Truth+Reco Efficiency; Number of primary vertices; Efficiency",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTruthRecoL1MuEfficiency","L1 vs Truth+Reco Efficiency; Average interactions per bunch crossing; Efficiency",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hTruthRecoL1EtaVsPhiEfficiency","L1 vs Truth+Reco in Eta-Phi; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hTruthRecoHLTPtEfficiency","HLT vs Truth+Reco Efficiency; Truth p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoHLTPt1PEfficiency","HLT vs Truth+Reco Efficiency; Truth 1 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoHLTPt3PEfficiency","HLT vs Truth+Reco Efficiency; Truth 3 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hTruthRecoHLTEtaEfficiency","HLT vs Truth+Reco Efficiency; Truth #eta; Efficiency",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hTruthRecoHLTPhiEfficiency","HLT vs Truth+Reco Efficiency; Truth #phi; Efficiency",16,-3.2,3.2));
	addHistogram(new TH1F("hTruthRecoHLTNTrackEfficiency","HLT vs Truth+Reco Efficiency; Number of tracks; Efficiency",10,0,10));
        addHistogram(new TH1F("hTruthRecoHLTNVtxEfficiency","HLT vs Truth+Reco Efficiency; Number of primary vertices; Efficiency",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hTruthRecoHLTMuEfficiency","HLT vs Truth+Reco Efficiency; Average interactions per bunch crossing; Efficiency",nbin_mu-1,bins_mu));
  	addHistogram(new TH2F("hTruthRecoHLTEtaVsPhiEfficiency","HLT vs Truth+Reco in  Eta-Phi; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));
	
	//--------------------
	//Efficiency Histograms for Combined Triggers
	//--------------------
  	
	addMonGroup( new MonGroup(this, "HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/TauComboEfficiency",run, ATTRIB_MANAGED, "") );
	addHistogram(new TH1F("hCombTauPtDenom",";True Tau p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombL1TauPtNum",";True Tau p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombHLTTauPtNum",";True Tau p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombL1TauPtEfficiency","L1 vs tau+tau Efficiency; True Tau p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombHLTTauPtEfficiency","HLT vs tau+tau Efficiency; True Tau p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));

	addHistogram(new TH1F("hCombelPtDenom",";True el p_{T} [GeV];",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombL1elPtNum",";True el p_{T} [GeV];",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombHLTelPtNum",";True el p_{T} [GeV];",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombL1elPtEfficiency","L1 vs tau+el Efficiency; True el p_{T} [GeV]; Efficiency",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombHLTelPtEfficiency","HLT vs tau+el Efficiency; True el p_{T} [GeV]; Efficiency",nbin_leppt-1,bins_leppt));

	addHistogram(new TH1F("hCombmuPtDenom",";True mu p_{T} [GeV];",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombL1muPtNum",";True mu p_{T} [GeV];",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombHLTmuPtNum",";True mu p_{T} [GeV];",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombL1muPtEfficiency","L1 vs tau+mu Efficiency; True mu p_{T} [GeV]; Efficiency",nbin_leppt-1,bins_leppt));
	addHistogram(new TH1F("hCombHLTmuPtEfficiency","HLT vs tau+mu Efficiency; True mu p_{T} [GeV]; Efficiency",nbin_leppt-1,bins_leppt));
	
	addHistogram(new TH1F("hCombOffjetPtDenom",";Offline jet mu p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombL1OffjetPtNum",";Offline jet mu p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombHLTOffjetPtNum",";Offline jet mu p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombL1OffjetPtEfficiency","L1 vs tau+OffJet Efficiency; Offline jet p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hCombHLTOffjetPtEfficiency","HLT vs tau+OffJet Efficiency; Offline jet p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));

	addHistogram(new TH1F("hCombMETDenom",";MET;",nbin_met-1,bins_met));
	addHistogram(new TH1F("hCombL1METNum",";MET;",nbin_met-1,bins_met));
	addHistogram(new TH1F("hCombHLTMETNum",";MET;",nbin_met-1,bins_met));
	addHistogram(new TH1F("hCombL1METEfficiency","L1 vs tau+met Efficiency; MET; Efficiency",nbin_met-1,bins_met));
	addHistogram(new TH1F("hCombHLTMETEfficiency","HLT vs tau+met Efficiency; MET; Efficiency",nbin_met-1,bins_met));
	
	addHistogram(new TH1F("hCombdRDenom","; dR;",nbin_dr-1,bins_dr));
	addHistogram(new TH1F("hCombL1dRNum","; dR;",nbin_dr-1,bins_dr));
	addHistogram(new TH1F("hCombHLTdRNum","; dR;",nbin_dr-1,bins_dr));
	addHistogram(new TH1F("hCombL1dREfficiency","L1 vs dR Efficiency; dR; Efficiency",nbin_dr-1,bins_dr));
	addHistogram(new TH1F("hCombHLTdREfficiency","HLT vs dR Efficiency; dR; Efficiency",nbin_dr-1,bins_dr));

	addHistogram(new TH1F("hCombdEtaDenom","; d#eta;",40,0,4.0));
	addHistogram(new TH1F("hCombL1dEtaNum","; d#eta;",40,0,4.0));
	addHistogram(new TH1F("hCombHLTdEtaNum","; d#eta;",40,0,4.0));
	addHistogram(new TH1F("hCombL1dEtaEfficiency","L1 vs dEta Efficiency; d#eta; Efficiency",40,0,4.0));
	addHistogram(new TH1F("hCombHLTdEtaEfficiency","HLT vs dEta Efficiency; d#eta; Efficiency",40,0,4.0));

        addHistogram(new TH1F("hCombdPhiDenom","; d#phi;",16,0,3.2));
        addHistogram(new TH1F("hCombL1dPhiNum","; d#phi;",16,0,3.2));
        addHistogram(new TH1F("hCombHLTdPhiNum","; d#phi;",16,0,3.2));
	addHistogram(new TH1F("hCombL1dPhiEfficiency","L1 vs dPhi Efficiency; d#phi; Efficiency",16,0,3.2));
	addHistogram(new TH1F("hCombHLTdPhiEfficiency","HLT vs dPhi Efficiency; d#phi; Efficiency",16,0,3.2));
       }

	//Reco
        addMonGroup( new MonGroup(this, "HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/RecoEfficiency",run, ATTRIB_MANAGED, "") );

        addHistogram(new TH1F("hRecoTauPtDenom",";Reco p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hRecoTauPt1PDenom",";Reco 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hRecoTauPt3PDenom",";Reco 3 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoTauEtaDenom","; #eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hRecoTauPhiDenom","; #phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hRecoTauNTrackDenom","; Number of tracks;",10,0,10));
        addHistogram(new TH1F("hRecoTauNVtxDenom","; Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hRecoTauMuDenom","; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hRecoTauEtaVsPhiDenom","; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hRecoL1PtNum","L1 vs Reco; Reco p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoL1Pt1PNum","L1 vs Reco; Reco 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoL1Pt3PNum","L1 vs Reco; Reco 3 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoL1EtaNum","L1 vs Reco; #eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hRecoL1PhiNum","L1 vs Reco; #phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hRecoL1NTrackNum","L1 vs Reco Number of tracks;",10,0,10));
        addHistogram(new TH1F("hRecoL1NVtxNum","L1 vs Reco; Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hRecoL1MuNum","L1 vs Reco; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hRecoL1EtaVsPhiNum","L1 vs Reco; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hRecoHLTPtNum","HLT vs Reco; Reco p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoHLTPt1PNum","HLT vs Reco; Reco 1 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoHLTPt3PNum","HLT vs Reco; Reco 3 prong p_{T} [GeV];",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoHLTEtaNum","HLT vs Reco; #eta;",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hRecoHLTPhiNum","HLT vs Reco; #phi;",16,-3.2,3.2));
	addHistogram(new TH1F("hRecoHLTNTrackNum","HLT vs Reco; Number of tracks;",10,0,10));
        addHistogram(new TH1F("hRecoHLTNVtxNum","HLT vs Reco; Number of primary vertices;",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hRecoHLTMuNum","HLT vs Reco; Average interactions per bunch crossing;",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hRecoHLTEtaVsPhiNum","HLT vs Reco; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hRecoL1PtEfficiency","L1 vs Reco Efficiency; Reco p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoL1Pt1PEfficiency","L1 vs Reco Efficiency; Reco 1 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoL1Pt3PEfficiency","L1 vs Reco Efficiency; Reco 3 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoL1EtaEfficiency","L1 vs Reco Efficiency; Reco #eta; Efficiency",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hRecoL1PhiEfficiency","L1 vs Reco Efficiency; Reco #phi; Efficiency",16,-3.2,3.2));
	addHistogram(new TH1F("hRecoL1NTrackEfficiency","L1 vs Reco Efficiency; Number of tracks; Efficiency",10,0,10));
        addHistogram(new TH1F("hRecoL1NVtxEfficiency","L1 vs Reco Efficiency; Number of primary vertices; Efficiency",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hRecoL1MuEfficiency","L1 vs Reco Efficiency; Average interactions per bunch crossing; Efficiency",nbin_mu-1,bins_mu));
	addHistogram(new TH2F("hRecoL1EtaVsPhiEfficiency","L1 vs Reco in Eta-Phi; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));

        addHistogram(new TH1F("hRecoHLTPtEfficiency","HLT vs Reco Efficiency; Reco p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoHLTPt1PEfficiency","HLT vs Reco Efficiency; Reco 1 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoHLTPt3PEfficiency","HLT vs Reco Efficiency; Reco 3 prong p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
        addHistogram(new TH1F("hRecoHLTEtaEfficiency","HLT vs Reco Efficiency; Reco #eta; Efficiency",nbin_eta-1,bins_eta));
        addHistogram(new TH1F("hRecoHLTPhiEfficiency","HLT vs Reco Efficiency; Reco #phi; Efficiency",16,-3.2,3.2));
	addHistogram(new TH1F("hRecoHLTNTrackEfficiency","HLT vs Reco Efficiency; Number of tracks; Efficiency",10,0,10));
        addHistogram(new TH1F("hRecoHLTNVtxEfficiency","HLT vs Reco Efficiency; Number of primary vertices; Efficiency",nbin_nvtx-1,bins_nvtx));
        addHistogram(new TH1F("hRecoHLTMuEfficiency","HLT vs Reco Efficiency; Average interactions per bunch crossing; Efficiency",nbin_mu-1,bins_mu));
  	addHistogram(new TH2F("hRecoHLTEtaVsPhiEfficiency","HLT vs Reco in  Eta-Phi; #eta; #phi",nbin_eta-1,bins_eta,16,-3.2,3.2));
    }
    if(m_RealZtautauEff)
      {
	addMonGroup( new MonGroup(this, "HLT/TauMon/Expert/"+trigItem+"/RealZtautauEff",run, ATTRIB_MANAGED, "") );
	addHistogram(new TH1F("hRealZttPtDenom","Offline Real Tau;Offline Tau p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hRealZttL1PtNum","L1 vs Offline Real Tau; Offline Tau p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hRealZttHLTPtNum","HLT vs Offline Real Tau; Offline Tau p_{T} [GeV];",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hRealZttL1PtEfficiency","L1 vs Offline Real Tau Efficiency; Offline Tau p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
	addHistogram(new TH1F("hRealZttHLTPtEfficiency","HLT vs Offline Real Tau Efficiency; Offline Tau p_{T} [GeV]; Efficiency",nbin_pt-1,bins_pt));
      }
}

void HLTTauMonTool::bookHistogramsAllItem(){

    addMonGroup(new MonGroup(this,"HLT/TauMon/Expert",run));
    addHistogram(new TH1F("hL1TBPCounts","L1 Before Prescale counts; Chains;Nevents",m_trigItems.size(),0,m_trigItems.size()));
    addHistogram(new TH1F("hL1Counts","L1 counts; Chains;Nevents",m_trigItems.size(),0,m_trigItems.size()));
    addHistogram(new TH1F("hL1CountsDebug","L1 counts; Chains;Nevents",m_trigItems.size(),0,m_trigItems.size()));
    addHistogram(new TH1F("hHLTCounts","HLT counts; Chains;Nevents",m_trigItems.size(),0,m_trigItems.size()));
    addHistogram(new TH1F("hHLTCountsDebug","HLT counts; Chains;Nevents",m_trigItems.size(),0,m_trigItems.size()));
    for(unsigned int i=0;i<m_trigItems.size(); ++i){
      hist("hL1TBPCounts")->GetXaxis()->SetBinLabel(i+1,m_trigItems.at(i).c_str());
      hist("hL1Counts")->GetXaxis()->SetBinLabel(i+1,m_trigItems.at(i).c_str());
      hist("hL1CountsDebug")->GetXaxis()->SetBinLabel(i+1,m_trigItems.at(i).c_str());
      hist("hHLTCounts")->GetXaxis()->SetBinLabel(i+1,m_trigItems.at(i).c_str());
      hist("hHLTCountsDebug")->GetXaxis()->SetBinLabel(i+1,m_trigItems.at(i).c_str());
    } 

    if(m_doTestTracking){
      addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/FTF_track_comparison",run));
      addHistogram(new TH1F("hFTFnTrack_1step","FTF number of tracks;number of tracks;Nevents",10,0,10));
      addHistogram(new TH1F("hFTFnTrack_2steps","FTF number of tracks;number of tracks;Nevents",10,0,10));
      addHistogram(new TH1F("hFTFnWideTrack_1step","FTF number of tracks;number of tracks;Nevents",10,0,10));
      addHistogram(new TH1F("hFTFnWideTrack_2steps","FTF number of tracks;number of tracks;Nevents",10,0,10));
    }

    std::vector<string> lowest_names;
    lowest_names.push_back("lowest_singletau");
//    lowest_names.push_back("lowest_ditau");
//    lowest_names.push_back("lowest_etau");
//    lowest_names.push_back("lowest_mutau");
//    lowest_names.push_back("lowest_mettau");
//    lowest_names.push_back("cosmic_chain");

    for(unsigned int i=0;i<lowest_names.size();++i){

      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/L1RoI",run));
      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/PreselectionTau",run));
      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau",run));
//      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/1p_nonCorrected",run));
//      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/mp_nonCorrected",run));
//      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/1p_Corrected",run));
//      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/mp_Corrected",run));
//      if(m_turnOnCurves) addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/TurnOnCurves/RecoEfficiency",run));
      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline",run));
//      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected",run));
//      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected",run));
      addMonGroup(new MonGroup(this,"HLT/TauMon/Shifter/"+lowest_names.at(i)+"/TurnOnCurves",run));
    }

    if(m_emulation){   
     addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/Emulation",run));
     addHistogram(new TH1F("hL1Emulation","; mismatched events",m_emulation_l1_tau.size(),-0.5,m_emulation_l1_tau.size()-0.5));
     addHistogram(new TH1F("hHLTEmulation","; mismatched events",m_emulation_hlt_tau.size(),-0.5,m_emulation_hlt_tau.size()-0.5));
     for(unsigned int i=0;i<m_emulation_l1_tau.size(); ++i){
       hist("hL1Emulation")->GetXaxis()->SetBinLabel(i+1,m_emulation_l1_tau.at(i).c_str());
     }
     for(unsigned int i=0;i<m_emulation_hlt_tau.size(); ++i){
       hist("hHLTEmulation")->GetXaxis()->SetBinLabel(i+1,m_emulation_hlt_tau.at(i).c_str());
     }
    }

}

void HLTTauMonTool::cloneHistogram(const std::string name, const std::string folder){

  std::string name_clone( hist(name,folder)->GetName() );
  name_clone += "_shifter";
  addHistogram( (TH1F*)hist(name,folder)->Clone( name_clone.c_str() ) );

}

void HLTTauMonTool::cloneHistogram2(const std::string name, const std::string folder){

  std::string name_clone( hist2(name,folder)->GetName() );
  name_clone += "_shifter";
  addHistogram( (TH2F*)hist2(name,folder)->Clone( name_clone.c_str() ) );

}



StatusCode HLTTauMonTool::fillHistogramsForItem(const std::string & trigItem){
    
    ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem " << trigItem);
    
    StatusCode sc;
    
    std::string trig_item_EF = "HLT_"+trigItem;
    std::string trig_item_L1(LowerChain( trig_item_EF ) );
    if(trig_item_L1=="") {ATH_MSG_WARNING("L1 chain for "<< trig_item_EF << " not found");}
    
    if(trigItem=="Dump"){

	const xAOD::EmTauRoIContainer* l1Tau_cont = 0;

        if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){ // retrieve arguments: container type, container key
          ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
          return StatusCode::FAILURE;
        } else{
          ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
        }

        xAOD::EmTauRoIContainer::const_iterator itEMTau = l1Tau_cont->begin();
        xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();

        for(; itEMTau!=itEMTau_e; ++itEMTau){
          if(!Selection(*itEMTau)) continue;   
          setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1RoI");
	  sc = fillL1Tau(*itEMTau);	
	  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1VsOffline");
          sc = fillL1TauVsOffline(*itEMTau);

	  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to retrieve L1 tau. Exiting!"); return StatusCode::FAILURE;}
	}
       
      
        const xAOD::TauJetContainer * tauJetCont = 0;
        
        if( evtStore()->retrieve(tauJetCont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged").isFailure() ){
            ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TauJetContainer_TrigTauRecMerged container. Exiting!");
            return StatusCode::FAILURE;
        }
        
        xAOD::TauJetContainer::const_iterator CI = tauJetCont->begin(),tauJetCont_end = tauJetCont->end();
         
        for(; CI!=tauJetCont_end; ++CI){
          if(!Selection(*CI)) continue;
	  sc = fillEFTau(*CI, trigItem, "basicVars");
	  sc = fillEFTau(*CI, trigItem, "1p_NonCorr");
	  sc = fillEFTau(*CI, trigItem, "mp_NonCorr");
	  sc = fillEFTau(*CI, trigItem, "1p_muCorr");
	  sc = fillEFTau(*CI, trigItem, "mp_muCorr");
	  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill BDT input histograms for fillEFTau(). Exiting!"); return StatusCode::FAILURE;}
	  sc = fillEFTauVsOffline(*CI, trigItem, "basicVars");
	  sc = fillEFTauVsOffline(*CI, trigItem, "1p_NonCorr");
	  sc = fillEFTauVsOffline(*CI, trigItem, "mp_NonCorr");
	  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill histograms for fillEFTauVsOffline(). Exiting!"); return StatusCode::FAILURE;}
	}
        
    } else {
        
      // L1 Histograms ...
     if (getTDT()->isPassed(trig_item_L1)) {
           
       ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem passed " << trig_item_L1);
       
       Trig::FeatureContainer f = ( getTDT()->features(trig_item_L1,m_L1flag) );  
       Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
       if(comb==combEnd){
         ATH_MSG_DEBUG("No features for chain " << trig_item_L1);
         setCurrentMonGroup("HLT/TauMon/Expert");
         hist("hL1CountsDebug")->Fill(trigItem.c_str(),1.);
       }
       for(;comb!=combEnd;++comb){

         const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb->get<TrigRoiDescriptor>("initialRoI",m_L1flag);
         std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin(), roi_e = vec_roi.end();

         const xAOD::EmTauRoIContainer* l1Tau_cont = 0;
         if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){ // retrieve arguments: container type, container key
           ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
           //return StatusCode::FAILURE;
         } else {
           ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
         }
         xAOD::EmTauRoIContainer::const_iterator itEMTau;
         xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();

         for(; roi != roi_e; ++roi)
           if(roi->cptr()){
             for(itEMTau = l1Tau_cont->begin(); itEMTau!=itEMTau_e; ++itEMTau){
     	       if(roi->cptr()->roiWord()!=(*itEMTau)->roiWord()) continue;
     	       if(!Selection(*itEMTau)) continue;
               setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1RoI");
               sc = fillL1Tau(*itEMTau);
               if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill L1RoI histo. Exiting!"); return StatusCode::FAILURE;}
               setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1VsOffline");
               sc = fillL1TauVsOffline(*itEMTau);
               if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill L1VsOffline histo. Exiting!"); return StatusCode::FAILURE;}
             }
           }
        }
     }// end L1 histos

//     // Testing tau+met chains...
//     if(getTDT()->isPassed("HLT_tau35_medium1_tracktwo_xe70_L1XE45")) {
//        ATH_MSG_WARNING ("Event passed HLT_tau35_medium1_tracktwo_xe70_L1XE45");
//        Trig::FeatureContainer f = ( getTDT()->features("HLT_tau35_medium1_tracktwo_xe70_L1XE45") );
//        Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
//        if(comb==combEnd) ATH_MSG_WARNING ("But NO combination of features associated");
//        if(comb!=combEnd) ATH_MSG_WARNING ("WITH combination of features associated");
//        const std::vector< Trig::Feature<xAOD::TauJetContainer> > vec_HLTtau = comb->get<xAOD::TauJetContainer>();
//        std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator CI = vec_HLTtau.begin(), CI_e = vec_HLTtau.end();
//        if(CI==CI_e) ATH_MSG_WARNING("TrigTauMerged TauJet container EMPTY in HLT_tau35_medium1_tracktwo_xe70_L1XE45");
//     }
     // HLT histsos ...
     if (getTDT()->isPassed(trig_item_EF)) {

       ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem passed " << trig_item_EF);

       Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,m_Preselectionflag) );
       Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
       if(comb==combEnd){
         ATH_MSG_DEBUG("No features for chain " << trig_item_EF);
         setCurrentMonGroup("HLT/TauMon/Expert");
       hist("hHLTCountsDebug")->Fill(trigItem.c_str(),1.);
         //return StatusCode::FAILURE;
       }
       for(;comb!=combEnd;++comb){

         const std::vector< Trig::Feature<xAOD::TauJetContainer> >  vec_preseltau = comb->get<xAOD::TauJetContainer>("TrigTauRecPreselection",m_Preselectionflag);
         std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator preselCI = vec_preseltau.begin(), preselCI_e = vec_preseltau.end();
         if(preselCI==preselCI_e) ATH_MSG_DEBUG("TrigTauPreselection TauJet container EMPTY in " << trig_item_EF);
         if(preselCI!=preselCI_e){
           ATH_MSG_DEBUG("Item "<< trigItem << ": " << vec_preseltau.size() << " " << preselCI->label() << " containers");  
           for(; preselCI != preselCI_e; ++preselCI)
             if(preselCI->cptr()){
             if(preselCI->cptr()->size()==0) ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << preselCI->cptr()->size() << " TauJets"); 
             ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << preselCI->cptr()->size() << " TauJets");
      	        xAOD::TauJetContainer::const_iterator tauItr = preselCI->cptr()->begin();
               xAOD::TauJetContainer::const_iterator tauEnd = preselCI->cptr()->end();
                    
               for(; tauItr != tauEnd; ++tauItr) {
     	  if(!Selection(*tauItr)) continue;
      	  	  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/PreselectionTau");
                 if(*tauItr) sc = fillPreselTau(*tauItr);
      	          if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill PreselTau histo. Exiting!"); return StatusCode::FAILURE;}	
      		  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/PreselectionVsOffline");
     	  if(*tauItr) sc = fillPreselTauVsOffline(*tauItr);
                 if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill PreselectionVsOffline histo. Exiting!"); return StatusCode::FAILURE;}
                 }
               }
         }              

         const std::vector< Trig::Feature<xAOD::TauJetContainer> > vec_HLTtau = comb->get<xAOD::TauJetContainer>();
         std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator CI = vec_HLTtau.begin(), CI_e = vec_HLTtau.end();
         if(CI==CI_e) ATH_MSG_DEBUG("TrigTauMerged TauJet container EMPTY in " << trig_item_EF); 
         ATH_MSG_DEBUG("Item "<< trigItem << ": " << vec_HLTtau.size() << " " << CI->label() << " containers");
         for(; CI != CI_e; ++CI){
           if(CI->cptr()){
             if(CI->cptr()->size()==0) ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << CI->cptr()->size() << " TauJets");
             ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << CI->cptr()->size() << " TauJets");
             xAOD::TauJetContainer::const_iterator tauItr = CI->cptr()->begin();
             xAOD::TauJetContainer::const_iterator tauEnd = CI->cptr()->end();

             for(; tauItr != tauEnd; ++tauItr) {
     	if(!Selection(*tauItr)) continue;
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "basicVars");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "1p_NonCorr");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "mp_NonCorr");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "1p_muCorr");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "mp_muCorr");
     	if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill BDT input histograms for fillEFTau(). Exiting!"); return StatusCode::FAILURE;}
     	if(m_truth) if(*tauItr) sc = fillEFTauVsTruth(*tauItr, trigItem);
     	if(*tauItr) sc = fillEFTauVsOffline(*tauItr, trigItem, "basicVars");
     	if(*tauItr) sc = fillEFTauVsOffline(*tauItr, trigItem, "1p_NonCorr");
     	if(*tauItr) sc = fillEFTauVsOffline(*tauItr, trigItem, "mp_NonCorr");
     	if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill histograms for fillEFTauVsOffline(). Exiting!"); return StatusCode::FAILURE;}
           }
         }
       }
     }
     }
   }
    
    if( m_turnOnCurves) {
      if(m_truth) sc = TruthTauEfficiency(trigItem, "Truth");
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill Truth eff curves. Exiting!"); return StatusCode::FAILURE;}
      if(m_truth) sc = TruthTauEfficiency(trigItem, "Truth+Reco");
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill Truth+Reco eff curves. Exiting!"); return StatusCode::FAILURE;}
      sc = TauEfficiency(trigItem,m_turnOnCurvesDenom);
      //sc = RecoTauEfficiency(trigItem);
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill Reco eff curves. Exiting!"); return StatusCode::FAILURE;}
      if(m_truth) sc = TauEfficiencyCombo(trigItem);
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill combo eff curves. Exiting!"); return StatusCode::FAILURE;}
    }  
    
    if(m_RealZtautauEff)
      {
	sc = RealZTauTauEfficiency(trigItem);
	if(sc.isFailure()){ ATH_MSG_WARNING("Failed RealZTauTauEfficiency(). Exiting!"); return StatusCode::FAILURE;}
      }   
    
    if (!sc.isSuccess()) {
        ATH_MSG_WARNING("Could not fill histograms for given item.");
    }
    
    return sc;
    
}


StatusCode HLTTauMonTool::fillL1Tau(const xAOD::EmTauRoI * aL1Tau){

  ATH_MSG_DEBUG("HLTTauMonTool::fillL1Tau");

  if(!aL1Tau){
    ATH_MSG_WARNING("Invalid L1Tau pointer. Exiting");
    return StatusCode::FAILURE;
  }

  hist("hL1RoIEta")->Fill(aL1Tau->eta());
  hist("hL1RoIPhi")->Fill(aL1Tau->phi());
  hist2("hL1EtaVsPhi")->Fill(aL1Tau->eta(),aL1Tau->phi());
  hist("hL1RoIeT")->Fill(aL1Tau->eT()/CLHEP::GeV);

  uint8_t isoBit = aL1Tau->isol();
  if(isoBit/128) hist("hL1RoIisol")->Fill(8);
  if((isoBit/64)%2) hist("hL1RoIisol")->Fill(7);
  if((isoBit/32)%2) hist("hL1RoIisol")->Fill(6);
  if((isoBit/16)%2) hist("hL1RoIisol")->Fill(5);
  if((isoBit/8)%2) hist("hL1RoIisol")->Fill(4);
  if((isoBit/4)%2) hist("hL1RoIisol")->Fill(3);
  if((isoBit/2)%2) hist("hL1RoIisol")->Fill(2);
  if((isoBit/1)%2) hist("hL1RoIisol")->Fill(1);
  hist("hL1RoITauClus")->Fill(aL1Tau->tauClus()/CLHEP::GeV);
  hist("hL1RoIEMIso")->Fill(aL1Tau->emIsol()/CLHEP::GeV);
  hist("hL1RoIHadCore")->Fill(aL1Tau->hadCore()/CLHEP::GeV);
  hist("hL1RoIHadIsol")->Fill(aL1Tau->hadIsol()/CLHEP::GeV);
  hist2("hL1RoITauClusEMIso")->Fill(aL1Tau->tauClus()/CLHEP::GeV,aL1Tau->emIsol()/CLHEP::GeV);

  return StatusCode::SUCCESS;

}


StatusCode HLTTauMonTool::fillPreselTau(const xAOD::TauJet *aEFTau){

    ATH_MSG_DEBUG ("HLTTauMonTool::fillPreselTau");

    if(!aEFTau) {
        ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
        return StatusCode::FAILURE;
    }

    int EFnTrack = aEFTau->nTracks();
    hist("hEFEt")->Fill(aEFTau->pt()/CLHEP::GeV);
    hist("hFTFnTrack")->Fill(EFnTrack);
    hist("hEta")->Fill(aEFTau->eta());
    hist("hPhi")->Fill(aEFTau->phi());
    float dRMax=-1; 
    aEFTau->detail(xAOD::TauJetParameters::dRmax, dRMax);
    hist("hdRmax")->Fill(dRMax); 
    hist2("hEFEtaVsPhi")->Fill(aEFTau->eta(),aEFTau->phi());
    hist2("hEtVsEta")->Fill(aEFTau->eta(),aEFTau->pt()/CLHEP::GeV);
    hist2("hEtVsPhi")->Fill(aEFTau->phi(),aEFTau->pt()/CLHEP::GeV);
    
    hist("hFTFnWideTrack")->Fill(aEFTau->nWideTracks());

    return StatusCode::SUCCESS;

}

StatusCode HLTTauMonTool::fillEFTau(const xAOD::TauJet *aEFTau, const std::string & trigItem, const std::string & BDTinput_type){
    
 ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTau");
  
  if(!aEFTau)
    {
      ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
      return StatusCode::FAILURE;
    }
 
  float etEM = 0;
  float etHad = 0;
  float emRadius = 0;
  float hadRadius = 0;
  float isoFrac = 0;
  float EMFrac = -1.0;
  float PSSFraction = 0;
  float BDTJetScore = 0;
  float innerTrkAvgDist = 0;
  float etOverPtLeadTrk = 0;
  float ipSigLeadTrk = 0;
  float SumPtTrkFrac = 0;
  float ChPiEMEOverCaloEME = 0;
  float EMPOverTrkSysP = 0;
  float centFrac = 0;
  float trFlightPathSig = 0;
  float massTrkSys = 0;
  float dRmax = 0;
  float mEflowApprox = 0;
  float ptRatioEflowApprox = 0;

  float innerTrkAvgDistCorr = 0;
  float etOverPtLeadTrkCorr = 0;
  float ipSigLeadTrkCorr = 0;
  float SumPtTrkFracCorr = 0;
  float ChPiEMEOverCaloEMECorr = 0;
  float EMPOverTrkSysPCorr = 0;
  float centFracCorr = 0;
  float trFlightPathSigCorr = 0;
  float massTrkSysCorr = 0;
  float dRmaxCorr = 0;
  float mEflowApproxCorr = 0;
  float ptRatioEflowApproxCorr = 0;
  float mu = 0;

  int EFnTrack = aEFTau->nTracks();
  bool is1P(false), isMP(false);   
  if(EFnTrack==1) is1P = true;
  if(EFnTrack>1) isMP = true;
  //Pileup
  mu = Pileup();
  
  if(BDTinput_type == "basicVars")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau");
      hist("hEFEt")->Fill(aEFTau->pt()/CLHEP::GeV);
      hist("hEFEta")->Fill(aEFTau->eta());
      hist("hEFPhi")->Fill(aEFTau->phi());
      hist("hEFnTrack")->Fill(EFnTrack);
      hist("hEFnWideTrack")->Fill(aEFTau->nWideTracks());
      hist2("hEFEtaVsPhi")->Fill(aEFTau->eta(),aEFTau->phi());
      hist2("hEFEtVsPhi")->Fill(aEFTau->phi(),aEFTau->pt()/CLHEP::GeV);
      hist2("hEFEtVsEta")->Fill(aEFTau->eta(),aEFTau->pt()/CLHEP::GeV);
      if(aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEM) && aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHad))
	{
	  hist("hEFEtRaw")->Fill( (etEM + etHad)/CLHEP::GeV);
	  EMFrac = etEM / (etEM + etHad);
	  hist("hEFEMFraction")->Fill(EMFrac);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::EMRadius, emRadius)) hist("hEFEMRadius")->Fill(emRadius);
      if(aEFTau->detail(xAOD::TauJetParameters::hadRadius, hadRadius)) hist("hEFHADRadius")->Fill(hadRadius);  
      if(aEFTau->detail(xAOD::TauJetParameters::isolFrac, isoFrac)) hist("hEFIsoFrac")->Fill(isoFrac);
      if(aEFTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction)) hist("hEFPSSFraction")->Fill(PSSFraction);
      if( BDTJetScore = aEFTau->discriminant(xAOD::TauJetParameters::TauID::BDTJetScore) )
	{
	  if(is1P) hist("hScore1p")->Fill(BDTJetScore);
	  if(isMP) hist("hScoremp")->Fill(BDTJetScore);
	}
    }
  else if(BDTinput_type == "1p_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/1p_nonCorrected");
      if(is1P)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist)) 
	    {
	      hist("hEFinnerTrkAvgDist1PNCorr")->Fill(innerTrkAvgDist);
	      profile("hEFinnerTrkAvgDist1PNCmu")->Fill(mu, innerTrkAvgDist);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk))
	    {
	      hist("hEFetOverPtLeadTrk1PNCorr")->Fill(etOverPtLeadTrk);
	      profile("hEFetOverPtLeadTrk1PNCmu")->Fill(mu, etOverPtLeadTrk);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk))
	    {
	      hist("hEFipSigLeadTrk1PNCorr")->Fill(ipSigLeadTrk);
	      profile("hEFipSigLeadTrk1PNCmu")->Fill(mu, ipSigLeadTrk);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFrac))
	    {
	      hist("hEFSumPtTrkFrac1PNCorr")->Fill(SumPtTrkFrac);
	      profile("hEFSumPtTrkFrac1PNCmu")->Fill(mu, SumPtTrkFrac);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME))
	    {
	      hist("hEFChPiEMEOverCaloEME1PNCorr")->Fill(ChPiEMEOverCaloEME);
	      profile("hEFChPiEMEOverCaloEME1PNCmu")->Fill(mu, ChPiEMEOverCaloEME);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP))
	    {
	      hist("hEFEMPOverTrkSysP1PNCorr")->Fill(EMPOverTrkSysP);
	      profile("hEFEMPOverTrkSysP1PNCmu")->Fill(mu, EMPOverTrkSysP);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac))
	    {
	      hist("hEFcentFrac1PNCorr")->Fill(centFrac);
	      profile("hEFcentFrac1PNCmu")->Fill(mu, centFrac);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox)) 
	    {
	      hist("hEFptRatioEflowApprox1PNCorr")->Fill(ptRatioEflowApprox);
	      profile("hEFptRatioEflowApprox1PNCmu")->Fill(mu, ptRatioEflowApprox);
	    }
        }
    }
  else if(BDTinput_type == "mp_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/mp_nonCorrected");
      if(isMP)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist))
	    {
	      hist("hEFinnerTrkAvgDistMPNCorr")->Fill(innerTrkAvgDist);
	      profile("hEFinnerTrkAvgDistMPNCmu")->Fill(mu, innerTrkAvgDist);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk))
	    {
	      hist("hEFetOverPtLeadTrkMPNCorr")->Fill(etOverPtLeadTrk);
	      profile("hEFetOverPtLeadTrkMPNCmu")->Fill(mu, etOverPtLeadTrk);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME))
	    {
	      hist("hEFChPiEMEOverCaloEMEMPNCorr")->Fill(ChPiEMEOverCaloEME);
	      profile("hEFChPiEMEOverCaloEMEMPNCmu")->Fill(mu, ChPiEMEOverCaloEME);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP))
	    {
	      hist("hEFEMPOverTrkSysPMPNCorr")->Fill(EMPOverTrkSysP);
	      profile("hEFEMPOverTrkSysPMPNCmu")->Fill(mu, EMPOverTrkSysP);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac))
	    {
	      hist("hEFcentFracMPNCorr")->Fill(centFrac);
	      profile("hEFcentFracMPNCmu")->Fill(mu, centFrac);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox))
	    {
	      hist("hEFptRatioEflowApproxMPNCorr")->Fill(ptRatioEflowApprox);
	      profile("hEFptRatioEflowApproxMPNCmu")->Fill(mu, ptRatioEflowApprox);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::dRmax, dRmax))
	    {
	      hist("hEFdRmaxMPNCorr")->Fill(dRmax);      
	      profile("hEFdRmaxMPNCmu")->Fill(mu, dRmax);      
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys))
	    {
	      hist("hEFmassTrkSysMPNCorr")->Fill(massTrkSys/CLHEP::GeV);
	      profile("hEFmassTrkSysMPNCmu")->Fill(mu, massTrkSys/CLHEP::GeV);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig))
	    {
	      hist("hEFtrFlightPathSigMPNCorr")->Fill(trFlightPathSig);
	      profile("hEFtrFlightPathSigMPNCmu")->Fill(mu, trFlightPathSig);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::mEflowApprox, mEflowApprox))
	    {
	      hist("hEFmEflowApproxMPNCorr")->Fill(mEflowApprox);
	      profile("hEFmEflowApproxMPNCmu")->Fill(mu, mEflowApprox);
	    }
        }
    }
  else if(BDTinput_type == "1p_muCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/1p_Corrected");
      if(is1P)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDistCorrected, innerTrkAvgDistCorr)) 
	    {
	      hist("hEFinnerTrkAvgDist1PCorr")->Fill(innerTrkAvgDistCorr);
	      profile("hEFinnerTrkAvgDist1PCmu")->Fill(mu, innerTrkAvgDistCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrkCorrected, etOverPtLeadTrkCorr))
	    {
	      hist("hEFetOverPtLeadTrk1PCorr")->Fill(etOverPtLeadTrkCorr);
	      profile("hEFetOverPtLeadTrk1PCmu")->Fill(mu, etOverPtLeadTrkCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrkCorrected, ipSigLeadTrkCorr))
	    {
	      hist("hEFipSigLeadTrk1PCorr")->Fill(ipSigLeadTrkCorr);
	      profile("hEFipSigLeadTrk1PCmu")->Fill(mu, ipSigLeadTrkCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::SumPtTrkFracCorrected, SumPtTrkFracCorr)) 
	    {
	      hist("hEFSumPtTrkFrac1PCorr")->Fill(SumPtTrkFracCorr);
	      profile("hEFSumPtTrkFrac1PCmu")->Fill(mu, SumPtTrkFracCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEMECorrected, ChPiEMEOverCaloEMECorr))
	    {
	      hist("hEFChPiEMEOverCaloEME1PCorr")->Fill(ChPiEMEOverCaloEMECorr);
	      profile("hEFChPiEMEOverCaloEME1PCmu")->Fill(mu, ChPiEMEOverCaloEMECorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysPCorrected, EMPOverTrkSysPCorr)) 
	    {
	      hist("hEFEMPOverTrkSysP1PCorr")->Fill(EMPOverTrkSysPCorr);
	      profile("hEFEMPOverTrkSysP1PCmu")->Fill(mu, EMPOverTrkSysPCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::centFracCorrected, centFracCorr))
	    {
	      hist("hEFcentFrac1PCorr")->Fill(centFracCorr);
	      profile("hEFcentFrac1PCmu")->Fill(mu, centFracCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApproxCorrected, ptRatioEflowApproxCorr))
	    {
	      hist("hEFptRatioEflowApprox1PCorr")->Fill(ptRatioEflowApproxCorr);
	      profile("hEFptRatioEflowApprox1PCmu")->Fill(mu, ptRatioEflowApproxCorr);
	    }
        }
    }
  else if(BDTinput_type == "mp_muCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/mp_Corrected");
      if(isMP)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDistCorrected, innerTrkAvgDistCorr)) 
	    {
	      hist("hEFinnerTrkAvgDistMPCorr")->Fill(innerTrkAvgDistCorr);
	      profile("hEFinnerTrkAvgDistMPCmu")->Fill(mu, innerTrkAvgDistCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrkCorrected, etOverPtLeadTrkCorr))
	    {
	      hist("hEFetOverPtLeadTrkMPCorr")->Fill(etOverPtLeadTrkCorr);
	      profile("hEFetOverPtLeadTrkMPCmu")->Fill(mu, etOverPtLeadTrkCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEMECorrected, ChPiEMEOverCaloEMECorr)) 
	    {
	      hist("hEFChPiEMEOverCaloEMEMPCorr")->Fill(ChPiEMEOverCaloEMECorr);
	      profile("hEFChPiEMEOverCaloEMEMPCmu")->Fill(mu, ChPiEMEOverCaloEMECorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysPCorrected, EMPOverTrkSysPCorr)) 
	    {
	      hist("hEFEMPOverTrkSysPMPCorr")->Fill(EMPOverTrkSysPCorr); 	   
	      profile("hEFEMPOverTrkSysPMPCmu")->Fill(mu, EMPOverTrkSysPCorr); 	   
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::centFracCorrected, centFracCorr))
	    {
	      hist("hEFcentFracMPCorr")->Fill(centFracCorr);
	      profile("hEFcentFracMPCmu")->Fill(mu, centFracCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApproxCorrected, ptRatioEflowApproxCorr))
	    {
	      hist("hEFptRatioEflowApproxMPCorr")->Fill(ptRatioEflowApproxCorr);
	      profile("hEFptRatioEflowApproxMPCmu")->Fill(mu, ptRatioEflowApproxCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::dRmaxCorrected, dRmaxCorr))
	    {
	      hist("hEFdRmaxMPCorr")->Fill(dRmaxCorr);      
	      profile("hEFdRmaxMPCmu")->Fill(mu, dRmaxCorr);      
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::massTrkSysCorrected, massTrkSysCorr))
	    {
	      hist("hEFmassTrkSysMPCorr")->Fill(massTrkSysCorr/CLHEP::GeV);
	      profile("hEFmassTrkSysMPCmu")->Fill(mu, massTrkSysCorr/CLHEP::GeV);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::trFlightPathSigCorrected, trFlightPathSigCorr))
	    {
	      hist("hEFtrFlightPathSigMPCorr")->Fill(trFlightPathSigCorr);
	      profile("hEFtrFlightPathSigMPCmu")->Fill(mu, trFlightPathSigCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::mEflowApproxCorrected, mEflowApproxCorr))
	    {
	      hist("hEFmEflowApproxMPCorr")->Fill(mEflowApproxCorr);
	      profile("hEFmEflowApproxMPCmu")->Fill(mu, mEflowApproxCorr);
	    }
        }
    }
  else
    {
      ATH_MSG_WARNING("Wrong BDT input type Failed to fill the histograms.Check fillEFTau(). Exiting.");
      return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::fillL1TauVsOffline(const xAOD::EmTauRoI *aL1Tau){

  ATH_MSG_DEBUG ("HLTTauMonTool::fillL1TauVsOffline");
    
  if(!aL1Tau) {
        ATH_MSG_WARNING("Invalid EmTauRoI pointer. Exiting");
        return StatusCode::FAILURE;
  }

  const xAOD::TauJet *aOfflineTau = 0;
  float tmpR = 0.3;
  const xAOD::TauJetContainer* tauCont = 0;
    
  if( m_storeGate->retrieve(tauCont, "TauJets").isFailure()  ){
        ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
        return StatusCode::FAILURE;
  }
    
  xAOD::TauJetContainer::const_iterator CI,offlinetau_end = tauCont->end();
  for(CI=tauCont->begin();CI!=offlinetau_end; ++CI){
        if((*CI)){
    
           TLorentzVector TauTLV = (*CI)->p4();
           double eta_Tau = TauTLV.Eta();
           if(fabs(eta_Tau) > 2.47) continue;
           double pt_Tau = TauTLV.Pt();
           if(pt_Tau<m_effOffTauPtCut) continue;
           int ntrack_TAU = (*CI)->nTracks();
           if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
           bool good_tau = (*CI)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
           bool not_a_electron = !( (*CI)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
           bool not_a_muon = !( (*CI)->isTau(xAOD::TauJetParameters::MuonVeto) );
           bool best_tau = good_tau && not_a_electron && not_a_muon;
           if(!best_tau) continue;
	   float eta = aL1Tau->eta();
	   float phi = aL1Tau->phi();
           double dR = deltaR(eta,TauTLV.Eta(),phi,TauTLV.Phi());
              if(dR < tmpR){
                tmpR = dR;
                aOfflineTau = (*CI);
            }
        }
  }

  if(aOfflineTau){
        ATH_MSG_DEBUG("Found an offline TauJet matching L1 EmTauRoI, dR = "<<tmpR);
  }else {
        ATH_MSG_DEBUG("Not found an offline TauJet matching EF tauJet");
        return StatusCode::SUCCESS;
  }

  // plots of L1 vs Offline ....

  return StatusCode::SUCCESS;

}


StatusCode HLTTauMonTool::fillPreselTauVsOffline(const xAOD::TauJet *aEFTau){
    
    ATH_MSG_DEBUG ("HLTTauMonTool::fillPreselTauVsOffline");
    
    if(!aEFTau) {
        ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
        return StatusCode::FAILURE;
    }
    
    const xAOD::TauJet *aOfflineTau = 0;
    float tmpR = 0.2;
    const xAOD::TauJetContainer* tauCont = 0;
    
    if( m_storeGate->retrieve(tauCont, "TauJets").isFailure()  ){
        ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
        return StatusCode::FAILURE;
    }
    
    xAOD::TauJetContainer::const_iterator CI,offlinetau_end = tauCont->end();
    for(CI=tauCont->begin();CI!=offlinetau_end; ++CI){
        if((*CI)){
            
            TLorentzVector TauTLV = (*CI)->p4();
            double eta_Tau = TauTLV.Eta();
            if(fabs(eta_Tau) > 2.47) continue;
            double pt_Tau = TauTLV.Pt();
            if(pt_Tau<m_effOffTauPtCut) continue;
            int ntrack_TAU = (*CI)->nTracks();
            if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
            bool good_tau = (*CI)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
            bool not_a_electron = !( (*CI)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
            bool not_a_muon = !( (*CI)->isTau(xAOD::TauJetParameters::MuonVeto) );
            bool best_tau = good_tau && not_a_electron && not_a_muon;
            if(!best_tau) continue;
            double dR = TauTLV.DeltaR(aEFTau->p4());
            if(dR < tmpR){
                tmpR = dR;
                aOfflineTau = (*CI);
            }
        }
    }
    
    if(aOfflineTau){
        ATH_MSG_DEBUG("Found an offline TauJet matching EF tauJet, dR = "<<tmpR);
    }else {
        ATH_MSG_DEBUG("Not found an offline TauJet matching EF tauJet");
        return StatusCode::SUCCESS;
    }

    int EFnTrack = aEFTau->nTracks(); 
    hist2("hPreselvsOffnTrks")->Fill(aOfflineTau->nTracks(), EFnTrack);
    hist2("hPreselvsOffnWideTrks")->Fill(aOfflineTau->nWideTracks(), aEFTau->nWideTracks());
    FillRelDiffHist(hist("hEFEtRatio"), aOfflineTau->pt(), aEFTau->pt(), 0, 1);
    FillRelDiffHist(hist("hEtaRatio"), aOfflineTau->eta(), aEFTau->eta(), 0, 2);
    hist("hPhiRatio")->Fill( deltaPhi(aOfflineTau->phi(), aEFTau->phi()));

    return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::fillEFTauVsTruth(const xAOD::TauJet *aEFTau, const std::string & trigItem)
{
  ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTauVsTruth");
  if(!aEFTau)
    {
      ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
      return StatusCode::FAILURE;
    }
  
  float mu(Pileup());
  float truthPt(-1.),truthPhi(-999.),truthEta(-999.);
  float tmpdR(0.2);
  //Truth Tau matching to EF(HLT Tau)
  TLorentzVector TruthTauTLV(0.,0.,0.,0.);
  const xAOD::TruthParticleContainer* truth_cont = 0;
  if( evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
    return StatusCode::FAILURE;
  }
  xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
  for (truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
    { 
      if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) // || (*truthItr)->status() == 10902))
        {
	  examineTruthTau(**truthItr);
	  float pt  = (*truthItr)->auxdata<double>("pt_vis");
	  float eta = (*truthItr)->auxdata<double>("eta_vis");
	  float phi = (*truthItr)->auxdata<double>("phi_vis");
	  float m   = (*truthItr)->auxdata<double>("m_vis");
	 
          TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
          double dR = TruthTauTLV.DeltaR(aEFTau->p4());
          if(dR < tmpdR)
            {
              tmpdR = dR;
              truthPt = TruthTauTLV.Pt();
       	      truthEta = TruthTauTLV.Eta();
	      truthPhi = TruthTauTLV.Phi();
            }
        }
     }
  if(truthPt>0.){
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsTruth");
      FillRelDiffProfile<float>(profile("hEtRatiovspt"), truthPt, aEFTau->pt(), truthPt/1000., 0, 1); 
      FillRelDiffProfile<float>(profile("hEtRatiovsmu"), truthPt, aEFTau->pt(), mu, 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovsphi"), truthPt, aEFTau->pt(), truthPhi, 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovseta"), truthPt, aEFTau->pt(), truthEta, 0, 1);

  }

  return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::fillEFTauVsOffline(const xAOD::TauJet *aEFTau, const std::string & trigItem, const std::string & BDTinput_type)
{
  ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTauVsOffline");
  if(!aEFTau)
    {
      ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
      return StatusCode::FAILURE;
    }
 
  float innerTrkAvgDist = 0;
  float etOverPtLeadTrk = 0;
  float ipSigLeadTrk = 0;
  float SumPtTrkFrac = 0;
  float ChPiEMEOverCaloEME = 0;
  float EMPOverTrkSysP = 0;
  float centFrac = 0;
  float ptRatioEflowApprox = 0;
  float dRMax = 0;
  float topoInvMass = 0;
  float trFlightPathSig = 0;
  float massTrkSys = 0;
  float mEflowApprox = 0;

  float innerTrkAvgDistOff = 0;
  float etOverPtLeadTrkOff = 0;
  float ipSigLeadTrkOff = 0;
  float SumPtTrkFracOff = 0;
  float ChPiEMEOverCaloEMEOff = 0;
  float EMPOverTrkSysPOff = 0;
  float centFracOff = 0;
  float ptRatioEflowApproxOff = 0;
  float dRMaxOff = 0;
  float topoInvMassOff = 0;
  float trFlightPathSigOff = 0;
  float massTrkSysOff = 0;
  float mEflowApproxOff = 0;

  float isoFrac = 0;
  float EMRadius = 0;
  float hadRadius = 0;
  float PSSFraction = 0;
  float EMFraction = 0;
  float EtRaw = 0;
  
  float isoFracOff = 0;
  float EMRadiusOff = 0;
  float hadRadiusOff = 0;
  float PSSFractionOff = 0;
  float EMFractionOff = 0;
  float EtRawOff = 0;
  
  float etEMAtEMScale = 0;
  float etEMAtEMScaleOff = 0;
  float etHadAtEMScale = 0;
  float etHadAtEMScaleOff = 0;
  float mu = 0;

  int EFnTrack = aEFTau->nTracks();
  bool is1P(false), isMP(false);
  if(EFnTrack==1) is1P = true;
  if(EFnTrack>1) isMP = true;

  //Offline Tau matching to EF(HLT Tau)
  const xAOD::TauJet *aOfflineTau = 0;
  const xAOD::TauJetContainer* tauCont = 0;
  float tmpR = 0.2;
  
  if(m_storeGate->retrieve(tauCont, "TauJets").isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
      return StatusCode::FAILURE;
    }
  
  xAOD::TauJetContainer::const_iterator CI,offlinetau_end = tauCont->end();
  for(CI=tauCont->begin();CI!=offlinetau_end; ++CI)
    {
      if((*CI))
	{
	  TLorentzVector TauTLV = (*CI)->p4();
	  double eta_Tau = TauTLV.Eta();
	  if(fabs(eta_Tau) > 2.47) continue;
	  double pt_Tau = TauTLV.Pt();
	  if(pt_Tau<m_effOffTauPtCut) continue;
	  int ntrack_TAU = (*CI)->nTracks();
	  if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
	  bool good_tau = (*CI)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
	  bool not_a_electron = !( (*CI)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
	  bool not_a_muon = !( (*CI)->isTau(xAOD::TauJetParameters::MuonVeto) );
	  bool best_tau = good_tau && not_a_electron && not_a_muon;
	  if(!best_tau) continue;
	  double dR = TauTLV.DeltaR(aEFTau->p4());
	  if(dR < tmpR)
	    { 
	      tmpR = dR;
	      aOfflineTau = (*CI);
	    }
        }
    }
  if(aOfflineTau)
    {
      ATH_MSG_DEBUG("Found an offline TauJet matching EF tauJet, dR = "<<tmpR);
    }
  else 
    {
      ATH_MSG_DEBUG("Not found an offline TauJet matching EF tauJet");
      return StatusCode::SUCCESS;
    }
  //Pileup
  mu = Pileup();

  if(BDTinput_type == "basicVars")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsOffline");

      //Basic Vars
      FillRelDiffHist(hist("hptRatio"), aOfflineTau->pt(), aEFTau->pt(), 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovspt"), aOfflineTau->pt(), aEFTau->pt(), aOfflineTau->pt()/1000., 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovseta"), aOfflineTau->pt(), aEFTau->pt(), aOfflineTau->eta(), 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovsphi"), aOfflineTau->pt(), aEFTau->pt(), aOfflineTau->phi(), 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovsmu"), aOfflineTau->pt(), aEFTau->pt(), mu, 0, 1);

      FillRelDiffHist(hist("hetaRatio"), aOfflineTau->eta(), aEFTau->eta(), 0, 2);
      hist("hphiRatio")->Fill(deltaPhi(aOfflineTau->phi(), aEFTau->phi()));
   
      if(aEFTau->detail(xAOD::TauJetParameters::EMRadius, EMRadius) && aOfflineTau->detail(xAOD::TauJetParameters::EMRadius, EMRadiusOff))
	{
	  FillRelDiffHist(hist("hEMRadiusRatio"), EMRadiusOff, EMRadius, 0, 1);
          FillRelDiffProfile<float>(profile("hEMRadiusRatiovspt"), EMRadiusOff, EMRadius, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hEMRadiusRatiovseta"), EMRadiusOff, EMRadius, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hEMRadiusRatiovsphi"), EMRadiusOff, EMRadius, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hEMRadiusRatiovsmu"), EMRadiusOff, EMRadius, mu, 0, 1);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::hadRadius, hadRadius) && aOfflineTau->detail(xAOD::TauJetParameters::hadRadius, hadRadiusOff))
	{
	  FillRelDiffHist(hist("hHadRadiusRatio"), hadRadiusOff, hadRadius, 0, 1);
          FillRelDiffProfile<float>(profile("hHADRadiusRatiovspt"), hadRadiusOff, hadRadius, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hHADRadiusRatiovseta"), hadRadiusOff, hadRadius, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hHADRadiusRatiovsphi"), hadRadiusOff, hadRadius, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hHADRadiusRatiovsmu"), hadRadiusOff, hadRadius, mu, 0, 1);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::isolFrac, isoFrac) && aOfflineTau->detail(xAOD::TauJetParameters::isolFrac, isoFracOff))
	{
	  FillRelDiffHist(hist("hIsoFracRatio"), isoFracOff, isoFrac, 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovspt"), isoFracOff, isoFrac, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovseta"), isoFracOff, isoFrac, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovsphi"), isoFracOff, isoFrac, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovsmu"), isoFracOff, isoFrac, mu, 0, 1);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac) && aOfflineTau->detail(xAOD::TauJetParameters::centFrac, centFracOff))
        {  
          FillRelDiffProfile<float>(profile("hCentFracRatiovspt"), centFracOff, centFrac, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hCentFracRatiovseta"), centFracOff, centFrac, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hCentFracRatiovsphi"), centFracOff, centFrac, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hCentFracRatiovsmu"), centFracOff, centFrac, mu, 0, 1);
        }
      if(aEFTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction) && aOfflineTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFractionOff))
	{
	  FillRelDiffHist(hist("hPSSFracRatio"), PSSFractionOff, PSSFraction, 0, 1);
          FillRelDiffProfile<float>(profile("hPSSFracRatiovspt"), PSSFractionOff, PSSFraction, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hPSSFracRatiovseta"), PSSFractionOff, PSSFraction, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hPSSFracRatiovsphi"), PSSFractionOff, PSSFraction, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hPSSFracRatiovsmu"), PSSFractionOff, PSSFraction, mu, 0, 1);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScale) && aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScale) && aOfflineTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScaleOff) && aOfflineTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScaleOff))
	{
	  EtRaw = (etEMAtEMScale + etHadAtEMScale)/CLHEP::GeV;
	  EtRawOff = (etEMAtEMScaleOff + etHadAtEMScaleOff)/CLHEP::GeV;
	  FillRelDiffHist(hist("hEtRawRatio"), EtRawOff, EtRaw, 0.1, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovspt"), EtRawOff, EtRaw, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovseta"), EtRawOff, EtRaw, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovsphi"), EtRawOff, EtRaw, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovsmu"), EtRawOff, EtRaw, mu, 0, 1);

          EMFraction = etEMAtEMScale / (etEMAtEMScale + etHadAtEMScale);
          EMFractionOff = etEMAtEMScaleOff / (etEMAtEMScaleOff + etHadAtEMScaleOff);
          FillRelDiffHist(hist("hEMFracRatio"), EMFractionOff, EMFraction, 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovspt"), EMFractionOff, EMFraction, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovseta"), EMFractionOff, EMFraction, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovsphi"), EMFractionOff, EMFraction, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovsmu"), EMFractionOff, EMFraction, mu, 0, 1);

	}
      if(aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScale) && aOfflineTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScaleOff))
	{
	  FillRelDiffHist(hist("hOffEFEMDiff"), etEMAtEMScaleOff, etEMAtEMScale, 0, 1);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScale) && aOfflineTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScaleOff))
	{
	  FillRelDiffHist(hist("hOffEFHADDiff"), etHadAtEMScaleOff, etHadAtEMScale, 0, 1);
	}
    }
  else if(BDTinput_type == "1p_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsOffline/BDT/1p_nonCorrected");
      if(is1P)
	{
	  if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist) && aOfflineTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDistOff))
	    {
	      FillRelDiffHist(hist("hInnerTrkAvgDistRatio1P"), innerTrkAvgDistOff, innerTrkAvgDist, 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVspt1P"), innerTrkAvgDistOff, innerTrkAvgDist, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVsmu1P"),  innerTrkAvgDistOff, innerTrkAvgDist, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk) && aOfflineTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrkOff))
	    {
	      FillRelDiffHist(hist("hEtOverPtLeadTrkRatio1P"), etOverPtLeadTrkOff, etOverPtLeadTrk, 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVspt1P"), etOverPtLeadTrkOff, etOverPtLeadTrk, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVsmu1P"), etOverPtLeadTrkOff, etOverPtLeadTrk, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk) && aOfflineTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrkOff))
	    {
	      FillRelDiffHist(hist("hIpSigLeadTrkRatio1P"), ipSigLeadTrkOff, ipSigLeadTrk, 0., 2);
	      FillRelDiffProfile<float>(profile("hIpSigLeadTrkVspt1P"), ipSigLeadTrkOff, ipSigLeadTrk, aOfflineTau->pt()/1000., 0., 2);
	      FillRelDiffProfile<float>(profile("hIpSigLeadTrkVsmu1P"), ipSigLeadTrkOff, ipSigLeadTrk, mu, 0., 2);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFrac) && aOfflineTau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFracOff))
	    {
	      FillRelDiffHist(hist("hSumPtTrkFracRatio1P"), SumPtTrkFracOff, SumPtTrkFrac, 0., 1);
	      FillRelDiffProfile<float>(profile("hSumPtTrkFracVspt1P"), SumPtTrkFracOff, SumPtTrkFrac, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hSumPtTrkFracVsmu1P"), SumPtTrkFracOff, SumPtTrkFrac, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME) && aOfflineTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEMEOff))
	    {
	      FillRelDiffHist(hist("hChPiEMEOverCaloEMERatio1P"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVspt1P"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVsmu1P"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP) && aOfflineTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysPOff))
	    {
	      FillRelDiffHist(hist("hEMPOverTrkSysPRatio1P"), EMPOverTrkSysPOff, EMPOverTrkSysP, 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVspt1P"), EMPOverTrkSysPOff, EMPOverTrkSysP, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVsmu1P"), EMPOverTrkSysPOff, EMPOverTrkSysP, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac) && aOfflineTau->detail(xAOD::TauJetParameters::centFrac, centFracOff))
	    {
	      FillRelDiffHist(hist("hCentFracRatio1P"), centFracOff, centFrac, 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVspt1P"), centFracOff, centFrac, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVsmu1P"), centFracOff, centFrac, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox) && aOfflineTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApproxOff))
	    {
	      FillRelDiffHist(hist("hPtRatioEflowApproxRatio1P"), ptRatioEflowApproxOff, ptRatioEflowApprox, 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVspt1P"), ptRatioEflowApproxOff, ptRatioEflowApprox, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVsmu1P"), ptRatioEflowApproxOff, ptRatioEflowApprox, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::dRmax, dRMax)&& aOfflineTau->detail(xAOD::TauJetParameters::dRmax, dRMaxOff))
	    {
	      FillRelDiffHist(hist("hDRmaxRatio1P"), dRMaxOff, dRMax, 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVspt1P"), dRMaxOff, dRMax, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVsmu1P"), dRMaxOff, dRMax, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::topoInvMass, topoInvMass) && aOfflineTau->detail(xAOD::TauJetParameters::topoInvMass, topoInvMassOff))
	    {
	      FillRelDiffHist(hist("hTopoInvMassRatio1P"), topoInvMassOff/CLHEP::GeV, topoInvMass/CLHEP::GeV, 0., 1);
	      FillRelDiffProfile<float>(profile("hTopoInvMassVspt1P"), topoInvMassOff/CLHEP::GeV, topoInvMass/CLHEP::GeV, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hTopoInvMassVsmu1P"), topoInvMassOff/CLHEP::GeV, topoInvMass/CLHEP::GeV, mu, 0., 1);
	    }
	}
    }
  else if(BDTinput_type == "mp_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsOffline/BDT/mp_nonCorrected");
      if(isMP)
	{
	  if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist) && aOfflineTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDistOff))
	    {
	      FillRelDiffHist(hist("hInnerTrkAvgDistRatioMP"), innerTrkAvgDistOff, innerTrkAvgDist, 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVsptMP"), innerTrkAvgDistOff, innerTrkAvgDist, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVsmuMP"), innerTrkAvgDistOff, innerTrkAvgDist, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk) && aOfflineTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrkOff))
	    {
	      FillRelDiffHist(hist("hEtOverPtLeadTrkRatioMP"), etOverPtLeadTrkOff, etOverPtLeadTrk, 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVsptMP"), etOverPtLeadTrkOff, etOverPtLeadTrk,  aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVsmuMP"), etOverPtLeadTrkOff, etOverPtLeadTrk, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME) && aOfflineTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEMEOff))
	    {
	      FillRelDiffHist(hist("hChPiEMEOverCaloEMERatioMP"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVsptMP"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME,  aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVsmuMP"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP) && aOfflineTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysPOff))
	    {
	      FillRelDiffHist(hist("hEMPOverTrkSysPRatioMP"), EMPOverTrkSysPOff, EMPOverTrkSysP, 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVsptMP"), EMPOverTrkSysPOff, EMPOverTrkSysP, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVsmuMP"), EMPOverTrkSysPOff, EMPOverTrkSysP, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac) && aOfflineTau->detail(xAOD::TauJetParameters::centFrac, centFracOff))
	    {
	      FillRelDiffHist(hist("hCentFracRatioMP"), centFracOff, centFrac, 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVsptMP"), centFracOff, centFrac, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVsmuMP"), centFracOff, centFrac, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox) && aOfflineTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApproxOff))
	    {
	      FillRelDiffHist(hist("hPtRatioEflowApproxRatioMP"), ptRatioEflowApproxOff, ptRatioEflowApprox, 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVsptMP"), ptRatioEflowApproxOff, ptRatioEflowApprox, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVsmuMP"), ptRatioEflowApproxOff, ptRatioEflowApprox, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::dRmax, dRMax)&& aOfflineTau->detail(xAOD::TauJetParameters::dRmax, dRMaxOff))
	    {
	      FillRelDiffHist(hist("hDRmaxRatioMP"), dRMaxOff, dRMax, 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVsptMP"), dRMaxOff, dRMax, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVsmuMP"), dRMaxOff, dRMax, mu, 0., 1);
	    }
	  if( aEFTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig) && aOfflineTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSigOff))
	    {
	      FillRelDiffHist(hist("hTrFlightPathSigRatioMP"), trFlightPathSigOff, trFlightPathSig, 0., 2);
	      FillRelDiffProfile<float>(profile("hTrFlightPathSigVsptMP"), trFlightPathSigOff, trFlightPathSig, aOfflineTau->pt()/1000., 0., 2);
	      FillRelDiffProfile<float>(profile("hTrFlightPathSigVsmuMP"), trFlightPathSigOff, trFlightPathSig, mu, 0., 2);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys) && aOfflineTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSysOff))
	    {
	      FillRelDiffHist(hist("hMassTrkSysRatioMP"), massTrkSysOff/CLHEP::GeV, massTrkSys/CLHEP::GeV, 0., 1);
	      FillRelDiffProfile<float>(profile("hMassTrkSysVsptMP"),  massTrkSysOff/CLHEP::GeV, massTrkSys/CLHEP::GeV, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hMassTrkSysVsmuMP"),  massTrkSysOff/CLHEP::GeV, massTrkSys/CLHEP::GeV, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::mEflowApprox, mEflowApprox) && aOfflineTau->detail(xAOD::TauJetParameters::mEflowApprox, mEflowApproxOff))
	    {
	      FillRelDiffHist(hist("hMEflowApproxRatioMP"), mEflowApproxOff, mEflowApprox, 0., 1);
	      FillRelDiffProfile<float>(profile("hMEflowApproxVsptMP"), mEflowApproxOff, mEflowApprox, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hMEflowApproxVsmuMP"), mEflowApproxOff, mEflowApprox, mu, 0., 1);
	    }
	}
    }
  else
    {
      ATH_MSG_WARNING("Wrong BDT input type Failed to fill the histograms.Check fillEFTauVsOffline(). Exiting.");
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------
// Testing methods
//------------------------------------------------------------------------------------
void HLTTauMonTool::testL1TopoNavigation(const std::string & trigItem){
	ATH_MSG_DEBUG("HLTTauMonTool::testL1TopoNavigation");
        std::string trig_item_EF = "HLT_"+trigItem;
	if (getTDT()->isPassed(trig_item_EF)){
		ATH_MSG_WARNING("Passed "<<trig_item_EF);	
		Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,TrigDefs::Physics) );
		Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
		if(comb==combEnd){
			ATH_MSG_WARNING("No combination found with TrigDefs::Physics!");
		}
		for(;comb!=combEnd;++comb){

			const xAOD::EmTauRoIContainer* l1Tau_cont = 0;
			if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){
				ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container");
			} else{
				ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
			}
			xAOD::EmTauRoIContainer::const_iterator itEMTau;
			xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();	
		
			const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb->get<TrigRoiDescriptor>("initialRoI",TrigDefs::Physics);
			std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin(), roi_e = vec_roi.end();
			if(roi==roi_e){
				ATH_MSG_WARNING("Combination found with TrigDefs::Physics without RoIs!");
			}
			ATH_MSG_WARNING("Combination found with TrigDefs::Physics with following RoIs:");
			for(; roi != roi_e; ++roi){
				if(roi->cptr()){
					ATH_MSG_WARNING("RoI Word " << roi->cptr()->roiWord() << ", eta " << roi->cptr()->eta() << ", phi " << roi->cptr()->phi() << ", roiId " << roi->cptr()->roiId() << ", l1Id " << roi->cptr()->l1Id() );
					for(itEMTau = l1Tau_cont->begin(); itEMTau!=itEMTau_e; ++itEMTau){
						if(roi->cptr()->roiWord()!=(*itEMTau)->roiWord()) continue;
						ATH_MSG_WARNING("EMTau RoI Word " << (*itEMTau)->roiWord() << ", eta " << (*itEMTau)->eta() << ", phi " << (*itEMTau)->phi() );
						const std::vector<std::string>& names = (*itEMTau)->thrNames();
						for(unsigned int i=0;i<names.size(); i++) ATH_MSG_WARNING("thrNames: " << names.at(i) );
					}
				}
			}
		}
//		Trig::FeatureContainer f_comp = ( getTDT()->features(trig_item_EF,TrigDefs::TrigComposite) );
//                Trig::FeatureContainer::combination_const_iterator comb_comp(f_comp.getCombinations().begin()), combEnd_comp(f_comp.getCombinations().end());
//                if(comb_comp==combEnd_comp){
//                        ATH_MSG_WARNING("No combination found with TrigDefs::TrigComposite!");
//                }
//                for(;comb_comp!=combEnd_comp;++comb_comp){
//                        const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb_comp->get<TrigRoiDescriptor>("initialRoI",TrigDefs::TrigComposite);
//                        std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin(), roi_e = vec_roi.end();
//                        if(roi==roi_e){
//                                ATH_MSG_WARNING("Combination found with TrigDefs::TrigComposite without RoIs!");
//                        }
//                        ATH_MSG_WARNING("Combination found with TrigDefs::TrigComposite with following RoIs:");
//                        for(; roi != roi_e; ++roi){
//                                if(roi->cptr()){
//                                        ATH_MSG_WARNING("RoI type" << roi->cptr()->roiWord() << ", eta " << roi->cptr()->eta() << ", phi " << roi->cptr()->phi());    
//                                        const std::vector<std::string>& names = roi->cptr()->thrNames();
//                                        for(unsigned int i=0;i<names.size(); i++) ATH_MSG_WARNING("thrNames: " << names.at(i) );
//                                }
//                        }
//                }

	}
}

void  HLTTauMonTool::testClusterNavigation(const xAOD::TauJet *aEFTau){

  ATH_MSG_DEBUG("HLTTauMonTool::testClusterNavigation");

  const xAOD::Jet* pJetSeed = *(aEFTau->jetLink());  

  xAOD::JetConstituentVector::const_iterator clusItr  = pJetSeed->getConstituents().begin();
  xAOD::JetConstituentVector::const_iterator clusItrE = pJetSeed->getConstituents().end();
  if(clusItr==clusItrE) ATH_MSG_WARNING("Cluster vector Empty!!");
  for (int clusCount = 0; clusItr != clusItrE; ++clusItr, ++clusCount) {
    ATH_MSG_WARNING( "Tau Cluster " << clusCount << " pt = " << (*clusItr)->pt()    
		<< " eta = " << (*clusItr)->eta()
		<< " phi = " << (*clusItr)->phi() );
  }
}

StatusCode  HLTTauMonTool::test2StepTracking(){

  setCurrentMonGroup("HLT/TauMon/Expert/FTF_track_comparison");

  const xAOD::TauJetContainer * tauPreselCont = 0;
  if( evtStore()->retrieve(tauPreselCont, "HLT_xAOD__TauJetContainer_TrigTauRecPreselection").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TauJetContainer_TrigTauRecPreselection container. Exiting!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Testing 2step tracking loop over TrigTauPreselection container...");
  xAOD::TauJetContainer::const_iterator tauPresel = tauPreselCont->begin(),tauPreselCont_end = tauPreselCont->end();
  for(; tauPresel!=tauPreselCont_end; ++tauPresel){
    float pt = (*tauPresel)->pt();
    float eta = (*tauPresel)->eta();
    float phi = (*tauPresel)->phi();
    int nTrack = (*tauPresel)->nTracks();
    ATH_MSG_DEBUG("tauPresel: pt "<< pt <<", eta " << eta << ", phi " << phi << ", #tracks "<< nTrack);
    //for (unsigned int i=0;i<(*tauPresel)->nTracks();++i) { 
    //  const xAOD::TrackParticle* trk = (*tauPresel)->track(i);
    //  ATH_MSG_DEBUG("track "<< i <<"-th: index "<< trk->index() << ", d0 " << trk->d0() << ", phi0 "<< trk->phi0() << ", theta " << trk->theta() << ", qOverP  " << trk->qOverP());
    //}
    std::vector< ElementLink< xAOD::TrackParticleContainer > > myLinks = (*tauPresel)->trackLinks();
    ATH_MSG_DEBUG("Size of links: " << myLinks.size());
    //for(unsigned int i=0;i<myLinks.size();++i){
    //  ATH_MSG_DEBUG("Get the link: " << i);
    //  ElementLink< xAOD::TrackParticleContainer > theLink = myLinks.at(i);
    //  ATH_MSG_DEBUG("IsValid: " << theLink.isValid());
    //  ATH_MSG_DEBUG("TheID: " << theLink.dataID());
    //  ATH_MSG_DEBUG("TheKey: " << theLink.key());
    //  ATH_MSG_DEBUG("TheIndex: " << theLink.index());
    //}
    
    bool is2step(false);
    if(myLinks.size()==0) continue;
    ElementLink< xAOD::TrackParticleContainer > theLink = myLinks.at(0);
    std::string FTFtrackName(theLink.dataID());
    if(FTFtrackName.find("TauIso_FTF")!=std::string::npos || FTFtrackName.find("TauCore_FTF")!=std::string::npos) is2step = true;

    if(is2step) hist("hFTFnTrack_2steps")->Fill((*tauPresel)->nTracks());
    if(!is2step) hist("hFTFnTrack_1step")->Fill((*tauPresel)->nTracks());
    if(is2step) hist("hFTFnWideTrack_2steps")->Fill((*tauPresel)->nWideTracks());
    if(!is2step) hist("hFTFnWideTrack_1step")->Fill((*tauPresel)->nWideTracks());

  }

  return StatusCode::SUCCESS; 
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
const xAOD::EmTauRoI * HLTTauMonTool::findLVL1_ROI(const TrigRoiDescriptor * roiDesc){
    
    const xAOD::EmTauRoIContainer*  l1Tau_cont = 0;
    if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){ // retrieve arguments: container type, container key
        ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
        return 0;
    } else{
        ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
    }
    
    xAOD::EmTauRoIContainer::const_iterator itEMTau = l1Tau_cont->begin();
    xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();
    
    unsigned int id = roiDesc->roiWord();
    for( xAOD::EmTauRoIContainer::const_iterator it=itEMTau; it!=itEMTau_e;++it){
        if(id==(*it)->roiWord()) return (*it);
    }
    return 0;
}


//-----------------------------------------------------
// Useful tools and matching methods
//-----------------------------------------------------

void HLTTauMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {
    
    quo->Divide(num, den, 1., 1., "b");
}

void HLTTauMonTool::divide(TH2 *num, TH2 *den, TH2 *quo) {

    quo->Divide(num, den, 1., 1., "b");
}

void HLTTauMonTool::divide(const std::string & h_name_num, const std::string & h_name_den, const std::string & h_name_div, const std::string & dir){
    
    divide(
           hist(h_name_num,dir),
           hist(h_name_den,dir),
           hist(h_name_div,dir)
           );
}

void HLTTauMonTool::divide2(const std::string & h_name_num, const std::string & h_name_den, const std::string & h_name_div, const std::string & dir){

    divide(
           hist2(h_name_num,dir),
           hist2(h_name_den,dir),
           hist2(h_name_div,dir)
           );
}

float  HLTTauMonTool::deltaEta(float eta1, float eta2)
{
    float dEta = eta1-eta2;
    return dEta;
}

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

void HLTTauMonTool::plotUnderOverFlow(TH1* hist){
    
    float underContent = hist->GetBinContent(0);
    float underError = hist->GetBinError(0);
    float firstContent = hist->GetBinContent(1)+underContent;
    float firstError = sqrt(hist->GetBinError(1)*hist->GetBinError(1) + underError*underError);
    
    hist->SetBinContent(1,firstContent);
    hist->SetBinError(1,firstError);
    hist->SetBinContent(0,0);
    hist->SetBinError(0,0);
    
    float overContent = hist->GetBinContent(hist->GetNbinsX()+1);
    float overError = hist->GetBinError(hist->GetNbinsX()+1);
    float lastContent = hist->GetBinContent(hist->GetNbinsX())+overContent;
    float lastError = sqrt(hist->GetBinError(hist->GetNbinsX())*hist->GetBinError(hist->GetNbinsX()) + overError*overError);
    
    hist->SetBinContent(hist->GetNbinsX(),lastContent);
    hist->SetBinError(hist->GetNbinsX(),lastError);
    hist->SetBinContent(hist->GetNbinsX()+1,0);
    hist->SetBinError(hist->GetNbinsX()+1,0);  
    
}

bool HLTTauMonTool::Selection(const xAOD::TauJet *aTau){
  
   int nTrk = aTau->nTracks();
   float pt = aTau->pt();
   float eta = aTau->eta();
   float phi = aTau->phi();

   if(m_selection_nTrkMax>-1 && nTrk>m_selection_nTrkMax) return false;
   if(m_selection_nTrkMin>-1 && nTrk<m_selection_nTrkMin) return false;
   if(m_selection_ptMax>-1 && pt>m_selection_ptMax) return false;
   if(m_selection_ptMin>-1 && pt<m_selection_ptMin) return false;
   if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) return false;
   if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) return false;
   if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) return false;
   if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) return false;

   return true;
}

bool HLTTauMonTool::Selection(const xAOD::EmTauRoI *aTau){
  
   float pt = aTau->tauClus();
   float eta = aTau->eta();
   float phi = aTau->phi();

   if(m_selection_ptMax>-1 && pt>m_selection_ptMax) return false;
   if(m_selection_ptMin>-1 && pt<m_selection_ptMin) return false;
   if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) return false;
   if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) return false;
   if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) return false;
   if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) return false;

   return true;
}

//=================================
// Turn On Curve Methods
//=================================

void HLTTauMonTool::examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const
{
    xTruthParticle.auxdecor<bool>("IsLeptonicTau") = false;
    
    TLorentzVector VisSumTLV;
    xTruthParticle.auxdecor<double>("pt_vis") = 0;
    xTruthParticle.auxdecor<double>("eta_vis") = 0;
    xTruthParticle.auxdecor<double>("phi_vis") = 0;
    xTruthParticle.auxdecor<double>("m_vis") = 0;
    xTruthParticle.auxdecor<int>("childChargeSum") = 0;
    xTruthParticle.auxdecor<int>("nTracks") = 0;
   
    if(!xTruthParticle.hasDecayVtx()) return;
    
    const xAOD::TruthVertex* decayvtx = xTruthParticle.decayVtx();
    if(decayvtx)
    {
        const std::size_t nChildren = decayvtx->nOutgoingParticles();
        for ( std::size_t iChild = 0; iChild != nChildren; ++iChild )
        {
            const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
	    if((abs(child->pdgId()) == 12 || abs(child->pdgId()) == 14 || abs(child->pdgId()) == 16)) continue;
	    if(child->status()==3) continue;
	    ATH_MSG_DEBUG("child "<< child->pdgId() << ", status "<< child->status() << ", charge "<< child->charge());
            if ((abs(child->pdgId()) == 11 || abs(child->pdgId()) == 13 || abs(child->pdgId()) == 15)) xTruthParticle.auxdecor<bool>("IsLeptonicTau") = true;
            VisSumTLV += child->p4();
            xTruthParticle.auxdecor<int>("childChargeSum") += child->charge();
	    xTruthParticle.auxdecor<int>("nTracks") += abs(child->charge());
        }
    }

    xTruthParticle.auxdecor<double>("pt_vis")  = VisSumTLV.Pt();
    xTruthParticle.auxdecor<double>("eta_vis") = VisSumTLV.Eta();
    xTruthParticle.auxdecor<double>("phi_vis") = VisSumTLV.Phi();
    xTruthParticle.auxdecor<double>("m_vis")   = VisSumTLV.M();
    if(xTruthParticle.auxdecor<int>("childChargeSum")!=xTruthParticle.charge() || xTruthParticle.auxdecor<int>("nTracks")%2==0)
      { 
	ATH_MSG_WARNING("Strange tau: charge " << xTruthParticle.auxdecor<int>("childChargeSum") << " and " << xTruthParticle.auxdecor<int>("nTracks")  << " tracks");
	const std::size_t nChildren = decayvtx->nOutgoingParticles();
	for ( std::size_t iChild = 0; iChild != nChildren; ++iChild )
	  {
	    const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
	    ATH_MSG_WARNING("child "<< child->pdgId() << ", status "<< child->status() << ", charge "<< child->charge());
	  }
      }
}


StatusCode HLTTauMonTool::TauEfficiency(const std::string & trigItem, const std::string & TauDenom){
	ATH_MSG_DEBUG("Efficiency wrt "<< TauDenom << " for trigItem" << trigItem);
	if(trigItem == "Dump") {ATH_MSG_DEBUG("Not computing efficiencies for Dump"); return StatusCode::SUCCESS;};

	std::string l1_chain(LowerChain("HLT_"+trigItem));

	std::vector<TLorentzVector> tlv_TauDenom;
	std::vector<int> ntrk_TauDenom;
	std::vector<bool> good_TauDenom;
	std::vector<TLorentzVector> tlv_tmp;

	float mu(Pileup());
	int nvtx(PrimaryVertices());

	// build vector of taus in denominator:
	if(TauDenom.find("Truth")!=std::string::npos && m_truth){
		const xAOD::TruthParticleContainer* truth_cont = 0;
		if( evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure() ){
    			ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
    			return StatusCode::FAILURE;
  		}
		xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
		for (truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
    		 {
      			if(abs((*truthItr)->pdgId()) == 15) ATH_MSG_DEBUG("Tau with status " << (*truthItr)->status() << " and charge " << (*truthItr)->charge());
      			if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) // || (*truthItr)->status() == 10902))
        		 {
	  			examineTruthTau(**truthItr);
	  			double pt  = (*truthItr)->auxdata<double>("pt_vis");
	  			double eta = (*truthItr)->auxdata<double>("eta_vis");
	  			double phi = (*truthItr)->auxdata<double>("phi_vis");
	  			double m   = (*truthItr)->auxdata<double>("m_vis");
	  			bool lep = (*truthItr)->auxdata<bool>("IsLeptonicTau");
	  			int ntracks = (*truthItr)->auxdata<int>("nTracks");
	  			if(pt < m_effOffTauPtCut || lep || fabs(eta) > 2.47 ) continue;
				TLorentzVector TruthTauTLV;
				TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
				tlv_TauDenom.push_back(TruthTauTLV);
				ntrk_TauDenom.push_back(ntracks);
				good_TauDenom.push_back(true);
			 }
		 }
	}

	if(TauDenom.find("Reco")!=std::string::npos){

		bool addToDenom(false);
		if(TauDenom.find("Truth")==std::string::npos || !m_truth) addToDenom = true;
 
		const xAOD::TauJetContainer * reco_cont = 0;
		if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() ){
    			ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
    			return StatusCode::FAILURE;
  		}
		xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
		for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
            	 {
	      		if(!Selection(*recoItr)) continue;
              		TLorentzVector TauTLV = (*recoItr)->p4();
              		double eta_Tau = TauTLV.Eta();
              		if(fabs(eta_Tau) > 2.47) continue;
 	      		double pt_Tau = TauTLV.Pt();
	      		if(pt_Tau<m_effOffTauPtCut) continue;
	      		int ntrack_TAU = (*recoItr)->nTracks();
	      		if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
			if(TauDenom.find("ID")!=std::string::npos){
   	      			bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
              			bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
              			bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
              			bool best_tau = good_tau && not_a_electron && not_a_muon;
  	      			if(!best_tau) continue;		
			}
			if(addToDenom){	
				tlv_TauDenom.push_back(TauTLV);
                                ntrk_TauDenom.push_back(ntrack_TAU);
                                good_TauDenom.push_back(true);		
			}
			if(!addToDenom){
				tlv_tmp.push_back(TauTLV);
			}
		}
		
		if(!addToDenom){
			for(unsigned int i=0;i<tlv_TauDenom.size();i++){
				if(!good_TauDenom.at(i)) continue;
				bool matched(false);
				for(unsigned int j=0;j<tlv_tmp.size();j++){
					if(tlv_tmp.at(j).DeltaR(tlv_TauDenom.at(i))<0.2) matched = true;
				}
				if(!matched) good_TauDenom.at(i) = false; 
			}
			tlv_tmp.clear();
		}
	}

	if(TauDenom.find("Presel")!=std::string::npos){
		bool addToDenom(false);
                if( (TauDenom.find("Truth")==std::string::npos || !m_truth) && TauDenom.find("Reco")==std::string::npos) addToDenom = true;
		
		const xAOD::TauJetContainer * hlt_cont = 0;
		if( evtStore()->retrieve(hlt_cont, "HLT_xAOD__TauJetContainer_TrigTauRecPreselection").isFailure() ){
    			ATH_MSG_WARNING("Failed to retrieve  HLT_xAOD__TauJetContainer_TrigTauRecPreselection container. Exiting.");
    			return StatusCode::FAILURE;
  		}
		xAOD::TauJetContainer::const_iterator hltItr, hlt_cont_end = hlt_cont->end();
		for(hltItr=hlt_cont->begin(); hltItr!=hlt_cont_end; ++hltItr){
			TLorentzVector hltTLV = (*hltItr)->p4();
			int ntrack_TAU = (*hltItr)->nTracks();
			int nWideTrack_TAU = (*hltItr)->nWideTracks();	
			if( !L1TauMatching(l1_chain, hltTLV, 0.3) ) continue;
			if(ntrack_TAU>3) continue;		
			if(trigItem.find("perf")==std::string::npos) if(ntrack_TAU==0 || nWideTrack_TAU>1) continue;
			if(trigItem.find("25")!=std::string::npos) if(hltTLV.Pt()<25000.) continue;
			if(trigItem.find("35")!=std::string::npos) if(hltTLV.Pt()<35000.) continue;
			if(trigItem.find("80")!=std::string::npos) if(hltTLV.Pt()<80000.) continue;
			if(trigItem.find("125")!=std::string::npos) if(hltTLV.Pt()<125000.) continue;
			if(trigItem.find("160")!=std::string::npos) if(hltTLV.Pt()<160000.) continue;
			if(addToDenom){
                                tlv_TauDenom.push_back(hltTLV);
                                ntrk_TauDenom.push_back(ntrack_TAU);
                                good_TauDenom.push_back(true);
                        }
                        if(!addToDenom){
                                tlv_tmp.push_back(hltTLV);
                        }
		}

		if(!addToDenom){
                        for(unsigned int i=0;i<tlv_TauDenom.size();i++){
                                if(!good_TauDenom.at(i)) continue;
                                bool matched(false);
                                for(unsigned int j=0;j<tlv_tmp.size();j++){
                                        if(tlv_tmp.at(j).DeltaR(tlv_TauDenom.at(i))<0.2) matched = true;
                                }
                                if(!matched) good_TauDenom.at(i) = false;
                        }
                        tlv_tmp.clear();
                }
	}

	setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/RecoEfficiency");
	// loop over taus in denominator and match with L1 and HLT taus:
	for(unsigned int i=0;i<tlv_TauDenom.size();i++){
		if(!good_TauDenom.at(i)) continue;
		int ntracks = ntrk_TauDenom.at(i);
		double pt = tlv_TauDenom.at(i).Pt();
		double eta = tlv_TauDenom.at(i).Eta();
		double phi = tlv_TauDenom.at(i).Phi();

	  	if(m_selection_nTrkMax>-1 && ntracks>m_selection_nTrkMax) continue;
   	  	if(m_selection_nTrkMin>-1 && ntracks<m_selection_nTrkMin) continue;
   	  	if(m_selection_ptMax>-1 && pt>m_selection_ptMax) continue;
   	  	if(m_selection_ptMin>-1 && pt<m_selection_ptMin) continue;
   	  	if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) continue;
   	  	if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) continue;
   	  	if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) continue;
   	  	if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) continue;

      		hist("hRecoTauPtDenom")->Fill(pt/1000.);
      		if(ntracks == 1) hist("hRecoTauPt1PDenom")->Fill(pt/1000.);
      		if(ntracks > 1) hist("hRecoTauPt3PDenom")->Fill(pt/1000.);
      		hist("hRecoTauEtaDenom")->Fill(eta);
      		hist("hRecoTauPhiDenom")->Fill(phi);
      		hist("hRecoTauNTrackDenom")->Fill(ntracks);
      		hist("hRecoTauNVtxDenom")->Fill(nvtx);
      		hist("hRecoTauMuDenom")->Fill(mu);
      		hist2("hRecoTauEtaVsPhiDenom")->Fill(eta,phi);
	
		if( HLTTauMatching(trigItem, tlv_TauDenom.at(i), 0.2)  ){
			hist("hRecoHLTPtNum")->Fill(pt/1000.);
	  		if(ntracks == 1) hist("hRecoHLTPt1PNum")->Fill(pt/1000.);
	  		if(ntracks > 1) hist("hRecoHLTPt3PNum")->Fill(pt/1000.);
	  		hist("hRecoHLTEtaNum")->Fill(eta);
	  		hist("hRecoHLTPhiNum")->Fill(phi);
	  		hist("hRecoHLTNTrackNum")->Fill(ntracks);
	  		hist("hRecoHLTNVtxNum")->Fill(nvtx);
	  		hist("hRecoHLTMuNum")->Fill(mu);
	  		hist2("hRecoHLTEtaVsPhiNum")->Fill(eta,phi);
		}
                if( L1TauMatching(l1_chain, tlv_TauDenom.at(i), 0.3)  ){
                        hist("hRecoL1PtNum")->Fill(pt/1000.);
                        if(ntracks == 1) hist("hRecoL1Pt1PNum")->Fill(pt/1000.);
                        if(ntracks > 1) hist("hRecoL1Pt3PNum")->Fill(pt/1000.);
                        hist("hRecoL1EtaNum")->Fill(eta);
                        hist("hRecoL1PhiNum")->Fill(phi);
                        hist("hRecoL1NTrackNum")->Fill(ntracks);
                        hist("hRecoL1NVtxNum")->Fill(nvtx);
                        hist("hRecoL1MuNum")->Fill(mu);
                        hist2("hRecoL1EtaVsPhiNum")->Fill(eta,phi);
                }
	}

	return StatusCode::SUCCESS;
}


StatusCode HLTTauMonTool::TauEfficiencyCombo(const std::string & trigItem){ 

  ATH_MSG_DEBUG("Combo Efficiency wrt for trigItem" << trigItem);
  if(trigItem == "Dump") {ATH_MSG_DEBUG("Not computing efficiencies for Dump"); return StatusCode::SUCCESS;};
  if(!m_truth) {ATH_MSG_DEBUG("Not computing efficiencies, doTruth=False"); return StatusCode::SUCCESS;};
  
  std::string l1_chain(LowerChain("HLT_"+trigItem));
  std::string hlt_chain = "HLT_"+trigItem;
  
  ATH_MSG_DEBUG("+++++++++++++++++++");
  ATH_MSG_DEBUG("HLT: "<< hlt_chain);
  ATH_MSG_DEBUG("L1: "<< l1_chain);

  std::vector<TLorentzVector> tlv_truthMatched;

  //float offline_met(0.);
  std::vector<TLorentzVector> tlv_OffJets;

  std::vector<TLorentzVector> tlv_truthTau;
  std::vector<bool> tlv_truthTauMatched;
  std::vector<TLorentzVector> tlv_truthElectron;
  std::vector<bool> tlv_truthElectronMatched;
  std::vector<TLorentzVector> tlv_truthMuon;
  std::vector<bool> tlv_truthMuonMatched;
  
  //loop over truth particles to get taus, electrons and muons
  
  const xAOD::TruthParticleContainer* truth_cont = 0;
  if(evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
      return StatusCode::FAILURE;
    }
  xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
  for(truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
    {
      // get truth taus
      if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) 
	{
	  examineTruthTau(**truthItr);
	  double pt  = (*truthItr)->auxdata<double>("pt_vis");
	  double eta = (*truthItr)->auxdata<double>("eta_vis");
	  double phi = (*truthItr)->auxdata<double>("phi_vis");
	  double m   = (*truthItr)->auxdata<double>("m_vis");
	  bool lep = (*truthItr)->auxdata<bool>("IsLeptonicTau");
	  //int ntracks = (*truthItr)->auxdata<int>("nTracks");
	  if(pt >= m_effOffTauPtCut && !lep && fabs(eta) < 2.47 )
	    {
	      TLorentzVector TruthTauTLV;
	      TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
	      tlv_truthTau.push_back(TruthTauTLV);
	      tlv_truthTauMatched.push_back(false);
	    }
	}

      // get truth electrons
      if(abs((*truthItr)->pdgId()) == 11 && (*truthItr)->status() == 1)
	{
	  TLorentzVector TruthElTLV((*truthItr)->p4());
	  tlv_truthElectron.push_back(TruthElTLV);
	  tlv_truthElectronMatched.push_back(false);
	}

      // get truth muons
      if(abs((*truthItr)->pdgId()) == 13 && (*truthItr)->status() == 1)
	{
	  TLorentzVector TruthMuonTLV((*truthItr)->p4());
	  tlv_truthMuon.push_back(TruthMuonTLV);
	  tlv_truthMuonMatched.push_back(false);
	}
    }

  // truth tau matching with offline:
  const xAOD::TauJetContainer * reco_cont = 0;
  if(evtStore()->retrieve(reco_cont, "TauJets").isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
      return StatusCode::FAILURE;
    }
  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
  for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
    {
      if(!Selection(*recoItr)) continue;
      TLorentzVector TauTLV = (*recoItr)->p4();
      double eta_Tau = TauTLV.Eta();
      if(fabs(eta_Tau) > 2.47) continue;
      double pt_Tau = TauTLV.Pt();
      if(pt_Tau<m_effOffTauPtCut) continue;
      int ntrack_TAU = (*recoItr)->nTracks();
      if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
      bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
      bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
      bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
      bool best_tau = good_tau && not_a_electron && not_a_muon;
      if(!best_tau) continue;
      
      for(unsigned int truth=0;truth<tlv_truthTau.size();truth++)
	{
	  if(tlv_truthTau.at(truth).DeltaR(TauTLV)<0.2) tlv_truthTauMatched.at(truth)=true;
	}
    }

  // truth electron matching with offline
  const xAOD::ElectronContainer * elec_cont = 0; 
  if(evtStore()->retrieve(elec_cont, "Electrons").isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve Electrons container. Exiting.");
      return StatusCode::FAILURE;
    }
  xAOD::ElectronContainer::const_iterator elecItr, elec_cont_end = elec_cont->end();
  for(elecItr=elec_cont->begin(); elecItr!=elec_cont_end; ++elecItr)
    {
      // selection for good electron??
      TLorentzVector ElecTLV = (*elecItr)->p4();
      for(unsigned int truth=0;truth<tlv_truthElectron.size();truth++)
	{
	  if(tlv_truthElectron.at(truth).DeltaR(ElecTLV)<0.1) tlv_truthElectronMatched.at(truth)=true;
	}
    }

  // truth muon matching with offline
  const xAOD::MuonContainer * muon_cont = 0;
  if(evtStore()->retrieve(muon_cont, "Muons").isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve Muons container. Exiting.");
      return StatusCode::FAILURE;
    }
  xAOD::MuonContainer::const_iterator muonItr, muon_cont_end = muon_cont->end();
  for(muonItr=muon_cont->begin(); muonItr!=muon_cont_end; ++muonItr)
    {
      // selection for good muon??
      TLorentzVector MuonTLV = (*muonItr)->p4();
      for(unsigned int truth=0;truth<tlv_truthMuon.size();truth++)
	{
	  if(tlv_truthMuon.at(truth).DeltaR(MuonTLV)<0.1) tlv_truthMuonMatched.at(truth)=true;
	}
    }

  // put all truth particles together
  for(unsigned int truth=0;truth<tlv_truthTau.size();truth++) if(tlv_truthTauMatched.at(truth)) tlv_truthMatched.push_back(tlv_truthTau.at(truth));
  for(unsigned int truth=0;truth<tlv_truthElectron.size();truth++) if(tlv_truthElectronMatched.at(truth)) tlv_truthMatched.push_back(tlv_truthElectron.at(truth));
  for(unsigned int truth=0;truth<tlv_truthMuon.size();truth++) if(tlv_truthMuonMatched.at(truth)) tlv_truthMatched.push_back(tlv_truthMuon.at(truth));

  // get offline jets, only the ones not overlapping with truth taus, electrons or muons
  const xAOD::JetContainer * jet_cont = 0;
  if(evtStore()->retrieve(jet_cont, "AntiKt4EMTopoJets").isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve AntiKt4EMTopoJets container. Exiting.");
      return StatusCode::FAILURE;
    } 
  xAOD::JetContainer::const_iterator jetItr, jet_cont_end = jet_cont->end();
  for(jetItr=jet_cont->begin(); jetItr!=jet_cont_end; ++jetItr)
    {
      // selection for good jets??
      TLorentzVector JetTLV = (*jetItr)->p4();
      bool truth_matched(false);
      for(unsigned int truth=0;truth<tlv_truthMatched.size();truth++)
	{
	  if(tlv_truthMatched.at(truth).DeltaR(JetTLV)<0.2) truth_matched=true;
	}
      if(!truth_matched) tlv_OffJets.push_back(JetTLV);
    }

 // get offline met
 const xAOD::MissingETContainer *m_off_met_cont = 0;
 StatusCode sc = m_storeGate->retrieve(m_off_met_cont, "MET_Reference_AntiKt4LCTopo");
 if (sc.isFailure() || !m_off_met_cont) 
   {
     ATH_MSG_WARNING("Could not retrieve Reconstructed MET term with Key MET_Reference_AntiKt4LCTopo : m_off_met_cont = 0");
   }
 
 const xAOD::MissingET  *m_off_met = 0;
 float off_met = -9e9;
 if (m_off_met_cont && m_off_met_cont->size())
   {
     m_off_met = m_off_met_cont->at(0);
     float off_ex = (m_off_met_cont->at(0)->mpx())/CLHEP::GeV;
     float off_ey = (m_off_met_cont->at(0)->mpy())/CLHEP::GeV;
     off_met = sqrt(off_ex*off_ex+off_ey+off_ey);
     ATH_MSG_DEBUG("m_off_met:" << m_off_met );
   }
 //offline_met = off_met;

 // compute dR,dPhi,dEta between two leading truth-matched objects
 // compute efficiencies looking at TDT bit as funciton of leading truth tau, leading truth electron, leading truth muon, leading jet pt, met, dR/dEta/dPhi between the two leading objects

 setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/TauComboEfficiency");
 double pt_tau = -1., pt_el = -1., pt_mu = -1., pt_offJet = -1.;
 for(unsigned int t_tau=0;t_tau<tlv_truthTauMatched.size();t_tau++)
   {
     if(tlv_truthTauMatched.at(t_tau) && tlv_truthTau.at(t_tau).Pt()>25000.)
       {
	 if(pt_tau < tlv_truthTau.at(t_tau).Pt()) pt_tau = tlv_truthTau.at(t_tau).Pt();
       }
   }
 if(pt_tau>0.) hist("hCombTauPtDenom")->Fill(pt_tau/1000.);
 if(pt_tau>0.) if(getTDT()->isPassed(l1_chain))  hist("hCombL1TauPtNum")->Fill(pt_tau/1000.);
 if(pt_tau>0.) if(getTDT()->isPassed(hlt_chain)) hist("hCombHLTTauPtNum")->Fill(pt_tau/1000.);
 
 for(unsigned int t_el=0;t_el<tlv_truthElectronMatched.size();t_el++)
   {
     if(tlv_truthElectronMatched.at(t_el) && tlv_truthElectron.at(t_el).Pt()>15000.)
       {
	 if(pt_el < tlv_truthElectron.at(t_el).Pt()) pt_el = tlv_truthElectron.at(t_el).Pt();
       }
   }
 if(pt_el>0.) hist("hCombelPtDenom")->Fill(pt_el/1000.);
 if(pt_el>0.) if(getTDT()->isPassed(l1_chain))  hist("hCombL1elPtNum")->Fill(pt_el/1000.);
 if(pt_el>0.) if(getTDT()->isPassed(hlt_chain)) hist("hCombHLTelPtNum")->Fill(pt_el/1000.);

 for(unsigned int t_mu=0;t_mu<tlv_truthMuonMatched.size();t_mu++)
   {
     if(tlv_truthMuonMatched.at(t_mu) && tlv_truthMuon.at(t_mu).Pt()>14000.)
       {
	 if(pt_mu < tlv_truthMuon.at(t_mu).Pt()) pt_mu = tlv_truthMuon.at(t_mu).Pt();
       }
   }
 if(pt_mu>0.) hist("hCombmuPtDenom")->Fill(pt_mu/1000.);
 if(pt_mu>0.) if(getTDT()->isPassed(l1_chain))  hist("hCombL1muPtNum")->Fill(pt_mu/1000.);
 if(pt_mu>0.) if(getTDT()->isPassed(hlt_chain)) hist("hCombHLTmuPtNum")->Fill(pt_mu/1000.); 

 for(unsigned int off_j=0; off_j<tlv_OffJets.size();off_j++)
   {
     if(pt_offJet < tlv_OffJets.at(off_j).Pt() && tlv_OffJets.at(off_j).Pt()>25000. ) pt_offJet = tlv_OffJets.at(off_j).Pt();
   }
 if(pt_offJet>0.) hist("hCombOffjetPtDenom")->Fill(pt_offJet/1000.);
 if(pt_offJet>0.) if(getTDT()->isPassed(l1_chain))  hist("hCombL1OffjetPtNum")->Fill(pt_offJet/1000.);
 if(pt_offJet>0.) if(getTDT()->isPassed(hlt_chain)) hist("hCombHLTOffjetPtNum")->Fill(pt_offJet/1000.);

 if(off_met>20000.) hist("hCombMETDenom")->Fill(off_met);
 if(off_met>20000.) if(getTDT()->isPassed(l1_chain))  hist("hCombL1METNum")->Fill(off_met);
 if(off_met>20000.) if(getTDT()->isPassed(hlt_chain)) hist("hCombHLTMETNum")->Fill(off_met);

 double lead_pt =-1., sub_lead_pt=-1., dR=0, dEta=0, dPhi=0;
 unsigned int ind_1 = 0, ind_2 = 0; 
 if(tlv_truthMatched.size() >=2)
   {
     for(unsigned int i=0;i<tlv_truthMatched.size();i++)
       {    
	 if(lead_pt < tlv_truthMatched.at(i).Pt())
	   {
	     lead_pt = tlv_truthMatched.at(i).Pt();
	     ind_1 = i;
	   }
       }
     for(unsigned int j=0;j<tlv_truthMatched.size();j++)
       {
	 if(j == ind_1) continue;
	 if(sub_lead_pt < tlv_truthMatched.at(j).Pt())
	   {
	     sub_lead_pt = tlv_truthMatched.at(j).Pt();
	     ind_2 = j;
	   }
       }
     dR = fabs(deltaR(tlv_truthMatched.at(ind_1).Eta(),tlv_truthMatched.at(ind_2).Eta(),tlv_truthMatched.at(ind_1).Phi(),tlv_truthMatched.at(ind_2).Phi()));
     dEta = fabs(deltaEta(tlv_truthMatched.at(ind_1).Eta(),tlv_truthMatched.at(ind_2).Eta()));
     dPhi = fabs(deltaPhi(tlv_truthMatched.at(ind_1).Phi(),tlv_truthMatched.at(ind_2).Phi()));
     hist("hCombdRDenom")->Fill(dR);
     hist("hCombdEtaDenom")->Fill(dEta);
     hist("hCombdPhiDenom")->Fill(dPhi);
     if(getTDT()->isPassed(l1_chain))
       {
	 hist("hCombL1dRNum")->Fill(dR);
	 hist("hCombL1dEtaNum")->Fill(dEta);
	 hist("hCombL1dPhiNum")->Fill(dPhi);
       }
     if(getTDT()->isPassed(hlt_chain))
       {
	 hist("hCombHLTdRNum")->Fill(dR);
	 hist("hCombHLTdEtaNum")->Fill(dEta);
	 hist("hCombHLTdPhiNum")->Fill(dPhi);
       }
   }

 return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::TruthTauEfficiency(const std::string & trigItem, const std::string & TauCont_type)
{
  ATH_MSG_DEBUG("Truth Tau Matching to Offline and Online Taus for trigItem" << trigItem);
  
  std::string l1_chain(LowerChain("HLT_"+trigItem));

  double pt = 0, eta = 0, phi = 0, m = 0;
  int ntracks = 0, nvtx = 0;
  float mu = 0;
  bool lep;
  TLorentzVector TruthTauTLV(0.,0.,0.,0.);
  TLorentzVector TauTLV(0.,0.,0.,0.);
  
  const xAOD::TruthParticleContainer* truth_cont = 0;
  const xAOD::TauJetContainer * reco_cont = 0;
  const xAOD::TauJetContainer * hlt_cont = 0;
  const xAOD::EmTauRoIContainer * L1_cont = 0;

  if( evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
    return StatusCode::FAILURE;
  }

  if( evtStore()->retrieve(hlt_cont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve  HLT_xAOD__TauJetContainer_TrigTauRecMerged container. Exiting.");
    return StatusCode::FAILURE;
  }

  if( m_storeGate->retrieve(L1_cont, "LVL1EmTauRoIs").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoIs container. Exiting.");
    return StatusCode::FAILURE;
  }

  if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
    return StatusCode::FAILURE;
  } 
  
  xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
  xAOD::TauJetContainer::const_iterator hltItr, hlt_cont_end = hlt_cont->end();
  xAOD::EmTauRoIContainer::const_iterator L1Itr, L1_cont_end = L1_cont->end();

  std::vector<bool> truth_matched_to_L1;
  std::vector<bool> truth_matched_to_hlt;
  std::vector<bool> truth_matched_to_reco;
  std::vector<bool> truthReco_matched_to_L1;
  std::vector<bool> truthReco_matched_to_hlt;

  for (truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
    {
      if(abs((*truthItr)->pdgId()) == 15) ATH_MSG_DEBUG("Tau with status " << (*truthItr)->status() << " and charge " << (*truthItr)->charge());
      if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) // || (*truthItr)->status() == 10902))
        {
	  examineTruthTau(**truthItr);
	  pt  = (*truthItr)->auxdata<double>("pt_vis");
	  eta = (*truthItr)->auxdata<double>("eta_vis");
	  phi = (*truthItr)->auxdata<double>("phi_vis");
	  m   = (*truthItr)->auxdata<double>("m_vis");
	  lep = (*truthItr)->auxdata<bool>("IsLeptonicTau");
	  ntracks = (*truthItr)->auxdata<int>("nTracks");
	  if(pt < m_effOffTauPtCut || lep || fabs(eta) > 2.47 ) continue;
	  if(m_selection_nTrkMax>-1 && ntracks>m_selection_nTrkMax) continue;
   	  if(m_selection_nTrkMin>-1 && ntracks<m_selection_nTrkMin) continue;
   	  if(m_selection_ptMax>-1 && pt>m_selection_ptMax) continue;
   	  if(m_selection_ptMin>-1 && pt<m_selection_ptMin) continue;
   	  if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) continue;
   	  if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) continue;
   	  if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) continue;
   	  if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) continue;

          TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
	  
	  truth_matched_to_L1.push_back(false);
	  truth_matched_to_hlt.push_back(false);
	  truth_matched_to_reco.push_back(false);
          truthReco_matched_to_L1.push_back(false);
          truthReco_matched_to_hlt.push_back(false);

	  //Primary Vertices
          nvtx = PrimaryVertices();
          //Pile up
          mu = Pileup();
	  
	  // get list of truth-matched reco taus
          for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
            {
	      if(!Selection(*recoItr)) continue;
              TauTLV = (*recoItr)->p4();
              double eta_Tau = TauTLV.Eta();
              if(fabs(eta_Tau) > 2.47) continue;
 	      double pt_Tau = TauTLV.Pt();
	      if(pt_Tau<m_effOffTauPtCut) continue;
	      int ntrack_TAU = (*recoItr)->nTracks();
	      if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
   	      //bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
              //bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
              //bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
              //bool best_tau = good_tau && not_a_electron && not_a_muon;
  	      //if(!best_tau) continue;
              double dR = TauTLV.DeltaR(TruthTauTLV);
              if(dR <= 0.2) truth_matched_to_reco.back()=true;
            }
	  
	  // matching with HLT
	  if(trigItem == "Dump")
	    {
	      for(hltItr=hlt_cont->begin(); hltItr!=hlt_cont_end; ++hltItr)
		{
		  TauTLV = (*hltItr)->p4();
		  double dR = TauTLV.DeltaR(TruthTauTLV);
		  if(dR <= 0.2) truth_matched_to_hlt.back()=true;
		  if( dR <= 0.2 && truth_matched_to_reco.back() ) truthReco_matched_to_hlt.back()=true;
		}
	    }
	  else 
	    {
	      bool HLTTaumatched = HLTTauMatching(trigItem, TruthTauTLV, 0.2);
	      if(HLTTaumatched) truth_matched_to_hlt.back()=true;
	      if(HLTTaumatched && truth_matched_to_reco.back()) truthReco_matched_to_hlt.back()=true;
	      if(truth_matched_to_hlt.back()) ATH_MSG_DEBUG("HLT tau matched to truth taus");
	      if(truthReco_matched_to_hlt.back()) ATH_MSG_DEBUG("HLT tau matched to truth+reco taus");
	    }
	  
          // matching with L1
          if(trigItem=="Dump")
	    {
	      for(L1Itr=L1_cont->begin(); L1Itr!=L1_cont_end; ++L1Itr)
		{
		  float L1_eta = (*L1Itr)->eta();
		  float L1_phi = (*L1Itr)->phi();
		  float dR = deltaR(L1_eta,TruthTauTLV.Eta(),L1_phi,TruthTauTLV.Phi());
		  if(dR <= 0.3) truth_matched_to_L1.back()=true;
		  if( dR <= 0.3 && truth_matched_to_reco.back() ) truthReco_matched_to_L1.back()=true;
		}
	    } 
	  else 
	    {
	      bool L1Taumatched = L1TauMatching(l1_chain, TruthTauTLV, 0.3);
	      if(L1Taumatched) truth_matched_to_L1.back()=true;
	      if(L1Taumatched && truth_matched_to_reco.back() ) truthReco_matched_to_L1.back()=true;
	      if(truth_matched_to_L1.back()) ATH_MSG_DEBUG("L1 tau matched to truth tau");
	      if(truthReco_matched_to_L1.back()) ATH_MSG_DEBUG("L1 tau matched to truth+reco tau");     
	    }
	  
	  if(TauCont_type == "Truth")
            {
	      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/TruthEfficiency");
	      
	      hist("hTrueTauPtDenom")->Fill(pt/1000.);
	      if(ntracks == 1) hist("hTrueTauPt1PDenom")->Fill(pt/1000.);
	      if(ntracks > 1) hist("hTrueTauPt3PDenom")->Fill(pt/1000.);
	      hist("hTrueTauEtaDenom")->Fill(eta);
	      hist("hTrueTauPhiDenom")->Fill(phi);
	      hist("hTrueTauNTrackDenom")->Fill(ntracks);
	      hist("hTrueTauNVtxDenom")->Fill(nvtx);
              hist("hTrueTauMuDenom")->Fill(mu);
	      hist2("hTrueTauEtaVsPhiDenom")->Fill(eta,phi);
	      
	      if(truth_matched_to_L1.back())
		{
		  hist("hTrueL1PtNum")->Fill(pt/1000.);
		  if(ntracks == 1) hist("hTrueL1Pt1PNum")->Fill(pt/1000.);
		  if(ntracks > 1) hist("hTrueL1Pt3PNum")->Fill(pt/1000.);
		  hist("hTrueL1EtaNum")->Fill(eta);
		  hist("hTrueL1PhiNum")->Fill(phi);
		  hist("hTrueL1NTrackNum")->Fill(ntracks);
		  hist("hTrueL1NVtxNum")->Fill(nvtx);
		  hist("hTrueL1MuNum")->Fill(mu);
		  hist2("hTrueL1EtaVsPhiNum")->Fill(eta,phi);
		}
              if(truth_matched_to_hlt.back())
		{
		  hist("hTrueHLTPtNum")->Fill(pt/1000.);
		  if(ntracks == 1) hist("hTrueHLTPt1PNum")->Fill(pt/1000.);
		  if(ntracks > 1) hist("hTrueHLTPt3PNum")->Fill(pt/1000.);
		  hist("hTrueHLTEtaNum")->Fill(eta);
		  hist("hTrueHLTPhiNum")->Fill(phi);
		  hist("hTrueHLTNTrackNum")->Fill(ntracks);
		  hist("hTrueHLTNVtxNum")->Fill(nvtx);
		  hist("hTrueHLTMuNum")->Fill(mu);
		  hist2("hTrueHLTEtaVsPhiNum")->Fill(eta,phi);
		}
            }
	  else if(TauCont_type == "Truth+Reco")
            {
	      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/Truth+RecoEfficiency");
	      
	      if(truth_matched_to_reco.back())
		{	
		  hist("hTruthRecoTauPtDenom")->Fill(pt/1000.);
		  if(ntracks == 1) hist("hTruthRecoTauPt1PDenom")->Fill(pt/1000.);
		  if(ntracks > 1) hist("hTruthRecoTauPt3PDenom")->Fill(pt/1000.);
		  hist("hTruthRecoTauEtaDenom")->Fill(eta);
		  hist("hTruthRecoTauPhiDenom")->Fill(phi);
		  hist("hTruthRecoTauNTrackDenom")->Fill(ntracks);
		  hist("hTruthRecoTauNVtxDenom")->Fill(nvtx);
		  hist("hTruthRecoTauMuDenom")->Fill(mu);
		  hist2("hTruthRecoTauEtaVsPhiDenom")->Fill(eta,phi);

		  if(truthReco_matched_to_L1.back())
		    {
		      hist("hTruthRecoL1PtNum")->Fill(pt/1000.);
		      if(ntracks == 1) hist("hTruthRecoL1Pt1PNum")->Fill(pt/1000.);
		      if(ntracks > 1) hist("hTruthRecoL1Pt3PNum")->Fill(pt/1000.);
		      hist("hTruthRecoL1EtaNum")->Fill(eta);
		      hist("hTruthRecoL1PhiNum")->Fill(phi);
		      hist("hTruthRecoL1NTrackNum")->Fill(ntracks);
		      hist("hTruthRecoL1NVtxNum")->Fill(nvtx);
		      hist("hTruthRecoL1MuNum")->Fill(mu);
		      hist2("hTruthRecoL1EtaVsPhiNum")->Fill(eta,phi);
		    }
		  if(truthReco_matched_to_hlt.back())
		    {
		      hist("hTruthRecoHLTPtNum")->Fill(pt/1000.);
		      if(ntracks == 1) hist("hTruthRecoHLTPt1PNum")->Fill(pt/1000.);
		      if(ntracks > 1) hist("hTruthRecoHLTPt3PNum")->Fill(pt/1000.);
		      hist("hTruthRecoHLTEtaNum")->Fill(eta);
		      hist("hTruthRecoHLTPhiNum")->Fill(phi);
		      hist("hTruthRecoHLTNTrackNum")->Fill(ntracks);
		      hist("hTruthRecoHLTNVtxNum")->Fill(nvtx);
		      hist("hTruthRecoHLTMuNum")->Fill(mu);
		      hist2("hTruthRecoHLTEtaVsPhiNum")->Fill(eta,phi);
		    }
		}
            }
	  else
	    {
	      ATH_MSG_WARNING("Failed to fill the histograms. Exiting.");
	      return StatusCode::FAILURE;
	    }
	}
    }//End of Truth Particle Container iter
    
  return StatusCode::SUCCESS;
}

//StatusCode HLTTauMonTool::RecoTauEfficiency(const std::string & trigItem)
//{
//  ATH_MSG_DEBUG("Recoh Tau Matching to L1 and HLT Taus for trigItem" << trigItem);
// 
//  std::string l1_chain(LowerChain("HLT_"+trigItem));
// 
//  double pt = 0, eta = 0, phi = 0;
//  int ntracks = 0, nvtx = 0;
//  float mu = 0;
//  nvtx = PrimaryVertices();
//  mu = Pileup();
//      
//  TLorentzVector RecTauTLV(0.,0.,0.,0.);
//  TLorentzVector TauTLV(0.,0.,0.,0.);
//  
//  const xAOD::TauJetContainer * reco_cont = 0;
//  const xAOD::TauJetContainer * hlt_cont = 0;
//  const xAOD::EmTauRoIContainer * L1_cont = 0;
//  
//  if( evtStore()->retrieve(hlt_cont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged").isFailure() ){
//    ATH_MSG_WARNING("Failed to retrieve  HLT_xAOD__TauJetContainer_TrigTauRecMerged container. Exiting.");
//    return StatusCode::FAILURE;
//  }
//  
//  if( m_storeGate->retrieve(L1_cont, "LVL1EmTauRoIs").isFailure() ){
//    ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoIs container. Exiting.");
//    return StatusCode::FAILURE;
//  }
//  
//  if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() ){
//    ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
//    return StatusCode::FAILURE;
//  } 
//  
//  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
//  xAOD::TauJetContainer::const_iterator hltItr, hlt_cont_end = hlt_cont->end();
//  xAOD::EmTauRoIContainer::const_iterator L1Itr, L1_cont_end = L1_cont->end();
//
//  std::vector<bool> reco_matched_to_L1;
//  std::vector<bool> reco_matched_to_hlt;
//  
//  for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
//    {
//      RecTauTLV = (*recoItr)->p4();
//      eta = RecTauTLV.Eta();
//      phi = RecTauTLV.Phi();
//      if(fabs(eta) > 2.47) continue;
//      pt = RecTauTLV.Pt();
//      if(pt < m_effOffTauPtCut) continue;
//      ntracks = (*recoItr)->nTracks();
//      if(ntracks !=1 && ntracks !=3) continue;
//      //bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
//      //bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
//      //bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
//      //bool best_tau = good_tau && not_a_electron && not_a_muon;
//      //if(!best_tau) continue;
//
//      reco_matched_to_L1.push_back(false);
//      reco_matched_to_hlt.push_back(false);
//
//      // matching with HLT
//      if(trigItem == "Dump")
//	{
//	  for(hltItr=hlt_cont->begin(); hltItr!=hlt_cont_end; ++hltItr)
//	    {
//	      TauTLV = (*hltItr)->p4();
//	      double dR = TauTLV.DeltaR(RecTauTLV);
//	      if(dR <= 0.2) reco_matched_to_hlt.back()=true;
//	    }
//	}
//      else 
//	{
//	  bool HLTTaumatched = HLTTauMatching(trigItem, RecTauTLV, 0.2);
//	  if(HLTTaumatched) reco_matched_to_hlt.back()=true;
//	  if(reco_matched_to_hlt.back()) ATH_MSG_DEBUG("HLT tau matched to reco taus");
//	}
//      
//      // matching with L1
//      if(trigItem=="Dump")
//	{
//	  for(L1Itr=L1_cont->begin(); L1Itr!=L1_cont_end; ++L1Itr)
//	    {
//	      float L1_eta = (*L1Itr)->eta();
//	      float L1_phi = (*L1Itr)->phi();
//	      float dR = deltaR(L1_eta,RecTauTLV.Eta(),L1_phi,RecTauTLV.Phi());
//	      if(dR <= 0.3) reco_matched_to_L1.back()=true;
//	    }
//	} 
//      else 
//	{
//	  bool L1Taumatched = L1TauMatching(l1_chain, RecTauTLV, 0.3);
//	  if(L1Taumatched) reco_matched_to_L1.back()=true;
//	  if(reco_matched_to_L1.back()) ATH_MSG_DEBUG("L1 tau matched to reco tau");
//	}
//      
//      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/RecoEfficiency");
//      
//      hist("hRecoTauPtDenom")->Fill(pt/1000.);
//      if(ntracks == 1) hist("hRecoTauPt1PDenom")->Fill(pt/1000.);
//      if(ntracks > 1) hist("hRecoTauPt3PDenom")->Fill(pt/1000.);
//      hist("hRecoTauEtaDenom")->Fill(eta);
//      hist("hRecoTauPhiDenom")->Fill(phi);
//      hist("hRecoTauNTrackDenom")->Fill(ntracks);
//      hist("hRecoTauNVtxDenom")->Fill(nvtx);
//      hist("hRecoTauMuDenom")->Fill(mu);
//      hist2("hRecoTauEtaVsPhiDenom")->Fill(eta,phi);
//
//      if(reco_matched_to_L1.back())
//	{
//	  hist("hRecoL1PtNum")->Fill(pt/1000.);
//	  if(ntracks == 1) hist("hRecoL1Pt1PNum")->Fill(pt/1000.);
//	  if(ntracks > 1) hist("hRecoL1Pt3PNum")->Fill(pt/1000.);
//	  hist("hRecoL1EtaNum")->Fill(eta);
//	  hist("hRecoL1PhiNum")->Fill(phi);
//	  hist("hRecoL1NTrackNum")->Fill(ntracks);
//	  hist("hRecoL1NVtxNum")->Fill(nvtx);
//	  hist("hRecoL1MuNum")->Fill(mu);
//	  hist2("hRecoL1EtaVsPhiNum")->Fill(eta,phi);
//	}
//      if(reco_matched_to_hlt.back())
//	{
//	  hist("hRecoHLTPtNum")->Fill(pt/1000.);
//	  if(ntracks == 1) hist("hRecoHLTPt1PNum")->Fill(pt/1000.);
//	  if(ntracks > 1) hist("hRecoHLTPt3PNum")->Fill(pt/1000.);
//	  hist("hRecoHLTEtaNum")->Fill(eta);
//	  hist("hRecoHLTPhiNum")->Fill(phi);
//	  hist("hRecoHLTNTrackNum")->Fill(ntracks);
//	  hist("hRecoHLTNVtxNum")->Fill(nvtx);
//	  hist("hRecoHLTMuNum")->Fill(mu);
//	  hist2("hRecoHLTEtaVsPhiNum")->Fill(eta,phi);
//	}
//	
//    }//End of reco_cont Iter
// return StatusCode::SUCCESS;
//}

bool HLTTauMonTool::HLTTauMatching(const std::string & trigItem, const TLorentzVector & TLV, double DR)
{
  if(trigItem == "Dump")
    {
      ATH_MSG_WARNING("No HLT matching for Dump Item return false");
      return false;
    }
  
  std::string trig_item_EF= "HLT_"+trigItem;
  //std::string l1_chain(LowerChain("HLT_"+trigItem));

  if(getTDT()->isPassed(trig_item_EF)  )
    {
      ATH_MSG_DEBUG("HTLTauMonTool::TruthTauEfficiency event passed " << trig_item_EF << ", obtaining list of features now");
      Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF) );
      Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
      if(comb==combEnd)
      	{
      	  ATH_MSG_DEBUG("No features for chain " << trig_item_EF << ", HLTMatching return false");
      	  return false;
      	}
      
      int nComb(0);
      for(;comb!=combEnd;++comb)
	{
	  ATH_MSG_DEBUG("-------- Trigger " << trigItem  << ", combination " << nComb << "-th, active? "<< comb->active() << ", TEs "<< comb->tes().size());
	  nComb++;
	  
	  const std::vector<Trig::Feature<xAOD::TauJetContainer> >  vec_tauEF = comb->get<xAOD::TauJetContainer>();
	  std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator Itr_tauEF = vec_tauEF.begin(), vec_tauEF_end = vec_tauEF.end();
 		  
	  ATH_MSG_DEBUG("vector of EF TauJetsContainer, size: " << vec_tauEF.size());
	  
	  for(;Itr_tauEF!=vec_tauEF_end;++Itr_tauEF)
	    {
	      if(Itr_tauEF->cptr())
		{
		  xAOD::TauJetContainer::const_iterator Itr_tauEFjet = Itr_tauEF->cptr()->begin(), Itr_tauEF_end = Itr_tauEF->cptr()->end();
		  ATH_MSG_DEBUG("TauJetsContainer, " << Itr_tauEF->cptr()->size() << " HLT taus passed");
		  
		  for(;Itr_tauEFjet!=Itr_tauEF_end;++Itr_tauEFjet)
		    {
 		      if(*Itr_tauEFjet)
			{
			  ATH_MSG_DEBUG("HLT tau: pt " << (*Itr_tauEFjet)->pt() << ", #trk " << (*Itr_tauEFjet)->nTracks());
			  double delR = (*Itr_tauEFjet)->p4().DeltaR(TLV);
			  if(delR <= DR) return true;
			}			
		    }
		}
	    }  
	}
    }
 return false;
}

bool HLTTauMonTool::L1TauMatching(const std::string & l1_item, const TLorentzVector & TLV, double DR)
{
  if(l1_item == "Dump")
    {
      ATH_MSG_DEBUG("No L1 matching for Dump Item return false");
      return false;
    }

  if(getTDT()->isPassed( l1_item ))
    {     
      ATH_MSG_DEBUG("HTLTauMonTool::TruthTauEfficiency event passed " << l1_item << ", obtaining list of features now");
      
      Trig::FeatureContainer f = ( getTDT()->features(l1_item) );
      Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
      if(comb==combEnd)
	{
	  ATH_MSG_DEBUG("No features for chain " <<l1_item <<", L1Matching return false");
	  return false;
	}
      for(;comb!=combEnd;++comb)
	{
	  const std::vector<Trig::Feature<TrigRoiDescriptor> >  vec_tauL1 = comb->get<TrigRoiDescriptor>("initialRoI");
	  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator Itr_tauL1 = vec_tauL1.begin(), vec_tauL1_end = vec_tauL1.end();
	  
	  ATH_MSG_DEBUG("HTLTauMonTool::TruthTauEfficiency " << vec_tauL1.size() << " L1 taus passed");
	  
	  for(;Itr_tauL1!=vec_tauL1_end;++Itr_tauL1)
	    {
	      if(Itr_tauL1->cptr())
		{
		  float L1_eta = Itr_tauL1->cptr()->eta();
		  float L1_phi = Itr_tauL1->cptr()->phi();
		  float delR = deltaR(L1_eta,TLV.Eta(),L1_phi,TLV.Phi());
		  if(delR <= DR) return true;
		}
	    }
	}
    }
  return false;
}

int HLTTauMonTool::PrimaryVertices(){
  int nGoodVtx(0);
  const xAOD::VertexContainer* primary = 0;
  if( evtStore()->retrieve(primary, "PrimaryVertices" ).isFailure() ){
    ATH_MSG_DEBUG("Failed to retrieve PrimaryVertices container, returning -1!");
    return -1;
    }
  
  xAOD::VertexContainer::const_iterator vtxIt = primary->begin(), vtxIt_e = primary->end();
  for(;vtxIt!=vtxIt_e;++vtxIt){
   /// if((*vtxIt)->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
   if((*vtxIt)->nTrackParticles() < 3 )  continue; 
   nGoodVtx++;
   }

  return nGoodVtx;
}

float HLTTauMonTool::Pileup(){
  float Pileup(0.);
  const xAOD::EventInfo* evtInfo = 0;
  if( evtStore()->retrieve(evtInfo, "EventInfo" ).isFailure() ){
    ATH_MSG_DEBUG("Failed to retrieve EventInfo container, returning -1!");
    return -1;
    }
  Pileup = evtInfo->averageInteractionsPerCrossing();
  return Pileup;
}

//=====================================================================================

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
            if ( (checkMode == 1 && val1 > checkVal) || (checkMode == 2 && fabs(val1) > checkVal) ){
                val1 = (val2 - val1) / val1;
            
               // if (val1 >= hist->GetXaxis()->GetXmax()) {
               //  int nbins = hist->GetNbinsX();
               //   val1 = hist->GetBinLowEdge(nbins)+0.5*hist->GetBinWidth(nbins);
               // }
               // else if (val1 < hist->GetXaxis()->GetXmin())
               //    val1 = hist->GetXaxis()->GetXmin();
            
                hist->Fill(val1);
	  }
        }
        //else hist->Fill(hist->GetXaxis()->GetXmin()); // fill invalid values in the first visible bin instead of the underflow bin
    }
    //else hist->Fill(hist->GetXaxis()->GetXmin()); // fill invalid values in the first visible bin instead of the underflow bin
    
}

template <class T> void HLTTauMonTool::FillRelDiffProfile(TProfile *prof, float val1, float val2, T val3, float checkVal, int checkMode)
{
  if(val1 != Analysis::TauDetails::DEFAULT)
    {
      if(val2 != Analysis::TauDetails::DEFAULT)
        {
          if((checkMode == 1 && val1 > checkVal) || (checkMode == 2 && fabs(val1) > checkVal))
            {
              val1 = (val2 - val1) / val1;
              prof->Fill(val3, val1);
            }
        }
    }
}

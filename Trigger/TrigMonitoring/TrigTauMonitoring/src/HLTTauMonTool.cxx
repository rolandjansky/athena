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

#include "JetTagEvent/TrackAssociation.h"
#include "Particle/TrackParticleContainer.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/JetTagInfoBase.h"
#include "JetEvent/Jet.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "TrigParticle/TrigTau.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"

#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "TrigParticle/TrigTauContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
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


#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "HLTTauMonTool.h"


using namespace std;
using namespace AnalysisUtils;
const float PI=2.0*acos(0.);
const float TWOPI=2.0*PI;
HLTTauMonTool::HLTTauMonTool(const std::string & type, const std::string & n, const IInterface* parent)
  : IHLTMonTool(type, n, parent){
  //    m_trigDecTool("TrigDec::TrigDecisionTool/oldTrigDecisionTool") {
  
  ATH_MSG_DEBUG("HLTTauMonTool::HLTTauMonTool()");

  declareProperty("tauTriggItem", m_trigItems);
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

  //  StatusCode sc = m_trigDecTool.retrieve();
  //  if(!sc.isSuccess()) {
  //    ATH_MSG_ERROR("Unable to retrieve the TrigDecisionTool !!");
  //    return sc;
  //  }
  //  ATH_MSG_DEBUG("  --> Found AlgTool \"TrigDecisionTool\"");


 
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
      std::string trig_item = "EF_"+m_trigItems[j];
      if (m_trigItems[j] != "Dump" && m_trigItems[j] != "EF_tauNoCut_cosmic"){
// 	if (getTDT()->isPassed(trig_item))
	sc = fillHistogramsForItem(m_trigItems[j]); 
      }
      else sc = fillHistogramsForItem(m_trigItems[j]);
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
    ///L2 vs L1
    for(unsigned int i=0;i<m_trigItems.size();++i){
      if(m_trigItems[i]=="Dump") continue;
      divide(hist("hL2RoIEtaNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),
	     hist("hL1RoIEtaDenom", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"), 
	     hist("hL2vsL1EtaEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );
      
      divide(hist("hL2RoIPhiNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hL1RoIPhiDenom", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hL2vsL1PhiEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );

      divide(hist("hL2RoIEtNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hL1RoIEtDenom", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hL2vsL1EtEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );
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
      ///EF vs L2
      divide(hist("hEFRoIEtaNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hL2RoIEtaNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hEFvsL2EtaEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );
      
      divide(hist("hEFRoIPhiNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hL2RoIPhiNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hEFvsL2PhiEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );

      divide(hist("hEFRoIEtNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hL2RoIEtNum", "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency"),  
	     hist("hEFvsL2EtEfficiency" , "HLT/TauMon/"+m_trigItems[i]+"/RelativeEfficiency/Efficiency") );
    
      if(m_fakeJetEff) {
	divide(hist("LeadJet1_eta_num_1p3p", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_eta_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_eta_1p3p_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet2_eta_num_1p3p", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_eta_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_eta_1p3p_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet1_phi_num_1p3p", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_phi_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_phi_1p3p_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet2_phi_num_1p3p", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_phi_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_phi_1p3p_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet1_pt_num_1p3p", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_pt_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_pt_1p3p_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet2_pt_num_1p3p", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_pt_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_pt_1p3p_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );


	divide(hist("LeadJet1_eta_num_tauRec", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_eta_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_eta_tauRec_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet2_eta_num_tauRec", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_eta_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_eta_tauRec_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet1_phi_num_tauRec", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_phi_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_phi_tauRec_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet2_phi_num_tauRec", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_phi_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_phi_tauRec_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet1_pt_num_tauRec", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_pt_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet1_pt_tauRec_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );

	divide(hist("LeadJet2_pt_num_tauRec", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_pt_denom", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets"),
	       hist("LeadJet2_pt_tauRec_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Efficiency") );


	divide(hist("LeadJet1_pt_num_tauRec_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_1540_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_tauRec_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_1540_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_tauRec_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_4080_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet2_pt_num_tauRec_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_4080_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet1_pt_num_tauRec_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_80120_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_tauRec_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_80120_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_tauRec_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_120160_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_tauRec_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_120160_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_tauRec_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_160200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_tauRec_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_160200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_tauRec_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_200400_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_tauRec_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_200400_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_tauRec_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_400800_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_tauRec_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_400800_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet1_pt_num_tauRec_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tauRec_8001200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet2_pt_num_tauRec_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tauRec_8001200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );



	divide(hist("LeadJet1_pt_num_1p3p_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_1540_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_1p3p_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_1540", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_1540_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_1p3p_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_4080_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet2_pt_num_1p3p_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_4080", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_4080_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet1_pt_num_1p3p_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_80120_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_1p3p_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_80120", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_80120_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_1p3p_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_120160_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_1p3p_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_120160", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_120160_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_1p3p_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_160200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_1p3p_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_160200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_160200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_1p3p_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_200400_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_1p3p_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_200400", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_200400_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet1_pt_num_1p3p_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_400800_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );

	divide(hist("LeadJet2_pt_num_1p3p_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_400800", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_400800_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet1_pt_num_1p3p_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_denom_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet1_pt_tau1p3p_8001200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );


	divide(hist("LeadJet2_pt_num_1p3p_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_denom_8001200", "HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned"),
	       hist("LeadJet2_pt_tau1p3p_8001200_Eff","HLT/TauMon/"+m_trigItems[i]+"/FakeJets/Binned/Efficiency") );
      }
      if(m_offlineEff) {
	// Eta
	divide(hist("hTauEtaL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauEta", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioEta" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauEtaL2", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauEta", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL2OfflineRatioEta" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauEtaEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauEta", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioEta" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	// Phi
	divide(hist("hTauPhiL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPhi", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioPhi" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );
      
     	divide(hist("hTauPhiL2", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPhi", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL2OfflineRatioPhi" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauPhiEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPhi", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioPhi" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	// Pt      
	divide(hist("hTauPtL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPt", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioPt" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauPtL2", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPt", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL2OfflineRatioPt" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );
      
	divide(hist("hTauPtEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauPt", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioPt" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	// Nvtx
	divide(hist("hTauNvtxL1", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauNvtx", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL1OfflineRatioNvtx" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauNvtxL2", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauNvtx", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hL2OfflineRatioNvtx" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );

	divide(hist("hTauNvtxEF", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"),
	       hist("hTauNvtx", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio"), 
	       hist("hEFOfflineRatioNvtx" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio") );


      }

      if(m_offlineEffBDTMedium) {
	// Eta
	divide(hist("hTauEtaL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );
	
	divide(hist("hTauEtaL2BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL2OfflineRatioEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	divide(hist("hTauEtaEFBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hEFOfflineRatioEtaBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	// Phi
	divide(hist("hTauPhiL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );
	
	divide(hist("hTauPhiL2BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL2OfflineRatioPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	divide(hist("hTauPhiEFBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hEFOfflineRatioPhiBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );
	
	// Pt
	divide(hist("hTauPtL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	divide(hist("hTauPtL2BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL2OfflineRatioPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );
	
	divide(hist("hTauPtEFBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hEFOfflineRatioPtBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	// Nvtx
	divide(hist("hTauNvtxL1BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauNvtxBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL1OfflineRatioNvtxBDTMedium" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

	divide(hist("hTauNvtxL2BDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"),
	       hist("hTauNvtxBDTMedium", "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/BDTMedium"), 
	       hist("hL2OfflineRatioNvtxBDTMedium" , "HLT/TauMon/"+m_trigItems[i]+"/OfflineRatio/Ratio/BDTMedium") );

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

  //addHistogram(new TH1F("hL1RoINotMatchedEta","L1 RoI Eta of TrigRoiDescriptor not mathed to any LVL1_ROI; #eta; Nevents",26,-2.6,2.6));
  
  addHistogram(new TH1F("hL1RoITauClus","L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI",40,0.,100.));
  addHistogram(new TH1F("hL1RoIEMIso","L1 RoI EM Isol ; E_{T}^{EM Isol}[GeV]; N RoI",16,-2,30));
  addHistogram(new TH1F("hL1RoIHadCore","L1 RoI HAD Core ; E_{T}^{HAD}[GeV]; N RoI",16,-2,30));
  addHistogram(new TH1F("hL1RoIHadIsol","L1 RoI HAD Isol ; E_{T}^{HAD Isol}[GeV]; N RoI",16,-2,30));

  
  //--------------------
  // L2 TrigCaloCLuster
  //--------------------
  addMonGroup(new MonGroup(this,"HLT/TauMon/"+trigItem+"/T2CaloTau",run));
  addHistogram(new TH1F("hEta","L2 TrigCaloCluster, #eta; #eta ; N Clusters",26,-2.6,2.6));
  addHistogram(new TH1F("hPhi","L2 TrigCaloCluster, #phi; #phi ; N Clusters",32,-3.2,3.2));
  addHistogram(new TH2F("hEtaVsPhi","L2 TrigCaloCluster Eta vs Phi; #eta ; #phi ; N Clusters",26,-2.6,2.6,32,-3.2,3.2));

  addHistogram(new TH1F("hEMenergy","L2 TrigCaloCluster, EM energy; E^{EM}[GeV];N Clusters",21,-5,100));
  addHistogram(new TH1F("hHADenergy","L2 TrigCaloCluster, HAD energy; E^{HAD}[GeV];N Clusters",21,-5,100));

  addHistogram(new TH1F("hEMRadius3S","L2 TrigCaloCluster, weighted EM Calo radius, 3 samplings; EMRadius3S ; N Clusters",22,-0.02,0.2));
  addHistogram(new TH1F("hCaloRadius","L2 TrigCaloCluster, weighted EM Calo radius; CaloRadius ; N Clusters",22,-0.02,0.2));
  
  addHistogram(new TH1F("hIsoFrac","L2 TrigCaloCluster, Isolation fraction; IsolFrac; N Clusters",21,-0.1,2.0));
  addHistogram(new TH1F("hStripWidth","L2 TrigCaloCluster, strip width; StripWidth ; N Clusters",42,-0.01,0.2));

  addHistogram(new TH1F("hEtRawWide","L2 TrigCaloCluster, E_{T} in wide cone ;E_{T}[GeV] ;N Clusters",40,0.,100.));
  addHistogram(new TH1F("hEtRawMedium","L2 TrigCaloCluster, E_{T} in medium cone ;E_{T}[GeV] ;N Clusters",40,0.,100.));
  addHistogram(new TH1F("hEtRawNarrow","L2 TrigCaloCluster, E_{T} in narrow cone ;E_{T}[GeV] ;N Clusters",40,0.,100.));
  addHistogram(new TH1F("hEMFraction","L2 TrigCaloCluster, EM/(EM+HAD); EMFraction; N Clusters",25,-0.05,1.2));
  addHistogram(new TH1F("hCoreFraction","L2 TrigCaloCluster, Core fraction; CoreFrac; N Clusters",25,-0.05,1.2));

  //--------------------
  // L2 ID
  //--------------------
  if (m_TracksInfo){
    addMonGroup(new MonGroup(this,"HLT/TauMon/"+trigItem+"/T2IDTau",run));
    addHistogram(new TH1F("hLeadingTkPt","L2 leading tk. p_{T};p_{T}[GeV];Nevents",20,-5.,75.));
    addHistogram(new TH1F("hNCoreTracks","L2 ID number of core tracks;n core tracks;Nevents",10,0,10));
    addHistogram(new TH1F("hNIsoTracks","L2 ID number of isolation tracks;n iso tracks;Nevents",10,0,10));
    addHistogram(new TH1F("hNSlowTracks","L2 ID number of slow tracks;N slow tracks;Nevents",10,0,10));
    addHistogram(new TH1F("hCharge","L2 ID candidate charge;charge;Nevents",10,-5.0,5.0));
    addHistogram(new TH1F("hScalarSumPtCore","L2 ID scalar sum of tracks p_{T} in core;p_{T} sum core [GeV];Nevents",20,-5.,75.));
    addHistogram(new TH1F("hScalarSumPtIso","L2 ID scalar sum of tracks p_{T} in isol;p_{T} sum iso [GeV];Nevents",20,-5.,75.));
    addHistogram(new TH1F("hSumPtRatio","L2 ID scalar sum of tracks p_{T} in iso/core;Ratio;Nevents", 26, -0.04, 1.0));
//     addHistogram(new TH1F("hDeltaZ0wrtLdTk","L2 ID dZ0 w.r.t. leading track;dZ0[mm];Ntracks",201,-2.01,2.01));
  }

  
  //--------------------
  //L2 Final
  //--------------------
  addMonGroup(new MonGroup(this,"HLT/TauMon/"+trigItem+"/T2TauFinal",run));

  addHistogram(new TH1F("hL2EFlow","L2 simple E flow E_{T};E_{T}[GeV];Nevents",40,0.0,100.0));  
  addHistogram(new TH1F("hEtCombined","L2 simple Et combined from tracks direction and cluster energy;ET [GeV];Nevents",40,0.0,100.0));    
  addHistogram(new TH1F("hL2TrkAvgDist", "L2 track average distance;trkAvgDist;Nevents",32,-0.01,0.31));
  addHistogram(new TH1F("hL2EtOverPtLeadTrk","L2 et over leading trk. p_{T};etOverPtLeadTrk;Nevents",22,-0.5,10.5));  

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
  addHistogram(new TH1F("hEFDRmax", "Max dR of associated tracks at EF; dRmax at EF; Candidates",21,-0.005,0.205));
  addHistogram(new TH1F("hEFEMFraction", "Em Fraction at EF; EM Fraction at EF; Candidates",44,-0.05,1.05));
  addHistogram(new TH1F("hEFTopoInvMass", "TopoInvMass at EF; TopoInvMass at EF [GeV]; Candidates",51,-0.2,20.2));
  addHistogram(new TH1F("hEFMassTrkSys", "MassTrkSys at EF; MassTrkSys at EF [GeV]; Candidates",26,-0.2,5.0));
  addHistogram(new TH1F("hEFIpSigLeadTrk", "IpSigLeadTrk at EF; IpSigLeadTrk at EF; Candidates",40,-10.0,10.0));
  addHistogram(new TH1F("hEFTrFlightPathSig", "TrFlightPathSig at EF; TrFlightPathSig at EF; Candidates",40,-10.0,10.0));
  addHistogram(new TH1F("hAuthor","Author of EF Tau;1-tauRec 2-tau1p3p 3-both 4-tauRec Exc 5-tau1p3p Exc;Nevents",6,0,6));


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
  addHistogram(new TH1F("hIsoFracRatio","Relative difference in IsoFrac;IsoFrac relative diff; Candidates",50,-0.5,0.5));
  addHistogram(new TH1F("hEtOverPtLeadTrkRatio","Relative difference in EtOvLdPt ;EtOvLeadPtTrack relative diff; Candidates",50,-0.5,0.5));

  addHistogram(new TH1F("hCentFracRatio","Relative difference in Centrality Fraction (EF-Offline)/Offline; CentFrac relative diff; Candidates",50,-0.5,0.5));
  addHistogram(new TH1F("hNWideTrackRatio","Relative difference in number of wide tracks (EF-Offline)/Offline; NWideTrack relative diff; Candidates", 50,-2.5,2.5));
  addHistogram(new TH1F("hNTrackRatio","Relative difference in number of tracks (EF-Offline)/Offline; NTrack relative diff; Candidates", 20,-1.0,1.0));
  addHistogram(new TH1F("hIpSigLeadTrkRatio","Relative difference in IpSigLeadTrk (EF-Offline)/Offline; IpSigLeadTrk relative diff; Candidates",80,-4.0,4.0));
  addHistogram(new TH1F("hTrFlightPathSigRatio","Relative difference in TrFlightPathSig (EF-Offline)/Offline; TrFlightPathSig relative diff; Candidates", 80,-4.0,4.0));
  addHistogram(new TH1F("hMassTrkSysRatio","Relative difference in MassTrkSys (EF-Offline)/Offline; MassTrkSys relative diff; Candidates", 40,-2.0,2.0));
  addHistogram(new TH1F("hDRmaxRatio","Relative difference in DRmax (EF-Offline)/Offline; DRmax relative diff; Candidates", 40,-2.0,2.0));
  addHistogram(new TH1F("hTrkAvgDistRatio","Relative difference in trkAvgDist (EF-Offline)/Offline; TrkAvgDist relative diff; Candidates",40,-2.0,2.0));

  // Temp variables to debug hIpSigLeadTrkRatio
//   addHistogram(new TH1F("hEFZ0","EF Z_{0}; Z_{0} [mm]; nTrks", 120, -300.0, 300.0));
//   addHistogram(new TH1F("hOffZ0","Offline Z_{0}; Z_{0} [mm]; nTrks", 120, -300.0, 300.0));

  //--------------------
  //Efficiency
  //--------------------
  if(trigItem!="Dump"){
    addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/RelativeEfficiency", run, ATTRIB_MANAGED, "") );

    addHistogram(new TH1F("hL1RoIEtaDenom","L1 ROI, #eta; #eta ; Nevents",26,-2.6,2.6));
    addHistogram(new TH1F("hL1RoIPhiDenom","L1 ROI, #phi; #phi ; Nevents",32,-3.2,3.2));
    addHistogram(new TH1F("hL1RoIEtDenom","L1 ROI, E_{T}; E_{T} [GeV] ; Nevents",40,0,100));

    addHistogram(new TH1F("hL2RoIEtaNum","L1 ROI for items passing L2, #eta; #eta ; Nevents",26,-2.6,2.6));
    addHistogram(new TH1F("hL2RoIPhiNum","L1 ROI for items passing L2, #phi; #phi ; Nevents",32,-3.2,3.2));
    addHistogram(new TH1F("hL2RoIEtNum","L1 ROI for items passing L2, E_{T}; E_{T} [GeV] ; Nevents",40,0,100));

    addHistogram(new TH1F("hEFRoIEtaNum","L1 ROI for items passing EF, #eta; #eta ; Nevents",26,-2.6,2.6));
    addHistogram(new TH1F("hEFRoIPhiNum","L1 ROI for items passing EF, #phi; #phi ; Nevents",32,-3.2,3.2));
    addHistogram(new TH1F("hEFRoIEtNum","L1 ROI for items passing EF, E_{T}; E_{T} [GeV] ; Nevents",40,0,100));


    addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/RelativeEfficiency/Efficiency", run, ATTRIB_MANAGED, "", "effAsPerCent") );

    addHistogram(new TH1F("hL2vsL1EtaEfficiency","Relative Efficiency of L2 w.r.t.L1; #eta^{L1 RoI}; Efficiency",26,-2.6,2.6));
    addHistogram(new TH1F("hL2vsL1PhiEfficiency","Relative Efficiency of L2 w.r.t.L1; #phi^{L1 RoI}; Efficiency",32,-3.2,3.2));
    addHistogram(new TH1F("hL2vsL1EtEfficiency","Relative Efficiency of L2 w.r.t.L1; E_{T}^{L1 RoI} [GeV]; Efficiency",40,0,100));

    addHistogram(new TH1F("hEFvsL1EtaEfficiency","Relative Efficiency of EF w.r.t.L1; #eta^{L1 RoI}; Efficiency",26,-2.6,2.6));
    addHistogram(new TH1F("hEFvsL1PhiEfficiency","Relative Efficiency of EF w.r.t.L1; #phi^{L1 RoI}; Efficiency",32,-3.2,3.2));
    addHistogram(new TH1F("hEFvsL1EtEfficiency","Relative Efficiency of EF w.r.t.L1; E_{T}^{L1 RoI} [GeV]; Efficiency",40,0,100));

    addHistogram(new TH1F("hEFvsL2EtaEfficiency","Relative Efficiency of EF w.r.t.L2; #eta^{L1 RoI}; Efficiency",26,-2.6,2.6));
    addHistogram(new TH1F("hEFvsL2PhiEfficiency","Relative Efficiency of EF w.r.t.L2; #phi^{L1 RoI}; Efficiency",32,-3.2,3.2));
    addHistogram(new TH1F("hEFvsL2EtEfficiency","Relative Efficiency of EF w.r.t.L2; E_{T}^{L1 RoI} [GeV]; Efficiency",40,0,100));

      

    //--------------------
    //EF vs L2, L1
    //--------------------
    addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/EFvsL2L1", run, ATTRIB_MANAGED, "") );

    addHistogram(new TH2F("hEtL1VsL2","L1 vs L2: E_{T};L1 E_{T} [GeV];L2 E_{T} [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtaL1VsL2","L1 vs L2: #eta;L1 #eta;L2 #eta",26,-2.6,2.6,26,-2.6,2.6));
    addHistogram(new TH2F("hPhiL1VsL2","L1 vs L2: #phi;L1 #phi;L2 #phi",32,-3.2,3.2,32,-3.2,3.2));

    addHistogram(new TH2F("hEtEFVsL1","EF vs L1: E_{T};EF E_{T} [GeV];L1 E_{T} [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtaEFVsL1","EF vs L1: #eta;EF #eta;L1 #eta",26,-2.6,2.6,26,-2.6,2.6));
    addHistogram(new TH2F("hPhiEFVsL1","EF vs L1: #phi;EF #phi;L1 #phi",32,-3.2,3.2,32,-3.2,3.2));

    addHistogram(new TH2F("hEtEFVsL2","EF vs L2: E_{T};EF E_{T} [GeV];L2 E_{T} [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtaEFVsL2","EF vs L2: #eta;EF #eta;L2 #eta",26,-2.6,2.6,26,-2.6,2.6));
    addHistogram(new TH2F("hPhiEFVsL2","EF vs L2: #phi;EF #phi;L2 #phi",32,-3.2,3.2,32,-3.2,3.2));


    addHistogram(new TH2F("hEtL1VsL2Narrow","L1 vs L2: E_{T};L1 E_{T} [GeV];L2 E_{T} Narrow [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtL1VsL2Wide","L1 vs L2: E_{T};L1 E_{T} [GeV];L2 E_{T} Wide [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtL1VsL2Medium","L1 vs L2: E_{T};L1 E_{T} [GeV];L2 E_{T} Medium [GeV]",40,0.0,100.0,40,0.0,100.0));

    addHistogram(new TH2F("hEtEFVsL2Narrow","EF vs L2: E_{T};Uncalibrated EF E_{T} [GeV];L2 E_{T} Narrow [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtEFVsL2Wide","EF vs L2: E_{T};Uncalibrated EF E_{T} [GeV];L2 E_{T} Wide [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("hEtEFVsL2Medium","EF vs L2: E_{T};Uncalibrated EF E_{T} [GeV];L2 E_{T} Medium [GeV]",40,0.0,100.0,40,0.0,100.0));


    addHistogram(new TH2F("hEtEFUncalibVsL1","EF vs L1: E_{T};Uncalibrated EF E_{T} [GeV];L1 E_{T} [GeV]",40,0.0,100.0,40,0.0,100.0));
    addHistogram(new TH2F("EFL2EtRatioVsEta","Ratio of EF E_{T} to L2 E_{T} (Wide cone) Vs #eta at L2; #eta;EF ET/L2 ET Wide [GeV]",26,-2.6,2.6,20,0.0,2.0));
    addHistogram(new TH2F("EFL2EtRatioVsPhi","Ratio of EF E_{T} to L2 E_{T} (Wide cone) Vs #phi at L2; #phi;EF ET/L2 ET Wide [GeV]",32,-3.2,3.2,20,0.0,2.0));
    addHistogram(new TH2F("L2HADEWideVsEta","L2 HAD Energy (Wide cone) Vs #eta at L2; #eta; L2 HAd energy (Wide Cone)[GeV];",26,-2.6,2.6,40,0.0,100.0));
    addHistogram(new TH2F("L2HADEWideVsPhi","L2 HAD energy (Wide cone) Vs #phi at L2; #phi; L2 HAd energy (Wide Cone)[GeV];",32,-3.2,3.2,40,0.0,100.0));



    //--------------------
    // Fakes
    //--------------------
    if(m_fakeJetEff) {
      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/FakeJets", run, ATTRIB_MANAGED, "") );
	
      addHistogram(new TH1F("LeadJet1_eta_denom","Leading jet, #eta; #eta ; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("LeadJet1_eta_num_1p3p","Leading jet matched to tau1p3p , #eta; #eta ; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("LeadJet1_eta_num_tauRec","Leading jet matched to tauRec , #eta; #eta ; Nevents",26,-2.6,2.6));
	
	
      addHistogram(new TH1F("LeadJet2_eta_denom","2nd Leading jet, #eta; #eta ; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("LeadJet2_eta_num_1p3p","2nd Leading jet matched to tau1p3p , #eta; #eta ; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("LeadJet2_eta_num_tauRec","2nd Leading jet matched to tauRec , #eta; #eta ; Nevents",26,-2.6,2.6));
	
      addHistogram(new TH1F("LeadJet1_phi_denom","Leading jet, #phi; #phi ; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("LeadJet1_phi_num_1p3p","Leading jet matched to tau1p3p , #phi; #phi ; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("LeadJet1_phi_num_tauRec","Leading jet matched to tauRec , #phi; #phi ; Nevents",32,-3.2,3.2));
	
	
      addHistogram(new TH1F("LeadJet2_phi_denom","2nd Leading jet, #phi; #phi ; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("LeadJet2_phi_num_1p3p","2nd Leading jet matched to tau1p3p , #phi; #phi ; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("LeadJet2_phi_num_tauRec","2nd Leading jet matched to tauRec , #phi; #phi ; Nevents",32,-3.2,3.2));
	
      addHistogram(new TH1F("LeadJet1_pt_denom","Leading jet, p_{t}; p_{T} [GeV]; Nevents",100,0,500));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p","Leading jet matched to tau1p3p , p_{T}; p_{T} [GeV]; Nevents",100,0,500));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec","Leading jet matched to tauRec , p_{T}; p_{T} [GeV]; Nevents",100,0,500));
	
      addHistogram(new TH1F("LeadJet2_pt_denom","2nd Leading jet, p_{T}; p_{T} [GeV]; Nevents",100,0,500));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p","2nd Leading jet matched to tau1p3p , p_{T}; p_{T} [GeV]; Nevents",100,0,500));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec","2nd Leading jet matched to tauRec , p_{T}; p_{T} [GeV]; Nevents",100,0,500));
	

      //--------------------
      // eff with fakes
      //--------------------
      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/FakeJets/Efficiency", run, ATTRIB_MANAGED, "", "effAsPerCent") );
		
      addHistogram(new TH1F("LeadJet1_eta_1p3p_Eff","Efficiency for leading jet matched to tau1p3p , #eta; #eta ; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("LeadJet1_eta_tauRec_Eff","Efficiency for leading jet matched to tauRec , #eta; #eta ; Nevents",26,-2.6,2.6));
	
      addHistogram(new TH1F("LeadJet2_eta_1p3p_Eff","Efficiency for 2nd leading jet matched to tau1p3p , #eta; #eta ; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("LeadJet2_eta_tauRec_Eff","Efficiency for 2nd leading jet matched to tauRec , #eta; #eta ; Nevents",26,-2.6,2.6));
	
      addHistogram(new TH1F("LeadJet1_phi_1p3p_Eff","Efficiency for leading jet matched to tau1p3p , #phi; #phi ; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("LeadJet1_phi_tauRec_Eff","Efficiency for leading jet matched to tauRec , #phi; #phi ; Nevents",32,-3.2,3.2));
	
      addHistogram(new TH1F("LeadJet2_phi_1p3p_Eff","Efficiency for 2nd leading jet matched to tau1p3p , #phi; #phi ; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("LeadJet2_phi_tauRec_Eff","Efficiency for 2nd leading jet matched to tauRec , #phi; #phi ; Nevents",32,-3.2,3.2));
	
      addHistogram(new TH1F("LeadJet1_pt_1p3p_Eff","Efficiency for leading jet matched to tau1p3p , p_{T}; p_{T}[GeV] ; Nevents",100,0,500));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_Eff","Efficiency for leading jet matched to tauRec , p_{T}; p_{T} [GeV]; Nevents",100,0,500));
	
      addHistogram(new TH1F("LeadJet2_pt_1p3p_Eff","Efficiency for 2nd leading jet matched to tau1p3p , p_{T}; p_{T} [GeV]; Nevents",100,0,500));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_Eff","Efficiency for 2nd leading jet matched to tauRec , p_{T}; p_{T} [GeV]; Nevents",100,0,500));
	
      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/FakeJets/Binned", run, ATTRIB_MANAGED, "") );
	
      addHistogram(new TH1F("LeadJet1_pt_denom_1540","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet2_pt_denom_1540","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet1_pt_denom_4080","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet2_pt_denom_4080","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet1_pt_denom_80120","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet2_pt_denom_80120","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet1_pt_denom_120160","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet2_pt_denom_120160","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet1_pt_denom_160200","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet2_pt_denom_160200","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet1_pt_denom_200400","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet2_pt_denom_200400","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet1_pt_denom_400800","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet2_pt_denom_400800","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet1_pt_denom_8001200","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
      addHistogram(new TH1F("LeadJet2_pt_denom_8001200","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
	
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_1540","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_1540","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_4080","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_4080","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_80120","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_80120","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_120160","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_120160","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_160200","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_160200","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_200400","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_200400","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_400800","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_400800","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet1_pt_num_1p3p_8001200","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
      addHistogram(new TH1F("LeadJet2_pt_num_1p3p_8001200","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
	
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_1540","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_1540","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_4080","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_4080","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_80120","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_80120","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_120160","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_120160","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_160200","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_160200","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_200400","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_200400","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_400800","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_400800","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet1_pt_num_tauRec_8001200","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
      addHistogram(new TH1F("LeadJet2_pt_num_tauRec_8001200","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));

      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/FakeJets/Binned/Efficiency", run, ATTRIB_MANAGED, "",  "effAsPerCent") );
	
      addHistogram(new TH1F("LeadJet1_pt_tauRec_1540_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_1540_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_4080_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_4080_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_80120_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_80120_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_120160_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_120160_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_160200_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_160200_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_200400_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_200400_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_400800_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_400800_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet1_pt_tauRec_8001200_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
      addHistogram(new TH1F("LeadJet2_pt_tauRec_8001200_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
	
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_1540_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_1540_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,14,41));
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_4080_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_4080_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,39,81));
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_80120_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_80120_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,79,121));
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_120160_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_120160_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,119,161));
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_160200_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_160200_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,159,201));
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_200400_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_200400_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,199,401));
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_400800_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_400800_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,399,801));
      addHistogram(new TH1F("LeadJet1_pt_tau1p3p_8001200_Eff","Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
      addHistogram(new TH1F("LeadJet2_pt_tau1p3p_8001200_Eff","Second Leading jet, p_{t}; p_{T} [GeV]; Nevents",1,799,1201));
    }

    
    //--------------------
    // offline eff
    //--------------------
    if(m_offlineEff) {
      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio", run, ATTRIB_MANAGED, "") );
	
      addHistogram(new TH1F("hTauEta","Offline Tau Eta ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaL1","Eta of Offline Tau matched to L1 ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaL2","Eta of Offline Tau matched to L2 ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaEF","Eta of Offline Tau matched to EF ; #eta; Nevents",26,-2.6,2.6));

      addHistogram(new TH1F("hTauPhi","Offline Tau Phi ; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiL1","phi of Offline Tau matched to L1; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiL2","phi of Offline Tau matched to L2 ; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiEF","phi of Offline Tau matched to EF ; #phi; Nevents",32,-3.2,3.2));

      addHistogram(new TH1F("hTauPt","Offline Tau pt ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtL1","pT of Offline Tau matched to L1 ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtL2","pT of Offline Tau matched to L2 ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtEF","pT of Offline Tau matched to EF ; p_{T}[GeV]; Nevents",21,-5,100));

      addHistogram(new TH1F("hTauNvtx","Offline number of Tau; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxL1","number of Tau matched to L1; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxL2","number of Tau matched to L2; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxEF","number of Tau matched to EF; nVtx; Candidates",30,0.5,30.5));

      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio/Ratio", run, ATTRIB_MANAGED, "", "effAsPerCent") );
      std::string tmpstr = boost::lexical_cast<std::string>(m_effOffTauPtCut);
      
      addHistogram(new TH1F("hL1OfflineRatioEta",
			    TString("Ratio of L1 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));
      addHistogram(new TH1F("hL2OfflineRatioEta",
			    TString("Ratio of L2 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));
      addHistogram(new TH1F("hEFOfflineRatioEta",
			    TString("Ratio of EF w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));

      addHistogram(new TH1F("hL1OfflineRatioPhi",
			    TString("Ratio of L1 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));
      addHistogram(new TH1F("hL2OfflineRatioPhi",
			    TString("Ratio of L2 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));
      addHistogram(new TH1F("hEFOfflineRatioPhi",
			    TString("Ratio of EF w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));

      addHistogram(new TH1F("hL1OfflineRatioPt",TString("Ratio of L1 w.r.t. Offline ;Offline p_{T}[GeV]; Ratio"),21,-5,100));
      addHistogram(new TH1F("hL2OfflineRatioPt",TString("Ratio of L2 w.r.t. Offline ;Offline p_{T}[GeV]; Ratio"),21,-5,100));
      addHistogram(new TH1F("hEFOfflineRatioPt",TString("Ratio of EF w.r.t. Offline ;Offline p_{T}[GeV]; Ratio"),21,-5,100));

      addHistogram(new TH1F("hL1OfflineRatioNvtx",
			    TString("Ratio of L1 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
      addHistogram(new TH1F("hL2OfflineRatioNvtx",
			    TString("Ratio of L2 w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
      addHistogram(new TH1F("hEFOfflineRatioNvtx",
			    TString("Ratio of EF w.r.t. Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
    }
    
    if(m_offlineEffBDTMedium) {
      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio/BDTMedium", run, ATTRIB_MANAGED, "") );
      
      addHistogram(new TH1F("hTauEtaBDTMedium","Offline BDTMedium Tau Eta ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaL1BDTMedium","Eta of Offline BDTMedium Tau matched to L1 ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaL2BDTMedium","Eta of Offline BDTMedium Tau matched to L2 ; #eta; Nevents",26,-2.6,2.6));
      addHistogram(new TH1F("hTauEtaEFBDTMedium","Eta of Offline BDTMedium Tau matched to EF ; #eta; Nevents",26,-2.6,2.6));

      addHistogram(new TH1F("hTauPhiBDTMedium","Offline BDTMedium Tau Phi ; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiL1BDTMedium","phi of Offline BDTMedium Tau matched to L1; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiL2BDTMedium","phi of Offline BDTMedium Tau matched to L2 ; #phi; Nevents",32,-3.2,3.2));
      addHistogram(new TH1F("hTauPhiEFBDTMedium","phi of Offline BDTMedium Tau matched to EF ; #phi; Nevents",32,-3.2,3.2));

      addHistogram(new TH1F("hTauPtBDTMedium","Offline BDTMedium Tau pt ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtL1BDTMedium","pT of Offline BDTMedium Tau matched to L1 ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtL2BDTMedium","pT of Offline BDTMedium Tau matched to L2 ; p_{T}[GeV]; Nevents",21,-5,100));
      addHistogram(new TH1F("hTauPtEFBDTMedium","pT of Offline BDTMedium Tau matched to EF ; p_{T}[GeV]; Nevents",21,-5,100));
      
      addHistogram(new TH1F("hTauNvtxBDTMedium","Offline number of BDTMedium Tau; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxL1BDTMedium","number of BDTMedium Tau matched to L1; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxL2BDTMedium","number of BDTMedium Tau matched to L2; nVtx; Candidates",30,0.5,30.5));
      addHistogram(new TH1F("hTauNvtxEFBDTMedium","number of BDTMedium Tau matched to EF; nVtx; Candidates",30,0.5,30.5));
      
      addMonGroup( new MonGroup(this, "HLT/TauMon/"+trigItem+"/OfflineRatio/Ratio/BDTMedium", run, ATTRIB_MANAGED, "", "effAsPerCent") );
      std::string tmpstr = boost::lexical_cast<std::string>(m_effOffTauPtCut);
      
      addHistogram(new TH1F("hL1OfflineRatioEtaBDTMedium",
			    TString("Ratio of L1 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));
      addHistogram(new TH1F("hL2OfflineRatioEtaBDTMedium",
			    TString("Ratio of L2 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));
      addHistogram(new TH1F("hEFOfflineRatioEtaBDTMedium",
			    TString("Ratio of EF w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #eta; Ratio"),26,-2.6,2.6));

      addHistogram(new TH1F("hL1OfflineRatioPhiBDTMedium",
			    TString("Ratio of L1 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));
      addHistogram(new TH1F("hL2OfflineRatioPhiBDTMedium",
			    TString("Ratio of L2 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));
      addHistogram(new TH1F("hEFOfflineRatioPhiBDTMedium",
			    TString("Ratio of EF w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV);Offline #phi; Ratio"),32,-3.2,3.2 ));

      addHistogram(new TH1F("hL1OfflineRatioPtBDTMedium","Ratio of L1 w.r.t. BDTMedium Offline ;Offline p_{T}[GeV]; Ratio",21,-5,100));
      addHistogram(new TH1F("hL2OfflineRatioPtBDTMedium","Ratio of L2 w.r.t. BDTMedium Offline ;Offline p_{T}[GeV]; Ratio",21,-5,100));
      addHistogram(new TH1F("hEFOfflineRatioPtBDTMedium","Ratio of EF w.r.t. BDTMedium Offline ;Offline p_{T}[GeV]; Ratio",21,-5,100));

      addHistogram(new TH1F("hL1OfflineRatioNvtxBDTMedium",
			    TString("Ratio of L1 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
      addHistogram(new TH1F("hL2OfflineRatioNvtxBDTMedium",
			    TString("Ratio of L2 w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
      addHistogram(new TH1F("hEFOfflineRatioNvtxBDTMedium",
			    TString("Ratio of EF w.r.t. BDTMedium Offline (#tau^{Off} p_{T} > "+tmpstr+" GeV); N^{Offline}_{vtx}; Ratio"),30,0.5,30.5));
    }
  }
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fillHistogramsForItem(const std::string & trigItem){

  StatusCode sc;
  
  ///////////////////////// L1
  //MzN: causes an Fatal Error 
  sc = fillL1ForItem(trigItem);
  
  
  ///////////////////////// L2 Calo
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/T2CaloTau");
  
  std::string trig_item_L2 = "L2_"+trigItem;
  std::string trig_item_EF = "EF_"+trigItem;
  
  if(trigItem=="Dump"){
    const DataHandle<TrigTauClusterContainer> l2clus;
    if (  (m_storeGate->retrieve(l2clus, "HLT_TrigT2CaloTau") ).isSuccess() ) {
      TrigTauClusterContainer::const_iterator CI,l2clus_end = l2clus->end();
      for(CI=l2clus->begin();CI!=l2clus_end; ++CI) fillTrigTauCluster((*CI));
    }
  }
  else{          
    if (getTDT()->isPassed(trig_item_L2)) {
      const std::vector<Trig::Feature<TrigTauCluster> > vec_tauClust = (getTDT()->features("L2_"+trigItem)).get<TrigTauCluster>();
      std::vector<Trig::Feature<TrigTauCluster> >::const_iterator CI = vec_tauClust.begin();
      for(;CI != vec_tauClust.end();++CI) fillTrigTauCluster(CI->cptr());
    }
  }
  ///////////////////////// L2 ID
  if (m_TracksInfo){
    setCurrentMonGroup("HLT/TauMon/"+trigItem+"/T2IDTau");
    
    if(trigItem=="Dump"){
      const DataHandle<TrigTauTracksInfoCollection> l2tkInfo;
      if ((m_storeGate->retrieve(l2tkInfo, "HLT") ).isSuccess() ) {
	TrigTauTracksInfoCollection::const_iterator CI,l2tkInfo_end = l2tkInfo->end();
	for(CI=l2tkInfo->begin();CI!=l2tkInfo_end; ++CI) fillTrigTauTracksInfo(*CI);
      }
    }
    else{
      if (getTDT()->isPassed(trig_item_L2)) {
	const std::vector<Trig::Feature<TrigTauTracksInfo> > vec_tauTracks = (getTDT()->features("L2_"+trigItem)).get<TrigTauTracksInfo>();
	std::vector<Trig::Feature<TrigTauTracksInfo> >::const_iterator CI = vec_tauTracks.begin();
	for(;CI != vec_tauTracks.end(); ++CI) fillTrigTauTracksInfo(CI->cptr());
      }
    }
  }
  ///////////////////////// L2 Final
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/T2TauFinal");
  
  if(trigItem=="Dump"){
    const DataHandle<TrigTauContainer> l2tau;
    if (  (m_storeGate->retrieve(l2tau, "HLT") ).isSuccess() ) {
      TrigTauContainer::const_iterator CI,l2tau_end = l2tau->end();
      for(CI=l2tau->begin();CI!=l2tau_end; ++CI) fillL2Tau(*CI);
    }
  }
  else{
    if (getTDT()->isPassed(trig_item_L2)) {
      const std::vector<Trig::Feature<TrigTau> > vec_tau = (getTDT()->features("L2_"+trigItem)).get<TrigTau>();
      std::vector<Trig::Feature<TrigTau> >::const_iterator CI = vec_tau.begin();
      for(;CI != vec_tau.end(); ++CI) fillL2Tau(CI->cptr());
    }
  }
  
  ///////////////////////// EF
  if(trigItem=="Dump"){
    const DataHandle<Analysis::TauJetContainer> eftau;
    if (  (m_storeGate->retrieve(eftau, "HLT_TrigTauRecMerged") ).isSuccess() ) {
      Analysis::TauJetContainer::const_iterator CI,eftau_end = eftau->end();
      setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFTau");
      for(CI=eftau->begin();CI!=eftau_end; ++CI) fillEFTau(*CI);
      setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFVsOffline");
      for(CI=eftau->begin();CI!=eftau_end; ++CI) fillEFTauVsOffline(*CI);
    }
  }
  else{
    if (getTDT()->isPassed(trig_item_EF)) {
      
      const std::vector<Trig::Feature<Analysis::TauJetContainer> > vec_tauEFClust = (getTDT()->features("EF_"+trigItem)).get<Analysis::TauJetContainer>();
      std::vector<Trig::Feature<Analysis::TauJetContainer> >::const_iterator CI = vec_tauEFClust.begin();
      
      for(; CI != vec_tauEFClust.end(); ++CI)
	if(CI->cptr()){
	  Analysis::TauJetContainer::const_iterator tauItr = CI->cptr()->begin();
	  Analysis::TauJetContainer::const_iterator tauEnd = CI->cptr()->end();
	  
	  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFTau");
	  for(tauItr = CI->cptr()->begin(); tauItr != tauEnd; ++tauItr) {if(*tauItr) fillEFTau(*tauItr);}
	  
	  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFVsOffline");
	  for(tauItr = CI->cptr()->begin(); tauItr != tauEnd; ++tauItr) {if(*tauItr) fillEFTauVsOffline(*tauItr);}
	}
    }
  }
  if(trigItem!="Dump"){
    fillEFvsL2(trigItem);
    sc = fillEfficiency(trigItem);
    if(m_fakeJetEff) sc = fillTagandProbe(trigItem);
    if(m_offlineEff) sc = OfflineEfficiency(trigItem);
    if(m_offlineEffBDTMedium) sc = OfflineEfficiencyBDTMedium(trigItem);
  }
  
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not fill histograms for given item.");
  }
  return sc;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fillL1ForItem(const std::string & trigItem){

  //////////////////////// L1
  const LVL1_ROI * lvl1ROI;
  StatusCode sc = m_storeGate->retrieve(lvl1ROI);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not find LVL1_ROI in StoreGate.");
    return StatusCode::SUCCESS; // Don't stop proccessing events
  }
  else{
    ATH_MSG_VERBOSE("find LVL1_ROI in SG");
  }
  
  LVL1_ROI::emtaus_type::const_iterator itEMTau = lvl1ROI->getEmTauROIs().begin();
  LVL1_ROI::emtaus_type::const_iterator itEMTau_e = lvl1ROI->getEmTauROIs().end();
  LVL1_ROI::emtaus_type::const_iterator matchedL1;
  
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/L1RoI");
  
  if( trigItem=="Dump"){
    for( LVL1_ROI::emtaus_type::const_iterator it=itEMTau; it!=itEMTau_e;++it){
      hist("hL1RoIEta")->Fill(it->eta());
      hist("hL1RoIPhi")->Fill(it->phi());
      hist2("hL1EtaVsPhi")->Fill(it->eta(),it->phi());
     
      hist("hL1RoITauClus")->Fill(it->getTauClus()/CLHEP::GeV);
      hist("hL1RoIEMIso")  ->Fill(it->getEMIsol()/CLHEP::GeV);
      hist("hL1RoIHadCore")->Fill(it->getHadCore()/CLHEP::GeV);
      hist("hL1RoIHadIsol")->Fill(it->getHadIsol()/CLHEP::GeV);
    }
  }  

  else {
    std::string trig_item_EF = "EF_"+trigItem;
    
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
	  for( LVL1_ROI::emtaus_type::const_iterator it=itEMTau; it!=itEMTau_e;++it){
	    if(id==it->getROIWord()){ 
 	      //matched = true;
 	      
 	      hist("hL1RoITauClus")->Fill(it->getTauClus()/CLHEP::GeV);
 	      hist("hL1RoIEMIso")  ->Fill(it->getEMIsol()/CLHEP::GeV);
 	      hist("hL1RoIHadCore")->Fill(it->getHadCore()/CLHEP::GeV);
 	      hist("hL1RoIHadIsol")->Fill(it->getHadIsol()/CLHEP::GeV); 	     
 	    }
 	  }
	  
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::fillTrigTauCluster(const TrigTauCluster* aCluster){
  if(!aCluster) return;
  
  hist("hEta")->Fill(aCluster->eta());
  hist("hPhi")->Fill(aCluster->phi());
  hist2("hEtaVsPhi")->Fill(aCluster->eta(), aCluster->phi()); 
  
  hist("hEMenergy")->Fill(aCluster->EMenergy()/CLHEP::GeV);
  hist("hHADenergy")->Fill(aCluster->HADenergy()/CLHEP::GeV);
  
  hist("hEMRadius3S")->Fill(aCluster->EMRadius3S());
  hist("hCaloRadius")->Fill(aCluster->CaloRadius());
  
  hist("hIsoFrac")->Fill(aCluster->IsoFrac());
  hist("hStripWidth")->Fill(aCluster->stripWidth());

  
  float ERawWide = 0;
  float ERawMedium = 0;
  float ERawNarrow = 0;
  
  for (int i=0; i<4; ++i) {
    ERawWide +=  aCluster->EMenergyWide(i);
    ERawMedium +=  aCluster->EMenergyMedium(i);
    ERawNarrow +=  aCluster->EMenergyNarrow(i);
  }
  
  for (int j=0; j<3; ++j) {
    ERawWide +=  aCluster->HADenergyWide(j);
    ERawMedium +=  aCluster->HADenergyMedium(j);
    ERawNarrow +=  aCluster->HADenergyNarrow(j);
  }
  
  ERawWide =  ERawWide / CLHEP::GeV;
  ERawMedium =  ERawMedium / CLHEP::GeV;
  ERawNarrow =  ERawNarrow / CLHEP::GeV;

  hist("hEtRawWide")->Fill(ERawWide / cosh(aCluster->eta()));
  hist("hEtRawMedium")->Fill(ERawMedium / cosh(aCluster->eta()));
  hist("hEtRawNarrow")->Fill(ERawNarrow / cosh(aCluster->eta()));
  
  float m_EMFraction  = -1.0;
  //float m_EdRFraction  = -1.0;
  if( (aCluster->EMenergy()+aCluster->HADenergy())!=0.){
    m_EMFraction  = aCluster->EMenergy()/( aCluster->EMenergy()+ aCluster->HADenergy());
    
  }
  
  hist("hEMFraction")->Fill(m_EMFraction);
  hist("hCoreFraction")->Fill(aCluster->CoreFrac());

}



///////////////////////////////////////////////////////////
void HLTTauMonTool::fillTrigTauTracksInfo(const TrigTauTracksInfo *aTkInfo){
  if(!aTkInfo) return;
  
  hist("hLeadingTkPt")->Fill(aTkInfo->leadingTrackPt()/CLHEP::GeV); 
  hist("hNCoreTracks")->Fill(aTkInfo->nCoreTracks());
  hist("hNIsoTracks")->Fill(aTkInfo->nIsoTracks());
  hist("hNSlowTracks")->Fill(aTkInfo->nSlowTracks());
  hist("hCharge")->Fill(aTkInfo->charge());      
  hist("hScalarSumPtCore")->Fill(aTkInfo->scalarPtSumCore()/CLHEP::GeV);      
  hist("hScalarSumPtIso")->Fill(aTkInfo->scalarPtSumIso()/CLHEP::GeV);      
  float m_sumPtRatio = aTkInfo->scalarPtSumIso()/40000;// assume 40 GeV is average candidate.			    
  if(aTkInfo->scalarPtSumCore()>1e-5) m_sumPtRatio = aTkInfo->scalarPtSumIso()/aTkInfo->scalarPtSumCore();
  hist("hSumPtRatio")->Fill(m_sumPtRatio);

  // dZ0 w.r.t. leading track; only tracks with pt > 1 GeV and |dZ0| < 2 mm are filled.
//   std::cout << "aTkInfo = " << aTkInfo << "; aTkInfo->trackCollection() = " << aTkInfo->trackCollection() << std::endl;
//   if(!aTkInfo->trackCollection()) return;

//   float m_pTCut = 1.0*CLHEP::GeV;
//   float m_dZ0Max = 2.0;// mm
//   TrigInDetTrackCollection::const_iterator itr = aTkInfo->trackCollection()->begin();
//   TrigInDetTrackCollection::const_iterator itr_last = aTkInfo->trackCollection()->end();
//   double ldTrkPt = -999.9;
//   double ldTrkZ0 = -999.9;
//   for (;itr!=itr_last;++itr) {
//     float pt = fabs( (*itr)->param()->pT());
//     if ( pt > ldTrkPt) {
//       ldTrkPt = pt;
//       ldTrkZ0 = (*itr)->param()->z0();
//     }
//   }

//   std::cout << "Leading Z0 = " << ldTrkZ0 << std::endl;

//   itr = aTkInfo->trackCollection()->begin();
//   int tkindex = 0;
//   for (;itr!=itr_last;++itr,++tkindex) {
//     //ptMin selection
//     float pt = fabs( (*itr)->param()->pT());
//     if ( pt < m_pTCut )
//       continue;

//     float dz0 = (*itr)->param()->z0() - ldTrkZ0;
//     if ( fabs(dz0) > m_dZ0Max)
//       continue;

//     std::cout << "deltaZ0 of trk[" << tkindex << "] = " << dz0 << "[mm]" << std::endl;
//     hist("hDeltaZ0wrtLdTk")->Fill(dz0);
//   }

}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::fillL2Tau(const TrigTau* aL2Tau){
  if(!aL2Tau) return;
  
  hist("hL2EFlow")->Fill(aL2Tau->simpleEtFlow()/CLHEP::GeV);
  hist("hEtCombined")->Fill(aL2Tau->et()/CLHEP::GeV);
  hist("hL2TrkAvgDist")->Fill(aL2Tau->trkAvgDist());
  hist("hL2EtOverPtLeadTrk")->Fill(aL2Tau->etOverPtLeadTrk());
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::fillEFTau(const Analysis::TauJet *aEFTau){
  if(!aEFTau) return;
  
  hist("hEFEt")        ->Fill(aEFTau->et()/CLHEP::GeV);
  hist("hEFnTrack")    ->Fill(aEFTau->numTrack());
  hist("hEta")         ->Fill(aEFTau->eta());
  hist("hPhi")         ->Fill(aEFTau->phi());

  hist2("hEFEtaVsPhi") ->Fill(aEFTau->eta(),aEFTau->phi());
  hist2("hEtVsEta")    ->Fill(aEFTau->eta(),aEFTau->et()/CLHEP::GeV);
  hist2("hEtVsPhi")    ->Fill(aEFTau->phi(),aEFTau->et()/CLHEP::GeV);
  
  const Analysis::TauCommonDetails* aTauDetails = aEFTau->details<const Analysis::TauCommonDetails>();
  if (aTauDetails){
    hist("hEFnWideTrack")    ->Fill(aTauDetails->seedCalo_nWideTrk());
    hist("hEFEtRaw")         ->Fill((aTauDetails->seedCalo_etEMAtEMScale()/CLHEP::GeV + 
				     aTauDetails->seedCalo_etHadAtEMScale())/CLHEP::GeV);   

    hist("hEFEMRadius")      ->Fill(aTauDetails->seedCalo_EMRadius());  
    hist("hEFHADRadius")     ->Fill(aTauDetails->seedCalo_hadRadius());  
 
    hist("hEFTrkAvgDist")    ->Fill(aTauDetails->seedCalo_trkAvgDist());
    hist("hEFEtovPtLd")      ->Fill(aTauDetails->etOverPtLeadTrk());
    hist("hEFCentFrac")      ->Fill(aTauDetails->seedCalo_centFrac());
    hist("hEFDRmax")         ->Fill(aTauDetails->seedCalo_dRmax());

    double m_EFEMFrac = -1.0;
    if ((aTauDetails->seedCalo_etEMAtEMScale() + aTauDetails->seedCalo_etHadAtEMScale()) != 0.)
      m_EFEMFrac = aTauDetails->seedCalo_etEMAtEMScale()/
                  (aTauDetails->seedCalo_etEMAtEMScale() + aTauDetails->seedCalo_etHadAtEMScale());

    hist("hEFEMFraction")    ->Fill(m_EFEMFrac);
    hist("hEFTopoInvMass")   ->Fill(aTauDetails->topoInvMass()/CLHEP::GeV);
    hist("hEFMassTrkSys")    ->Fill(aTauDetails->massTrkSys()/CLHEP::GeV);
    hist("hEFIpSigLeadTrk")  ->Fill(aTauDetails->ipSigLeadTrk());
    hist("hEFTrFlightPathSig") ->Fill(aTauDetails->trFlightPathSig());
  }

  
  if ((aEFTau)->hasAuthor( TauJetParameters::tauRec ))
    hist("hAuthor")->Fill(1);
  if  ((aEFTau)->hasAuthor( TauJetParameters::tau1P3P))
      hist("hAuthor")->Fill(2);
  if ((aEFTau)->hasAuthor( TauJetParameters::tauRec ) && (aEFTau)->hasAuthor( TauJetParameters::tau1P3P))
      hist("hAuthor")->Fill(3);
  if ((aEFTau)->hasAuthor( TauJetParameters::tauRec ) &! (aEFTau)->hasAuthor( TauJetParameters::tau1P3P))
      hist("hAuthor")->Fill(4);
  if ((aEFTau)->hasAuthor( TauJetParameters::tau1P3P ) &! (aEFTau )->hasAuthor( TauJetParameters::tauRec))
      hist("hAuthor")->Fill(5);
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::fillEFTauVsOffline(const Analysis::TauJet *aEFTau){
  if(!aEFTau) return;

  const Analysis::TauCommonDetails* aTauDetails = aEFTau->details<const Analysis::TauCommonDetails>();

  if ( ! aTauDetails ) {
    ATH_MSG_DEBUG("Not found details of the online tau");
    return;
  }

  //match offline - EF
  const Analysis::TauJet *aOfflineTau = 0;
  float tmpR = 0.3;
  const DataHandle<Analysis::TauJetContainer> offlinetau;
  if (  (m_storeGate->retrieve(offlinetau, "TauRecContainer") ).isSuccess() ) {
    Analysis::TauJetContainer::const_iterator CI,offlinetau_end = offlinetau->end();
    for(CI=offlinetau->begin();CI!=offlinetau_end; ++CI){
      if((*CI)){
	if( (*CI)->hlv().deltaR(aEFTau->hlv())<tmpR ){
	  tmpR = (*CI)->hlv().deltaR(aEFTau->hlv());
	  aOfflineTau = (*CI);
	}
      }
    }
  }

  if(aOfflineTau)  ATH_MSG_DEBUG("Found a offline TauJet matching EF tauJet, dR = "<<tmpR);
  else {  
    ATH_MSG_DEBUG("Not found a offline TauJet matching EF tauJet");
    return;
  }

  const Analysis::TauCommonDetails*  aTauDetailsOffline = aOfflineTau->details<const Analysis::TauCommonDetails>();
  if ( ! aTauDetailsOffline ) {
    ATH_MSG_DEBUG("Not found details of the offline tau");
    return;
  }

  //fill everything
  //matching plots
  hist2("hMatchdRVsEta")->Fill(aEFTau->eta(),tmpR);
  hist2("hMatchdEtaVsEta")->Fill(aEFTau->eta(),aEFTau->eta() - aOfflineTau->eta());
  hist2("hMatchdPhiVsEta")->Fill(aEFTau->eta(),aEFTau->phi() - aOfflineTau->phi());


  //2D ratio style plots
  
  
  float emet = aTauDetailsOffline->seedCalo_etEMAtEMScale();
  if ( emet > 0.1 ) {
    emet = (aTauDetails->seedCalo_etEMAtEMScale() - emet) / emet;
    hist2("hOffEFEMDiffVsEta")->Fill(aEFTau->eta(),emet);
    hist2("hOffEFEMDiffVsPhi")->Fill(aEFTau->phi(),emet);
  }
  
  float hadet = aTauDetailsOffline->seedCalo_etHadAtEMScale();
  if ( hadet > 0.1 ) {
    hadet = (aTauDetails->seedCalo_etHadAtEMScale() - hadet) / hadet;
    hist2("hOffEFHADDiffVsEta")->Fill(aEFTau->eta(),hadet);
    hist2("hOffEFHADDiffVsPhi")->Fill(aEFTau->phi(),hadet);
  }
  
  float emRadius = aTauDetailsOffline->seedCalo_EMRadius();
  if ( emRadius > 0.001 ) {
    emRadius = (aTauDetails->seedCalo_EMRadius() - emRadius) / emRadius;
    hist2("hEMRadiusRatioVsEta")->Fill(aEFTau->eta(), emRadius);
  }

  float isoFrac = aTauDetailsOffline->seedCalo_isolFrac();
  if ( isoFrac > 0.001 ) {
    isoFrac = (aTauDetails->seedCalo_isolFrac() - isoFrac) / isoFrac;
    hist2("hIsoFracRatioVsEta")->Fill(aEFTau->eta(), isoFrac);
  }

  float etOverPtLeadTrk = aTauDetailsOffline->etOverPtLeadTrk();
  if ( etOverPtLeadTrk > 0.001) {
    etOverPtLeadTrk = ( aTauDetails->etOverPtLeadTrk() - etOverPtLeadTrk) / etOverPtLeadTrk;
    hist2("hEtOverPtLeadTrkRatioVsEta")->Fill(aEFTau->eta(), etOverPtLeadTrk);
  }

  //1D ratio plots

  FillRelDiffHist(hist("hOffEFEMDiff"), aTauDetailsOffline->seedCalo_etEMAtEMScale(), aTauDetails->seedCalo_etEMAtEMScale(), 0.1, 1);
  FillRelDiffHist(hist("hOffEFHADDiff"), aTauDetailsOffline->seedCalo_etHadAtEMScale(), aTauDetails->seedCalo_etHadAtEMScale(), 0.1, 1);
  FillRelDiffHist(hist("hEMRadiusRatio"), aTauDetailsOffline->seedCalo_EMRadius(), aTauDetails->seedCalo_EMRadius(), 0.001, 1);
  FillRelDiffHist(hist("hIsoFracRatio"), aTauDetailsOffline->seedCalo_isolFrac(), aTauDetails->seedCalo_isolFrac(), 0.001, 1);
  FillRelDiffHist(hist("hEtOverPtLeadTrkRatio"), aTauDetailsOffline->etOverPtLeadTrk(), aTauDetails->etOverPtLeadTrk(), 0.001, 1);
  FillRelDiffHist(hist("hCentFracRatio"), aTauDetailsOffline->seedCalo_centFrac(), aTauDetails->seedCalo_centFrac(), 0.001, 1);
  FillRelDiffHist(hist("hNWideTrackRatio"), aTauDetailsOffline->seedCalo_nWideTrk(), aTauDetails->seedCalo_nWideTrk(), 0, 1);
  FillRelDiffHist(hist("hNTrackRatio"), aOfflineTau->numTrack(), aEFTau->numTrack(), 0, 1);
  FillRelDiffHist(hist("hIpSigLeadTrkRatio"), aTauDetailsOffline->ipSigLeadTrk(), aTauDetails->ipSigLeadTrk(), 0.1, 2);
  FillRelDiffHist(hist("hTrFlightPathSigRatio"), aTauDetailsOffline->trFlightPathSig(), aTauDetails->trFlightPathSig(), 0.1, 2);
  FillRelDiffHist(hist("hMassTrkSysRatio"), aTauDetailsOffline->massTrkSys()/CLHEP::GeV, aTauDetails->massTrkSys()/CLHEP::GeV, 0.1, 1);
  FillRelDiffHist(hist("hDRmaxRatio"), aTauDetailsOffline->seedCalo_dRmax(), aTauDetails->seedCalo_dRmax(), 0.001, 1);
  FillRelDiffHist(hist("hTrkAvgDistRatio"), aTauDetailsOffline->seedCalo_trkAvgDist(), aTauDetails->seedCalo_trkAvgDist(), 0.001, 1);

  //2D plots
  hist2("hEFvsOffnTrks")->Fill(aOfflineTau->numTrack(),aEFTau->numTrack());

//   //Debug plots
//   for (unsigned int i = 0; i < aEFTau->numTrack(); i++) {
//     const Rec::TrackParticle *track = aEFTau->track(i);
//     if (!track) continue;
//     const Trk::MeasuredPerigee* pxPerigee = track->measuredPerigee();
//     hist("hEFZ0")->Fill(pxPerigee->parameters()[Trk::z0]);
//   }
//   for (unsigned int i = 0; i < aOfflineTau->numTrack(); i++) {
//     const Rec::TrackParticle *track = aOfflineTau->track(i);
//     if (!track) continue;
//     const Trk::MeasuredPerigee* pxPerigee = track->measuredPerigee();
//     hist("hOffZ0")->Fill(pxPerigee->parameters()[Trk::z0]);
//   }

  return;
}

//----------------------------------------
// EF and L2 comparison
//----------------------------------------
void HLTTauMonTool::fillEFvsL2(const std::string& trigItem){
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/EFvsL2L1");         
  
  Trig::FeatureContainer f = getTDT()->features("EF_"+trigItem );
  if (f.getCombinations().size() < 1) return;

  Trig::FeatureContainer::combination_const_iterator cIt;     
  
  for( cIt = f.getCombinations().begin(); cIt != f.getCombinations().end(); ++cIt ) {
    
    std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = cIt->get<TrigRoiDescriptor>("initialRoI");
    if ( initRois.size() < 1 ) return;
     
    Trig::Feature<TrigRoiDescriptor> d;
    d = initRois[0];
    
    Trig::Feature<EmTau_ROI> l1TauROI = getTDT()->ancestor<EmTau_ROI>(d);

    if ( l1TauROI.empty() ) return;
    
    std::vector<Trig::Feature<TrigTauCluster> > L2tau = cIt->get<TrigTauCluster>();
    if ( L2tau.empty() ) return;
    
    const TrigTauCluster* l2TauCluster (0);
    for(std::vector< Trig::Feature<TrigTauCluster> >::const_iterator matchedL2 =
	  L2tau.begin(); matchedL2!= L2tau.end(); ++ matchedL2 ){
      if ( matchedL2->te()->getActiveState() ) {
	l2TauCluster = matchedL2->cptr();       
      }
    }

    if (!l2TauCluster) return;

    std::vector<Trig::Feature<Analysis::TauJetContainer> > eftaujet = cIt->get<Analysis::TauJetContainer>();
    if ( eftaujet.empty() ) return;
    if ( !eftaujet[0].te()->getActiveState() ) return;
    const Analysis::TauJetContainer* vec_tauEF = eftaujet[0];
    //Analysis::TauJet* efTau = *(vec_tauEF->begin());
    const Analysis::TauJet* efTau = *(vec_tauEF->begin());
     

    float L2_E_Medium = 0.0;
    float L2_E_Narrow = 0.0;
    float L2_E_Wide = 0.0;
    
    float L2_ET_Medium = 0.0;
    float L2_ET_Narrow = 0.0;
    float L2_ET_Wide = 0.0;
    
    float EF_ET = 0.0;
    
    float L2_HAD_WIDE = 0.0;
    for (int i=0;i<4;++i){
      L2_E_Medium += l2TauCluster->EMenergyMedium(i);
      L2_E_Narrow += l2TauCluster->EMenergyNarrow(i);
      L2_E_Wide += l2TauCluster->EMenergyWide(i);	  
      
    }
    for (int j=0;j<3;++j){
      L2_E_Medium += l2TauCluster->HADenergyMedium(j);
      L2_E_Narrow += l2TauCluster->HADenergyNarrow(j);
      L2_HAD_WIDE += l2TauCluster->HADenergyWide(j);
      L2_E_Wide += l2TauCluster->HADenergyWide(j);	  
      
    }
    
    
    L2_ET_Medium = L2_E_Medium / cosh(l2TauCluster->eta());
    L2_ET_Narrow = L2_E_Narrow / cosh(l2TauCluster->eta());
    L2_ET_Wide = L2_E_Wide / cosh(l2TauCluster->eta());
    
    const Analysis::TauCommonDetails* aTauDetails = efTau->details<const Analysis::TauCommonDetails>();
    if (aTauDetails){
      EF_ET = aTauDetails->seedCalo_etEMAtEMScale() + aTauDetails->seedCalo_etHadAtEMScale();
      hist2("hEtEFUncalibVsL1")->Fill(EF_ET/CLHEP::GeV,l1TauROI.cptr()->getTauClus()/CLHEP::GeV);
      hist2("hEtEFVsL2Narrow")->Fill(EF_ET/CLHEP::GeV,L2_ET_Narrow/CLHEP::GeV);
      hist2("hEtEFVsL2Wide")->Fill(EF_ET/CLHEP::GeV,L2_ET_Wide/CLHEP::GeV);
      hist2("hEtEFVsL2Medium")->Fill(EF_ET/CLHEP::GeV,L2_ET_Medium/CLHEP::GeV);
      hist2("EFL2EtRatioVsEta")->Fill(l2TauCluster->eta(),EF_ET/L2_ET_Wide );
      hist2("EFL2EtRatioVsPhi")->Fill(l2TauCluster->phi(),EF_ET/L2_ET_Wide);
      hist2("L2HADEWideVsEta")->Fill(l2TauCluster->eta(),L2_HAD_WIDE/CLHEP::GeV);
      hist2("L2HADEWideVsPhi")->Fill(l2TauCluster->phi(),L2_HAD_WIDE/CLHEP::GeV);
    }
    else hist2("hEtEFUncalibVsL1")->Fill(-20.,-20.);
    
    hist2("hEtL1VsL2")->Fill(l1TauROI.cptr()->getTauClus()/CLHEP::GeV, l2TauCluster->et()/CLHEP::GeV);
    hist2("hEtaL1VsL2")->Fill(l1TauROI.cptr()->getEta(), l2TauCluster->eta());
    hist2("hPhiL1VsL2")->Fill(l1TauROI.cptr()->getPhi(), l2TauCluster->phi());
    
    hist2("hEtEFVsL1")->Fill(efTau->et()/CLHEP::GeV,l1TauROI.cptr()->getTauClus()/CLHEP::GeV);
    hist2("hEtaEFVsL1")->Fill(efTau->eta(),l1TauROI.cptr()->getEta());
    hist2("hPhiEFVsL1")->Fill(efTau->phi(),l1TauROI.cptr()->getPhi());
    
    hist2("hEtEFVsL2")->Fill(efTau->et()/CLHEP::GeV,l2TauCluster->et()/CLHEP::GeV);
    hist2("hEtaEFVsL2")->Fill(efTau->eta(),l2TauCluster->eta());
    hist2("hPhiEFVsL2")->Fill(efTau->phi(),l2TauCluster->phi());
    
    hist2("hEtL1VsL2Narrow")->Fill(l1TauROI.cptr()->getTauClus()/CLHEP::GeV,L2_ET_Narrow/CLHEP::GeV);
    hist2("hEtL1VsL2Wide")->Fill(l1TauROI.cptr()->getTauClus()/CLHEP::GeV,L2_ET_Wide/CLHEP::GeV);
    hist2("hEtL1VsL2Medium")->Fill(l1TauROI.cptr()->getTauClus()/CLHEP::GeV,L2_ET_Medium/CLHEP::GeV);         
    
    
  }


  
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fillEfficiency(const std::string & trigItem){
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/RelativeEfficiency");

  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("L2_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);

  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFall.begin();
  for (; CI!=vec_L1roi_EFall.end();++CI){
    if(!(CI->cptr())) continue;
    hist("hL1RoIEtaDenom")->Fill(CI->cptr()->eta());
    hist("hL1RoIPhiDenom")->Fill(CI->cptr()->phi());
    const EmTau_ROI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(aEmTau_ROI) {
      hist("hL1RoIEtDenom")->Fill(aEmTau_ROI->getTauClus()/CLHEP::GeV);
      ATH_MSG_DEBUG(" TrigRoiDescriptor (eta,phi): ("<<CI->cptr()->eta()<<", "<<CI->cptr()->phi()<<")"
		    <<" matched EmTau_ROI (eta,phi,et): "
		    <<aEmTau_ROI->eta()<<", "<<aEmTau_ROI->phi()<<", "<<aEmTau_ROI->getTauClus()<<")");
    }
  }


  ///////////////Get passed L1 RoIs

  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_L2pass = (getTDT()->features("L2_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
 
  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("EF_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");

  for(CI = vec_L1roi_L2pass.begin(); CI!=vec_L1roi_L2pass.end(); ++CI){
    if(!(CI->cptr())) continue;
    hist("hL2RoIEtaNum")->Fill(CI->cptr()->eta());
    hist("hL2RoIPhiNum")->Fill(CI->cptr()->phi());
    const EmTau_ROI *aEmTau_ROI = findLVL1_ROI(*CI);
    if(aEmTau_ROI) hist("hL2RoIEtNum")->Fill(aEmTau_ROI->getTauClus()/CLHEP::GeV);
  } 

  for(CI = vec_L1roi_EFpass.begin(); CI!=vec_L1roi_EFpass.end(); ++CI){
    if(!(CI->cptr())) continue;
    hist("hEFRoIEtaNum")->Fill(CI->cptr()->eta());
    hist("hEFRoIPhiNum")->Fill(CI->cptr()->phi());
    const EmTau_ROI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(aEmTau_ROI) hist("hEFRoIEtNum")->Fill(aEmTau_ROI->getTauClus()/CLHEP::GeV);
  }

  ATH_MSG_DEBUG(" number of PassL1RoIs at L2: "<<vec_L1roi_L2pass.size()
		<<" at EF: "<<vec_L1roi_EFpass.size());

  if(vec_L1roi_EFall.size()>0)  ATH_MSG_DEBUG(" L2/L1 eff: "<<vec_L1roi_L2pass.size()/(float)vec_L1roi_EFall.size());


  if(vec_L1roi_L2pass.size()>vec_L1roi_EFall.size())
    ATH_MSG_DEBUG(" number of PassL1RoIs at L2: "<<vec_L1roi_L2pass.size()
		  <<" terminals.size(): "<<vec_L1roi_EFall.size());

  return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
const EmTau_ROI * HLTTauMonTool::findLVL1_ROI(const TrigRoiDescriptor * roiDesc){

  const LVL1_ROI * lvl1ROI;
  StatusCode sc = m_storeGate->retrieve(lvl1ROI);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not find LVL1_ROI in StoreGate.");
    return 0;
  }
  LVL1_ROI::emtaus_type::const_iterator itEMTau = lvl1ROI->getEmTauROIs().begin();
  LVL1_ROI::emtaus_type::const_iterator itEMTau_e = lvl1ROI->getEmTauROIs().end();
 
  unsigned int id = roiDesc->roiWord();
  for( LVL1_ROI::emtaus_type::const_iterator it=itEMTau; it!=itEMTau_e;++it){
    if(id==it->getROIWord()) return &(*it);
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

StatusCode HLTTauMonTool::fillTagandProbe(const std::string & trigItem){
  StatusCode sc;
  const JetCollection* RecjetCont = 0;
  
  sc=m_storeGate->retrieve(RecjetCont, "Cone4H1TowerJets");
  if( sc.isFailure() || !RecjetCont ) {
    *m_log << MSG::DEBUG
	   << "No Rec jet container found in TDS"
	   << endreq;
    return StatusCode::SUCCESS;
  }
  
  JetCollection::const_iterator it_Rec   = RecjetCont->begin();
  JetCollection::const_iterator it_e_Rec = RecjetCont->end();
  
  const  Jet*  leadjet1 = 0; Float_t ptmax1 = 0.;
  const  Jet*  leadjet2 = 0; Float_t ptmax2 = 0.;
  
  for( ; it_Rec!=it_e_Rec; it_Rec++) {
    //Jet* jet = *it_Rec;
    const Jet* jet = *it_Rec;
    double jet_pt  = jet->pt() ;
    double jet_eta = jet->eta();
    if ( jet_pt < 15000. || abs(jet_eta) > 2.5 ) continue;
    if (jet_pt >= ptmax1) {
      ptmax2   = ptmax1;           leadjet2 = leadjet1; 
      ptmax1   = jet_pt;           leadjet1 = jet; 
    }
    else if ( jet_pt >= ptmax2 && jet_pt < ptmax1 ) {
      ptmax2   = jet_pt; leadjet2 = jet;
    }
  }
  bool tagJ1   = false;
  bool tagJ2   = false;
  const Analysis::Tau1P3PDetails* aTau1p3pDetailsJ1 = 0;
  const Analysis::Tau1P3PDetails* aTau1p3pDetailsJ2 = 0;
  const Analysis::TauRecDetails*  aTauRecDetailsJ1  = 0;
  const Analysis::TauRecDetails*  aTauRecDetailsJ2  = 0;
  
  if (leadjet1 && leadjet2  && BackToBackJets(leadjet1, leadjet2) )  {
    
    if ( NTracks(leadjet1) >= 4  ) tagJ1 = true;
    if ( NTracks(leadjet2) >= 4  ) tagJ2 = true;
        
    const  Analysis::TauJet* tauJet_J1   =  EFTauMatchedJet(trigItem, leadjet1);
    const  Analysis::TauJet* tauJet_J2   =  EFTauMatchedJet(trigItem, leadjet2);
    
    if (tauJet_J1) {
      aTau1p3pDetailsJ1 = tauJet_J1->details<const Analysis::Tau1P3PDetails>();
      aTauRecDetailsJ1 =  tauJet_J1->details<const Analysis::TauRecDetails>();

      
    }
    
    if (tauJet_J2){
      
      aTau1p3pDetailsJ2 = tauJet_J2->details<const Analysis::Tau1P3PDetails>();
      aTauRecDetailsJ2 =  tauJet_J2->details<const Analysis::TauRecDetails>();
    } 
    
    if (tagJ1 || tagJ2)  {
      FillTagJet(leadjet1, leadjet2, trigItem);
      FillTagJetBinned(leadjet1, leadjet2, trigItem);
    }
    

    if ( (tagJ1 && aTau1p3pDetailsJ2) || (tagJ2 && aTau1p3pDetailsJ1) )  {
      FillProbeJet1p3p(leadjet1, leadjet2,trigItem);
      FillProbeJet1p3pBinned(leadjet1, leadjet2,trigItem);
    }
    
    if ( (tagJ1 && aTauRecDetailsJ2)  || (tagJ2 && aTauRecDetailsJ1) )   {
      FillProbeJettauRec(leadjet1,leadjet2,trigItem);
      FillProbeJettauRecBinned(leadjet1,leadjet2,trigItem);
    }
    
  } // dijet events
  else  return StatusCode::SUCCESS;
  return StatusCode::SUCCESS;
  
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
float  HLTTauMonTool::delta_phi(float phi1, float phi2)
{
  float PHI=fabs(phi1-phi2);
  return (PHI<=PI)? PHI : TWOPI-PHI;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
const Analysis::TauJet* HLTTauMonTool::OfflineTauMatchedJet( const Jet* jet){
  float tmpR = 0.2;
  const Analysis::TauJet *tauJetM = 0;
  const Analysis::TauJetContainer*  offlinetau= 0;
  if (  (m_storeGate->retrieve(offlinetau, "TauRecContainer") ).isSuccess() ) {
    for (Analysis::TauJetContainer::const_iterator iTauJet = offlinetau->begin();
	 iTauJet != offlinetau->end();
	 iTauJet++) {
      Analysis::TauJet tauJet = **iTauJet;
      if( abs(tauJet.eta()) < 2.5 && tauJet.pt() > 15000.) {
	if( tauJet.hlv().deltaR(jet->hlv()) < tmpR ){
	  tmpR = tauJet.hlv().deltaR(jet->hlv());
	  tauJetM = (*iTauJet); 
	} 
      }
    }
  } 
  return tauJetM;
  
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
int HLTTauMonTool::NTracks(const Jet* jet){
  int Ngoodtr = 0;
  const  Analysis::TrackAssociation* taj1 = (jet)->getAssociation<Analysis::TrackAssociation>("Tracks");
  if( taj1) {
    std::vector<const Rec::TrackParticle*>* trackVector = taj1->tracks();
    for(std::vector<const Rec::TrackParticle*>::iterator trkItr = trackVector->begin(); trkItr !=trackVector->end(); ++trkItr) {
      const Rec::TrackParticle* aTemp = *trkItr;
      if ( aTemp->pt() > 1000.0 ) ++Ngoodtr;
    }     	 
  }
  return Ngoodtr;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
const Analysis::TauJet* HLTTauMonTool::EFTauMatchedJet(const std::string & trigItem, const Jet* jet){
  float tmpR = 0.2;
  const Analysis::TauJet *tauJetM = 0;

  const std::vector<Trig::Feature<Analysis::TauJetContainer> > vec_tauEFClust = (getTDT()->features("EF_"+trigItem)).get<Analysis::TauJetContainer>();
  std::vector<Trig::Feature<Analysis::TauJetContainer> >::const_iterator CI = vec_tauEFClust.begin();
 
  for(; CI != vec_tauEFClust.end(); ++CI)
    if(CI->cptr()){
      Analysis::TauJetContainer::const_iterator tauItr = CI->cptr()->begin();
      Analysis::TauJetContainer::const_iterator tauEnd = CI->cptr()->end();
      for(; tauItr != tauEnd; ++tauItr) {
	Analysis::TauJet tauJet = **tauItr;
	if( abs(tauJet.eta()) < 2.5 && tauJet.pt() > 15000.) {
	  if( tauJet.hlv().deltaR(jet->hlv()) < tmpR ){
	    tmpR = tauJet.hlv().deltaR(jet->hlv());
	    tauJetM = (*tauItr); 
	  } 
	} 
      } 
    }        
  return tauJetM;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool HLTTauMonTool::BackToBackJets(const Jet* jet1, const Jet* jet2){

  float mindphi = 2.84;
  float maxdphi = 3.44;
  bool isbacktoback = false;

  float acop =  delta_phi(jet1->phi(),jet2->phi());
  float dpt  =  jet1->pt() - jet2->pt();
  
  if ( acop >= mindphi && acop <= maxdphi  && dpt < (jet1->pt() / 2.0) ){
    isbacktoback = true;
  }
  return isbacktoback;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::FillTagJet(const Jet* leadjet1, const Jet* leadjet2,const std::string & trigItem){
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/FakeJets");

  hist("LeadJet1_pt_denom")  -> Fill(leadjet1->pt()/CLHEP::GeV);
  hist("LeadJet1_eta_denom") -> Fill(leadjet1->eta());
  hist("LeadJet1_phi_denom") -> Fill(leadjet1->phi());
  
  hist("LeadJet2_pt_denom")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  hist("LeadJet2_eta_denom") -> Fill(leadjet2->eta());
  hist("LeadJet2_phi_denom") -> Fill(leadjet2->phi());
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::FillProbeJet1p3p(const Jet* leadjet1, const Jet* leadjet2,const std::string & trigItem){
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/FakeJets");
  hist("LeadJet1_pt_num_1p3p")  -> Fill(leadjet1->pt()/CLHEP::GeV);
  hist("LeadJet1_eta_num_1p3p") -> Fill(leadjet1->eta());
  hist("LeadJet1_phi_num_1p3p") -> Fill(leadjet1->phi());
  
  hist("LeadJet2_pt_num_1p3p")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  hist("LeadJet2_eta_num_1p3p") -> Fill(leadjet2->eta());
  hist("LeadJet2_phi_num_1p3p") -> Fill(leadjet2->phi());
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::FillProbeJettauRec(const Jet* leadjet1, const Jet* leadjet2,const std::string & trigItem){
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/FakeJets");

  hist("LeadJet1_pt_num_tauRec")  -> Fill(leadjet1->pt()/CLHEP::GeV);
  hist("LeadJet1_eta_num_tauRec") -> Fill(leadjet1->eta());
  hist("LeadJet1_phi_num_tauRec") -> Fill(leadjet1->phi());
  
  hist("LeadJet2_pt_num_tauRec")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  hist("LeadJet2_eta_num_tauRec") -> Fill(leadjet2->eta());
  hist("LeadJet2_phi_num_tauRec") -> Fill(leadjet2->phi());
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void HLTTauMonTool::FillTagJetBinned(const Jet* leadjet1, const Jet* leadjet2,const std::string & trigItem){
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/FakeJets/Binned");

  if (leadjet1->pt()/CLHEP::GeV > 15 && leadjet1->pt()/CLHEP::GeV <= 40)  hist("LeadJet1_pt_denom_1540")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 15 && leadjet2->pt()/CLHEP::GeV <= 40)  hist("LeadJet2_pt_denom_1540")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 40 && leadjet1->pt()/CLHEP::GeV <= 80)  hist("LeadJet1_pt_denom_4080")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 40 && leadjet2->pt()/CLHEP::GeV <= 80)  hist("LeadJet2_pt_denom_4080")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 80 && leadjet1->pt()/CLHEP::GeV <= 120)  hist("LeadJet1_pt_denom_80120")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 80 && leadjet2->pt()/CLHEP::GeV <= 120)  hist("LeadJet2_pt_denom_80120")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 120 && leadjet1->pt()/CLHEP::GeV <= 160)  hist("LeadJet1_pt_denom_120160")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 120 && leadjet2->pt()/CLHEP::GeV <= 160)  hist("LeadJet2_pt_denom_120160")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 160 && leadjet1->pt()/CLHEP::GeV <= 200)  hist("LeadJet1_pt_denom_160200")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 160 && leadjet2->pt()/CLHEP::GeV <= 200)  hist("LeadJet2_pt_denom_160200")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 200 && leadjet1->pt()/CLHEP::GeV <= 400)  hist("LeadJet1_pt_denom_200400")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 200 && leadjet2->pt()/CLHEP::GeV <= 400)  hist("LeadJet2_pt_denom_200400")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 400 && leadjet1->pt()/CLHEP::GeV <= 800)  hist("LeadJet1_pt_denom_400800")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 400 && leadjet2->pt()/CLHEP::GeV <= 800)  hist("LeadJet2_pt_denom_400800")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 800 && leadjet1->pt()/CLHEP::GeV <= 1200)  hist("LeadJet1_pt_denom_8001200")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 800 && leadjet2->pt()/CLHEP::GeV <= 1200)  hist("LeadJet2_pt_denom_8001200")  -> Fill(leadjet2->pt()/CLHEP::GeV);




  
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::FillProbeJet1p3pBinned(const Jet* leadjet1, const Jet* leadjet2,const std::string & trigItem){

  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/FakeJets/Binned");

  if (leadjet1->pt()/CLHEP::GeV > 15 && leadjet1->pt()/CLHEP::GeV <= 40)  hist("LeadJet1_pt_num_1p3p_1540")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 15 && leadjet2->pt()/CLHEP::GeV <= 40)  hist("LeadJet2_pt_num_1p3p_1540")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 40 && leadjet1->pt()/CLHEP::GeV <= 80)  hist("LeadJet1_pt_num_1p3p_4080")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 40 && leadjet2->pt()/CLHEP::GeV <= 80)  hist("LeadJet2_pt_num_1p3p_4080")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 80 && leadjet1->pt()/CLHEP::GeV <= 120)  hist("LeadJet1_pt_num_1p3p_80120")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 80 && leadjet2->pt()/CLHEP::GeV <= 120)  hist("LeadJet2_pt_num_1p3p_80120")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 120 && leadjet1->pt()/CLHEP::GeV <= 160)  hist("LeadJet1_pt_num_1p3p_120160")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 120 && leadjet2->pt()/CLHEP::GeV <= 160)  hist("LeadJet2_pt_num_1p3p_120160")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 160 && leadjet1->pt()/CLHEP::GeV <= 200)  hist("LeadJet1_pt_num_1p3p_160200")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 160 && leadjet2->pt()/CLHEP::GeV <= 200)  hist("LeadJet2_pt_num_1p3p_160200")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 200 && leadjet1->pt()/CLHEP::GeV <= 400)  hist("LeadJet1_pt_num_1p3p_200400")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 200 && leadjet2->pt()/CLHEP::GeV <= 400)  hist("LeadJet2_pt_num_1p3p_200400")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 400 && leadjet1->pt()/CLHEP::GeV <= 800)  hist("LeadJet1_pt_num_1p3p_400800")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 400 && leadjet2->pt()/CLHEP::GeV <= 800)  hist("LeadJet2_pt_num_1p3p_400800")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 800 && leadjet1->pt()/CLHEP::GeV <= 1200)  hist("LeadJet1_pt_num_1p3p_8001200")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 800 && leadjet2->pt()/CLHEP::GeV <= 1200)  hist("LeadJet2_pt_num_1p3p_8001200")  -> Fill(leadjet2->pt()/CLHEP::GeV);

}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void HLTTauMonTool::FillProbeJettauRecBinned(const Jet* leadjet1, const Jet* leadjet2,const std::string & trigItem){
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/FakeJets/Binned");

  if (leadjet1->pt()/CLHEP::GeV > 15 && leadjet1->pt()/CLHEP::GeV <= 40)  hist("LeadJet1_pt_num_tauRec_1540")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 15 && leadjet2->pt()/CLHEP::GeV <= 40)  hist("LeadJet2_pt_num_tauRec_1540")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 40 && leadjet1->pt()/CLHEP::GeV <= 80)  hist("LeadJet1_pt_num_tauRec_4080")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 40 && leadjet2->pt()/CLHEP::GeV <= 80)  hist("LeadJet2_pt_num_tauRec_4080")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 80 && leadjet1->pt()/CLHEP::GeV <= 120)  hist("LeadJet1_pt_num_tauRec_80120")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 80 && leadjet2->pt()/CLHEP::GeV <= 120)  hist("LeadJet2_pt_num_tauRec_80120")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 120 && leadjet1->pt()/CLHEP::GeV <= 160)  hist("LeadJet1_pt_num_tauRec_120160")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 120 && leadjet2->pt()/CLHEP::GeV <= 160)  hist("LeadJet2_pt_num_tauRec_120160")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 160 && leadjet1->pt()/CLHEP::GeV <= 200)  hist("LeadJet1_pt_num_tauRec_160200")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 160 && leadjet2->pt()/CLHEP::GeV <= 200)  hist("LeadJet2_pt_num_tauRec_160200")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  
  if (leadjet1->pt()/CLHEP::GeV > 200 && leadjet1->pt()/CLHEP::GeV <= 400)  hist("LeadJet1_pt_num_tauRec_200400")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 200 && leadjet2->pt()/CLHEP::GeV <= 400)  hist("LeadJet2_pt_num_tauRec_200400")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 400 && leadjet1->pt()/CLHEP::GeV <= 800)  hist("LeadJet1_pt_num_tauRec_400800")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 400 && leadjet2->pt()/CLHEP::GeV <= 800)  hist("LeadJet2_pt_num_tauRec_400800")  -> Fill(leadjet2->pt()/CLHEP::GeV);
  
  if (leadjet1->pt()/CLHEP::GeV > 800 && leadjet1->pt()/CLHEP::GeV <= 1200)  hist("LeadJet1_pt_num_tauRec_8001200")  -> Fill(leadjet1->pt()/CLHEP::GeV);   
  if (leadjet2->pt()/CLHEP::GeV > 800 && leadjet2->pt()/CLHEP::GeV <= 1200)  hist("LeadJet2_pt_num_tauRec_8001200")  -> Fill(leadjet2->pt()/CLHEP::GeV);

}


bool HLTTauMonTool::Match_Offline_L1(const Analysis::TauJet *aOfflineTau, const std::string & trigItem){
  
  bool matchedL1=false;
  float tmpR = 5.0;   

  // const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("EF_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);

  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("L2_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFall.begin();
  for (; CI!=vec_L1roi_EFall.end();++CI){
    if(!(CI->cptr())) continue;
    const EmTau_ROI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(!aEmTau_ROI) continue;
    if ( (aOfflineTau->hlv().deltaR(aEmTau_ROI->hlv() ) ) < tmpR) {
      tmpR = aOfflineTau->hlv().deltaR(aEmTau_ROI->hlv());
    }   
  }  


  if (tmpR <0.2) matchedL1=true;
  return matchedL1;
  
}


bool HLTTauMonTool::Match_Offline_L2(const Analysis::TauJet *aOfflineTau, const std::string & trigItem){
  
  bool matchedL2=false;
  float tmpR = 5.0;

  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_L2pass = (getTDT()->features("L2_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_L2pass.begin();
  for(; CI!=vec_L1roi_L2pass.end(); ++CI){
    if(!(CI->cptr())) continue;      
    const EmTau_ROI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(!aEmTau_ROI) continue;
    if ( (aOfflineTau->hlv().deltaR(aEmTau_ROI->hlv() ) ) < tmpR) {
      tmpR = aOfflineTau->hlv().deltaR(aEmTau_ROI->hlv());
    }      

  }
  


  if (tmpR < 0.2) matchedL2 = true;
  return matchedL2;
  
}


bool HLTTauMonTool::Match_Offline_EF(const Analysis::TauJet *aOfflineTau, const std::string & trigItem){
  
  bool matchedEF=false;
  float tmpR = 5.0;

  const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("EF_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFpass.begin();
  for(; CI!=vec_L1roi_EFpass.end(); ++CI){
    if(!(CI->cptr())) continue;      
    const EmTau_ROI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
    if(!aEmTau_ROI) continue;
    if ( (aOfflineTau->hlv().deltaR(aEmTau_ROI->hlv() ) ) < tmpR) {
      tmpR = aOfflineTau->hlv().deltaR(aEmTau_ROI->hlv());
    }      

  } 




  if (tmpR < 0.2) matchedEF = true;
  
  return matchedEF;
  
}



StatusCode HLTTauMonTool::OfflineEfficiency(const std::string & trigItem){
  
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/OfflineRatio");
  
  const Analysis::TauJet *aOfflineTau = 0;
  
  std::vector<int> nTaus (4,0);
  const DataHandle<Analysis::TauJetContainer> offlinetau;
  if (  (m_storeGate->retrieve(offlinetau, "TauRecContainer") ).isSuccess() ) {
    Analysis::TauJetContainer::const_iterator CI,offlinetau_end = offlinetau->end();
    for(CI=offlinetau->begin();CI!=offlinetau_end; ++CI){
      if(!(*CI)) continue;
      if (((*CI)->pt()/CLHEP::GeV) < 15.0 || abs((*CI)->eta()) > 2.5) continue;      
      if ( ((*CI)->hasAuthor( TauJetParameters::tau1P3P)) && ! ((*CI)->hasAuthor( TauJetParameters::tauRec)) ) continue;
      hist("hTauPt")  -> Fill((*CI)->pt()/CLHEP::GeV);
      if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
	nTaus.at(0) += 1;
	hist("hTauEta")  -> Fill((*CI)->eta());
	hist("hTauPhi")  -> Fill((*CI)->phi());
      }
      aOfflineTau = (*CI);
      
      if(Match_Offline_L1(aOfflineTau, trigItem)) {
	hist("hTauPtL1")  -> Fill((*CI)->pt()/CLHEP::GeV);
	if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
	  nTaus.at(1) += 1;
	  hist("hTauEtaL1")  -> Fill((*CI)->eta());
	  hist("hTauPhiL1")  -> Fill((*CI)->phi());
	}
	if(Match_Offline_L2(aOfflineTau, trigItem)) {
	  hist("hTauPtL2")  -> Fill((*CI)->pt()/CLHEP::GeV);
	  if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
	    nTaus.at(2) += 1;
	    hist("hTauEtaL2")  -> Fill((*CI)->eta());   
	    hist("hTauPhiL2")  -> Fill((*CI)->phi()); 
	  }
	  if(Match_Offline_EF(aOfflineTau, trigItem)) {
	    hist("hTauPtEF")  -> Fill((*CI)->pt()/CLHEP::GeV);
	    if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
	      nTaus.at(3) += 1;
	      hist("hTauEtaEF")  -> Fill((*CI)->eta());
	      hist("hTauPhiEF")  -> Fill((*CI)->phi());
	    }
	  }
	}
      }
    }
  }

  const VxContainer* vxContainer(0);
  if (evtStore()->contains<VxContainer>("VxPrimaryCandidate")) {
    if (evtStore()->retrieve(vxContainer,"VxPrimaryCandidate").isFailure() ) {
      ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key VxPrimaryCandidate");
      return StatusCode::SUCCESS;
    }
  } else {
    ATH_MSG_DEBUG ("StoreGate doesn't contain primary vertex container with key VxPrimaryCandidate");
    return StatusCode::SUCCESS;
  }

  int nVtx = vxContainer->size();
  int nbins = hist("hTauNvtx")->GetNbinsX();
  if (nVtx > 0){
    if (nVtx <= nbins) {
      if (nTaus.at(0) > 0) hist("hTauNvtx")  ->SetBinContent(nVtx, (nTaus.at(0) + hist("hTauNvtx")  ->GetBinContent(nVtx)));
      if (nTaus.at(1) > 0) hist("hTauNvtxL1")->SetBinContent(nVtx, (nTaus.at(1) + hist("hTauNvtxL1")->GetBinContent(nVtx)));
      if (nTaus.at(2) > 0) hist("hTauNvtxL2")->SetBinContent(nVtx, (nTaus.at(2) + hist("hTauNvtxL2")->GetBinContent(nVtx)));
      if (nTaus.at(3) > 0) hist("hTauNvtxEF")->SetBinContent(nVtx, (nTaus.at(3) + hist("hTauNvtxEF")->GetBinContent(nVtx)));
    }
    else {
      if (nTaus.at(0) > 0) hist("hTauNvtx")  ->SetBinContent(nbins+1, (nTaus.at(0) + hist("hTauNvtx")  ->GetBinContent(nbins+1)));
      if (nTaus.at(1) > 0) hist("hTauNvtxL1")->SetBinContent(nbins+1, (nTaus.at(1) + hist("hTauNvtxL1")->GetBinContent(nbins+1)));
      if (nTaus.at(2) > 0) hist("hTauNvtxL2")->SetBinContent(nbins+1, (nTaus.at(2) + hist("hTauNvtxL2")->GetBinContent(nbins+1)));
      if (nTaus.at(3) > 0) hist("hTauNvtxEF")->SetBinContent(nbins+1, (nTaus.at(3) + hist("hTauNvtxEF")->GetBinContent(nbins+1)));
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode HLTTauMonTool::OfflineEfficiencyBDTMedium(const std::string & trigItem){
  
  setCurrentMonGroup("HLT/TauMon/"+trigItem+"/OfflineRatio/BDTMedium");
  
  const Analysis::TauJet *aOfflineTau = 0;
  
  std::vector<int> nTaus (4,0);
  const DataHandle<Analysis::TauJetContainer> offlinetau;
  if (  (m_storeGate->retrieve(offlinetau, "TauRecContainer") ).isSuccess() ) {
    Analysis::TauJetContainer::const_iterator CI,offlinetau_end = offlinetau->end();
    for(CI=offlinetau->begin();CI!=offlinetau_end; ++CI){
      if(!(*CI)) continue;
      if (((*CI)->pt()/CLHEP::GeV) < 15.0 || abs((*CI)->eta()) > 2.5) continue;      
      if ( ((*CI)->hasAuthor( TauJetParameters::tau1P3P)) && ! ((*CI)->hasAuthor( TauJetParameters::tauRec)) ) continue;
      
      const Analysis::TauPID *p_tauid = (*CI)->tauID();
      
      if (p_tauid && p_tauid->isTau( TauJetParameters::JetBDTSigMedium)){
	hist("hTauPtBDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
	if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
	  nTaus.at(0) += 1;
	  hist("hTauEtaBDTMedium")  -> Fill((*CI)->eta());
	  hist("hTauPhiBDTMedium")  -> Fill((*CI)->phi());
	}	
      	aOfflineTau = (*CI);
	
	if(Match_Offline_L1(aOfflineTau, trigItem)) {
	  hist("hTauPtL1BDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
	  if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
	    nTaus.at(1) += 1;
	    hist("hTauEtaL1BDTMedium")  -> Fill((*CI)->eta());   
	    hist("hTauPhiL1BDTMedium")  -> Fill((*CI)->phi()); 
	  }
	  if(Match_Offline_L2(aOfflineTau, trigItem)) {
	    hist("hTauPtL2BDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
	    if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
	      nTaus.at(2) += 1;
	      hist("hTauEtaL2BDTMedium")  -> Fill((*CI)->eta());
	      hist("hTauPhiL2BDTMedium")  -> Fill((*CI)->phi());
	    }
	    if(Match_Offline_EF(aOfflineTau, trigItem)) {
	      hist("hTauPtEFBDTMedium")  -> Fill((*CI)->pt()/CLHEP::GeV);
	      if (((*CI)->pt()/CLHEP::GeV) > m_effOffTauPtCut) {
		nTaus.at(3) += 1;
		hist("hTauEtaEFBDTMedium")  -> Fill((*CI)->eta());
		hist("hTauPhiEFBDTMedium")  -> Fill((*CI)->phi());
	      }
	    }
	  }
	}	
      }      
    }
  }

  const VxContainer* vxContainer(0);
  if (evtStore()->contains<VxContainer>("VxPrimaryCandidate")) {
    if (evtStore()->retrieve(vxContainer,"VxPrimaryCandidate").isFailure() ) {
      ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key VxPrimaryCandidate");
      return StatusCode::SUCCESS;
    }
  } else {
    ATH_MSG_DEBUG ("StoreGate doesn't contain primary vertex container with key VxPrimaryCandidate");
    return StatusCode::SUCCESS;
  }

  int nVtx = vxContainer->size();
  int nbins = hist("hTauNvtxBDTMedium")->GetNbinsX();
  if (nVtx > 0){
    if (nVtx <= nbins) {
      if (nTaus.at(0) > 0) hist("hTauNvtxBDTMedium")  ->SetBinContent(nVtx, (nTaus.at(0) + hist("hTauNvtxBDTMedium")  ->GetBinContent(nVtx)));
      if (nTaus.at(1) > 0) hist("hTauNvtxL1BDTMedium")->SetBinContent(nVtx, (nTaus.at(1) + hist("hTauNvtxL1BDTMedium")->GetBinContent(nVtx)));
      if (nTaus.at(2) > 0) hist("hTauNvtxL2BDTMedium")->SetBinContent(nVtx, (nTaus.at(2) + hist("hTauNvtxL2BDTMedium")->GetBinContent(nVtx)));
      if (nTaus.at(3) > 0) hist("hTauNvtxEFBDTMedium")->SetBinContent(nVtx, (nTaus.at(3) + hist("hTauNvtxEFBDTMedium")->GetBinContent(nVtx)));
    }
    else {
      if (nTaus.at(0) > 0) hist("hTauNvtxBDTMedium")  ->SetBinContent(nbins+1, (nTaus.at(0) + hist("hTauNvtxBDTMedium")  ->GetBinContent(nbins+1)));
      if (nTaus.at(1) > 0) hist("hTauNvtxL1BDTMedium")->SetBinContent(nbins+1, (nTaus.at(1) + hist("hTauNvtxL1BDTMedium")->GetBinContent(nbins+1)));
      if (nTaus.at(2) > 0) hist("hTauNvtxL2BDTMedium")->SetBinContent(nbins+1, (nTaus.at(2) + hist("hTauNvtxL2BDTMedium")->GetBinContent(nbins+1)));
      if (nTaus.at(3) > 0) hist("hTauNvtxEFBDTMedium")->SetBinContent(nbins+1, (nTaus.at(3) + hist("hTauNvtxEFBDTMedium")->GetBinContent(nbins+1)));
    }
  }

  return StatusCode::SUCCESS;
}


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

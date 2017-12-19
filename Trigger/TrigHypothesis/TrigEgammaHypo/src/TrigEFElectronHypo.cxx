/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFElectronHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFElectronHypo
//
// MAINTAINED BY: Ryan M. White, Alessandro Tricoli
//
// MODIFIED: A.Tricoli: Use of offline tools to build isEM (Jan 2010)
// MODIFIED: R.M. White: Use PATCore tools to build isEM (Aug 2013)
// MODIFIED: R.M. White: xAOD Migration (April 2014) 
// ********************************************************************
// Comments on xAOD
// Removing checks for showershape checks, track particle checks
// Assume that electrons must have this information
//
#include <list>
#include <iterator>
#include <sstream>
#include <map>

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigEgammaHypo/TrigEFElectronHypo.h"
#include "egammaEvent/egammaParamDefs.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgammaCnv/xAODElectronMonFuncs.h"
#include "VxVertex/RecVertex.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "PATCore/TAccept.h"            // for TAccept
#include "PATCore/TResult.h"            // for TResult
#include "xAODTrigger/TrigPassBits.h"

using std::string;

namespace {
    template <class DEST,class SRC>
        inline DEST** my_pp_cast(SRC** ptr) {
            return (DEST**)(ptr);
        }
}

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFElectronHypo::TrigEFElectronHypo(const std::string& name, 
				   ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
    m_primaryVertex(Amg::Vector3D()), 
    m_trackToVertexTool("Reco::TrackToVertex")
  
{
  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("CaloCutsOnly", m_caloCutsOnly);
  
  //isEM offline
  declareProperty("ApplyIsEM",m_applyIsEM = false);
  declareProperty("ApplyEtIsEM",m_applyEtIsEM = false);
  declareProperty("IsEMrequiredBits",m_IsEMrequiredBits = 0xF2);
  
  declareProperty("egammaElectronCutIDToolName",m_egammaElectronCutIDToolName="");
  //TSelector
  //Athena Selector Tool
  declareProperty("AthenaElectronLHIDSelectorToolName", m_athElectronLHIDSelectorToolName="");
  declareProperty("UseAthenaElectronLHIDSelectorTool", m_useAthElectronLHIDSelector=false);
  
  //Tool for track extrapolation to vertex  	  
  declareProperty("trackToVertexTool", m_trackToVertexTool,  
		  "Tool for track extrapolation to vertex"); 

  /** Luminosity tool */
  declareProperty("LuminosityTool", m_lumiBlockMuTool, "Luminosity Tool");
  
  declareProperty("emEt",m_emEt = -3.*CLHEP::GeV);

  typedef const DataVector<xAOD::Electron> xAODElectronDV_type;
  
  // Cluster and ShowerShape Monitoring
  declareMonitoredCollection("El_E237",   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_e237);
  declareMonitoredCollection("El_E277",   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_e277);
  declareMonitoredCollection("El_Reta",   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_Reta);
  declareMonitoredCollection("El_Rphi",   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_Rphi);
  declareMonitoredCollection("El_Rhad",   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_Rhad);
  declareMonitoredCollection("El_Rhad1",   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_Rhad1);
  declareMonitoredCollection("El_EtHad1", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_ethad1);
  declareMonitoredCollection("El_WEta1",  *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_weta1);
  declareMonitoredCollection("El_WEta2",  *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_weta2);
  declareMonitoredCollection("El_F1",     *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_f1);
  declareMonitoredCollection("El_F3",     *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_f3);
  declareMonitoredCollection("El_F3core",     *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_f3core);
  declareMonitoredCollection("El_Emax2",*my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_e2tsts1);
  declareMonitoredCollection("El_Emins1", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_emins1);
  declareMonitoredCollection("El_Emax", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_emaxs1);
  declareMonitoredCollection("El_DEmaxs1", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getDEmaxs1);
  declareMonitoredCollection("El_wtots1", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_wtots1);
  declareMonitoredCollection("El_Fracs1", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_fracs1);
  declareMonitoredCollection("El_Eratio", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_Eratio);
  declareMonitoredCollection("El_DeltaE", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getShowerShape_DeltaE);
  declareMonitoredCollection("El_ClusterEt37",   *my_pp_cast<xAODElectronDV_type>(&m_EgammaContainer), &getEtCluster37);
  declareMonitoredCollection("El_EtCone20",     *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getIsolation_etcone20);
  declareMonitoredCollection("El_PtCone20",     *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getIsolation_ptcone20);
  declareMonitoredCollection("El_Eta",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getCluster_eta);
  declareMonitoredCollection("El_Phi",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getCluster_phi);
  declareMonitoredCollection("El_ClusterEt",     *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getCluster_et);
  declareMonitoredCollection("El_Eta2",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getEta2);
  declareMonitoredCollection("El_EnergyBE0",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getEnergyBE0);
  declareMonitoredCollection("El_EnergyBE1",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getEnergyBE1);
  declareMonitoredCollection("El_EnergyBE2",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getEnergyBE2);
  declareMonitoredCollection("El_EnergyBE3",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getEnergyBE3);
  declareMonitoredCollection("El_Eaccordion",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getEaccordion);
  declareMonitoredCollection("El_E0Eaccordion",           *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getE0Eaccordion);

  //Track-related monitoring accesible from xAOD::Electron
  declareMonitoredCollection("nBLayerHits",            *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrackSummary_numberOfInnermostPixelLayerHits);
  declareMonitoredCollection("nPixelHits",             *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrackSummary_numberOfPixelHits);
  declareMonitoredCollection("nSCTHits",               *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrackSummary_numberOfSCTHits);
  declareMonitoredCollection("nTRTHits",               *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrackSummary_numberOfTRTHits);
  declareMonitoredCollection("nTRTHitsHighTh",         *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrackSummary_numberOfTRTHighThresholdHits);
  declareMonitoredCollection("nTRTHitsHighThOutliers", *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrackSummary_numberOfTRTHighThresholdOutliers);
  declareMonitoredCollection("nTRTHitsOutliers",       *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrackSummary_numberOfTRTOutliers);
  declareMonitoredCollection("TrackPt",                *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrack_pt);
  declareMonitoredCollection("d0",                *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrack_d0);
  declareMonitoredCollection("z0",                *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getTrack_z0);
  declareMonitoredCollection("dEta",                   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getCaloTrackMatch_deltaEta2);
  declareMonitoredCollection("dPhi",                   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getCaloTrackMatch_deltaPhi2);
  declareMonitoredCollection("dPhiRescaled",                   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getCaloTrackMatch_deltaPhiRescaled2);
  declareMonitoredCollection("rTRT",                   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &rTRT);
  declareMonitoredCollection("SigmaD0",                   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getSigmaD0);
  declareMonitoredCollection("D0sig",                   *my_pp_cast <xAODElectronDV_type>(&m_EgammaContainer), &getD0sig);
  declareMonitoredVariable("CutCounter",m_NofPassedCuts);//Event Counter*/
  
  //Isolation
  declareProperty("ApplyIsolation", m_applyIsolation=false);
  declareProperty("EtConeSizes",    m_EtConeSizes = 3);
  declareProperty("RelEtConeCut",   m_RelEtConeCut);
  declareProperty("EtConeCut",      m_EtConeCut);  
  declareProperty("PtConeSizes",    m_PtConeSizes = 6);
  declareProperty("RelPtConeCut",   m_RelPtConeCut);
  declareProperty("PtConeCut",      m_PtConeCut);
  declareProperty("useClusETforCaloIso",      m_useClusETforCaloIso = true);
  declareProperty("useClusETforTrackIso",      m_useClusETforTrackIso = false);

  //isEM monitoring 
  declareMonitoredStdContainer("IsEMBeforeCut",m_NcandIsEM);//per-object counter of failing isEM bits
  declareMonitoredStdContainer("IsEMAfterCut",m_NcandIsEMAfterCut);//per-object counter of failing isEM bits
  //Monitor isEM 32-Bit Pattern Before Cuts
  declareMonitoredStdContainer("IsEMRequiredBitsBeforeCut",m_IsEMRequiredBits);
  //Monitor isEM 32-Bit Pattern After Cuts
  declareMonitoredStdContainer("IsEMRequiredBitsAfterCut",m_IsEMRequiredBitsAfterCut); 
  // Monitor pileup 
  declareMonitoredStdContainer("mu",m_avgmu);
  // Monitor liekihood output 
  declareMonitoredStdContainer("LikelihoodRatio",m_lhval);

  prepareMonitoringVars();
  //Initialize pointers 
  m_totalTimer = nullptr;
  m_timerPIDTool = nullptr;
  m_EgammaContainer = nullptr;
  
  //isEM monitoring 
  m_NcandIsEM.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
  m_NcandIsEMAfterCut.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
  m_IsEMRequiredBits.assign(32,0);
  m_IsEMRequiredBitsAfterCut.assign(32,0);
}

void TrigEFElectronHypo::prepareMonitoringVars() {
  //isEM monitoring 

  m_NcandIsEM.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton

  //
  m_NcandIsEMAfterCut.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton


  //Monitor isEM 32-Bit Pattern Before Cuts
  m_IsEMRequiredBits.assign(32,0);

  //Monitor isEM 32-Bit Pattern After Cuts
  m_IsEMRequiredBitsAfterCut.assign(32,0);
  
  m_NofPassedCuts=-1;
  m_NofPassedCutsIsEM=-1;
  m_NofPassedCutsIsEMTrig=-1;
}


/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFElectronHypo::~TrigEFElectronHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFElectronHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
    ATH_MSG_DEBUG( name() << " in initialize()");

  // Initialize timing service
  //------------------------------
  if (timerSvc())
    m_totalTimer = addTimer("TrigEFElectronHypoTot");  

 
  //retrieving TrackToVertex:    
  if ( m_trackToVertexTool.retrieve().isFailure() ) {  
      ATH_MSG_ERROR("Failed to retrieve tool " << m_trackToVertexTool);
      return HLT::BAD_JOB_SETUP;  
  } else {  
    ATH_MSG_DEBUG("Retrieved tool " << m_trackToVertexTool);
  }

  //-------------------------------------------------------------------------------
  // Use egammaElectronCutIDTool to run the Offline isEM Buildre in the Hypo.
  // The egammaElectronCutIDToolName runs the Electron Selecton only.
  //------------------------------------------------------------------------------
  
  if (m_egammaElectronCutIDToolName=="") {
      ATH_MSG_DEBUG("Electron IsEM PID is disabled, no tool specified "); 
      m_egammaElectronCutIDTool=ToolHandle<IAsgElectronIsEMSelector>();
  } 
  else {
      m_egammaElectronCutIDTool=ToolHandle<IAsgElectronIsEMSelector>(m_egammaElectronCutIDToolName);    
      if(m_egammaElectronCutIDTool.retrieve().isFailure()) {
          ATH_MSG_ERROR("Unable to retrieve " << m_egammaElectronCutIDTool<< " tool ");
          return HLT::BAD_JOB_SETUP; 
      } 
      else {
          ATH_MSG_DEBUG("Tool " << m_egammaElectronCutIDTool << " retrieved");
          if (timerSvc()) m_timerPIDTool = addTimer("AsgElectronIsEMTool");
      }
  }
  
  if (m_athElectronLHIDSelectorToolName=="") {
      ATH_MSG_DEBUG("Electron LH PID is disabled, no tool specified  "); 
       m_athElectronLHIDSelectorTool=ToolHandle<IAsgElectronLikelihoodTool>();
  }
  else {
      m_athElectronLHIDSelectorTool=ToolHandle<IAsgElectronLikelihoodTool>(m_athElectronLHIDSelectorToolName);
      if(m_athElectronLHIDSelectorTool.retrieve().isFailure()) {
	  ATH_MSG_ERROR("Unable to retrieve " << m_athElectronLHIDSelectorTool);
	  return HLT::BAD_JOB_SETUP; 
      } 
      else{
          ATH_MSG_DEBUG("Tool " << m_athElectronLHIDSelectorTool << " retrieved");
          if (timerSvc()) m_timerPIDTool = addTimer("AsgElectronLHTool");
      }
  }
  // For now, just try to retrieve the lumi tool
  if (m_lumiBlockMuTool.retrieve().isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve Luminosity Tool");
  } else {
      ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");
  }
  //print summary info
  ATH_MSG_INFO("REGTEST: Particle Identification tool: " << m_egammaElectronCutIDToolName);
  ATH_MSG_INFO("REGTEST: Athena LH Particle Identification tool: " << m_athElectronLHIDSelectorToolName);
  //------------------------------------------------------------------------------


  //Check Isolation Cone Sizes
  if(m_applyIsolation){
    if ( m_EtConeCut.size() != m_EtConeSizes ) {
      ATH_MSG_ERROR(" m_EtConeCut size is " <<  m_EtConeCut.size() << " but needs " << m_EtConeSizes);
      return HLT::BAD_JOB_SETUP;
    }
    if ( m_RelEtConeCut.size() != m_EtConeSizes ) {
      ATH_MSG_ERROR(" m_RelEtConeCut size is " <<  m_RelEtConeCut.size() << " but needs " << m_EtConeSizes);
      return HLT::BAD_JOB_SETUP;
    }
    if ( m_PtConeCut.size() != m_PtConeSizes ) {
      ATH_MSG_ERROR(" m_PtConeCut size is " <<  m_PtConeCut.size() << " but needs " << m_PtConeSizes);
      return HLT::BAD_JOB_SETUP;
    }
    if ( m_RelPtConeCut.size() != m_PtConeSizes ) {
      ATH_MSG_ERROR(" m_RelPtConeCut size is " <<  m_RelPtConeCut.size() << " but needs " << m_PtConeSizes);
      return HLT::BAD_JOB_SETUP;
    }

    //Define mapping between vector of Isolation Cone Sizes and variable names 
    m_mapEtCone.insert(std::pair<int, string>(0, "etcone20")); 
    m_mapEtCone.insert(std::pair<int, string>(1, "etcone30")); 
    m_mapEtCone.insert(std::pair<int, string>(2, "etcone40"));
    //
    m_mapPtCone.insert(std::pair<int, string>(0, "ptcone20")); 
    m_mapPtCone.insert(std::pair<int, string>(1, "ptcone30")); 
    m_mapPtCone.insert(std::pair<int, string>(2, "ptcone40")); 
    m_mapPtCone.insert(std::pair<int, string>(3, "ptvarcone20")); 
    m_mapPtCone.insert(std::pair<int, string>(4, "ptvarcone30")); 
    m_mapPtCone.insert(std::pair<int, string>(5, "ptvarcone40")); 
    //
    m_mapRelEtCone.insert(std::pair<int, string>(0, "etcone20/ele_pt")); 
    m_mapRelEtCone.insert(std::pair<int, string>(1, "etcone30/ele_pt")); 
    m_mapRelEtCone.insert(std::pair<int, string>(2, "etcone40/ele_pt")); 
    //
    m_mapRelPtCone.insert(std::pair<int, string>(0, "ptcone20/ele_pt")); 
    m_mapRelPtCone.insert(std::pair<int, string>(1, "ptcone30/ele_pt")); 
    m_mapRelPtCone.insert(std::pair<int, string>(2, "ptcone40/ele_pt")); 
    m_mapRelPtCone.insert(std::pair<int, string>(3, "ptvarcone20/ele_pt")); 
    m_mapRelPtCone.insert(std::pair<int, string>(4, "ptvarcone30/ele_pt")); 
    m_mapRelPtCone.insert(std::pair<int, string>(5, "ptvarcone40/ele_pt")); 

  }//end of if(m_applyIsolation)

  //Print Out
  ATH_MSG_INFO("Initialization of TrigEFElectronHypo completed successfully");
    
    // print cuts
  if (m_acceptAll) 
      ATH_MSG_INFO("AcceptAll property is set: taking all events");
  else 
      ATH_MSG_INFO("AcceptAll property not set: applying selection");
    
    ATH_MSG_INFO(" ApplyIsEM: "<< m_applyIsEM);
    ATH_MSG_INFO(" ApplyIsEMEt: "<< m_applyEtIsEM);
    ATH_MSG_INFO(" emEt: "<< m_emEt);
    ATH_MSG_INFO(" IsEMRequired: "<< m_IsEMrequiredBits);
  
  return HLT::OK;
 }

HLT::ErrorCode TrigEFElectronHypo::hltBeginRun() {
  prepareMonitoringVars();
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFElectronHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFElectronHypo::hltExecute(const HLT::TriggerElement* outputTE,
                                              bool& accepted) {
  // ----------------------------------------------------------------------

  m_EgammaContainer = 0;
  m_NofPassedCuts=-1;
  m_NofPassedCutsIsEM=-1;
  m_NofPassedCutsIsEMTrig=-1;
  m_lhval.clear();
  m_avgmu.clear();
  // Time total TrigEFElectronHypo execution time.
  // -------------------------------------
  if (timerSvc()) m_totalTimer->start();    


  ATH_MSG_DEBUG(name() << ": in execute()");
 
  // default value, it will be set to true if selection satisfied
  accepted=false;

  
  // get egamma objects from the trigger element:
  //--------------------------------------------------
  
  std::vector<const xAOD::ElectronContainer*> vectorEgammaContainers;
  
  HLT::ErrorCode stat = getFeatures(outputTE, vectorEgammaContainers, "");

  if (stat != HLT::OK ) {
    ATH_MSG_WARNING(" Failed to get egammaContainer's from the trigger element" );
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  } 

  ATH_MSG_DEBUG(" Got " << vectorEgammaContainers.size() 
          << " egammaContainers's associated to the TE ");

  if (vectorEgammaContainers.size() < 1) {
    ATH_MSG_DEBUG(" empty egammaContainer from the trigger element" );
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  } 

  m_EgammaContainer = vectorEgammaContainers.back();

  if(m_EgammaContainer == 0){
      ATH_MSG_ERROR(" REGTEST: Retrieval of egammaContainer from vector failed");
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  }
  
  // AcceptAll property = true means selection cuts should not be applied
  if (m_acceptAll) {
      accepted = true;
      ATH_MSG_DEBUG("AcceptAll property is set: taking all events ");
  } 
  else 
      ATH_MSG_DEBUG("AcceptAll property not set: applying selection");


  // generate TrigPassBits mask to flag which egamma objects pass hypo cuts
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::ElectronContainer>(m_EgammaContainer);
  //counters for each cut
  int Ncand[10];
  for(int i=0;i<10;i++) Ncand[i]=0;

  //isEM monitoring variables
  for(unsigned int i=0;i<32;i++) m_NcandIsEM[i]=0;//28->32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton 
  for(unsigned int i=0;i<32;i++) m_NcandIsEMAfterCut[i]=0;//28->32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
  m_NofPassedCuts = 0;
  m_NofPassedCutsIsEM = 0;
  m_NofPassedCutsIsEMTrig = 0;

  //Monitor the required isEM bits Before/After Cuts
  for(unsigned int i=0;i<32;i++) m_IsEMRequiredBits[i]=0;
  for(unsigned int i=0;i<32;i++) m_IsEMRequiredBitsAfterCut[i]=0;

  //Monitor the required isEM 32-Bit pattern Before Cuts
  for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton	
    m_IsEMRequiredBits[i]+= ((m_IsEMrequiredBits & (0x1<<i)) != 0); 
  }
  
  // Retrieve lumi information
  bool useLumiTool=false;
  double mu = 0.;
  double avg_mu = 0.;
  if(m_lumiBlockMuTool){
      useLumiTool=true;
      mu = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
      avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
      ATH_MSG_DEBUG("REGTEST: Retrieved Mu Value : " << mu);
      ATH_MSG_DEBUG("REGTEST: Average Mu Value   : " << avg_mu);
      m_avgmu.push_back(avg_mu);
  }
  
  for (const auto& egIt : *m_EgammaContainer){

    int cutIndex=0;
           
    Ncand[cutIndex++]++;

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST Ncand[0]: " << Ncand[0] << endmsg;
    if(m_acceptAll){
        xBits->markPassing(egIt,m_EgammaContainer,true);
        continue;
    }
    //-------------------------------------------------------------
    //Apply cut on IsEM bit pattern re-running the  Offline Builder
    if( m_applyIsEM){ 
        //In order to force the tool to pick up the same cuts for Et>20 GeV, change the trigger threshold
        double temp_EtThreshold=m_emEt; // Use the object Et rather than the trigger threshold

        ATH_MSG_DEBUG("Et for Cut selector " << temp_EtThreshold);
        //To re-run the Offline Electron isEM Builder
        //Old and new tool 
        unsigned int isEMTrig = 0;
        // again calculate PID but use Athena Selector Tool
        bool isLHAcceptTrig = false;
        float lhval=0;
        if(m_useAthElectronLHIDSelector){
            //Check the tool

            if (m_athElectronLHIDSelectorTool == 0) {
                ATH_MSG_ERROR(m_athElectronLHIDSelectorTool << " null, hypo continues but no AthenaLHSelector cut applied");
            }else{
                if (timerSvc()) m_timerPIDTool->start(); //timer
                if(useLumiTool){
                    const Root::TAccept& acc = m_athElectronLHIDSelectorTool->accept(egIt,avg_mu);
                    lhval=m_athElectronLHIDSelectorTool->getTResult().getResult(0);
                    ATH_MSG_DEBUG("LHValue with mu " << lhval);
                    m_lhval.push_back(lhval);
                    isLHAcceptTrig = (bool) (acc);
                }
                else {
                    ATH_MSG_DEBUG("Lumi tool returns mu = 0, do not pass mu");
                    const Root::TAccept& acc = m_athElectronLHIDSelectorTool->accept(egIt);
                    lhval=m_athElectronLHIDSelectorTool->getTResult().getResult(0);
                    ATH_MSG_DEBUG("LHValue without mu " << lhval);
                    m_lhval.push_back(lhval);
                    isLHAcceptTrig = (bool) (acc);
                }

                ATH_MSG_DEBUG("AthenaLHSelectorTool: TAccept = " << isLHAcceptTrig);
                ATH_MSG_DEBUG("Stored Result LHVLoose " << egIt->passSelection("LHVLoose"));
                ATH_MSG_DEBUG("Stored Result LHLoose " << egIt->passSelection("LHLoose"));
                ATH_MSG_DEBUG("Stored Result LHMedium " << egIt->passSelection("LHMedium"));
                ATH_MSG_DEBUG("Stored Result LHTight " << egIt->passSelection("LHTight"));
                if (timerSvc()) m_timerPIDTool->stop(); //timer
            }
        }
        else  if (m_egammaElectronCutIDTool == 0) {
            ATH_MSG_ERROR(m_egammaElectronCutIDTool << " null, hypo continues but no isEM cut applied");
        }else{
            if (timerSvc()) m_timerPIDTool->start(); //timer
            if ( m_egammaElectronCutIDTool->execute(egIt).isFailure() ) 
                ATH_MSG_DEBUG("problem with egammaElectronCutIDTool, egamma object not stored");
            isEMTrig = m_egammaElectronCutIDTool->IsemValue();
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                <<" isEMTrig = "
                    << std::hex << isEMTrig
                    << endmsg;
            unsigned int isEMbit=0;
            ATH_MSG_DEBUG("isEMVLoose " << egIt->selectionisEM(isEMbit,"isEMVLoose"));
            ATH_MSG_DEBUG("isEMVLoose " << std::hex << isEMbit);
            ATH_MSG_DEBUG("isEMLoose " << egIt->selectionisEM(isEMbit,"isEMLoose"));
            ATH_MSG_DEBUG("isEMLoose " << std::hex << isEMbit);
            ATH_MSG_DEBUG("isEMMedium " << egIt->selectionisEM(isEMbit,"isEMMedium"));
            ATH_MSG_DEBUG("isEMMedium " << std::hex << isEMbit);
            ATH_MSG_DEBUG("isEMTight " << egIt->selectionisEM(isEMbit,"isEMTight"));
            ATH_MSG_DEBUG("isEMTight " << std::hex << isEMbit);
            if (timerSvc()) m_timerPIDTool->stop(); //timer
        }
        
        //Monitor isEM Before Cut
        for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
            m_NcandIsEM[i]+= ((isEMTrig & (0x1<<i)) != 0); 
        }

        //Apply cut from LH selector 
        if(m_useAthElectronLHIDSelector){
            ATH_MSG_DEBUG(m_athElectronLHIDSelectorTool << " AthenaLHSelectorTool configured, hypo continues with TAccept ");
            if( !isLHAcceptTrig) {
                ATH_MSG_DEBUG(m_athElectronLHIDSelectorTool << " AthenaLHSelectorTool, hypo fails isEM ");
                continue;
            }
        }
        //Apply cut on IsEMTrigCut bit pattern
        else {
          ATH_MSG_DEBUG(m_egammaElectronCutIDTool << " AthenaSelectorTool configured, hypo continues with isEMTrig ");
            if( (isEMTrig & m_IsEMrequiredBits)!=0 ) {
                ATH_MSG_DEBUG("REGTEST IsEM = " << std::hex << isEMTrig 
                    << " cut not satisfied for pattern:" << std::hex << m_IsEMrequiredBits);
                continue; 
            }
        } 


        //Monitor isEM After Cut
        for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
            m_NcandIsEMAfterCut[i]+= ((isEMTrig & (0x1<<i)) != 0); 
        }

        //Monitor the required isEM 32-Bit pattern After Cut
        for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton	
            m_IsEMRequiredBitsAfterCut[i]+= ((m_IsEMrequiredBits & (0x1<<i)) != 0); 
        }


    }//end of if( m_applyIsEM)
    Ncand[cutIndex++]++;
    
    //---------------------------------------------
    //Other Trigger Specific Cuts
    //---------------------------------------------
    const xAOD::CaloCluster* clus = egIt->caloCluster();
    if(!clus) {
        ATH_MSG_DEBUG("REGTEST no cluster pointer in egamma object ");
      continue;
    }
    if( clus->et() < m_emEt) {
        ATH_MSG_DEBUG("REGTEST Et cut no satisfied: "<< clus->et() << "< cut: " << m_emEt);
      continue;
    }
    Ncand[cutIndex++]++;
    
    ATH_MSG_DEBUG("REGTEST Ncand[1]: " << Ncand[1]);

    //---------------------------------------------
    //Isolation Cuts
    //---------------------------------------------
    if(!m_applyIsolation) {
      //For CutCount monitoring histogram,to fill isolation bins when no isolation cut is applied
      Ncand[cutIndex++]++;//Abs. Etcone
      Ncand[cutIndex++]++;//Abs. Ptcone
      Ncand[cutIndex++]++;//Rel. Etcone
      Ncand[cutIndex++]++;//Rel. Ptcone
    }
    else{
      
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Apply Isolation"  << endmsg;	
	
	//--Declare vectors of isolation variables for different cone sizes
	std::vector<float>  EtCone, PtCone;	
	//--Fill vectors of Absolute isolation variables for different cone sizes
        float val_float=-99;
        egIt->isolationValue(val_float,xAOD::Iso::etcone20);
	EtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::etcone30);
	EtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::etcone40);
	EtCone.push_back(val_float);

	//
        egIt->isolationValue(val_float,xAOD::Iso::ptcone20);
	PtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::ptcone30);
	PtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::ptcone40);
	PtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::ptvarcone20);
	PtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::ptvarcone30);
	PtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::ptvarcone40);
	PtCone.push_back(val_float);
	
	//printout
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Absolute Calo Isolation (vector size = " << EtCone.size() << ") :" << endmsg;	  
	  for(std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++) {
	    msg() << MSG::DEBUG << "***   " << m_mapEtCone[iConeSize]
		  << ", Cut = " << m_EtConeCut[iConeSize] 		  
		  << ", Value = " << EtCone[iConeSize] << endmsg;
	  }
	  msg() << MSG::DEBUG << "Absolute Track Isolation (vector size = " << PtCone.size()<< ") :" << endmsg;
	  for(std::size_t iConeSize = 0; iConeSize < PtCone.size(); iConeSize++) {
	    msg() << MSG::DEBUG << "***   " << m_mapPtCone[iConeSize] 
		  << ", Cut = " << m_PtConeCut[iConeSize] 
		  << ", Value = " << PtCone[iConeSize] << endmsg;
	  }
	}
	

	//--Cut on Absolute Calo Isolation
	bool absEtConeCut_ispassed = true;
	for(std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++) {
	  //NB: -ve values in cut means DO NOT CUT
	  if( ( m_EtConeCut[iConeSize] > 0.) && (EtCone[iConeSize] > m_EtConeCut[iConeSize])) {
	      ATH_MSG_DEBUG("REGTEST Absolute Calo Isolation " << m_mapEtCone[iConeSize] 
                      << " NOT satisfied: "<< EtCone[iConeSize] 
                      << " > cut: " << m_EtConeCut[iConeSize]);
	    absEtConeCut_ispassed = false;
	    break;//skip remaining etcone sizes if one fails
	  }
	}
	
	if(!absEtConeCut_ispassed) continue;//if one isolation cut fails on one egamma object go to next egamma object
	Ncand[cutIndex++]++;//Abs. EtCone
	

	//--Cut on Absolute Track Isolation
	bool absPtConeCut_ispassed = true;
	for(std::size_t iConeSize = 0; iConeSize < PtCone.size(); iConeSize++) {
	  
	  //NB: -ve values in cut means DO NOT CUT
	  if( ( m_PtConeCut[iConeSize] > 0.) && (PtCone[iConeSize] > m_PtConeCut[iConeSize])) {
	      ATH_MSG_DEBUG("REGTEST Absolute Track Isolation " << m_mapPtCone[iConeSize] 
                      << " NOT satisfied: "<< PtCone[iConeSize] 
                      << " > cut: " << m_PtConeCut[iConeSize]);
	    absPtConeCut_ispassed = false;
	    break;//skip remaining ptcone sizes if one fails
	  }
	}
	
	if(!absPtConeCut_ispassed) continue;//if one isolation cut fails on one egamma object go to next egamma object
	Ncand[cutIndex++]++;//Abs. PtCone
	

	//--Relative isolation
	std::vector<float>  RelEtCone, RelPtCone;

	//--Check that CaloCluster exists, if so use cluster ET as Denonimator in Relative Isolation
	float caloIso_ele_pt=-9999.;
	float trkIso_ele_pt=-9999.;
	float ele_clus_pt=-9999.;
	float ele_trk_pt=-9999.;
        
        // Cluster must exist set the et from cluster
        ele_clus_pt=clus->et();

	//--Check that TrackParticle exists, if so use track ET as Denonimator in Relative Isolation
	if(!(egIt->trackParticle())) {
	  
	  ATH_MSG_DEBUG("TrackParticle does NOT Exist, do NOT use Electron Track PT as Denominator in Relative Isolation");
	}else{
	  ATH_MSG_DEBUG("TrackParticle Exists, may use Electron Track PT as denominator in relative Isolation varariables");
	  ele_trk_pt=egIt->trackParticle()->pt();
	}

	//--Define the electron pt variable to use as denominator in Relative Isolation
 
	if(m_useClusETforCaloIso) {
	  caloIso_ele_pt=ele_clus_pt;	
	  ATH_MSG_DEBUG("For Relative Calo Isolation use ele_clus_pt = " << caloIso_ele_pt);
	}
	else {
	  caloIso_ele_pt=ele_trk_pt;
	  ATH_MSG_DEBUG("For Relative Calo Isolation use ele_trk_pt = " << caloIso_ele_pt);
	}
	if(m_useClusETforTrackIso) {
	  trkIso_ele_pt=ele_clus_pt;
	  msg() << MSG::DEBUG << "For Relative Track Isolation use ele_clus_pt = " << trkIso_ele_pt << endmsg;
	}
	else {
	  trkIso_ele_pt=ele_trk_pt;
	  msg() << MSG::DEBUG << "For Relative Track Isolation use ele_trk_pt = " << trkIso_ele_pt << endmsg;
	}

	
	//--Fill vectors of Relative isolation variables for different cone sizes
	for (std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++){
	  
	  if(caloIso_ele_pt > 0.) RelEtCone.push_back(EtCone[iConeSize]/caloIso_ele_pt);
	  else RelEtCone.push_back(0.);
	}
	
	for (std::size_t iConeSize = 0; iConeSize < PtCone.size(); iConeSize++){
	  
	  if(trkIso_ele_pt > 0.) RelPtCone.push_back(PtCone[iConeSize]/trkIso_ele_pt);
	  else RelPtCone.push_back(0.);
	}
	
	
	
	//printout
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Relative Calo Isolation (vector size = " << RelEtCone.size()<< ") :"  << endmsg;
	  for(std::size_t iConeSize = 0; iConeSize < RelEtCone.size(); iConeSize++) {
	    msg() << MSG::DEBUG << "***   " << m_mapRelEtCone[iConeSize] 
		  << ", Cut = "   << m_RelEtConeCut[iConeSize]
		  << ", Value = " << RelEtCone[iConeSize] << endmsg;
	  }
	  msg() << MSG::DEBUG << "Relative Track Isolation Cuts (vector size = " << RelPtCone.size()<< ") :"  << endmsg;
	  for(std::size_t iConeSize = 0; iConeSize < RelPtCone.size(); iConeSize++) {
	    msg() << MSG::DEBUG << "***   " << m_mapRelPtCone[iConeSize] 
		  << ", Cut = "    << m_RelPtConeCut[iConeSize]
		  << ", Value = "  << RelPtCone[iConeSize] << endmsg;
	  }
	}
	
	//--Cut on Relative Calo Isolation
	bool relEtConeCut_ispassed = true;
	for(std::size_t iConeSize = 0; iConeSize < RelEtCone.size(); iConeSize++) {
	  //NB: -ve values in cut means DO NOT CUT
	  if( ( m_RelEtConeCut[iConeSize] > 0.) && (RelEtCone[iConeSize] > m_RelEtConeCut[iConeSize])) {
	      ATH_MSG_DEBUG("REGTEST Relative Calo Isolation " << m_mapRelEtCone[iConeSize] 
                      << " NOT satisfied: "<< RelEtCone[iConeSize] << " > cut: " 
                      << m_RelEtConeCut[iConeSize]);
	    relEtConeCut_ispassed = false;
	    break;//skip remaining etcone sizes if one fails
	  }
	}
	
	if(!relEtConeCut_ispassed) continue;//if one isolation cut fails on one egamma object go to next egamma object
	Ncand[cutIndex++]++;//Rel. Etcone        
	
	
	//--Cut on Relative Track Isolation
	bool relPtConeCut_ispassed = true;
	for(std::size_t iConeSize = 0; iConeSize < RelPtCone.size(); iConeSize++) {
	  //NB: -ve values in cut means DO NOT CUT
	  if( ( m_RelPtConeCut[iConeSize] > 0.) && (RelPtCone[iConeSize] > m_RelPtConeCut[iConeSize])) {
	      ATH_MSG_DEBUG("REGTEST Relative Track Isolation " << m_mapRelPtCone[iConeSize] 
                      << " NOT satisfied: "<< RelPtCone[iConeSize] << " > cut: " 
                      << m_RelPtConeCut[iConeSize]);
	    relPtConeCut_ispassed = false;
	    break;//skip remaining ptcone sizes if one fails
	  }
	}
	
	if(!relPtConeCut_ispassed) continue;//if one isolation cut fails on one egamma object go to next egamma object
	Ncand[cutIndex++]++;//Re. Ptcone
	
    }//end of if(m_applyIsolation)
    
    //-------------------------------------------------
    // At least one Egamma matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    Ncand[cutIndex++]++;
    accepted=true;
    xBits->markPassing(egIt,m_EgammaContainer,true);
  }//end of loop over egamma container
  
  //Count No of Events passing individual cuts
  // Input, Pid, Et, AbsEtCone, AbsPtCone, RelEtCone, RelPtCone, Passed
  for(int i=0; i<8; i++) m_NofPassedCuts+=(Ncand[i]>0);

  
  // print out Result     
  ATH_MSG_DEBUG("REGTEST Result = " <<(accepted ? "passed" : "failed"));
  ATH_MSG_DEBUG("REGTEST AcceptAll= " <<(m_acceptAll ? "true (no cuts)" : "false (selection applied)"));

  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");

  // Time total TrigEFElectronHypo execution time.
  // -------------------------------------

  if (timerSvc()) m_totalTimer->stop();

  return HLT::OK;
}


// ==============================================================
double TrigEFElectronHypo::findImpact(const xAOD::TrackParticle* track) const
{
  //
  // recalculate transverse impact parameter
  // in case no Vertex is provided by the user,
  // beam position will be used if available
  //

  double trackD0 = -9999.;
  // protection against bad pointers
  if (track==0) return -9999.;
  
  // use beam spot
  const Trk::Perigee* perigee =
    m_trackToVertexTool->perigeeAtBeamspot(*track);
  
  if (perigee==0) {
    if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING <<"No perigee using beam spot; no d0 calculation"<<endmsg;    
    perigee = m_trackToVertexTool->perigeeAtVertex(*track, m_primaryVertex);    
  }

  // destroy object
  if (perigee!=0)
    trackD0 = perigee->parameters()[Trk::d0];
  delete perigee;

  return trackD0; 
} 

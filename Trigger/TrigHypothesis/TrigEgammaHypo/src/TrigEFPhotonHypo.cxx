/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFPhotonHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFPhotonHypo
//
// AUTHOR:   Valeria Perez Reale
//           Phillip Urquijo
//           Alessandro Tricoli
//           Ryan Mackenzie White (xAOD)
//
// PURPOSE:  Performs the calorimeter shower shape selection for egamma at EF
//
// MODIFIED: V. perez reale: Migration to new steering    1/12/06           
//           G. Khoriauli: Add monitoring histograms for new monitoring framework 10/05/07
//           V. Perez-Reale: Fix bug on AcceptAll (present since 1/12/06) 10/05/07
//           A.Tricoli: Use of offline tools to build isEM (Jan 2010)
//           R.Goncalo: add TrigPassBits
//           R.M. White migration to xAOD (April 2014)
//
// ********************************************************************
// xAOD Comments
// Need to differentiate between Electron and Photon for the selector
// Disable HighLumi and only use Electron container rather than Egamma/Photon
// Removing old trigger photon selection
// Notice that Et threshold cut done differently
//
#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigEgammaHypo/TrigEFPhotonHypo.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ITHistSvc.h"
#include <TH1F.h>
//#include "PATCore/IAthSelectorTool.h"
//#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgammaCnv/xAODPhotonMonFuncs.h"
#include "PATCore/TAccept.h"            // for TAccept
#include "PATCore/TResult.h"            // for TResult
// to add TrigPassBits 
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassFlags.h"

namespace {
    template <class DEST,class SRC>
        inline DEST** my_pp_cast(SRC** ptr) {
            return (DEST**)(ptr);
        }
}

TrigEFPhotonHypo::TrigEFPhotonHypo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator)
{

  declareProperty("AcceptAll",      m_acceptAll   = true);

  //Trig isEMTrigCut cuts adopted from offline
  //The cut methods and variables are slightly different online for electrons and photons, so 
  //both methods are used here (for low and high lumi) photons.

 // declareProperty("LumiLevel",m_LumiLevel="LowLumi");//LowLumi is the default, to use electron cuts - BC 2012, removed to match changes in offline, replaced with usePhotonCuts. "HighLumi" corresponds to usePhotonCuts = true, and "LowLumi" corresponds to usePhotonCuts = false.
  declareProperty("usePhotonCuts",m_usePhotonCuts = false);
  
  //Name of egammaElectronCutIDTool and egammaPhotonCutIDToolName
  declareProperty("egammaElectronCutIDToolName",m_egammaElectronCutIDToolName="");
  declareProperty("egammaPhotonCutIDToolName",m_egammaPhotonCutIDToolName="");

  //isEM offline
  declareProperty("ApplyIsEM",m_applyIsEM = false);
  declareProperty("IsEMrequiredBits",m_IsEMrequiredBits = 0xF2);

  declareProperty("emEt",m_emEt = 0.*CLHEP::GeV);
  
  typedef const DataVector<xAOD::Photon> xAODPhotonDV_type; 
  // Cluster and ShowerShape Monitoring
  declareMonitoredCollection("Ph_E237",   *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_e237);
  declareMonitoredCollection("Ph_E277",   *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_e277);
  declareMonitoredCollection("Ph_Reta",   *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_Reta);
  declareMonitoredCollection("Ph_Rphi",   *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_Rphi);
  declareMonitoredCollection("Ph_Rhad",   *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_Rhad);
  declareMonitoredCollection("Ph_Rhad1",   *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_Rhad1);
  declareMonitoredCollection("Ph_EtHad1", *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_ethad1);
  declareMonitoredCollection("Ph_WEta1",  *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_weta1);
  declareMonitoredCollection("Ph_WEta2",  *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_weta2);
  declareMonitoredCollection("Ph_F1",     *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_f1);
  declareMonitoredCollection("Ph_F3",     *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_f3);
  declareMonitoredCollection("Ph_F3core",     *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_f3core);
  declareMonitoredCollection("Ph_Emax2",*my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_e2tsts1);
  declareMonitoredCollection("Ph_Emins1", *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_emins1);
  declareMonitoredCollection("Ph_Emax", *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_emaxs1);
  declareMonitoredCollection("Ph_DEmaxs1", *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getDEmaxs1);
  declareMonitoredCollection("Ph_wtots1", *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_wtots1);
  declareMonitoredCollection("Ph_Fracs1", *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_fracs1);
  declareMonitoredCollection("Ph_Eratio", *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getShowerShape_Eratio);
  declareMonitoredCollection("Ph_EtCone20",     *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getIsolation_etcone20);
  declareMonitoredCollection("Ph_EnergyBE0",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getEnergyBE0);
  declareMonitoredCollection("Ph_EnergyBE1",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getEnergyBE1);
  declareMonitoredCollection("Ph_EnergyBE2",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getEnergyBE2);
  declareMonitoredCollection("Ph_EnergyBE3",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getEnergyBE3);
  declareMonitoredCollection("Ph_Eaccordion",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getEaccordion);
  declareMonitoredCollection("Ph_E0Eaccordion",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getE0Eaccordion);

  declareMonitoredCollection("Ph_ClusterEt37",   *my_pp_cast<xAODPhotonDV_type>(&m_EgammaContainer), &getEtCluster37);
  declareMonitoredCollection("Ph_Eta",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getCluster_eta);
  declareMonitoredCollection("Ph_Eta2",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getEta2);
  declareMonitoredCollection("Ph_Phi",           *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getCluster_phi);
  declareMonitoredCollection("Ph_ClusterEt",     *my_pp_cast <xAODPhotonDV_type>(&m_EgammaContainer), &getCluster_et);
  
  declareMonitoredVariable("CutCounter",m_NofPassedCuts);
  
  //isEM monitoring 
  declareMonitoredStdContainer("IsEMBeforeCut",m_NcandIsEM);//per-object counter of failing isEM bits
  declareMonitoredStdContainer("IsEMAfterCut",m_NcandIsEMAfterCut);//per-object counter of failing isEM bits
  declareMonitoredStdContainer("IsEMRequiredBitsBeforeCut",m_IsEMRequiredBits); 
  declareMonitoredStdContainer("IsEMRequiredBitsAfterCut",m_IsEMRequiredBitsAfterCut);

  prepareMonitoringVars();
  
  //Initialize pointers
  m_totalTimer = nullptr;
  m_timerPIDTool_Ele = nullptr;
  m_timerPIDTool_Pho = nullptr; 
}

void TrigEFPhotonHypo::prepareMonitoringVars() {
  //isEM monitoring

  m_NcandIsEM.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton

  //
  m_NcandIsEMAfterCut.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton

  //Monitor isEM 32-Bit Pattern Before Cuts
  m_IsEMRequiredBits.assign(32,0);

  //Monitor isEM 32-Bit Pattern After Cuts
  m_IsEMRequiredBitsAfterCut.assign(32,0);
}

TrigEFPhotonHypo::~TrigEFPhotonHypo()
{  }


HLT::ErrorCode TrigEFPhotonHypo::hltInitialize()
{
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in initialize()" << endreq;
  
  if (timerSvc())
    m_totalTimer = addTimer("TrigEFPhotonHypoTot");

  //-------------------------------------------------------------------------------
  // Use egammaElectronCutIDTool to run the Offline isEM. 
  // The egammaElectronCutIDTool runs the Electron Selecton only
  // independently on whether the egamma object has associated TrackParticle or not
  //------------------------------------------------------------------------------
  
  if (m_egammaElectronCutIDToolName=="") {
    ATH_MSG_DEBUG("egammaElectronCutID PID is disabled, no tool specified "); 
    m_egammaElectronCutIDTool=ToolHandle<IAsgElectronIsEMSelector>();
  } else {
    m_egammaElectronCutIDTool=ToolHandle<IAsgElectronIsEMSelector>(m_egammaElectronCutIDToolName);    
    if(m_egammaElectronCutIDTool.retrieve().isFailure()) {
      msg() << MSG::ERROR << "Unable to retrieve " << m_egammaElectronCutIDTool
	    << " tool " << endreq;
      return HLT::BAD_JOB_SETUP; 
    } 
    else {
      msg()<<MSG::DEBUG<<"Tool " << m_egammaElectronCutIDTool << " retrieved"<<endreq; 
      //timing
      if (timerSvc()) m_timerPIDTool_Ele = addTimer("m_egammaElectronCutIDToolName");
    }
  }
  
  //print summary info
  msg() << MSG::INFO << "REGTEST: Particle Identification tool: " << m_egammaElectronCutIDToolName << endreq;


  //-------------------------------------------------------------------------------
  //Use egammaPhotonCutIDTool to run offline isEM 
  // egammaPhotonCutIDTool runs the Photon Selecton only
  // independently on whether the egamma object has associated TrackParticle or not
  //------------------------------------------------------------------------------
  if (m_egammaPhotonCutIDToolName=="") {
    ATH_MSG_DEBUG("egammaPhotonCutID PID is disabled, no tool specified " );
    m_egammaPhotonCutIDTool=ToolHandle<IAsgPhotonIsEMSelector>();
  } else {
    m_egammaPhotonCutIDTool=ToolHandle<IAsgPhotonIsEMSelector>(m_egammaPhotonCutIDToolName);    
    if(m_egammaPhotonCutIDTool.retrieve().isFailure()) {
      msg() << MSG::ERROR << "Unable to retrieve " << m_egammaPhotonCutIDTool
	    << " tool " << endreq;
      return HLT::BAD_JOB_SETUP; 
    } 
    else {
      msg()<<MSG::DEBUG<<"Tool " << m_egammaPhotonCutIDTool << " retrieved"<<endreq; 
      //timing
      if (timerSvc()) m_timerPIDTool_Pho = addTimer("m_egammaPhotonCutIDToolName");
    }
  }
  
  //print summary info
  msg() << MSG::INFO << "REGTEST: Particle Identification tool: " << m_egammaPhotonCutIDToolName << endreq;
  //------------------------------------------------------------------------------
  
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG
	  << "Initialization of TrigEFPhotonHypo completed successfully"
	  << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigEFPhotonHypo::hltBeginRun() {
  prepareMonitoringVars();
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFPhotonHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFPhotonHypo::hltExecute(const HLT::TriggerElement* outputTE,
					    bool& pass) {
  // ----------------------------------------------------------------------

  m_EgammaContainer = 0;
  m_NofPassedCuts=-1;
  


  
  // Time total TrigEFPhotonHypo execution time.
  // -------------------------------------
  if (timerSvc()) m_totalTimer->start(); 

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << name() << ": in execute()" << endreq;
  
  // default value, it will be set to true if selection satisfied
  pass=false;
  
  // get egamma objects from the trigger element:
  //--------------------------------------------------
  // Use photon containers 
  std::vector<const xAOD::PhotonContainer*> vectorEgammaContainers;
  
  HLT::ErrorCode stat = getFeatures(outputTE, vectorEgammaContainers, "");

  if (stat != HLT::OK ) {
    msg() << MSG::WARNING 
	  << " Failed to get xAOD::PhotonContainer's from the trigger element" 
	  << endreq;
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  } 

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "REGTEST: Got " << vectorEgammaContainers.size() 
	  << " xAOD::PhotonContainers's associated to the TE " << endreq;

  if (vectorEgammaContainers.size() < 1) {
    msg() << MSG::DEBUG
	  << " empty xAOD::PhotonContainer from the trigger element" 
	  << endreq;
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  } 

  m_EgammaContainer = vectorEgammaContainers.back();

  if(m_EgammaContainer == 0){
    if ( msgLvl() <= MSG::ERROR )
      msg() << MSG::ERROR
	    << " REGTEST: Retrieval of xOAD::PhotonContainer from vector failed"
	    << endreq;
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  }
  // AcceptAll property = true means selection cuts should not be applied
  if (m_acceptAll) {
      pass = true;
      ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
  } 
  else 
      ATH_MSG_DEBUG("AcceptAll property not set: applying selection");

  // generate TrigPassBits mask to flag which egamma objects pass hypo cuts
  TrigPassBits* passBits = HLT::makeTrigPassBits(m_EgammaContainer);
  
  
  // adding TrigPassFlags for isEM bits
  const unsigned int flagSize = 32;

  // temporarily disable the TrigPassFlags until xAOD format is sorted out
  TrigPassFlags* isEMFlags = 0; //HLT::makeTrigPassFlags(m_EgammaContainer, flagSize);
  
  //counters for each cut
  int Ncand[10];
  for(int i=0; i<10; i++) Ncand[i]=0;
  Ncand[0]++;

  //isEM monitoring variables
  for(unsigned int i=0;i<32;i++) m_NcandIsEM[i]=0; 
  for(unsigned int i=0;i<32;i++) m_NcandIsEMAfterCut[i]=0;
  m_NofPassedCuts = 0;
  m_NofPassedCutsIsEM = 0;
  m_NofPassedCutsIsEMTrig = 0;

  //m_isEMTrig.clear(); //AT Aug2011: deactivate histogram egIsEM - outdated

  //Monitor the required isEM bits Before/After Cuts
  for(unsigned int i=0;i<32;i++) m_IsEMRequiredBits[i]=0; 
  for(unsigned int i=0;i<32;i++) m_IsEMRequiredBitsAfterCut[i]=0;

  //Monitor the required isEM 32-Bit pattern Before Cuts
  for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton	
    m_IsEMRequiredBits[i]+= ((m_IsEMrequiredBits & (0x1<<i)) != 0); 
  } 
  Ncand[1]++;

  //Something to do with old trigger cuts
  //removed
  //bool passed1=false;
  for (const auto& egIt : *m_EgammaContainer){

      //passed1 = false;
      //-------------------------------------------------------------
      //Apply cut on IsEM bit pattern re-running the  Offline Builder  
      if( m_applyIsEM){
          unsigned int isEMTrig = 0;

          //LOW LUMI
          //At low luminosity use the same cut algorithm as offline electrons
          //BC 2012 changed from lumilevel variable to usePhotonCuts, to match offline changes.
          //Only applies CaloCuts for Egamma Objects with AsgElectronIsEMSelector
          if (!m_usePhotonCuts){
              if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Applying LOW Lumi Cuts on Photons" << endreq;

              //re-run the offline isEM for Electrons
              if (m_egammaElectronCutIDTool == 0) {
                  msg() << MSG::ERROR << m_egammaElectronCutIDTool << " null, hypo continues but no isEM cut applied" << endreq;
              }else{
                  if (timerSvc()) m_timerPIDTool_Ele->start(); //timer
                  //Following method only performs CaloCuts and take Egamma (i.e. photons / electrons) object as input
                  //Ensure that we set the trigger threshold in python to set 20 - 30 GeV bin
                  
                  if ( m_egammaElectronCutIDTool->execute(egIt).isFailure() ) { 
                      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                          << "problem with egammaElectronCutIDTool, egamma object not stored"
                              << endreq;
                  } 
                  //AT jan 2010: get isEM value from m_egammaElectronCutIDTool->IsemValue(), not from (*egIt)->isem()
                  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<" IsemValue() = "<< m_egammaElectronCutIDTool->IsemValue()<< endreq;
                  isEMTrig = m_egammaElectronCutIDTool->IsemValue();
                  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                      <<" isEMTrig = "
                          << std::hex << isEMTrig
                          << endreq;
                  unsigned int isEMbit=0;
                  // isEM for calo-only not working in PID builder since there is no TAccept for Calo-Only
                  //ATH_MSG_DEBUG("isEMLoose " << egIt->selectionisEM(isEMbit,"isEMLoose"));
                  //ATH_MSG_DEBUG("isEMLoose " << std::hex << isEMbit);
                  //ATH_MSG_DEBUG("isEMMedium " << egIt->selectionisEM(isEMbit,"isEMMedium"));
                  //ATH_MSG_DEBUG("isEMMedium " << std::hex << isEMbit);
                  ATH_MSG_DEBUG("isEMTight " << egIt->selectionisEM(isEMbit,"isEMTight"));
                  ATH_MSG_DEBUG("isEMTight " << std::hex << isEMbit);
                  if (timerSvc()) m_timerPIDTool_Ele->stop(); //timer
              }
          }//end of "LowLumi"

          //HIGH LUMI
          //At high luminosity use the same cut algorithm as offline photon selection
          else{
              if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Applying HIGH Lumi Cuts on Photons" << endreq;
              // re-run the Offline isEM for Photons
              if (m_egammaPhotonCutIDTool == 0) {
                  msg() << MSG::ERROR << m_egammaPhotonCutIDTool << " null, hypo continues but no isEM cut applied" << endreq;
              }else{
                  if (timerSvc()) m_timerPIDTool_Pho->start(); //timer
                  if ( m_egammaPhotonCutIDTool->accept(egIt) ) {
                      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                          << "passes PID egammaPhotonCutIDTool with TAccept"
                              << endreq;
                  } 
                  // Get isEM value from m_egammaPhotonCutIDTool->IsemValue(), not from (*egIt)->isem()
                  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<" IsemValue() = "<< m_egammaPhotonCutIDTool->IsemValue()<< endreq;
                  isEMTrig = m_egammaPhotonCutIDTool->IsemValue();
                  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                      <<" isEMTrig = "
                          << std::hex << isEMTrig
                          << endreq;
                  if (timerSvc()) m_timerPIDTool_Pho->stop(); //timer
              }
          }//end of "HighLumi"
          //-------------------------------------------------------------

          //Monitor isEM
          for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
              m_NcandIsEM[i]+= ((isEMTrig & (0x1<<i)) != 0); 
          }

          // Set the isEM flag for this egamma object
          if(isEMFlags)
             HLT::setFlag(isEMFlags, egIt, m_EgammaContainer, HLT::AsFlag(isEMTrig, flagSize) );


          if( (isEMTrig & m_IsEMrequiredBits)!=0 ) {
              if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST IsEM = " << std::hex << isEMTrig 
                  << " cut not satisfied for pattern:" << std::hex << m_IsEMrequiredBits << endreq;
              continue; 
          }

          //Monitor isEM After Cut
          for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
              m_NcandIsEMAfterCut[i]+= ((isEMTrig & (0x1<<i)) != 0); 
          }

          //Monitor the required isEM 32-Bit pattern After Cut
          for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton	
              m_IsEMRequiredBitsAfterCut[i]+= ((m_IsEMrequiredBits & (0x1<<i)) != 0); 
          }

          // Do not count on m_NofPassedCuts since this cut might be disabled
          //       m_NofPassedCuts++;

      }//end of if( m_applyIsEM)
    Ncand[2]++;


    Ncand[3]++;

    //
    //other trigger specific cuts
    //

    const xAOD::CaloCluster* clus = egIt->caloCluster();
    if(!clus) {
      if(msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "REGTEST no cluster pointer in egamma object " << endreq;
      continue;
    }
    Ncand[4]++;
    if( clus->et() < m_emEt) {
      if(msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST Et cut no satisfied: "<< clus->et() << "< cut: " << m_emEt << endreq;
      continue;
    }
    Ncand[5]++;
    
    
    Ncand[6]++;
    pass = true;
    HLT::markPassing(passBits, egIt, m_EgammaContainer); // set bit for this egamma in TrigPassBits mask
  } // end of loop in egamma objects.
  
  for(int i=0; i<10; i++) m_NofPassedCuts+=(Ncand[i]>0);


  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST: Result = " <<(pass ? "passed" : "failed")<< endreq;
  }

  // store TrigPassBits result
  if ( attachBits(outputTE, passBits) != HLT::OK ) {
    msg() << MSG::ERROR << "Could not store TrigPassBits! " << endreq;
  }

  // store TrigPassFlags result
  if(isEMFlags) {
     if ( attachFlags(outputTE, isEMFlags, "isEM") != HLT::OK ) {
        msg() << MSG::ERROR << "Could not store isEM flags! " << endreq;
     }
  }

  // Time total TrigEFPhotonHypo execution time.
  // -------------------------------------
  if (timerSvc()) m_totalTimer->stop();

  return HLT::OK;
}


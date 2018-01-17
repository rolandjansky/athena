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
#include "xAODTrigger/TrigPassBits.h"
using std::string;

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

  declareProperty("egammaPhotonCutIDToolName",m_egammaPhotonCutIDToolName="");

  //isEM offline
  declareProperty("ApplyIsEM",m_applyIsEM = false);
  declareProperty("IsEMrequiredBits",m_IsEMrequiredBits = 0xF2);

  declareProperty("emEt",m_emEt = 0.*CLHEP::GeV);
  
  //Isolation
  declareProperty("ApplyIsolation", m_applyIsolation=false);
  declareProperty("EtConeSizes",    m_EtConeSizes = 3);
  declareProperty("RelEtConeCut",   m_RelEtConeCut);
  declareProperty("IsoOffset",   m_IsoOffset);
  declareProperty("EtConeCut",      m_EtConeCut);  
  
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
  m_EgammaContainer = nullptr;
  //isEM monitoring
  m_NcandIsEM.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
  m_NcandIsEMAfterCut.assign(32,0);//32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
  m_IsEMRequiredBits.assign(32,0);
  m_IsEMRequiredBitsAfterCut.assign(32,0);
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
  m_NofPassedCuts=-1;
  m_NofPassedCutsIsEM=-1; 
  m_NofPassedCutsIsEMTrig=-1; 
}

TrigEFPhotonHypo::~TrigEFPhotonHypo()
{  }


HLT::ErrorCode TrigEFPhotonHypo::hltInitialize()
{
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in initialize()" << endmsg;
  
  if (timerSvc())
    m_totalTimer = addTimer("TrigEFPhotonHypoTot");

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
	    << " tool " << endmsg;
      return HLT::BAD_JOB_SETUP; 
    } 
    else {
      msg()<<MSG::DEBUG<<"Tool " << m_egammaPhotonCutIDTool << " retrieved"<<endmsg; 
      //timing
      if (timerSvc()) m_timerPIDTool_Pho = addTimer("m_egammaPhotonCutIDToolName");
    }
  }
  
  //print summary info
  msg() << MSG::INFO << "REGTEST: Particle Identification tool: " << m_egammaPhotonCutIDToolName << endmsg;
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

      //Define mapping between vector of Isolation Cone Sizes and variable names 
      m_mapEtCone.insert(std::pair<int, string>(0, "topoetcone20")); 
      m_mapEtCone.insert(std::pair<int, string>(1, "topoetcone30")); 
      m_mapEtCone.insert(std::pair<int, string>(2, "topoetcone40"));
      //
      m_mapRelEtCone.insert(std::pair<int, string>(0, "topoetcone20/clus_et")); 
      m_mapRelEtCone.insert(std::pair<int, string>(1, "topoetcone30/clus_et")); 
      m_mapRelEtCone.insert(std::pair<int, string>(2, "topoetcone40/clus_et")); 
      //
      m_mapIsoOffset.insert(std::pair<int, string>(0, "topoetcone20_Offset")); 
      m_mapIsoOffset.insert(std::pair<int, string>(1, "topoetcone30_Offset")); 
      m_mapIsoOffset.insert(std::pair<int, string>(2, "topoetcone40_Offset")); 

  }//end of if(m_applyIsolation){
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG
	  << "Initialization of TrigEFPhotonHypo completed successfully"
	  << endmsg;

  return HLT::OK;
}


HLT::ErrorCode TrigEFPhotonHypo::hltBeginRun() {
  prepareMonitoringVars();
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFPhotonHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFPhotonHypo::hltExecute(const HLT::TriggerElement* outputTE,
					    bool& pass) {
  // ----------------------------------------------------------------------

  m_EgammaContainer = 0;
  m_NofPassedCuts=-1;
  m_NofPassedCutsIsEM=-1; 
  m_NofPassedCutsIsEMTrig=-1; 
  


  
  // Time total TrigEFPhotonHypo execution time.
  // -------------------------------------
  if (timerSvc()) m_totalTimer->start(); 

  ATH_MSG_DEBUG(name() << ": in execute()");
  if( m_applyIsEM)
      ATH_MSG_DEBUG("Apply Photon isEM with Tool " << m_egammaPhotonCutIDTool);
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
	  << endmsg;
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  } 

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "REGTEST: Got " << vectorEgammaContainers.size() 
	  << " xAOD::PhotonContainers's associated to the TE " << endmsg;

  if (vectorEgammaContainers.size() < 1) {
    msg() << MSG::DEBUG
	  << " empty xAOD::PhotonContainer from the trigger element" 
	  << endmsg;
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  } 

  m_EgammaContainer = vectorEgammaContainers.back();

  if(m_EgammaContainer == 0){
    if ( msgLvl() <= MSG::ERROR )
      msg() << MSG::ERROR
	    << " REGTEST: Retrieval of xOAD::PhotonContainer from vector failed"
	    << endmsg;
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
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::PhotonContainer>(m_EgammaContainer); 
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

      if(m_acceptAll){
          xBits->markPassing(egIt,m_EgammaContainer,true);
          continue;
      }
      //passed1 = false;
      //-------------------------------------------------------------
      //Apply cut on IsEM bit pattern re-running the  Offline Builder  
      if( m_applyIsEM){
          unsigned int isEMTrig = 0;

          ATH_MSG_DEBUG(" Applying HIGH Lumi Cuts on Photons");
          // re-run the Offline isEM for Photons
          if (m_egammaPhotonCutIDTool == 0) {
              msg() << MSG::ERROR << m_egammaPhotonCutIDTool << " null, hypo continues but no isEM cut applied" << endmsg;
          }else{
              if (timerSvc()) m_timerPIDTool_Pho->start(); //timer
              if ( m_egammaPhotonCutIDTool->accept(egIt) ) {
                  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                      << "passes PID egammaPhotonCutIDTool with TAccept"
                          << endmsg;
              } 
              // Get isEM value from m_egammaPhotonCutIDTool->IsemValue(), not from (*egIt)->isem()
              if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<" IsemValue() = "<< m_egammaPhotonCutIDTool->IsemValue()<< endmsg;
              isEMTrig = m_egammaPhotonCutIDTool->IsemValue();
              if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                  <<" isEMTrig = "
                      << std::hex << isEMTrig
                      << endmsg;
              if (timerSvc()) m_timerPIDTool_Pho->stop(); //timer
          }
          //-------------------------------------------------------------

          //Monitor isEM
          for(unsigned int i=0;i<32;++i) { //32-bit as it is in the Offline isEM for BitDefElecton and BitDefPhoton
              m_NcandIsEM[i]+= ((isEMTrig & (0x1<<i)) != 0); 
          }

          if( (isEMTrig & m_IsEMrequiredBits)!=0 ) {
              if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST IsEM = " << std::hex << isEMTrig 
                  << " cut not satisfied for pattern:" << std::hex << m_IsEMrequiredBits << endmsg;
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
        msg() << MSG::DEBUG << "REGTEST no cluster pointer in egamma object " << endmsg;
      continue;
    }
    Ncand[4]++;
    if( clus->et() < m_emEt) {
      if(msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST Et cut no satisfied: "<< clus->et() << "< cut: " << m_emEt << endmsg;
      continue;
    }
    Ncand[5]++;
    
    //---------------------------------------------
    //Isolation Cuts
    //---------------------------------------------
    if(!m_applyIsolation) {
      //For CutCount monitoring histogram,to fill isolation bins when no isolation cut is applied
      Ncand[6]++;//Abs. Etcone
      Ncand[6]++;//Abs. Ptcone
      Ncand[6]++;//Rel. Etcone
      Ncand[6]++;//Rel. Ptcone
    }
    else{
      ATH_MSG_DEBUG("Apply Isolation");
	
	//--Declare vectors of isolation variables for different cone sizes
	std::vector<float>  EtCone;
	//--Fill vectors of Absolute isolation variables for different cone sizes
        float val_float=-99;
        egIt->isolationValue(val_float,xAOD::Iso::topoetcone20);
	EtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::topoetcone30);
	EtCone.push_back(val_float);
        egIt->isolationValue(val_float,xAOD::Iso::topoetcone40);
	EtCone.push_back(val_float);
	
        //printout
        ATH_MSG_DEBUG("Absolute Calo Isolation (vector size = " << EtCone.size() << ") :"); 
        for(std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++) {
            ATH_MSG_DEBUG("***   " << m_mapEtCone[iConeSize]
                    << ", Cut = " << m_EtConeCut[iConeSize] 		  
                    << ", Value = " << EtCone[iConeSize]);
        }
	
	//--Cut on Absolute Calo Isolation
	bool absEtConeCut_ispassed = true;
	for(std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++) {
	  //NB: -ve values in cut means DO NOT CUT
	  if( ( m_EtConeCut[iConeSize] > 0.) && (EtCone[iConeSize] > m_EtConeCut[iConeSize])) {
	    ATH_MSG_DEBUG("REGTEST Absolute Calo Isolation " << m_mapEtCone[iConeSize] << " NOT satisfied: "<< EtCone[iConeSize] << " > cut: " << m_EtConeCut[iConeSize]);
	    absEtConeCut_ispassed = false;
	    break;//skip remaining etcone sizes if one fails
	  }
	}
	
	if(!absEtConeCut_ispassed) continue;//if one isolation cut fails on one egamma object go to next egamma object
	Ncand[6]++;//Abs. EtCone

	//--Relative isolation
	std::vector<float>  RelEtCone, RelPtCone;

	//--Check that CaloCluster exists, if so use cluster ET as Denonimator in Relative Isolation
	float clus_et=-9999.;
	if(!clus) {
	  ATH_MSG_INFO("CaloCluster dees NOT Exist, do NOT use Electron ET as Denominator in Relative Isolation");
	} else{
	  ATH_MSG_DEBUG("CaloCluster Exists, may use cluster ET as denominator in relative Isolation varariables");
	  clus_et=clus->et();
	}
	
	//--Fill vectors of Relative isolation variables for different cone sizes with offset
        //--Defined as (Etcone - Offset)/Et
	for (std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++){
	  if(clus_et > 0.) 
              RelEtCone.push_back(getIsolation(EtCone[iConeSize],m_IsoOffset[iConeSize],clus_et));
	  else 
              RelEtCone.push_back(0.);
	}
	
        //printout
        ATH_MSG_DEBUG("Relative Calo Isolation (vector size = " << RelEtCone.size()<< ") :");
        for(std::size_t iConeSize = 0; iConeSize < RelEtCone.size(); iConeSize++) {
            ATH_MSG_DEBUG("***   " << m_mapRelEtCone[iConeSize] 
                << ", Cut = "   << m_RelEtConeCut[iConeSize]
                << ", Offset = "<< m_IsoOffset[iConeSize]
                << ", Etcone = "<< EtCone[iConeSize]
                << ", Et =  "<< clus_et
                << ", Value  = " << RelEtCone[iConeSize]);
        }
	
	//--Cut on Relative Calo Isolation
        bool relEtConeCut_ispassed = true;
        for(std::size_t iConeSize = 0; iConeSize < RelEtCone.size(); iConeSize++) {
            //NB: -ve values in cut means DO NOT CUT
            if( ( m_RelEtConeCut[iConeSize] > 0.) && (RelEtCone[iConeSize] > m_RelEtConeCut[iConeSize])) {
                ATH_MSG_DEBUG("REGTEST Relative Calo Isolation " << m_mapRelEtCone[iConeSize] << " NOT satisfied: "
                        << RelEtCone[iConeSize] << " > cut: " << m_RelEtConeCut[iConeSize]);
                relEtConeCut_ispassed = false;
                break;//skip remaining etcone sizes if one fails
            }
        }
	
	if(!relEtConeCut_ispassed) continue;//if one isolation cut fails on one egamma object go to next egamma object
	Ncand[7]++;//Rel. Etcone       
    }
	
	
    // At least 1 Photon passes selection
    Ncand[8]++;
    pass = true;
    xBits->markPassing(egIt,m_EgammaContainer,true);
  } // end of loop in egamma objects.
  
  for(int i=0; i<10; i++) m_NofPassedCuts+=(Ncand[i]>0);


  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST: Result = " <<(pass ? "passed" : "failed")<< endmsg;
  }

  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");

  // Time total TrigEFPhotonHypo execution time.
  // -------------------------------------
  if (timerSvc()) m_totalTimer->stop();

  return HLT::OK;
}


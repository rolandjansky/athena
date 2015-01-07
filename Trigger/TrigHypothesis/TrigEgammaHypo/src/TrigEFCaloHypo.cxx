/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFCaloHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR:   P. Conde Muino
//           Performs the Hypothesis cuts on the Event Filter
//           Calorimeter variables. Create a photon-like object to pass to SelectorTools.
//
// MAINTAINED BY: R. M. White
// MODIFIED: Oct 2014, Update to xAOD addition of egamma Tools
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigEgammaHypo/TrigEFCaloHypo.h"
//

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/PhotonContainer.h"
//#include "xAODEgammaCnv/xAODPhotonMonFuncs.h" // Cannot use macros in more than one place
#include "CaloEvent/CaloCellContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCaloHypo::TrigEFCaloHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator),
    m_lumiTool("LuminosityTool")
{

  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("emEt",   m_emEt = -3.*CLHEP::GeV); 
  //isEM offline
  declareProperty("ApplyIsEM",m_applyIsEM = false);
  declareProperty("ApplyLH",m_applyLH = false);
  declareProperty("IsEMrequiredBits",m_IsEMrequiredBits = 0xF2);

  declareProperty("SelectorTool", m_SelectorTool, "Handle for selector tool");
  declareProperty("SelectorToolName", m_SelectorToolName, "Name for selector tool");
  declareProperty("LHSelectorTool", m_LHSelectorTool, "Handle for LH selector tool");
  declareProperty("LHSelectorToolName", m_LHSelectorToolName, "Name for LH selector tool");

  /** Luminosity tool */
  declareProperty("LuminosityTool", m_lumiTool, "Luminosity Tool");
  
  //Monitor collections
  declareMonitoredStdContainer("EnergyBE0",m_EBE0);
  declareMonitoredStdContainer("EnergyBE1",m_EBE1);
  declareMonitoredStdContainer("EnergyBE2",m_EBE2);
  declareMonitoredStdContainer("EnergyBE3",m_EBE3);
  declareMonitoredStdContainer("Eta",m_Eta);
  declareMonitoredStdContainer("EtaCalo",m_EtaCalo);
  declareMonitoredStdContainer("PhiCalo",m_PhiCalo);
  declareMonitoredStdContainer("E",m_E);
  declareMonitoredStdContainer("ECalib",m_ECalib);
  declareMonitoredStdContainer("ERes",m_ERes);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCaloHypo::~TrigEFCaloHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::INFO << "in initialize()" << endreq;

  // Initialize timing service
  //------------------------------
  if (timerSvc())
   m_totalTimer  = addTimer("TrigEFCaloHypoTot");

  m_SelectorTool=ToolHandle<IAsgElectronIsEMSelector>(m_SelectorToolName);
  if(m_SelectorTool.retrieve().isFailure()) {
      msg() << MSG::ERROR << "Unable to retrieve " << m_SelectorTool 
          << " tool " << endreq;
      return HLT::BAD_JOB_SETUP; 
  } 
  else ATH_MSG_DEBUG("Tool " << m_SelectorTool << " retrieved");
  
  m_LHSelectorTool=ToolHandle<IAsgElectronLikelihoodTool>(m_LHSelectorToolName);
  if(m_LHSelectorTool.retrieve().isFailure()) {
      msg() << MSG::ERROR << "Unable to retrieve " << m_LHSelectorTool 
          << " tool " << endreq;
      return HLT::BAD_JOB_SETUP; 
  } 
  else ATH_MSG_DEBUG("Tool " << m_LHSelectorTool << " retrieved");
  
  // For now, just try to retrieve the lumi tool
  if (m_lumiTool.retrieve().isFailure()) {
      ATH_MSG_DEBUG("Unable to retrieve Luminosity Tool");
      // 244            return HLT::ERROR;
  } else {
      ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");
  }
    
  ATH_MSG_DEBUG("IsEM " << m_IsEMrequiredBits);
  ATH_MSG_DEBUG("ApplyIsEM " << m_applyIsEM);
  ATH_MSG_DEBUG("ApplyLH " << m_applyLH);
  ATH_MSG_DEBUG("EtCut = " << m_emEt);
  ATH_MSG_DEBUG("AcceptAll " << m_acceptAll);
  
  ATH_MSG_DEBUG("Initialization of TrigEFCaloHypo completed successfully");

  return HLT::OK;
}




// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypo::hltFinalize(){
// ----------------------------------------------------------------------

 
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypo::hltExecute(const HLT::TriggerElement* outputTE,
					    bool& pass){
// ----------------------------------------------------------------------

   // Time total TrigEFCaloHypo execution time.
  // -------------------------------------
  if (timerSvc()) m_totalTimer->start();    

  //clear the monitoring vectors
  m_EBE0.clear();
  m_EBE1.clear();
  m_EBE2.clear();
  m_EBE3.clear();
  m_Eta.clear();
  m_EtaCalo.clear();
  m_PhiCalo.clear();
  m_E.clear();
  m_ECalib.clear();
  m_ERes.clear();

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::INFO << name() << ": in execute()" << endreq;
 

   // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode statt = getFeature(outputTE, roiDescriptor, "");
  
  if ( statt == HLT::OK ) {
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << "RoI id " << roiDescriptor->roiId()
	  << " LVL1 id " << roiDescriptor->l1Id()
	  << " located at   phi = " <<  roiDescriptor->phi()
	  << ", eta = " << roiDescriptor->eta() << endreq;
    }
  } else {
    if(msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }
    return HLT::NAV_ERROR;
  }

  // get CaloClusterContainer from the trigger element:
  //--------------------------------------------------


  bool accepted=false;
  // AcceptAll property = true means selection cuts should not be applied
  if (m_acceptAll) accepted=true;

  std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainer;
   
  HLT::ErrorCode stat = getFeatures(outputTE,vectorClusterContainer , "");

  if (stat != HLT::OK ) {
     msg() << MSG::WARNING 
 	<< " Failed to get vectorClusterContainers from the trigger element" 
 	<< endreq;
     if (timerSvc())m_totalTimer ->stop();
     return HLT::OK;
   } 

  ATH_MSG_DEBUG(" Got " << vectorClusterContainer.size()  << " vectorClusterContainer's associated to the TE ");
        
  // Shower Shape & CaloCellContainer
  const CaloCellContainer* pCaloCellContainer = 0;

  // Get vector of pointers to all CaloCellContainers from TE
  std::vector<const CaloCellContainer*> vectorCellContainer;

  stat = getFeatures(outputTE, vectorCellContainer);
  if ( stat != HLT::OK ) {
      ATH_MSG_ERROR("REGTEST: No CaloCellContainers retrieved for the trigger element");
      return HLT::OK;
  } else{
      ATH_MSG_DEBUG(" REGTEST: Got " << vectorCellContainer.size() << " CaloCellContainers associated to the TE ");
      // Check that there is only one CellContainer in the RoI
      if (vectorCellContainer.size() != 1){
          ATH_MSG_ERROR("REGTEST: Size of calo cell container vector is not 1 but " << vectorCellContainer.size());
          return HLT::ERROR;
      } else{
          // Get the last CellContainer if ShowerShapeTool is going to be run
          pCaloCellContainer = vectorCellContainer.back();
          if(!pCaloCellContainer){
              ATH_MSG_ERROR("Retrieval of CaloCellContainer from vector failed");
              return HLT::ERROR;
          } 
      }
  }

  std::vector<const xAOD::PhotonContainer*> vectorEgammaContainers;
  
  stat = getFeatures(outputTE, vectorEgammaContainers, "");

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

  const xAOD::PhotonContainer *egCont = vectorEgammaContainers.back();

  if(egCont == 0){
    if ( msgLvl() <= MSG::ERROR )
      msg() << MSG::ERROR
	    << " REGTEST: Retrieval of xOAD::PhotonContainer from vector failed"
	    << endreq;
    if (timerSvc()) m_totalTimer->stop();
    return HLT::OK;
  }

  
  const xAOD::CaloCluster *clus=0;
  for(const auto eg : *egCont){
      unsigned int isEMTrig = 0;
      bool isLHAcceptTrig = false;
      // First create the EL for clusters
      if(m_applyIsEM){
          ATH_MSG_DEBUG("REGTEST: Check Object, eta2 = " << fabsf(eg->caloCluster()->etaBE(2)) << " e = " << eg->caloCluster()->e());
          if(m_SelectorTool->execute(eg).isFailure())
              ATH_MSG_DEBUG("REGTEST:: Problem in isEM Selector");
          else isEMTrig = m_SelectorTool->IsemValue();
      }
      else if(m_applyLH){
          ATH_MSG_DEBUG("REGTEST:: CaloOnly LH Tool");
          const Root::TAccept& lhacc = m_LHSelectorTool->accept(eg); // use method for calo-only
          isLHAcceptTrig = (bool) (lhacc);
          ATH_MSG_DEBUG("REGTEST: Applying LH pid selection " << isLHAcceptTrig);
      }
      clus = eg->caloCluster();
      // Monitoring
      m_EBE0.push_back(clus->energyBE(0));
      m_EBE0.push_back(clus->energyBE(1));
      m_EBE0.push_back(clus->energyBE(2));
      m_EBE0.push_back(clus->energyBE(3));
      m_Eta.push_back(clus->eta());
      double tmpeta = -999.;
      clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
      double tmpphi = -999.;
      clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpphi);
      m_EtaCalo.push_back(tmpeta);
      m_PhiCalo.push_back(tmpphi);
//      m_E.push_back(clus->e());
      m_ECalib.push_back(clus->e());
//      m_ERes.push_back(clus->e()-newClus->e());
     
      if(m_applyIsEM){
          if( (isEMTrig & m_IsEMrequiredBits)!=0 ) {
              ATH_MSG_DEBUG("REGTEST IsEM = " << std::hex << isEMTrig 
                      << " cut not satisfied for pattern:" << std::hex << m_IsEMrequiredBits);
              continue;
          }
      }
      if(m_applyLH){
          if(!isLHAcceptTrig) {
              ATH_MSG_DEBUG("REGTEST: Fails LH");
              continue;
          }
      }
      if(clus->et() < m_emEt){
          ATH_MSG_DEBUG("REGTEST::Et cut no satisfied: "<< clus->et() << "< cut: " << m_emEt);
          continue;
      }
      accepted=true;
   
  }

  // Set event to pass 
  pass=accepted;
  ATH_MSG_DEBUG("REGTEST Result = " <<(accepted ? "accepted" : "not accepted"));
  ATH_MSG_DEBUG("REGTEST AcceptAll= " <<(m_acceptAll ? "true (no cuts)" : "false (selection applied)"));
  ATH_MSG_DEBUG("REGETST Result = " <<(pass ? "passed" : "failed"));
    
 
  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------

  if (timerSvc()) m_totalTimer->stop();

  return HLT::OK;
}

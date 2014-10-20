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
//#include "xAODEgammaCnv/xAODPhotonMonFuncs.h" // very annoying cannot use macros in more than one place
#include "CaloEvent/CaloCellContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"



class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCaloHypo::TrigEFCaloHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("Etcut",   m_EtCut = 22*CLHEP::GeV); // Default: 22 GeV
  declareProperty("DeltaPhiCut", m_deltaPhiCut = 0.1);
  declareProperty("DeltaEtaCut", m_deltaEtaCut = 0.1);
  declareProperty("EnergySecondSampling", m_energySecondSampling=0);
  declareProperty("isEM", m_isEM = 0 ); // Value of isEM.
  declareProperty("EtCalibFactor",m_etCalibFactor=1.02);
  
  declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" ); 

  // Boolean to call shower shape calculation and filling
  declareProperty("UseShowerShapeTool", m_UseShowerShapeTool=true, "Boolean to call shower shape calculation and filling");
  // Handles of instance of egammaShowerShape Tool to be run 
  declareProperty("ShowerShapeTool", m_ShowerShapeTool, "Handle of instance of egammaShowerShape Tool to be run");
  declareProperty("SelectorTool", m_SelectorTool, "Handle for selector tool");
  declareProperty("SelectorToolName", m_SelectorToolName, "Name for selector tool");

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

  if (m_ShowerShapeTool.empty()) {
      ATH_MSG_INFO("ShowerShape is empty");
      return HLT::BAD_JOB_SETUP;
  }

  // retrieve egammaShowerShape Tool
  if(m_ShowerShapeTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve "<<m_ShowerShapeTool);
      return HLT::BAD_JOB_SETUP; 
  }
  else ATH_MSG_DEBUG("Retrieved Tool " << m_ShowerShapeTool);

  m_SelectorTool=ToolHandle<IAsgElectronIsEMSelector>(m_SelectorToolName);
  if(m_SelectorTool.retrieve().isFailure()) {
      msg() << MSG::ERROR << "Unable to retrieve " << m_SelectorTool 
          << " tool " << endreq;
      return HLT::BAD_JOB_SETUP; 
  } 
  else ATH_MSG_DEBUG("Tool " << m_SelectorTool << " retrieved");
      
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
  if (m_timersvc) m_totalTimer->start();    


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
 
  
  double EtaRef = roiDescriptor->eta();
  double PhiRef = roiDescriptor->phi();


  // Phi was defined between [0,2*pi). LVL1 RoI's are still produced
  // with the older convention. 
  // What are negative phi's?????????
  //if(PhiRef <0) continue;

  if(PhiRef > 3.1415926 ) PhiRef -= 2*3.1415926;


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
     if (m_timersvc)m_totalTimer ->stop();
     return HLT::OK;
   } 

  if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::INFO << " Got " << vectorClusterContainer.size() 
          << " vectorClusterContainer's associated to the TE " << endreq;
  // Shower Shape & CaloCellContainer
  const CaloCellContainer* pCaloCellContainer = 0;

  // Get vector of pointers to all CaloCellContainers from TE
  std::vector<const CaloCellContainer*> vectorCellContainer;

  stat = getFeatures(outputTE, vectorCellContainer);
  if ( stat != HLT::OK ) {
      msg() << MSG::ERROR << "REGTEST: No CaloCellContainers retrieved for the trigger element" << endreq;
      return HLT::OK;
  } else{
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " REGTEST: Got " << vectorCellContainer.size()
          << " CaloCellContainers associated to the TE " << endreq;
      // Check that there is only one CellContainer in the RoI
      if (vectorCellContainer.size() != 1){
          msg() << MSG::ERROR
              << "REGTEST: Size of calo cell container vector is not 1 but " << vectorCellContainer.size()
              << endreq;
          return HLT::ERROR;
      } else{
          // Get the last CellContainer if ShowerShapeTool is going to be run
          pCaloCellContainer = vectorCellContainer.back();
          if(!pCaloCellContainer){
              msg() << MSG::ERROR << "Retrieval of CaloCellContainer from vector failed" << endreq;
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
    if (m_timersvc) m_totalTimer->stop();
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

  double temp_EtThreshold=m_EtCut;
  if (temp_EtThreshold>20000) temp_EtThreshold=20000;
  for(const auto eg : *egCont){
      // First create the EL for clusters
      const Root::TAccept& acc = m_SelectorTool->accept(eg,temp_EtThreshold);
      ATH_MSG_DEBUG("REGTEST: Applying pid selection " << (bool) (acc));
      accepted=true;
      pass=accepted;
   }
 
  // Original pure calo code 
  /*const xAOD::CaloClusterContainer * clusterContainer = vectorClusterContainer.front();

  unsigned int iclus=0;
  for(const auto *clus : *clusterContainer){

    // DeltaEta, DeltaPhi cuts:
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << " fabs(clus->eta()-EtaRef) " 
	  << fabs(clus->eta()-EtaRef) 
	  << " fabs(clus->phi()-PhiRef) " << fabs(clus->phi()-PhiRef) 
	  << endreq;
      
      msg() << MSG::DEBUG << " clus->eta() " << clus->eta() << " EtaRef " 
	  << EtaRef  << " clus->phi() " << clus->phi() << " PhiRef " 
	  << PhiRef << endreq;
    }

    // Disable cuts for now
    // if( fabs(clus->eta()-EtaRef) > m_deltaEtaCut) continue;
    // if( fabs(clus->phi()-PhiRef) > m_deltaPhiCut) continue;
    
    // Et cut:
    // if( clus->et()*m_etCalibFactor < m_EtCut) continue;
    

    //ATH_MSG_DEBUG("REGEST: ethad " << getShowerShape_ethad(eg));

    // Calculate shower shapes in all samplings
    if (m_UseShowerShapeTool) {
        // protection in case tool does not exist
        if (!m_ShowerShapeTool.empty()) {
            StatusCode sc = m_ShowerShapeTool->execute(clus,pCaloCellContainer);
            if ( sc.isFailure() ) {
                ATH_MSG_WARNING("call to ShowerShape returns failure ");
            }
            else {
                ATH_MSG_DEBUG("ShowerShape variables for cluster");
                float value=0;
                // E in 1x1 cells in pre sampler
                value=static_cast<float>(m_ShowerShapeTool->e011());
                ATH_MSG_DEBUG("e011 " << value);
                // E in 3x3 cells in pre sampler
                value=static_cast<float>(m_ShowerShapeTool->e033());
                ATH_MSG_DEBUG("e033 " << value);
                // E in 3x2 cells in S1
                value=static_cast<float>(m_ShowerShapeTool->e132());
                ATH_MSG_DEBUG("e132 " << value);
                // E in 15x2 cells in S1
                value=static_cast<float>(m_ShowerShapeTool->e1152());
                ATH_MSG_DEBUG("e1152 " << value);
                // fraction of E in S1
                value=static_cast<float>(m_ShowerShapeTool->f1());
                ATH_MSG_DEBUG("f1 " << value);
                // fraction of E in the core(e132) in S1
                value=static_cast<float>(m_ShowerShapeTool->f1core());
                ATH_MSG_DEBUG("f1core " << value);
                // corr width with 3 strips
                value=static_cast<float>(m_ShowerShapeTool->width3c());
                ATH_MSG_DEBUG("width3c " << value);
                // energy in second max
                value=static_cast<float>(m_ShowerShapeTool->esec());
                ATH_MSG_DEBUG("esec " << value);
                // energy strip of second max
                value=static_cast<float>(m_ShowerShapeTool->esec1());
                ATH_MSG_DEBUG("esec1 " << value);
                // fraction of E outside core in S1
                value=static_cast<float>(m_ShowerShapeTool->fracm());
                ATH_MSG_DEBUG("fracm " << value);
                // width with 5 strips
                value=static_cast<float>(m_ShowerShapeTool->width5());
                ATH_MSG_DEBUG("width5 " << value);
                //eta pos within cell in S1
                value=static_cast<float>(m_ShowerShapeTool->poscs1());
                ATH_MSG_DEBUG("pocs1 " << value);
                // asymmetry with 3 strips
                value=static_cast<float>(m_ShowerShapeTool->asy3());
                ATH_MSG_DEBUG("asy3 " << value);
                // diff position +/- 1 cells
                value=static_cast<float>(m_ShowerShapeTool->pos());
                ATH_MSG_DEBUG("pos " << value);
                // diff position +/- 7 cells
                value=static_cast<float>(m_ShowerShapeTool->pos7());
                ATH_MSG_DEBUG("pos7 " << value);
                // E of strip with min E
                value=static_cast<float>(m_ShowerShapeTool->emin());
                ATH_MSG_DEBUG("emin " << value);
                // E of strip with max E
                value=static_cast<float>(m_ShowerShapeTool->emax());
                ATH_MSG_DEBUG("emax " << value);
                // barycentre in eta in S1
                value=static_cast<float>(m_ShowerShapeTool->eta1());
                ATH_MSG_DEBUG("eta1 " << value);
                // total width in strips
                value=static_cast<float>(m_ShowerShapeTool->wtot());
                ATH_MSG_DEBUG("wtot " << value);
                //
                // information in the 2nd sampling
                //
                // E in 3x3 cells in S2
                value=static_cast<float>(m_ShowerShapeTool->e233());
                ATH_MSG_DEBUG("e233 " << value);
                // E in 3x5 cells in S2
                value=static_cast<float>(m_ShowerShapeTool->e235());
                ATH_MSG_DEBUG("e235 " << value);
                // E in 3x7 cells in S2
                value=static_cast<float>(m_ShowerShapeTool->e237());
                ATH_MSG_DEBUG("e237 " << value);
                // E in 5x5 cells in S2
                value=static_cast<float>(m_ShowerShapeTool->e255());
                ATH_MSG_DEBUG("e255 " << value);
                // E in 7x7 cells in S2
                value=static_cast<float>(m_ShowerShapeTool->e277());
                ATH_MSG_DEBUG("e277 " << value);
                // corr width in S2
                value=static_cast<float>(m_ShowerShapeTool->etaw());
                ATH_MSG_DEBUG("etaw " << value);
                // uncorr width in S2
                value=static_cast<float>(m_ShowerShapeTool->width());
                ATH_MSG_DEBUG("width " << value);
                // position in eta within cell in S2
                value=static_cast<float>(m_ShowerShapeTool->poscs2());
                ATH_MSG_DEBUG("poscs2 " << value);
                //
                // information in the 3rd sampling
                //
                // fraction of E in S3
                value=static_cast<float>(m_ShowerShapeTool->f3());
                ATH_MSG_DEBUG("f3 " << value);
                // fraction of E in the core (e333) in S3
                value=static_cast<float>(m_ShowerShapeTool->f3core());
                ATH_MSG_DEBUG("f3core " << value);
                // E in 3x3 cells in S3
                value=static_cast<float>(m_ShowerShapeTool->e333());
                ATH_MSG_DEBUG("e333 " << value);
                // E in 3x5 cells in S3
                value=static_cast<float>(m_ShowerShapeTool->e335());
                ATH_MSG_DEBUG("e335 " << value);
                // E in 3x7 cells in S3
                value=static_cast<float>(m_ShowerShapeTool->e337());
                ATH_MSG_DEBUG("e337 " << value);
                // E in 7x7 cells in S3
                value=static_cast<float>(m_ShowerShapeTool->e377());
                ATH_MSG_DEBUG("e377 " << value);
                //
                // information combining all samplings
                //
                // ratio of energy in 3x3/3x7 cells
                value=static_cast<float>(m_ShowerShapeTool->reta3337_allcalo());
                ATH_MSG_DEBUG("reta3337 " << value);
                // core energy
                value=static_cast<float>(m_ShowerShapeTool->ecore());
                ATH_MSG_DEBUG("ecore " << value);
                //
                // information combining different shower shape
                //
                float valueSecond=0;
                /// @brief  e237/e277
                value=static_cast<float>(m_ShowerShapeTool->e277());
                valueSecond=static_cast<float>(m_ShowerShapeTool->e237());
                if(valueSecond != 0)
                    ATH_MSG_DEBUG("Reta " << value/valueSecond);
                /// @brief  e233/e237
                value=static_cast<float>(m_ShowerShapeTool->e233());
                valueSecond=static_cast<float>(m_ShowerShapeTool->e237());
                if(valueSecond != 0)
                    ATH_MSG_DEBUG("Rphi " << value/valueSecond);
                /// @brief (emaxs1-e2tsts1)/(emaxs1+e2tsts1)
                value=static_cast<float>(m_ShowerShapeTool->emax());
                valueSecond=static_cast<float>(m_ShowerShapeTool->esec1());
                if(fabs(valueSecond+value) != 0)
                    ATH_MSG_DEBUG("Eratio " << (value-valueSecond)/(value+valueSecond));
                value=static_cast<float>(m_ShowerShapeTool->emin());
                ATH_MSG_DEBUG("DeltaE " << (valueSecond-value));
            } // shower shape variables
        } // end shower shape tool
        iclus++; // increment the counter for clusters
    }
    
    // ---------------------------------------------------
    // At least one cluster passed cuts: accept the event:
    // ---------------------------------------------------

    accepted=true;
    pass=accepted;
  } // end of loop in clusters*/

  if(msgLvl() <= MSG::DEBUG) {
  
    msg() << MSG::DEBUG << "Result = " <<(pass ? "passed" : "failed")<< endreq;
  }
    
 
  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------

  if (m_timersvc) m_totalTimer->stop();

  return HLT::OK;
}

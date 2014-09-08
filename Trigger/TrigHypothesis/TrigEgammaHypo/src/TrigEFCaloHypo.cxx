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
//           Calorimeter variables.
//
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
//#include "LArRecEvent/LArCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"



class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCaloHypo::TrigEFCaloHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Etcut",   m_EtCut = 22*CLHEP::GeV); // Default: 22 GeV
  declareProperty("DeltaPhiCut", m_deltaPhiCut = 0.1);
  declareProperty("DeltaEtaCut", m_deltaEtaCut = 0.1);
  declareProperty("EnergySecondSampling", m_energySecondSampling=0);
  declareProperty("isEM", m_isEM = 0 ); // Value of isEM.
  declareProperty("EtCalibFactor",m_etCalibFactor=1.02);
  
  declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" ); 


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

 
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::INFO
	<< "Initialization of TrigEFCaloHypo completed successfully"
	<< endreq;

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

  std::vector<const CaloClusterContainer*> vectorClusterContainer;
   
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


  const CaloClusterContainer * clusterContainer = vectorClusterContainer.front();


  for(CaloClusterContainer::const_iterator clusIt = clusterContainer->begin();
      clusIt != clusterContainer->end(); clusIt++){

    const CaloCluster *clus = *clusIt;

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

    if( fabs(clus->eta()-EtaRef) > m_deltaEtaCut) continue;
    if( fabs(clus->phi()-PhiRef) > m_deltaPhiCut) continue;
	  

    // Et cut:
    
    if( clus->et()*m_etCalibFactor < m_EtCut) continue;

    
    // ---------------------------------------------------
    // At least one cluster passed cuts: accept the event:
    // ---------------------------------------------------

    accepted=true;
    pass=accepted;
  } // end of loop in clusters

  if(msgLvl() <= MSG::DEBUG) {
  
    msg() << MSG::DEBUG << "Result = " <<(pass ? "passed" : "failed")<< endreq;
  }
    
 
  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------

  if (m_timersvc) m_totalTimer->stop();

  return HLT::OK;
}

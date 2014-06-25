/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigLoFRemoval.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigEFLongLivedParticles
//
// AUTHOR:   A. Policicchio
//           This is an Hlt algorithm that retrieves EF jets and calo cells
//           for the LoF removal 
//
// ********************************************************************
//
#include <sstream>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigEFLongLivedParticles/TrigLoFRemoval.h"
//
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
//
#include "FourMomUtils/P4DescendingSorters.h"
//
#include "TrigSteeringEvent/PhiHelper.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "AthenaKernel/Timeout.h"

//
#define PI 3.14159265


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigLoFRemoval::TrigLoFRemoval(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::FexAlgo(name, pSvcLocator)
 {


  // Monitored variables...
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
  declareMonitoredVariable("CellContainerSize", m_CellContainerSize);

}
//}; // Supress the semicolon to make it compatible with gcc 3.4.4

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigLoFRemoval::~TrigLoFRemoval()
{  }
//{  }; // Supress the semicolon to make it compatible with gcc 3.4.4

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

HLT::ErrorCode TrigLoFRemoval::hltInitialize()
{


  msg() << MSG::INFO << "in initialize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigLoFRemoval::hltFinalize(){

  msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigLoFRemoval::hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE){

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endreq;

  // Monitoring, we set-up default :
  m_et = -99000.;
  m_njet = 0.0;
  m_eta = -99.;
  m_phi = -99.;

  // Some debug output:
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "outputTE->getId(): " << outputTE->getId() << endreq;
    msg() << MSG::DEBUG << "inputTE->getId(): " << inputTE->getId() << endreq;
  }



  const JetCollection* outJets(0);
  HLT::ErrorCode ec = getFeature(outputTE, outJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get JetCollections " << endreq;
    return ec;
  }

  if( outJets == 0 ){
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Got no JetCollections associated to the TE! " << endreq;
    return HLT::OK;
  }

  std::vector<const Jet*> theJets(outJets->begin(), outJets->end());

  //check size of JetCollection
  if( theJets.size() == 0 ){
    if(msgLvl() <= MSG::DEBUG) msg()<< MSG::DEBUG << " Size of JetCollection is 0! " << endreq;
    return HLT::OK;
  }
  std::sort (theJets.begin(), theJets.end(), P4Sorters::Descending::Et());

  // only one jet per JetCollection (for the moment, change it in the future)
 
  double etLjet = 0;
  double etaLjet = 0;
  double phiLjet = 0;
  int count = 0;
  for (const Jet* aJet : theJets) {
    count++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet Et: " << aJet->et() << endreq;
    if(count==1) {
      etLjet = aJet->et();
      etaLjet = aJet->eta();
      phiLjet = aJet->phi();
    }
  }
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " Highest Et Jet Et= "   << etLjet        << endreq;
    msg() << MSG::DEBUG << " Highest Et Jet eta = " << etaLjet       << endreq;
    msg() << MSG::DEBUG << " Highest Et Jet phi = " << phiLjet       << endreq;
  }

  // monitoring
  m_et = etLjet;
  m_eta = etaLjet;
  m_phi = phiLjet;
  m_njet = (float)theJets.size();


  // We retrieve the CellContainer from the Trigger Element...
  std::vector<const CaloCellContainer*> vectorOfCellContainers;

  if( getFeatures(outputTE, vectorOfCellContainers, "") != HLT::OK) {
    msg() << MSG::WARNING << "Failed to get TrigCells" << endreq;   
    return HLT::OK;
  }
 
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size() << " CellContainers" << endreq;
 
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfCellContainers.size() < 1) {
    msg() << MSG::WARNING << "No cells to analyse, leaving!" << endreq;
    return HLT::OK;
  }

  // get last ccontainer to be put in vector (should also be the only one)
  const CaloCellContainer* theCellCont = vectorOfCellContainers.back();

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " Retrieved a Cell Container of Size= " << theCellCont->size() << endreq;
  }

  CaloCellContainer* theCellContLoF = 0;
  theCellContLoF = new CaloCellContainer(SG::VIEW_ELEMENTS);
  for(CaloCellContainer::const_iterator iter = theCellCont->begin(); iter != theCellCont->end(); ++iter){
    //LoF cell selection:
    //-tile
    //-provenance
    //-energy
    if((*iter)->caloDDE()->is_tile() && (*iter)->energy() > 240.0){
      //-dPhi
      //-dR
      float d_phi = HLT::wrapPhi((*iter)->phi() - phiLjet);
      float d_eta = (*iter)->eta() - etaLjet;

      if(fabs(d_phi) < 0.2 && sqrt(d_eta*d_eta + d_phi*d_phi) > 0.3){
        //-early
        float t = (*iter)->time();
        if(t < -2.0){
          //-halo time
          float x = (*iter)->x();
          float y = (*iter)->y();
          float z = (*iter)->z();
          float c = 299.792458;//mm per ns
          float r = sqrt(x*x + y*y);
          if((fabs(t - (z-sqrt(z*z + r*r))/c) < 5.0) || (fabs(t - (-z-sqrt(z*z + r*r))/c) < 5.0)){
            theCellContLoF->push_back((*iter));
          }
        }
      }
    }
  }

  //now save the cell container
  std::string persKey = "TrigCaloCellLoF";
  std::string cellCollKey;
  HLT::ErrorCode sc = getUniqueKey( theCellContLoF, cellCollKey, persKey );
  if (sc != HLT::OK) {
    msg() << MSG::DEBUG << "Could not retrieve the cell collection key" << endreq;
    return sc;
  }     
  if ( store()->record(theCellContLoF, cellCollKey).isFailure() ) {
    msg() << MSG::ERROR << "Could not record a cell container with key " << cellCollKey << endreq;
  }

  sc = reAttachFeature(outputTE, theCellContLoF, cellCollKey, persKey );
  if (sc != HLT::OK) {
    msg() << MSG::WARNING << "Could not record a cell container with key " << cellCollKey << " " << name() << endreq;
  } else {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: Recorded the cell container " << endreq;
  }
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " REGTEST: Produced a Cell Container of Size= " << theCellContLoF->size() << endreq;

  //monitoring
  m_CellContainerSize = (float)theCellContLoF->size();

  return HLT::OK;

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"
#include "CaloDmDetDescr/CaloDmDescrManager.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "TBRec/TBDMContainerSplitter.h"

TBDMContainerSplitter::TBDMContainerSplitter(const std::string& name, 
				   ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator),
  m_DMHitsKey("LArCalibrationHitDeadMaterial"),
  m_DMHitsCaloKey("LArCalibrationHitDeadMaterial_Calo"),
  m_DMHitsLeakKey("LArCalibrationHitDeadMaterial_Leakage"),
  m_eventStore(0), m_caloDmDescrManager(0)
{ 
  declareProperty("InputDMContainer",m_DMHitsKey);
  declareProperty("OutputDM_Calo",m_DMHitsCaloKey);
  declareProperty("OutputDM_Leak",m_DMHitsLeakKey);
}

TBDMContainerSplitter::~TBDMContainerSplitter()
{ }

StatusCode TBDMContainerSplitter::initialize()
{
  
  MsgStream log(messageService(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  
  // Get StoreGateSvc //
  StatusCode sc = service ( "StoreGateSvc" , m_eventStore ) ;  
  if( sc.isFailure() ) {
    log<<MSG::FATAL<<" Cannot locate StoreGateSvc " << endreq ;
    sc = StatusCode::FAILURE ;
    return sc ;
  }

  m_caloDmDescrManager =  CaloDmDescrManager::instance();
  if(!m_caloDmDescrManager) {
     log<<MSG::FATAL<<" Could not get the CaloDmDescrManager !! "<<endreq;
     return StatusCode::FAILURE ;
  }

  log << MSG::INFO << "end of initialize()" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode TBDMContainerSplitter::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TBDMContainerSplitter::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Executing TBDMContainerSplitter " << endreq;

  StatusCode sc;
  // Retrieve Original DM container
  const CaloCalibrationHitContainer* inDMCont;
  sc = m_eventStore->retrieve(inDMCont, m_DMHitsKey);
  if ( sc.isFailure() ) {
    log << MSG::ERROR
	<< "Cannot retrieve CaloCalibrationHitContainer "<<m_DMHitsKey<<" from StoreGate" << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  // Create two new DMCont
  CaloCalibrationHitContainer* outDMContCalo = new CaloCalibrationHitContainer();
  CaloCalibrationHitContainer* outDMContLeak = new CaloCalibrationHitContainer();

  // Loop over original, and fill two new ones
  CaloCalibrationHitContainer::const_iterator chIter  = inDMCont->begin();
  CaloCalibrationHitContainer::const_iterator chIterE = inDMCont->end();
  for(;chIter!=chIterE;chIter++)  {
      Identifier myId = (*chIter)->cellID();
      CaloDmDescrElement* myCDDE(0);
      myCDDE = m_caloDmDescrManager->get_element(myId);
      CaloCalibrationHit *chit = new CaloCalibrationHit(myId, (*chIter)->energyEM(), (*chIter)->energyNonEM(), (*chIter)->energyInvisible(), (*chIter)->energyEscaped());
      if ( myCDDE ) {
          if(myCDDE->eta()>2.5 && myCDDE->phi() > M_PI/2 && myCDDE->phi() < M_PI){ // Calo hit
             outDMContCalo->push_back(chit);
          } else{ //Leak hit
             outDMContLeak->push_back(chit);
          }
      } else { // No CDDE, assume leak hit
         outDMContLeak->push_back(chit);
      }
  }

  // Store new containers
  sc = m_eventStore->record(outDMContCalo, m_DMHitsCaloKey);
  if ( sc.isFailure() ) {
         log << MSG::ERROR << "Could not store: "<<m_DMHitsCaloKey<<" to StoreGate" << endreq;
         setFilterPassed(false);
         return StatusCode::SUCCESS;
  }
  sc = m_eventStore->record(outDMContLeak, m_DMHitsLeakKey);
  if ( sc.isFailure() ) {
         log << MSG::ERROR << "Could not store: "<<m_DMHitsLeakKey<<" to StoreGate" << endreq;
         setFilterPassed(false);
         return StatusCode::SUCCESS;
  }


  setFilterPassed(true);
  return StatusCode::SUCCESS;

}


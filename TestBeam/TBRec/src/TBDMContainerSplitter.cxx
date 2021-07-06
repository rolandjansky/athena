/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBDMContainerSplitter.h"

#include "CaloDmDetDescr/CaloDmDescrManager.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

TBDMContainerSplitter::TBDMContainerSplitter(const std::string& name, 
				   ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_DMHitsKey("LArCalibrationHitDeadMaterial"),
  m_DMHitsCaloKey("LArCalibrationHitDeadMaterial_Calo"),
  m_DMHitsLeakKey("LArCalibrationHitDeadMaterial_Leakage"),
  m_caloDmDescrManager(0)
{ 
  declareProperty("InputDMContainer",m_DMHitsKey);
  declareProperty("OutputDM_Calo",m_DMHitsCaloKey);
  declareProperty("OutputDM_Leak",m_DMHitsLeakKey);
}

TBDMContainerSplitter::~TBDMContainerSplitter()
{ }

StatusCode TBDMContainerSplitter::initialize()
{
  ATH_MSG_INFO ( "in initialize()" );

  m_caloDmDescrManager =  CaloDmDescrManager::instance();
  if(!m_caloDmDescrManager) {
    ATH_MSG_FATAL(" Could not get the CaloDmDescrManager !! ");
    return StatusCode::FAILURE ;
  }

  ATH_MSG_INFO ( "end of initialize()" );
  return StatusCode::SUCCESS;
}


StatusCode TBDMContainerSplitter::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TBDMContainerSplitter::execute()
{
  ATH_MSG_DEBUG ( "Executing TBDMContainerSplitter " );

  // Retrieve Original DM container
  const CaloCalibrationHitContainer* inDMCont;
  StatusCode sc = evtStore()->retrieve(inDMCont, m_DMHitsKey);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR
      ( "Cannot retrieve CaloCalibrationHitContainer "<<m_DMHitsKey<<" from StoreGate" );
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  // Create two new DMCont
  CaloCalibrationHitContainer* outDMContCalo = new CaloCalibrationHitContainer();
  CaloCalibrationHitContainer* outDMContLeak = new CaloCalibrationHitContainer();

  // Loop over original, and fill two new ones
  for (const CaloCalibrationHit* hit : *inDMCont) {
      Identifier myId = hit->cellID();
      CaloDmDescrElement* myCDDE(0);
      myCDDE = m_caloDmDescrManager->get_element(myId);
      CaloCalibrationHit *chit = new CaloCalibrationHit(myId, hit->energyEM(), hit->energyNonEM(), hit->energyInvisible(), hit->energyEscaped());
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
  sc = evtStore()->record(outDMContCalo, m_DMHitsCaloKey);
  if ( sc.isFailure() ) {
         ATH_MSG_ERROR ( "Could not store: "<<m_DMHitsCaloKey<<" to StoreGate" );
         setFilterPassed(false);
         return StatusCode::SUCCESS;
  }
  sc = evtStore()->record(outDMContLeak, m_DMHitsLeakKey);
  if ( sc.isFailure() ) {
         ATH_MSG_ERROR ( "Could not store: "<<m_DMHitsLeakKey<<" to StoreGate" );
         setFilterPassed(false);
         return StatusCode::SUCCESS;
  }


  setFilterPassed(true);
  return StatusCode::SUCCESS;

}


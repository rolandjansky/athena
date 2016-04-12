/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     CaloTowerMaker.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma, 
CREATED:  Feb 5, 2001

PURPOSE:  This serves as the main algorithm hook that clients must invoke 
          to reconstruct CaloTower's.  This class expects to
	  instantiate sub-algorithms to fill the CaloTowerContainer. 
Updated:  Move to StoreGate, May 2001 (SR)
Updated:  use ListItem, set property defaults, clean-up. (SR 11/01)
Updated:  isolate alg-factory stuff (12/01)
********************************************************************/

#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloRec/CaloTowerMaker.h"
#include "CaloEvent/CaloTowerContainer.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloTowerMaker::CaloTowerMaker(const std::string& name, 
			       ISvcLocator* pSvcLocator) 
  :AthAlgorithm(name, pSvcLocator) ,
   m_towerContainer("TowerContainer"), 
   m_netatower(50), m_nphitower(64),
   m_etamin(-2.5), m_etamax(2.5)
{
  // Read in the following from jobOptions file:
  declareProperty("TowerContainer", m_towerContainer);
  declareProperty("TowerBuilders",  m_towerBuilderNames);
  declareProperty("neta_tower",m_netatower);
  declareProperty("nphi_tower",m_nphitower);
  declareProperty("eta_min",m_etamin);
  declareProperty("eta_max",m_etamax);

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloTowerMaker::~CaloTowerMaker()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode CaloTowerMaker::initialize()
{
  MsgStream  log(messageService(),name());
 
  // Create cell builder Algorithm Hooks:

  // CREATE tower builder algorithms

  std::vector<std::string>::iterator it = m_towerBuilderNames.begin();

  for (; it < m_towerBuilderNames.end(); it++) 
  {

    ListItem alg(*it);
    Algorithm* pAlg;

    StatusCode sc = createSubAlgorithm(alg.type(), alg.name(), pAlg);    

    if( sc.isFailure() ) 
    {
      log << MSG::FATAL << "Unable to create " << *it
	  << " tower builder" 
	  << endreq;
      return sc;
    } 

    // Save all the Correction Algorithm pointers in m_CellCorrections.

    m_towerBuilderPointers.push_back(pAlg);

    // Pass the location of Tower Container in TDS to the Correction Classes.
    pAlg->setProperty(StringProperty("TowerContainer",m_towerContainer));

  }     

  log << MSG::INFO 
      << "Initialization completed successfully" 
      << endreq;

  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////
// EXECUTE:
// The execute method will execute the individual sub-algorithms 
// created in the initialize method.
/////////////////////////////////////////////////////////////////////

StatusCode CaloTowerMaker::execute()
{
  MsgStream  log(messageService(),name());

  // create a CaloTowerContainer and register in TDS
  CaloTowerSeg towerseg(m_netatower,m_nphitower,m_etamin,m_etamax); 
  CaloTowerContainer* towerContainer = new CaloTowerContainer(towerseg);

  log << MSG::DEBUG 
	<< " registering CaloTowerContainer "<<m_towerContainer 
 	<< endreq;

  //
  StatusCode sc = evtStore()->record(towerContainer, m_towerContainer);
  if (sc != StatusCode::SUCCESS) {
      log << MSG::ERROR 
	<< "Error registering CaloTowerContainer in TDS "
	<< endreq;
    return sc;
  } 


  // run tower builders

  std::vector<Algorithm*>::iterator it = m_towerBuilderPointers.begin();
  for (; it < m_towerBuilderPointers.end(); it++) 
  {
    (*it)->execute();
  }
    
  sc = evtStore()->setConst(towerContainer);

  

  // make navigable!
  sc = evtStore()->symLink(towerContainer,(INavigable4MomentumCollection*)0);
  if ( sc.isFailure() )
    {
      log << MSG::WARNING
	  << "could not link CaloTowerContainer as navigable container"
	  << endreq;
    }

  log << MSG::DEBUG 
      << "execute completed successfully" 
      << endreq;

  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
// Note that it is NOT NECESSARY to run the finalize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode CaloTowerMaker::finalize()
{

  MsgStream  log(messageService(),name());
  log << MSG::DEBUG 
      << "finalize completed successfully" 
      << endreq;

  return StatusCode::SUCCESS;

}








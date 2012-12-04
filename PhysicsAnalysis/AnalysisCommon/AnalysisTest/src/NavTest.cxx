/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTest/NavTest.h"

#include "StoreGate/StoreGate.h"
#include "GaudiKernel/MsgStream.h"

#include "egammaEvent/ElectronContainer.h"
#include "GeneratorObjects/McEventCollection.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

NavTest::NavTest(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator)
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavTest::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  StatusCode sc;

  // get StoreGate service
  sc = service("StoreGateSvc",m_storeGate);
  if (sc.isFailure())
    {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavTest::execute()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  StatusCode sc;

  // Retrieve Electron from AOD
  const ElectronContainer * eCont;
  sc =m_storeGate->retrieve(eCont, "ElectronCollection");
  if (sc.isFailure())
    {
      log << MSG::FATAL << "Electron could not be retrieved from StoreGate !" << endreq;
      return StatusCode::FAILURE;
    }

  log << MSG::INFO << " --- " << endreq;
  log << MSG::INFO << "Electron Size : " << eCont->size() << endreq;

  // loop over all Electron
  ElectronContainer::const_iterator itE  = eCont->begin();
  ElectronContainer::const_iterator itEe = eCont->end();
  for (; itE != itEe; ++itE)
    {
      log << MSG::INFO << "Electron pT : " << (*itE)->pt() << endreq;

      // get TrackParticle via ElementLink
      const Rec::TrackParticle *track = (*itE)->trackParticle();
      if (track == 0)
	log << MSG::INFO << "No Track" << endreq;	
      else
	log << MSG::INFO << "Track charge : " << track->charge() << endreq;		

      // get LAr cluster from ESD via ElementLink of egamma
      const CaloCluster *cls = (*itE)->cluster();
      if (cls == 0)
	{
	  log << MSG::INFO << "Null LArCluster" << endreq;
	  continue;
	}
      log << MSG::INFO << "LArCluster eta0 : " << cls->eta0() << endreq;
    }

  // Retrieve G4Truth directly from ESD
  const McEventCollection * g4Cont;
  sc =m_storeGate->retrieve(g4Cont, "G4Truth");
  if (sc.isFailure())
    {
      log << MSG::FATAL << "G4Truth could not be retrieved from StoreGate !" << endreq;
      return StatusCode::FAILURE;
    }

  log << MSG::INFO << " --- " << endreq;
  log << MSG::INFO << "G4Truth Size : " << g4Cont->size() << endreq;

  // loop over all McEvent
  McEventCollection::const_iterator itG  = g4Cont->begin();
  McEventCollection::const_iterator itGe = g4Cont->end();
  for (; itG != itGe; ++itG)
    log << MSG::INFO << "Size of particles : " << (*itG)->particles_size() << endreq;

 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavTest::finalize()
{
  return StatusCode::SUCCESS;
}

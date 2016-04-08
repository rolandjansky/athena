/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTest/NavTest.h"

#include "egammaEvent/ElectronContainer.h"
#include "GeneratorObjects/McEventCollection.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

NavTest::NavTest(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavTest::initialize()
{
  ATH_MSG_INFO ( "in initialize()" ) ;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavTest::execute()
{
  ATH_MSG_INFO ( "in execute()" ) ;

  // Retrieve Electron from AOD
  const ElectronContainer * eCont = nullptr;
  ATH_CHECK( evtStore()->retrieve(eCont, "ElectronCollection") );

  ATH_MSG_INFO ( " --- " ) ;
  ATH_MSG_INFO ( "Electron Size : " << eCont->size() ) ;

  // loop over all Electron
  for (const Analysis::Electron* ele : *eCont)
    {
      ATH_MSG_INFO ( "Electron pT : " << ele->pt() ) ;

      // get TrackParticle via ElementLink
      const Rec::TrackParticle *track = ele->trackParticle();
      if (track == 0)
	ATH_MSG_INFO ( "No Track" ) ;	
      else
	ATH_MSG_INFO ( "Track charge : " << track->charge() ) ;		

      // get LAr cluster from ESD via ElementLink of egamma
      const CaloCluster *cls = ele->cluster();
      if (cls == 0)
	{
	  ATH_MSG_INFO ( "Null LArCluster" ) ;
	  continue;
	}
      ATH_MSG_INFO ( "LArCluster eta0 : " << cls->eta0() ) ;
    }

  // Retrieve G4Truth directly from ESD
  const McEventCollection * g4Cont = nullptr;
  ATH_CHECK( evtStore()->retrieve(g4Cont, "G4Truth") );

  ATH_MSG_INFO ( " --- " ) ;
  ATH_MSG_INFO ( "G4Truth Size : " << g4Cont->size() ) ;

  // loop over all McEvent
  for (const HepMC::GenEvent* ev : *g4Cont) {
    ATH_MSG_INFO ( "Size of particles : " << ev->particles_size() ) ;
  }

 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavTest::finalize()
{
  return StatusCode::SUCCESS;
}

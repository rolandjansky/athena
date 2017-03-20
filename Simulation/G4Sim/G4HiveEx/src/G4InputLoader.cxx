/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// G4InputLoader.cxx 
// Implementation file for class G4InputLoader
/////////////////////////////////////////////////////////////////// 

#include "G4InputLoader.h"

#include "GaudiKernel/Property.h"

// #include "EventInfo/EventInfo.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AthenaKernel/errorcheck.h"

// #include "InDetIdentifier/PixelID.h"
// #include "InDetSimEvent/SiHitCollection.h"
// #include "InDetSimEvent/TRTUncompressedHitCollection.h"
// #include "TrackRecord/TrackRecordCollection.h"


G4InputLoader::G4InputLoader( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_mcTruth("GEN_EVENT")
{
  declareProperty( "McTruth", m_mcTruth="GEN_EVENT" );
}


StatusCode G4InputLoader::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const McEventCollection* genEvt = 0;
  CHECK(evtStore()->retrieve(genEvt, m_mcTruth));

  ATH_MSG_DEBUG(evtStore()->dump()); 

  return StatusCode::SUCCESS;
}

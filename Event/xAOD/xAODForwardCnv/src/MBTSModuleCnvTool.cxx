/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old EDM Includes:
//#include "TileEvent/TileCell.h"
#include "TileIdentifier/TileTBID.h"
#include "TileEvent/TileCellContainer.h"

//New EDM
#include "xAODForward/MBTSModuleAuxContainer.h"

// Local include(s):
#include "MBTSModuleCnvTool.h"


namespace xAODMaker {

  MBTSModuleCnvTool::MBTSModuleCnvTool(const std::string& type, 
				  const std::string& name,
				  const IInterface* parent )
    : AthAlgTool( type, name, parent ) {

    // Declare the interface(s) provided by the tool:
    declareInterface< IMBTSModuleCnvTool >(this);
  }
  
  StatusCode MBTSModuleCnvTool::initialize() {

    ATH_MSG_DEBUG( "Initializing - Package version: " << PACKAGE_VERSION );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode MBTSModuleCnvTool::convert( const TileCellContainer* aod,
				       xAOD::MBTSModuleContainer* xaod) const
  {  
    if (!aod) { 
      ATH_MSG_WARNING("No input MBTSModule Collection passed"); 
      return StatusCode::SUCCESS; 
    } 

    // Create the xAOD objects:
    const auto end = aod->end();
    for(auto itr = aod->begin(); itr != end; ++itr ) {
      //fill it here
      xAOD::MBTSModule *xmbtsmod = new xAOD::MBTSModule();
      xaod->push_back( xmbtsmod );

      xmbtsmod->setE((*itr)->energy());
      xmbtsmod->setEta((*itr)->eta());
      xmbtsmod->setPhi((*itr)->phi());
      xmbtsmod->setTime((*itr)->time());
      //xmbtsmod->setMbtsQuality((*itr)->quality()); // original
      xmbtsmod->setMbtsQuality ( ((*itr)->provenance()<<16 ) | (*itr)->quality());  //fix
      ATH_MSG_DEBUG( "MBTS: quality=" << (*itr)->quality() << " provenance=" << (*itr)->provenance() << " mbtsQuality=" << xmbtsmod->mbtsQuality()  );
      const TileTBID*  tileid = nullptr;
      CHECK( detStore() -> retrieve(tileid) );
      Identifier id = (*itr)->ID();

      xmbtsmod->setType(tileid->type(id));
      xmbtsmod->setMbtsModule(tileid->module(id));
      xmbtsmod->setChannel(tileid->channel(id));

    } 
      // Return gracefully:
    return StatusCode::SUCCESS;
  }
}


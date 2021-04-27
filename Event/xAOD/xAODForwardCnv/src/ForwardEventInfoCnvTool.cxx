/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old EDM Includes:
#include "TileEvent/MBTSCollisionTime.h"

// New EDM
#include "xAODForward/ForwardEventInfoAuxContainer.h"

// Local include(s):
#include "ForwardEventInfoCnvTool.h"


namespace xAODMaker {

  ForwardEventInfoCnvTool::ForwardEventInfoCnvTool(const std::string& type, 
				  const std::string& name,
				  const IInterface* parent )
    : AthAlgTool( type, name, parent ) {

    // Declare the interface(s) provided by the tool:
    declareInterface< IForwardEventInfoCnvTool >(this);
  }
  
  StatusCode ForwardEventInfoCnvTool::convert( const MBTSCollisionTime* aod,
				       xAOD::ForwardEventInfoContainer* xaod) const
  {  
    if (!aod) { 
      ATH_MSG_WARNING("No input ForwardEventInfo Collection passed"); 
      return StatusCode::SUCCESS; 
    } 

    // Create the xAOD objects:
    xAOD::ForwardEventInfo *xfwinfo = new xAOD::ForwardEventInfo();
    xaod->push_back( xfwinfo );

    xfwinfo->setTimeDiff(aod->timeA() - aod->timeC());
    xfwinfo->setTimeA(aod->timeA());
    xfwinfo->setTimeC(aod->timeC());
    xfwinfo->setCountA(aod->ncellA());
    xfwinfo->setCountC(aod->ncellC());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }
}


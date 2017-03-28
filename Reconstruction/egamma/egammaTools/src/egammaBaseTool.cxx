/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaBaseTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/IToolSvc.h"
#include "egammaRecEvent/egammaRec.h"

// INCLUDE Header Files:

// ====================================================================================
egammaBaseTool::egammaBaseTool(const std::string& type,
			       const std::string& name,
			       const IInterface* parent):
  AthAlgTool(type, name, parent)
{ 
  declareInterface<IegammaBaseTool>(this);
}

// ====================================================================================
egammaBaseTool::~egammaBaseTool() 
{ 
}

// ====================================================================================
StatusCode egammaBaseTool::initialize() 
{  

  return StatusCode::SUCCESS;

}


// ====================================================================================
StatusCode egammaBaseTool::execute(xAOD::Egamma* eg)
{
  //
  // standard execute method
  //
  if(eg)   return StatusCode::SUCCESS;

  return StatusCode::SUCCESS;

}

// ====================================================================================
StatusCode egammaBaseTool::contExecute(xAOD::ElectronContainer* /*electronContainer*/, 
				       xAOD::PhotonContainer* /*photonContainer*/)
{
  //
  // execute method for photon post processing
  //

  return StatusCode::SUCCESS;

}

// =========================================================================================
StatusCode egammaBaseTool::finalize()
{
  
  return StatusCode::SUCCESS;

}


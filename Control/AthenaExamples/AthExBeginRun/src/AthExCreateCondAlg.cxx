/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Package includes
#include "AthExCreateCondAlg.h"

// Gaudi inludes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "AthenaKernel/IOVTime.h"
#include "StoreGate/StoreGate.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

#include "IOVDbTestConditions/IOVDbTestAmdbCorrection.h"


AthExCreateCondAlg::AthExCreateCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("IovRuns", m_iovRuns, "list of IOV run numbers");
  declareProperty("Key", m_key = "/AthExBeginRun/AthExSimpleCond", "StoreGate key");
  declareProperty("Tag", m_tag = "COOL-TEST-1", "COOL tag");
}


StatusCode AthExCreateCondAlg::initialize() {

  ATH_MSG_INFO ( "initialize()");

  return StatusCode::SUCCESS;
}


/// Create conditions objects
StatusCode AthExCreateCondAlg::execute()
{
  if ( createCondObjects().isFailure() ) {
    ATH_MSG_ERROR ("Could not create conditions objects");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


StatusCode AthExCreateCondAlg::finalize()
{
  ATH_MSG_INFO ("finalize()");

  return StatusCode::SUCCESS;
}


/**
 * @brief Create conditions objects
 *
 * Creates a set of IOVDbTestAmdbCorrection objects and assigns run-based IOVs.
 * Every object will have slightly different values for 'rotation'.
 *
 */
StatusCode AthExCreateCondAlg::createCondObjects()
{
  StatusCode sc;
  
  ServiceHandle<IIOVRegistrationSvc> regSvc("IOVRegistrationSvc", name());
  if ( regSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL ("Failed to retrieve IOVRegistrationSvc");
    return StatusCode::FAILURE;
  }

  ToolHandle<IAthenaOutputStreamTool> streamTool("AthenaPoolOutputStreamTool/AthExCondStream");  
  if (streamTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ("Unable to find AthenaOutputStreamTool");
    return StatusCode::FAILURE;
  }  

  // ----------------------------------------------------------------------
  // Create conditions objects
  // ----------------------------------------------------------------------
  float f = 0;
  for (unsigned int i=0; i<m_iovRuns.value().size(); i++) {
    
    IOVDbTestAmdbCorrection* amdbCorr = new IOVDbTestAmdbCorrection;
    HepGeom::Point3D<double> trans(1.0, 2.0, 3.0);
    HepGeom::Point3D<double> rot(f, f, f);
    f += 0.1;
    amdbCorr->set(trans, rot, "amdb correction");

    ATH_MSG_INFO 
      ("Creating conditions object IOVDbTestAmdbCorrection: "
       << "trans = (" << trans.x() << "," << trans.y() << "," << trans.z()
       << ") rot = (" <<  rot.x() << "," << rot.y() << "," << rot.z()
       << ") for run " << m_iovRuns.value()[i] << " and higher");
      
    IAthenaOutputStreamTool::TypeKeyPairs typeKeys;
    typeKeys.push_back(IAthenaOutputStreamTool::TypeKeyPair("IOVDbTestAmdbCorrection", m_key.value()));
    sc = detStore()->record(amdbCorr, m_key.value());
    if (sc.isFailure())
      ATH_MSG_FATAL ("Cannot record condition in StoreGate");
  
    // ----------------------------------------------------------------------
    // Write conditions object to POOL file
    // ----------------------------------------------------------------------
    if (streamTool->connectOutput().isFailure()) {
      ATH_MSG_FATAL ("Could not connect stream to output");
      return( StatusCode::FAILURE);
    }
    
    if (streamTool->streamObjects(typeKeys).isFailure()) {
      ATH_MSG_FATAL ("Could not stream condition objects");
      return StatusCode::FAILURE;
    }

    if (streamTool->commitOutput().isFailure()) {
      ATH_MSG_FATAL ("Could not commit output stream");
      return StatusCode::FAILURE;
    }
    
    // ----------------------------------------------------------------------
    // Register object with IOV DB
    // ----------------------------------------------------------------------
    sc = regSvc->registerIOV("IOVDbTestAmdbCorrection", m_tag.value(),
                             m_iovRuns.value()[i], IOVTime::MAXRUN,
                             IOVTime::MINEVENT, IOVTime::MAXEVENT);
    if (sc.isFailure()) {
      ATH_MSG_FATAL ("Could not register object with IOVDb");
      return StatusCode::FAILURE;
    }
    
    detStore()->clearStore().ignore();
  }

  
  return StatusCode::SUCCESS;
}


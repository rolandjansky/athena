///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SlimmerAlg.cxx 
// Implementation file for class SlimmerAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExThinning includes
#include "AthExThinning/SlimmerAlg.h"
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExFatObject.h"
#include "AthExThinning/AthExFatObjectSlimmer.h"

// STL includes

// datamodel
#include "AthLinks/ElementLink.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/IThinningSvc.h"

// storegate
#include "StoreGate/StoreGateSvc.h"

namespace AthExThinning {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
SlimmerAlg::SlimmerAlg (const std::string& name, 
			ISvcLocator* pSvcLocator) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_thinSvc ("ThinningSvc", name)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty ("ThinningSvc",
		   m_thinSvc = IThinningSvc_t("ThinningSvc", name),
		   "handle to the thinning service instance");

  declareProperty ("Particles",
		   m_particlesName = "AthExParticles",
		   "input storegate location of particles");

  declareProperty ("FatObject",
		   m_fatObjectName = "AthExFatObject",
		   "output storegate location of fat objects");
}

// Destructor
///////////////
SlimmerAlg::~SlimmerAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode SlimmerAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  if (!m_thinSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR ("could not retrieve thinning svc [" 
		   << m_thinSvc.typeAndName() << "] !");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ("input particles [" << m_particlesName << "]");
  ATH_MSG_INFO ("slimming fat object at [" << m_fatObjectName << "]");

  return StatusCode::SUCCESS;
}

StatusCode SlimmerAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode SlimmerAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const AthExParticles* particles = 0;
  if (!evtStore()->retrieve (particles, m_particlesName).isSuccess()) {
    ATH_MSG_ERROR ("could not retrieve particles at [" 
		   << m_particlesName << "] !");
    return StatusCode::FAILURE;
  }

  typedef AthExFatObject::AthExParticleLink_t AthExParticleLink_t;
  AthExFatObject *fatobj = new AthExFatObject (AthExParticleLink_t(*particles,
								   0));
  if (!evtStore()->record (fatobj, m_fatObjectName).isSuccess()) {
    ATH_MSG_ERROR ("could not record fat object at ["
		   << m_fatObjectName << "] !");
    delete fatobj; fatobj = 0;
    return StatusCode::FAILURE;
  }

  if (!evtStore()->setConst (fatobj).isSuccess()) {
    ATH_MSG_INFO ("could not set fat object at [" << m_fatObjectName << "] to"
		  " const access");
  }

  // now register a slimmer with that fat object
  AthExFatObjectSlimmer *slimmer = new AthExFatObjectSlimmer (fatobj, this);
  if (!m_thinSvc->register_slimmer (slimmer).isSuccess()) {
    ATH_MSG_ERROR ("problem registering slimmer for fat object at ["
		   << m_fatObjectName << "] !");
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthExThinning

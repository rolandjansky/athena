/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeTruthStrategy.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "CylinderVolumeTruthStrategy.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"

// HepMC includes
#include "HepMC/SimpleVector.h"

/** Constructor **/
ISF::CylinderVolumeTruthStrategy::CylinderVolumeTruthStrategy(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_ri(0.),
  m_ro(1148.)
{
    declareInterface<ISF::ITruthStrategy>(this);

    // cylinder dimensions
    declareProperty("InnerRadius"        , m_ri=0.        );
    declareProperty("OuterRadius"        , m_ro=1148.     );
}

/** Destructor **/
ISF::CylinderVolumeTruthStrategy::~CylinderVolumeTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::CylinderVolumeTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    ATH_MSG_VERBOSE("Initialize successful");
    return StatusCode::SUCCESS;
}

StatusCode  ISF::CylinderVolumeTruthStrategy::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");

    ATH_MSG_VERBOSE("Finalize successful");
    return StatusCode::SUCCESS;
}

bool ISF::CylinderVolumeTruthStrategy::pass( ITruthIncident& ti) const
{
  // the current truth incident radius
  double r = ti.position().rho();

  // is the current radius on the surface?
  bool onSurf = (r>m_ri) && (r<m_ro);

  return onSurf;
}

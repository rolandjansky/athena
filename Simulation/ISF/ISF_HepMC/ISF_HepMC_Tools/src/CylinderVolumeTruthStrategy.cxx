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
#include "AtlasHepMC/SimpleVector.h"

/** Constructor **/
ISF::CylinderVolumeTruthStrategy::CylinderVolumeTruthStrategy(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_ri(0.),
  m_ro(1148.)
{
    // cylinder dimensions
    declareProperty("InnerRadius"        , m_ri=0.        );
    declareProperty("OuterRadius"        , m_ro=1148.     );
    declareProperty("Regions"            , m_regionListProperty );
}

/** Destructor **/
ISF::CylinderVolumeTruthStrategy::~CylinderVolumeTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::CylinderVolumeTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    for(auto region : m_regionListProperty.value()) {
      if(region < AtlasDetDescr::fFirstAtlasRegion || region >= AtlasDetDescr::fNumAtlasRegions) {
        ATH_MSG_ERROR("Unknown Region (" << region << ") specified. Please check your configuration.");
        return StatusCode::FAILURE;
      }
    }

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
  auto t_pos=ti.position();
  double r = std::sqrt(t_pos.x()*t_pos.x()+t_pos.y()*t_pos.y()+t_pos.z()*t_pos.z());

  // is the current radius on the surface?
  bool onSurf = (r>m_ri) && (r<m_ro);

  return onSurf;
}

bool ISF::CylinderVolumeTruthStrategy::appliesToRegion(unsigned short geoID) const
{
  return std::find( m_regionListProperty.begin(),
                    m_regionListProperty.end(),
                    geoID ) != m_regionListProperty.end();
}

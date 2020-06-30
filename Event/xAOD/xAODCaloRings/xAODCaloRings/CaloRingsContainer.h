// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_CALORINGSCONTAINER_H
#define XAODCALORINGS_CALORINGSCONTAINER_H

// Local include(s):
#include "xAODCaloRings/CaloRings.h"
#include "xAODCaloRings/versions/CaloRingsContainer_v1.h"

// EDM include(s).
#include "AthLinks/ElementLink.h"

// System include(s).
#include <vector>

namespace xAOD {

/// Definition of the current "CaloRings container version"
typedef CaloRingsContainer_v1 CaloRingsContainer;

/// ElementLink type pointing at such objects.
typedef std::vector< ElementLink< CaloRingsContainer > > CaloRingsLinks;

} // namespace xAOD

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloRingsContainer , 1146041125 , 1 )

#endif // XAODCALORINGS_CALORINGSCONTAINER_H

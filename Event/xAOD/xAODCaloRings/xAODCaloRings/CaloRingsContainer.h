/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsContainer.h 707323 2015-11-12 02:45:01Z wsfreund $ 
#ifndef XAODCALORINGS_CALORINGSCONTAINER_H
#define XAODCALORINGS_CALORINGSCONTAINER_H

// STL include(s):
#include <vector>

// Core include(s):
#include "AthLinks/ElementLink.h"
//#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODCaloRings/CaloRings.h"
#include "xAODCaloRings/versions/CaloRingsContainer_v1.h"

namespace xAOD {
/// Definition of the current "CaloRings container version"
typedef CaloRingsContainer_v1 CaloRingsContainer;
/// Declare element links vector
typedef std::vector< ElementLink< CaloRingsContainer > >
  CaloRingsLinks;
/// The CaloRings element links decorator type:
typedef SG::AuxElement::Decorator< xAOD::CaloRingsLinks > 
  caloRingsDeco_t;
/// The CaloRings element links reader type:
typedef SG::AuxElement::ConstAccessor< xAOD::CaloRingsLinks > 
  caloRingsReader_t;
} // namespace xAOD

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloRingsContainer , 1146041125 , 1 )

#endif // XAODCALORINGS_CALORINGSCONTAINER_H

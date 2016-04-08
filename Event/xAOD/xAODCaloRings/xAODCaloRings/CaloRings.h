/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRings.h 659538 2015-04-08 17:29:18Z wsfreund $
#ifndef XAODCALORINGS_CALORINGS_H
#define XAODCALORINGS_CALORINGS_H

// Local include(s):
#include "xAODCaloRings/versions/CaloRings_v1.h"

// It is expected that the last version of CaloRings also contains the last
// RingSet version. To avoid any mistake, let's include it here.
#include "xAODCaloRings/RingSet.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
/// Definition of the current "CaloRings version"
typedef CaloRings_v1  CaloRings;
} // namespace xAOD

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloRings , 156466847 , 1 )

#endif // XAODCALORINGS_CALORINGS_H


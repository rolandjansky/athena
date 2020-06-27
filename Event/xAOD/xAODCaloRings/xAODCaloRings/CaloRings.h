// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_CALORINGS_H
#define XAODCALORINGS_CALORINGS_H

// Local include(s):
#include "xAODCaloRings/versions/CaloRings_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

/// Definition of the current "CaloRings version"
typedef CaloRings_v1  CaloRings;

} // namespace xAOD

// CLID declaration for the type.
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloRings , 156466847 , 1 )

#endif // XAODCALORINGS_CALORINGS_H


// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_MUONACCESSORS_V1_H
#define XAODMUON_MUONACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODMuon/versions/Muon_v1.h"

namespace xAOD {

   // Helper functions for managing Muon Accessor objects

   /// This function holds on to Accessor objects that can be used by each
   /// Muon_v1 object at runtime to get/set parameter values on themselves.
   ///
   template <class T>
   const SG::AuxElement::Accessor< T >*
   parameterAccessorV1( Muon_v1::ParamDef type );
   
} // namespace xAOD

#include "MuonAccessors_v1.icc"
#endif // XAODMUON_MUONACCESSORS_V1_H

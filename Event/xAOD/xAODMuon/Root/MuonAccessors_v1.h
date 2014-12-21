// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonAccessors_v1.h 636019 2014-12-15 15:55:20Z emoyse $
#ifndef XAODMUON_MUONACCESSORS_V1_H
#define XAODMUON_MUONACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODMuon/Muon.h"

namespace xAOD {

   // Helper functions for managing Muon Accessor objects

   /// This function holds on to Accessor objects that can be used by each
   /// Muon_v1 object at runtime to get/set parameter values on themselves.
   ///
   template <class T>
   SG::AuxElement::Accessor< T >*
   parameterAccessorV1( Muon::ParamDef type );
   
} // namespace xAOD

#include "MuonAccessors_v1.icc"
#endif // XAODMUON_MUONACCESSORS_V1_H

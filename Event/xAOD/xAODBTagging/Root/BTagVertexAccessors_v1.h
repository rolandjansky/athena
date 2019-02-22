// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_BTAGGINGACCESSORS_V1_H
#define XAOD_BTAGGINGACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
// Local include(s):
//#include "xAODBTagging/BTaggingEnums.h"
#include "xAODBTagging/BTagVertex.h"

#include <string>

namespace xAOD {
  
  /// Helper function for managing BTagging Accessor objects
  ///
  /// This function holds on to Accessor objects that can be used by each
  /// BTagging_v1 object at runtime to get/set detail values on themselves.

/*  SG::AuxElement::Accessor< int >* taggerInfoAccessorV1int(BTagInfo info);
  SG::AuxElement::Accessor< float >* taggerInfoAccessorV1float(BTagInfo info);
  SG::AuxElement::Accessor< uint8_t >* taggerInfoAccessorV1uint8_t(BTagInfo info);
  SG::AuxElement::Accessor< std::string >* taggerInfoAccessorV1string(BTagInfo info);*/
  
} // namespace xAOD

#endif // XAOD_BTAGGINGACCESSORS_V1_H

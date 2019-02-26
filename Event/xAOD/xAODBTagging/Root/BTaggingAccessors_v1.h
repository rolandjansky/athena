// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAccessors_v1.h 602916 2014-06-20 15:08:25Z vacavant $
#ifndef XAOD_BTAGGINGACCESSORS_V1_H
#define XAOD_BTAGGINGACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
// Local include(s):
#include "xAODBTagging/BTaggingEnums.h"
#include "xAODBTagging/BTagging.h"

#include <string>
#include <vector>

namespace xAOD {
  
  /// Helper function for managing BTagging Accessor objects
  ///
  /// This function holds on to Accessor objects that can be used by each
  /// BTagging_v1 object at runtime to get/set detail values on themselves.

  const SG::AuxElement::Accessor< int >* taggerInfoAccessorV1int(BTagInfo info);
  const SG::AuxElement::Accessor< float >* taggerInfoAccessorV1float(BTagInfo info);
  const SG::AuxElement::Accessor< uint8_t >* taggerInfoAccessorV1uint8_t(BTagInfo info);
  const SG::AuxElement::Accessor< std::string >* taggerInfoAccessorV1string(BTagInfo info);
  const SG::AuxElement::Accessor< std::vector<int> >* taggerInfoAccessorV1vectorOfInts(BTagInfo info);
  const SG::AuxElement::Accessor< std::vector<float> >* taggerInfoAccessorV1vectorOfFloats(BTagInfo info);
  const SG::AuxElement::Accessor< std::vector<bool> >* taggerInfoAccessorV1vectorOfBools(BTagInfo info);
  
} // namespace xAOD

#endif // XAOD_BTAGGINGACCESSORS_V1_H

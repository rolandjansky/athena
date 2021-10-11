// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODBTAGGING_BTAGGINGCONTAINERFWD_H
#define XAODBTAGGING_BTAGGINGCONTAINERFWD_H

#include "AthContainers/DataVector.h"
#include "xAODBTagging/BTaggingFwd.h"

namespace xAOD {
   /// Definition of the current container version
  typedef DataVector<BTagging> BTaggingContainer;
}


#endif // XAODBTAGGING_BTAGGINGCONTAINERFWD_H

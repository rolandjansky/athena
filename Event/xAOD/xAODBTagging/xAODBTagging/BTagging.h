// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagging.h 744481 2016-05-03 13:36:43Z krasznaa $
#ifndef XAODBTAGGING_BTAGGING_H
#define XAODBTAGGING_BTAGGING_H

// Local include(s):
#include "xAODBTagging/versions/BTagging_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Definition of the current "BTagging version"
   typedef BTagging_v1 BTagging;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BTagging, 207121524, 1 )

#endif // XAODBTAGGING_BTAGGING_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagging.h 569109 2013-11-07 14:59:42Z krasznaa $
#ifndef XAODBTAGGING_BTAGVERTEX_H
#define XAODBTAGGING_BTAGVERTEX_H

// Local include(s):
#include "xAODBTagging/versions/BTagVertex_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Definition of the current "BTagging version"
   typedef BTagVertex_v1 BTagVertex;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BTagVertex, 96886315, 1 )

#endif // XAODBTAGGING_BTAGGING_H

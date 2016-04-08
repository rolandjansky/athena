// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_FORWARDEVENTINFOS_H
#define XAODFORWARD_FORWARDEVENTINFOS_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/versions/ForwardEventInfo_v1.h"

namespace xAOD {
   typedef ForwardEventInfo_v1 ForwardEventInfo;
}

// Declare a CLID for the type:
CLASS_DEF( xAOD::ForwardEventInfo, 116288219, 1 )

#endif // XAODFORWARD_ALFADATA_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTINFO_EVENTAUXINFO_H
#define XAODEVENTINFO_EVENTAUXINFO_H

// Local include(s):
#include "xAODEventInfo/versions/EventAuxInfo_v2.h"

namespace xAOD {
   /// Definition of the latest event auxiliary info version
   typedef EventAuxInfo_v2 EventAuxInfo;
}

// Declare a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EventAuxInfo, 38853911, 1 )

#endif // XAODEVENTINFO_EVENAUXTINFO_H

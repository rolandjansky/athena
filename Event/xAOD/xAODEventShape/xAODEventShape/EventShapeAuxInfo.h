// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventShapeAuxInfo.h 629489 2014-11-19 14:37:55Z krasznaa $
#ifndef XAODEVENTSHAPE_EVENTSHAPEAUXINFO_H
#define XAODEVENTSHAPE_EVENTSHAPEAUXINFO_H

// Local include(s):
#include "xAODEventShape/versions/EventShapeAuxInfo_v1.h"

namespace xAOD {
   /// Definition of the latest event shape auxiliary info version
   typedef EventShapeAuxInfo_v1 EventShapeAuxInfo;
}

// Declare a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EventShapeAuxInfo, 129946083, 1 )

#endif // XAODEVENTSHAPE_EVENTSHAPEAUXINFO_H

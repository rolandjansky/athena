// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectronContainer.h 631633 2014-11-27 17:47:44Z gwatts $
#ifndef XAODTRIGEGAMMA_TRIGELECTRONCONTAINER_H
#define XAODTRIGEGAMMA_TRIGELECTRONCONTAINER_H

// Local include(s):
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/versions/TrigElectronContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the container
   typedef TrigElectronContainer_v1 TrigElectronContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigElectronContainer , 1119645201 , 1 )

#endif // not XAODTRIGEGAMMA_TRIGELECTRONCONTAINER_H

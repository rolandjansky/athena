// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronContainer.h 615077 2014-09-04 13:42:41Z schaffer $
#ifndef XAODEGAMMA_ELECTRONCONTAINER_H
#define XAODEGAMMA_ELECTRONCONTAINER_H

// Local include(s):
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/versions/ElectronContainer_v1.h"

namespace xAOD {
   /// Definition of the current "electron container version"
   typedef ElectronContainer_v1 ElectronContainer;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::ElectronContainer , 1087532415 , 1 )
#endif // XAOD_STANDALONE

#endif // XAODEGAMMA_ELECTRONCONTAINER_H

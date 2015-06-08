// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronContainer.h 652975 2015-03-10 14:14:13Z christos $
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
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ElectronContainer , 1087532415 , 1 )

#endif // XAODEGAMMA_ELECTRONCONTAINER_H

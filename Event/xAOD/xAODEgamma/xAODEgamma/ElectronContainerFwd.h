// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEGAMMA_ELECTRONCONTAINERFWD_H
#define XAODEGAMMA_ELECTRONCONTAINERFWD_H

#include "AthContainers/DataVector.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/EgammaContainerFwd.h"

DATAVECTOR_BASE_FWD( xAOD::Electron, xAOD::Egamma );
namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Electron> ElectronContainer;
}


#endif // XAODEGAMMA_ELECTRONCONTAINERFWD_H

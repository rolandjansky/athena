// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEGAMMA_PHOTONCONTAINERFWD_H
#define XAODEGAMMA_PHOTONCONTAINERFWD_H

#include "AthContainers/DataVector.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaContainerFwd.h"

DATAVECTOR_BASE_FWD( xAOD::Photon, xAOD::Egamma );
namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Photon > PhotonContainer;
}


#endif // XAODEGAMMA_PHOTONCONTAINERFWD_H

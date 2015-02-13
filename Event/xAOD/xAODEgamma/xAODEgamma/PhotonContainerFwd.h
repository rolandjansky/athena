// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonContainerFwd.h 644948 2015-02-07 01:33:06Z christos $
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

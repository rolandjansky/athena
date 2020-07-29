// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaContainerFwd.h 644948 2015-02-07 01:33:06Z christos $
#ifndef XAODEGAMMA_EGAMMACONTAINERFWD_H
#define XAODEGAMMA_EGAMMACONTAINERFWD_H

#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/EgammaFwd.h"

DATAVECTOR_BASE_FWD( xAOD::Egamma, xAOD::IParticle);
namespace xAOD {
   /// Definition of the current "egamma container version"
  typedef DataVector<Egamma> EgammaContainer;
}

#endif // XAODEGAMMA_EGAMMACONTAINERFWD_H

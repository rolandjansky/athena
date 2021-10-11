// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETCONTAINERFWD_H
#define XAODJET_JETCONTAINERFWD_H

#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODJet/JetFwd.h"

DATAVECTOR_BASE_FWD(xAOD::Jet, xAOD::IParticle);
namespace xAOD {
   /// Definition of the current container version
  typedef DataVector<Jet> JetContainer;
}

#endif // XAODJET_JETCONTAINERFWD_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetAuxContainer.h 569516 2013-11-09 12:39:21Z janus $
#ifndef XAODTAU_TAUJETAUXCONTAINER_H
#define XAODTAU_TAUJETAUXCONTAINER_H

// Local include(s):
#include "xAODTau/versions/TauJetAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current taujet auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TauJetContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef TauJetAuxContainer_v1 TauJetAuxContainer;
}

#endif // XAODTAU_TAUJETAUXCONTAINER_H

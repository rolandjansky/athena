// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetAuxContainer.h 631748 2014-11-28 09:58:13Z janus $
#ifndef XAODTAU_TAUJETAUXCONTAINER_H
#define XAODTAU_TAUJETAUXCONTAINER_H

// Local include(s):
#include "xAODTau/versions/TauJetAuxContainer_v2.h"

namespace xAOD {
   /// Definition of the current taujet auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TauJetContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef TauJetAuxContainer_v2 TauJetAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TauJetAuxContainer, 1307768555 , 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTAU_TAUJETAUXCONTAINER_H

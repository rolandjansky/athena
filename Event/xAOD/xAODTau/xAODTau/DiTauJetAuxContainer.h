// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauJetAuxContainer.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDiTAU_DiTAUJETAUXCONTAINER_H
#define XAODDiTAU_DiTAUJETAUXCONTAINER_H

// Local include(s):
#include "xAODTau/versions/DiTauJetAuxContainer_v1.h"

namespace xAOD {
   typedef DiTauJetAuxContainer_v1 DiTauJetAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::DiTauJetAuxContainer , 1305731819 , 1 )
#endif // not XAOD_STANDALONE

#endif // XAODDITAU_DITAUJETAUXCONTAINER_H
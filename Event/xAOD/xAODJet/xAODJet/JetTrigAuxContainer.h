// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETTRIGAUXCONTAINER_H
#define XAODJET_JETTRIGAUXCONTAINER_H
#ifndef SIMULATIONBASE
// Local include(s):
#include "xAODJet/versions/JetTrigAuxContainer_v2.h"

namespace xAOD {
   /// Definition of the current jet trigger auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::JetContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef JetTrigAuxContainer_v2 JetTrigAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetTrigAuxContainer, 1285854532, 1 )

#endif //SIMULATIONBASE
#endif // XAODJET_JETAUXCONTAINER_H

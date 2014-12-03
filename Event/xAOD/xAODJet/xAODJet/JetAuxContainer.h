// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetAuxContainer.h 631914 2014-11-28 16:08:50Z krasznaa $
#ifndef XAODJET_JETAUXCONTAINER_H
#define XAODJET_JETAUXCONTAINER_H

// Local include(s):
#include "xAODJet/versions/JetAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current jet auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::JetContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef JetAuxContainer_v1 JetAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetAuxContainer, 1294631886, 1 )

#endif // XAODJET_JETAUXCONTAINER_H

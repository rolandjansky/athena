// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetContainer.h 631914 2014-11-28 16:08:50Z krasznaa $
#ifndef XAODJET_JETCONTAINER_H
#define XAODJET_JETCONTAINER_H

// Local include(s):
#include "xAODJet/Jet.h"
#include "xAODJet/versions/JetContainer_v1.h"

namespace xAOD {
   /// Definition of the current "jet container version"
   typedef JetContainer_v1 JetContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetContainer, 1244316195, 1 )

#endif // XAODJET_JETCONTAINER_H

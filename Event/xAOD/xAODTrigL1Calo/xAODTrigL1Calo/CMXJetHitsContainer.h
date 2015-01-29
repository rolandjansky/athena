// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHitsContainer.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_CMXJETHITSCONTAINER_H
#define XAODTRIGL1CALO_CMXJETHITSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXJetHits.h"
#include "xAODTrigL1Calo/versions/CMXJetHitsContainer_v2.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMMCPHits class
  typedef CMXJetHitsContainer_v2 CMXJetHitsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXJetHitsContainer , 1280264957 , 1 )
#endif 


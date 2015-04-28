// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHitsContainer.h 652810 2015-03-09 22:33:00Z morrisj $
#ifndef XAODTRIGL1CALO_CMXJETHITSCONTAINER_H
#define XAODTRIGL1CALO_CMXJETHITSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXJetHits.h"
#include "xAODTrigL1Calo/versions/CMXJetHitsContainer_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMXJetHits class
  typedef CMXJetHitsContainer_v1 CMXJetHitsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXJetHitsContainer , 1280264957 , 1 )
#endif 


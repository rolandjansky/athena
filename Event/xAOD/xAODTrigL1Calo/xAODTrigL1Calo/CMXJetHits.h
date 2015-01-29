// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHits.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_CMXJETHITS_H
#define XAODTRIGL1CALO_CMXJETHITS_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetHits_v2.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMMCPHits class
  typedef CMXJetHits_v2 CMXJetHits;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXJetHits , 162312467 , 1 )
#endif 


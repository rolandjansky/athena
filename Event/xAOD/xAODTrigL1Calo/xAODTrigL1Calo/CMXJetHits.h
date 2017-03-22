// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHits.h 791220 2017-01-01 04:52:41Z ssnyder $
#ifndef XAODTRIGL1CALO_CMXJETHITS_H
#define XAODTRIGL1CALO_CMXJETHITS_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetHits_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMXJetHits class
  typedef CMXJetHits_v1 CMXJetHits;

  typedef std::map<int, const CMXJetHits*> CMXJetHitsMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXJetHits , 162312467 , 1 )
#endif 


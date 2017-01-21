// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetTob.h 791220 2017-01-01 04:52:41Z ssnyder $
#ifndef XAODTRIGL1CALO_CMXJETTOB_H
#define XAODTRIGL1CALO_CMXJETTOB_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetTob_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMXJetTob class
  typedef CMXJetTob_v1 CMXJetTob;

  typedef std::map<int, const CMXJetTob*> CMXJetTobMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXJetTob , 260064743 , 1 )
#endif 


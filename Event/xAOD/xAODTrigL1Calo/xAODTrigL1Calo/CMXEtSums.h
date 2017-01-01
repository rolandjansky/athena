// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSums.h 791220 2017-01-01 04:52:41Z ssnyder $
#ifndef XAODTRIGL1CALO_CMXETSUMS_H
#define XAODTRIGL1CALO_CMXETSUMS_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSums_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMXEtSums class
  typedef CMXEtSums_v1 CMXEtSums;

  typedef std::map<int, const CMXEtSums*> CMXEtSumsMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXEtSums , 108293776 , 1 )
#endif 


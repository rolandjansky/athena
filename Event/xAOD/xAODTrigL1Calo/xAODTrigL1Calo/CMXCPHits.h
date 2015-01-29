// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPHits.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_CMXCPHITS_H
#define XAODTRIGL1CALO_CMXCPHITS_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPHits_v2.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMMCPHits class
  typedef CMXCPHits_v2 CMXCPHits;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXCPHits , 256949232 , 1 )
#endif 


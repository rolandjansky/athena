// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsContainer.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_CMXETSUMSCONTAINER_H
#define XAODTRIGL1CALO_CMXETSUMSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXEtSums.h"
#include "xAODTrigL1Calo/versions/CMXEtSumsContainer_v2.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMMCPHits class
  typedef CMXEtSumsContainer_v2 CMXEtSumsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXEtSumsContainer , 1179694440 , 1 )
#endif 


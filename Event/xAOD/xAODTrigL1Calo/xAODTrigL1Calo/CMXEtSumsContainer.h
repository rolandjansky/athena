// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsContainer.h 652810 2015-03-09 22:33:00Z morrisj $
#ifndef XAODTRIGL1CALO_CMXETSUMSCONTAINER_H
#define XAODTRIGL1CALO_CMXETSUMSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXEtSums.h"
#include "xAODTrigL1Calo/versions/CMXEtSumsContainer_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMXEtSums class
  typedef CMXEtSumsContainer_v1 CMXEtSumsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXEtSumsContainer , 1179694440 , 1 )
#endif 


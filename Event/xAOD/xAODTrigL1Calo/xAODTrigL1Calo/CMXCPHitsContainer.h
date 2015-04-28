// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPHitsContainer.h 652809 2015-03-09 22:01:53Z morrisj $
#ifndef XAODTRIGL1CALO_CMXCPHITSCONTAINER_H
#define XAODTRIGL1CALO_CMXCPHITSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXCPHits.h"
#include "xAODTrigL1Calo/versions/CMXCPHitsContainer_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMXCPHits class
  typedef CMXCPHitsContainer_v1 CMXCPHitsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXCPHitsContainer , 1303070088 , 1 )
#endif 


// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTobContainer.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_CMXCPTOBCONTAINER_H
#define XAODTRIGL1CALO_CMXCPTOBCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXCPTob.h"
#include "xAODTrigL1Calo/versions/CMXCPTobContainer_v2.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMMCPHits class
  typedef CMXCPTobContainer_v2 CMXCPTobContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXCPTobContainer , 1246252758 , 1 )
#endif 


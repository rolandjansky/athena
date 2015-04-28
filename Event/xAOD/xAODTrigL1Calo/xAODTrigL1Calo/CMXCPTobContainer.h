// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTobContainer.h 652810 2015-03-09 22:33:00Z morrisj $
#ifndef XAODTRIGL1CALO_CMXCPTOBCONTAINER_H
#define XAODTRIGL1CALO_CMXCPTOBCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXCPTob.h"
#include "xAODTrigL1Calo/versions/CMXCPTobContainer_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMXCPTob class
  typedef CMXCPTobContainer_v1 CMXCPTobContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXCPTobContainer , 1246252758 , 1 )
#endif 


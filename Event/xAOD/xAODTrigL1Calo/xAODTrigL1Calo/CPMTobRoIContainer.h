// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoIContainer.h 652818 2015-03-09 22:54:24Z morrisj $
#ifndef XAODTRIGL1CALO_CPMTOBROICONTAINER_H
#define XAODTRIGL1CALO_CPMTOBROICONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CPMTobRoI.h"
#include "xAODTrigL1Calo/versions/CPMTobRoIContainer_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CPMTobRoI class
  typedef CPMTobRoIContainer_v1 CPMTobRoIContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMTobRoIContainer , 1237399798 , 1 )
#endif 


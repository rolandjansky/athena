// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoIContainer.h 652818 2015-03-09 22:54:24Z morrisj $
#ifndef XAODTRIGL1CALO_JEMTOBROICONTAINER_H
#define XAODTRIGL1CALO_JEMTOBROICONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/JEMTobRoI.h"
#include "xAODTrigL1Calo/versions/JEMTobRoIContainer_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the JEMTobRoI class
  typedef JEMTobRoIContainer_v1 JEMTobRoIContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JEMTobRoIContainer , 1207570804 , 1 )
#endif 


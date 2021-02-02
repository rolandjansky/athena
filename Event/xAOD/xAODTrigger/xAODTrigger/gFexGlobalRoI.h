// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODGFEXGLOBALROI_H
#define XAODGFEXGLOBALROI_H

// Local include(s):
#include "xAODTrigger/versions/gFexGlobalRoI_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the eFexEMRoI class
  typedef gFexGlobalRoI_v1 gFexGlobalRoI;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::gFexGlobalRoI , 32073859 , 1 )
#endif 

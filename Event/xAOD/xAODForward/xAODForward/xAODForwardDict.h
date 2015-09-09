// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODForwardDict.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_XAODFORWARDDICT_H
#define XAODFORWARD_XAODFORWARDDICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODForward/ALFADataContainer.h"
#include "xAODForward/ALFADataAuxContainer.h"
#include "xAODForward/versions/ALFADataContainer_v1.h"
#include "xAODForward/versions/ALFADataAuxContainer_v1.h"
#include "xAODForward/versions/ALFAData_v1.h"

namespace{
   struct GCCXML_DUMMY_INSTANTIATION_ALFA_XAOD {

      xAOD::ALFADataContainer_v1 alfa_c1;

      DataLink<xAOD::ALFADataContainer_v1> alfa_l1;
      std::vector<DataLink<xAOD::ALFADataContainer_v1> > alfa_l2;

      ElementLink<xAOD::ALFADataContainer_v1> alfa_l3;
      std::vector<ElementLink<xAOD::ALFADataContainer_v1> > alfa_l4;
      std::vector<std::vector<ElementLink<xAOD::ALFADataContainer_v1> > >
         alfa_l5;
   };
}

#endif // XAODFORWARD_XAODFORWARDDICT_H

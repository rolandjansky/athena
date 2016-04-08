// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODForwardDict.h 708124 2015-11-16 13:24:22Z steinber $
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

#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ZdcModuleAuxContainer.h"
#include "xAODForward/versions/ZdcModuleContainer_v1.h"
#include "xAODForward/versions/ZdcModuleAuxContainer_v1.h"
#include "xAODForward/versions/ZdcModule_v1.h"

#include "xAODForward/MBTSModuleContainer.h"
#include "xAODForward/MBTSModuleAuxContainer.h"
#include "xAODForward/versions/MBTSModuleContainer_v1.h"
#include "xAODForward/versions/MBTSModuleAuxContainer_v1.h"
#include "xAODForward/versions/MBTSModule_v1.h"

#include "xAODForward/ForwardEventInfoContainer.h"
#include "xAODForward/ForwardEventInfoAuxContainer.h"
#include "xAODForward/versions/ForwardEventInfoContainer_v1.h"
#include "xAODForward/versions/ForwardEventInfoAuxContainer_v1.h"
#include "xAODForward/versions/ForwardEventInfo_v1.h"
namespace{
   struct GCCXML_DUMMY_INSTANTIATION_ALFA_XAOD {

      xAOD::ALFADataContainer_v1 alfa_c1;

      DataLink<xAOD::ALFADataContainer_v1> alfa_l1;
      std::vector<DataLink<xAOD::ALFADataContainer_v1> > alfa_l2;

      ElementLink<xAOD::ALFADataContainer_v1> alfa_l3;
      std::vector<ElementLink<xAOD::ALFADataContainer_v1> > alfa_l4;
      std::vector<std::vector<ElementLink<xAOD::ALFADataContainer_v1> > >
         alfa_l5;
     //   };

     //struct GCCXML_DUMMY_INSTANTIATION_ZDC_XAOD {

      xAOD::ZdcModuleContainer_v1 zdc_c1;

        DataLink<xAOD::ZdcModuleContainer_v1> zdc_l1;
      std::vector<DataLink<xAOD::ZdcModuleContainer_v1> > zdc_l2;

      ElementLink<xAOD::ZdcModuleContainer_v1> zdc_l3;
      std::vector<ElementLink<xAOD::ZdcModuleContainer_v1> > zdc_l4;
      std::vector<std::vector<ElementLink<xAOD::ZdcModuleContainer_v1> > > zdc_l5;

     //must also instantiate the element links used by any aux containers
     ElementLink< xAOD::TriggerTowerContainer > zdc_l6;
     std::vector< ElementLink< xAOD::TriggerTowerContainer > > zdc_l7;
     std::vector< std::vector<short> > zdc_l8;

     //and for mbtsmodule
     xAOD::MBTSModuleContainer_v1 mbts_c1;

     DataLink<xAOD::MBTSModuleContainer_v1> mbts_l1;
     std::vector<DataLink<xAOD::MBTSModuleContainer_v1> > mbts_l2;

     ElementLink<xAOD::MBTSModuleContainer_v1> mbts_l3;
     std::vector<ElementLink<xAOD::MBTSModuleContainer_v1> > mbts_l4;
     std::vector<std::vector<ElementLink<xAOD::MBTSModuleContainer_v1> > > mbts_l5;
    
     xAOD::ForwardEventInfoContainer_v1 fei_c1;

     DataLink<xAOD::ForwardEventInfoContainer_v1> fei_l1;
     std::vector<DataLink<xAOD::ForwardEventInfoContainer_v1> > fei_l2;

     ElementLink<xAOD::ForwardEventInfoContainer_v1> fei_l3;
     std::vector<ElementLink<xAOD::ForwardEventInfoContainer_v1> > fei_l4;
     std::vector<std::vector<ElementLink<xAOD::ForwardEventInfoContainer_v1> > > fei_l5;
  };

}

#endif // XAODFORWARD_XAODFORWARDDICT_H

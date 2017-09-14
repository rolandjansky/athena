// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODForwardDict.h 766963 2016-08-08 20:07:09Z ggach $
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

#include "xAODForward/AFPDataContainer.h"
#include "xAODForward/AFPDataAuxContainer.h"
#include "xAODForward/versions/AFPDataContainer_v1.h"
#include "xAODForward/versions/AFPDataAuxContainer_v1.h"
#include "xAODForward/versions/AFPData_v1.h"

#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHitAuxContainer.h"
#include "xAODForward/versions/AFPToFHitContainer_v1.h"
#include "xAODForward/versions/AFPToFHitAuxContainer_v1.h"
#include "xAODForward/versions/AFPToFHit_v1.h"

#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"
#include "xAODForward/versions/AFPSiHitContainer_v1.h"
#include "xAODForward/versions/AFPSiHitAuxContainer_v1.h"
#include "xAODForward/versions/AFPSiHit_v1.h"
#include "xAODForward/versions/AFPSiHitContainer_v2.h"
#include "xAODForward/versions/AFPSiHitAuxContainer_v2.h"
#include "xAODForward/versions/AFPSiHit_v2.h"

#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"
#include "xAODForward/versions/AFPSiHitsClusterContainer_v1.h"
#include "xAODForward/versions/AFPSiHitsClusterAuxContainer_v1.h"
#include "xAODForward/versions/AFPSiHitsCluster_v1.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"
#include "xAODForward/versions/AFPTrackContainer_v1.h"
#include "xAODForward/versions/AFPTrackAuxContainer_v1.h"
#include "xAODForward/versions/AFPTrack_v1.h"
#include "xAODForward/versions/AFPTrackContainer_v2.h"
#include "xAODForward/versions/AFPTrackAuxContainer_v2.h"
#include "xAODForward/versions/AFPTrack_v2.h"

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

namespace xAOD
{
  struct GCCXML_DUMMY_INSTANTIATION_AFP_XAOD
  {
    xAOD::AFPDataContainer_v1 afp_c1;
    DataLink<xAOD::AFPDataContainer_v1> afp_l1;
    std::vector<DataLink<xAOD::AFPDataContainer_v1> > afp_l2;
    ElementLink<xAOD::AFPDataContainer_v1> afp_l3;
    std::vector<ElementLink<xAOD::AFPDataContainer_v1> > afp_l4;
    std::vector<std::vector<ElementLink<xAOD::AFPDataContainer_v1> > > afp_l5;

    xAOD::AFPToFHitContainer_v1 afpToFHit_c1;
    DataLink<xAOD::AFPToFHitContainer_v1> afpToFHit_l1;
    std::vector<DataLink<xAOD::AFPToFHitContainer_v1> > afpToFHit_l2;
    ElementLink<xAOD::AFPToFHitContainer_v1> afpToFHit_l3;
    std::vector<ElementLink<xAOD::AFPToFHitContainer_v1> > afpToFHit_l4;
    std::vector<std::vector<ElementLink<xAOD::AFPToFHitContainer_v1> > > afpToFHit_l5;
    
    xAOD::AFPSiHitContainer_v1 afpSiHit_c1;
    DataLink<xAOD::AFPSiHitContainer_v1> afpSiHit_l1;
    std::vector<DataLink<xAOD::AFPSiHitContainer_v1> > afpSiHit_l2;
    ElementLink<xAOD::AFPSiHitContainer_v1> afpSiHit_l3;
    std::vector<ElementLink<xAOD::AFPSiHitContainer_v1> > afpSiHit_l4;
    std::vector<std::vector<ElementLink<xAOD::AFPSiHitContainer_v1> > > afpSiHit_l5;

    xAOD::AFPSiHitContainer_v2 afpSiHit_c21;
    DataLink<xAOD::AFPSiHitContainer_v2> afpSiHit_l21;
    std::vector<DataLink<xAOD::AFPSiHitContainer_v2> > afpSiHit_l22;
    ElementLink<xAOD::AFPSiHitContainer_v2> afpSiHit_l23;
    std::vector<ElementLink<xAOD::AFPSiHitContainer_v2> > afpSiHit_l24;
    std::vector<std::vector<ElementLink<xAOD::AFPSiHitContainer_v2> > > afpSiHit_l25;

    xAOD::AFPSiHitsClusterContainer_v1 afpSiHitsContainer_c1;
    DataLink<xAOD::AFPSiHitsClusterContainer_v1> afpSiHitsContainer_l1;
    std::vector<DataLink<xAOD::AFPSiHitsClusterContainer_v1> > afpSiHitsContainer_l2;
    ElementLink<xAOD::AFPSiHitsClusterContainer_v1> afpSiHitsContainer_l3;
    std::vector<ElementLink<xAOD::AFPSiHitsClusterContainer_v1> > afpSiHitsContainer_l4;
    std::vector<std::vector<ElementLink<xAOD::AFPSiHitsClusterContainer_v1> > > afpSiHitsContainer_l5;


    xAOD::AFPTrackContainer_v1 afpTrack_c1;
    DataLink<xAOD::AFPTrackContainer_v1> afpTrack_l1;
    std::vector<DataLink<xAOD::AFPTrackContainer_v1> > afpTrack_l2;
    ElementLink<xAOD::AFPTrackContainer_v1> afpTrack_l3;
    std::vector<ElementLink<xAOD::AFPTrackContainer_v1> > afpTrack_l4;
    std::vector<std::vector<ElementLink<xAOD::AFPTrackContainer_v1> > > afpTrack_l5;

  };

  struct GCCXML_DUMMY_INSTANTIATION_ALFA_XAOD
  {

    xAOD::ALFADataContainer_v1 alfa_c1;
    DataLink<xAOD::ALFADataContainer_v1> alfa_l1;
    std::vector<DataLink<xAOD::ALFADataContainer_v1> > alfa_l2;
    ElementLink<xAOD::ALFADataContainer_v1> alfa_l3;
    std::vector<ElementLink<xAOD::ALFADataContainer_v1> > alfa_l4;
    std::vector<std::vector<ElementLink<xAOD::ALFADataContainer_v1> > > alfa_l5;

    //		xAOD::AFPDataContainer_v1 afp_c1;
    //		DataLink<xAOD::AFPDataContainer_v1> afp_l1;
    //		std::vector<DataLink<xAOD::AFPDataContainer_v1> > afp_l2;
    //		ElementLink<xAOD::AFPDataContainer_v1> afp_l3;
    //		std::vector<ElementLink<xAOD::AFPDataContainer_v1> > afp_l4;
    //		std::vector<std::vector<ElementLink<xAOD::AFPDataContainer_v1> > > afp_l5;

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

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_xAODTrigMinBias_DICT_H
#define xAODTrigMinBias_xAODTrigMinBias_DICT_H

// Include all headers here that need to have dictionaries
// generated for them.

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

#include "xAODTrigMinBias/versions/TrigSpacePointCounts_v1.h"
#include "xAODTrigMinBias/versions/TrigSpacePointCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigSpacePointCountsAuxContainer_v1.h"

#include "xAODTrigMinBias/versions/TrigT2MbtsBits_v1.h"
#include "xAODTrigMinBias/versions/TrigT2MbtsBitsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigT2MbtsBitsAuxContainer_v1.h"

#include "xAODTrigMinBias/versions/TrigHisto2D_v1.h"
#include "xAODTrigMinBias/versions/TrigHisto2DContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigHisto2DAuxContainer_v1.h"

#include "xAODTrigMinBias/versions/TrigVertexCounts_v1.h"
#include "xAODTrigMinBias/versions/TrigVertexCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigVertexCountsAuxContainer_v1.h"

#include "xAODTrigMinBias/versions/TrigTrackCounts_v1.h"
#include "xAODTrigMinBias/versions/TrigTrackCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigTrackCountsAuxContainer_v1.h"

#include "xAODTrigMinBias/versions/TrigT2ZdcSignals_v1.h"
#include "xAODTrigMinBias/versions/TrigT2ZdcSignalsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigT2ZdcSignalsAuxContainer_v1.h"

//i don't know what for
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGMINBIAS {
      xAOD::TrigSpacePointCountsContainer_v1 c1;
      DataLink< xAOD::TrigSpacePointCountsContainer_v1 > dl1;
      std::vector< DataLink< xAOD::TrigSpacePointCountsContainer_v1 > > dl2;
      ElementLink< xAOD::TrigSpacePointCountsContainer_v1 > el1;
      std::vector< ElementLink< xAOD::TrigSpacePointCountsContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::TrigSpacePointCountsContainer_v1 > > > el3;
      
      xAOD::TrigT2MbtsBitsContainer_v1 ct1;
      DataLink< xAOD::TrigT2MbtsBitsContainer_v1 > dlt1;
      std::vector< DataLink< xAOD::TrigT2MbtsBitsContainer_v1 > > dlt2;
      ElementLink< xAOD::TrigT2MbtsBitsContainer_v1 > elt1;
      std::vector< ElementLink< xAOD::TrigT2MbtsBitsContainer_v1 > > elt2;
      std::vector< std::vector< ElementLink< xAOD::TrigT2MbtsBitsContainer_v1 > > > elt3;
      
      xAOD::TrigVertexCountsContainer_v1 cv1;
      DataLink< xAOD::TrigVertexCountsContainer_v1 > dlv1;
      std::vector< DataLink< xAOD::TrigVertexCountsContainer_v1 > > dlv2;
      ElementLink< xAOD::TrigVertexCountsContainer_v1 > elv1;
      std::vector< ElementLink< xAOD::TrigVertexCountsContainer_v1 > > elv2;
      std::vector< std::vector< ElementLink< xAOD::TrigVertexCountsContainer_v1 > > > elv3;
      
      xAOD::TrigTrackCountsContainer_v1 ctt1;
      DataLink< xAOD::TrigTrackCountsContainer_v1 > dltt1;
      std::vector< DataLink< xAOD::TrigTrackCountsContainer_v1 > > dltt2;
      ElementLink< xAOD::TrigTrackCountsContainer_v1 > eltt1;
      std::vector< ElementLink< xAOD::TrigTrackCountsContainer_v1 > > eltt2;
      std::vector< std::vector< ElementLink< xAOD::TrigTrackCountsContainer_v1 > > > eltt3;

      xAOD::TrigT2ZdcSignalsContainer_v1 czdct1;
      DataLink< xAOD::TrigT2ZdcSignalsContainer_v1 > dlzdct1;
      std::vector< DataLink< xAOD::TrigT2ZdcSignalsContainer_v1 > > dlzdct2;
      ElementLink< xAOD::TrigT2ZdcSignalsContainer_v1 > elzdct1;
      std::vector< ElementLink< xAOD::TrigT2ZdcSignalsContainer_v1 > > elzdct2;
      std::vector< std::vector< ElementLink< xAOD::TrigT2ZdcSignalsContainer_v1 > > > elzdct3;

   };
} // private namespace

#endif

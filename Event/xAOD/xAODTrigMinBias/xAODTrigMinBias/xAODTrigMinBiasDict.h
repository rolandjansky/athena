// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef xAODTrigMinBias_xAODTrigMinBias_DICT_H
#define xAODTrigMinBias_xAODTrigMinBias_DICT_H

// Local include(s).
#include "xAODTrigMinBias/TrigSpacePointCounts.h"
#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h"
#include "xAODTrigMinBias/versions/TrigSpacePointCounts_v1.h"
#include "xAODTrigMinBias/versions/TrigSpacePointCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigSpacePointCountsAuxContainer_v1.h"

#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"
#include "xAODTrigMinBias/versions/TrigT2MbtsBits_v1.h"
#include "xAODTrigMinBias/versions/TrigT2MbtsBitsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigT2MbtsBitsAuxContainer_v1.h"

#include "xAODTrigMinBias/TrigHisto2D.h"
#include "xAODTrigMinBias/TrigHisto2DContainer.h"
#include "xAODTrigMinBias/TrigHisto2DAuxContainer.h"
#include "xAODTrigMinBias/versions/TrigHisto2D_v1.h"
#include "xAODTrigMinBias/versions/TrigHisto2DContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigHisto2DAuxContainer_v1.h"

#include "xAODTrigMinBias/TrigVertexCounts.h"
#include "xAODTrigMinBias/TrigVertexCountsContainer.h"
#include "xAODTrigMinBias/TrigVertexCountsAuxContainer.h"
#include "xAODTrigMinBias/versions/TrigVertexCounts_v1.h"
#include "xAODTrigMinBias/versions/TrigVertexCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigVertexCountsAuxContainer_v1.h"

#include "xAODTrigMinBias/TrigTrackCounts.h"
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCountsAuxContainer.h"
#include "xAODTrigMinBias/versions/TrigTrackCounts_v1.h"
#include "xAODTrigMinBias/versions/TrigTrackCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigTrackCountsAuxContainer_v1.h"

#include "xAODTrigMinBias/TrigT2ZdcSignals.h"
#include "xAODTrigMinBias/TrigT2ZdcSignalsContainer.h"
#include "xAODTrigMinBias/TrigT2ZdcSignalsAuxContainer.h"
#include "xAODTrigMinBias/versions/TrigT2ZdcSignals_v1.h"
#include "xAODTrigMinBias/versions/TrigT2ZdcSignalsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigT2ZdcSignalsAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGMINBIAS {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD,
                                           TrigSpacePointCountsContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigT2MbtsBitsContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigHisto2DContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigVertexCountsContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigTrackCountsContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigT2ZdcSignalsContainer_v1 );
   };
}

#endif // xAODTrigMinBias_xAODTrigMinBias_DICT_H

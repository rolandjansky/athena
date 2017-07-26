// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPREPRAWDATADICT_H
#define INDETPREPRAWDATADICT_H 1

//#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"

#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "Identifier/Identifier.h"
#include "AthLinks/ElementLink.h"

/** helper class to help reflex/gccxml to discover the right types
 */
template<class T>
struct PrdWrapper
{
  typedef typename T::const_iterator prd_iterator;
};

#define EXPAND_PRD(T, suffix)						\
  T ## Container m_prd_ ## suffix ;					\
  PrdWrapper<T ## Container> m_prd_wrapper_ ## suffix ;			\
  PrdWrapper<T ## Container> ::prd_iterator m_prd_citr_ ## suffix ;	\
  ::EventContainers::IdentifiableCache< Trk::PrepRawDataCollection< T > > m_dummy_dlv_ ## suffix ; \
  T ## Container ::const_iterator m_dummy_iter_ ## suffix  
  
namespace InDet {
    struct InDetPrepRawDataDict_Dummy
    {
      DataVector<Identifier>                     m_identifierVector;
      DataVector< SiClusterCollection >          m_dummyDVSiClusColl;
      DataVector< SCT_ClusterCollection >        m_dummyDVSCTClusColl;
      DataVector< PixelClusterCollection >       m_dummyDVPixelClusColl;
      DataVector< TRT_DriftCircleCollection >    m_dummyDVTRTColl;
      SiClusterContainer                         m_dummySiContainer;
      
      EXPAND_PRD(InDet::SiCluster, si_cluster);
      EXPAND_PRD(InDet::PixelCluster, pixel_cluster);
      EXPAND_PRD(InDet::SCT_Cluster, sct_cluster);
      EXPAND_PRD(InDet::TRT_DriftCircle, trt_driftcircle);

      ElementLink<InDet::PixelClusterContainer> m_dummyel_pixel_cluster;
      ElementLink<InDet::SCT_ClusterContainer> m_dummyel_sct_cluster;
      ElementLink<InDet::TRT_DriftCircleContainer> m_dummyel_trt_driftcircle;
    };
}

#undef EXPAND_DLV

#endif




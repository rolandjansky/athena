/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataCLASS_DEF.h
//   Header file for class PrepRawDataCLASS_DEF
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the CLASS_DEF for Containers and Collections
///////////////////////////////////////////////////////////////////
// Version 1.0 25/09/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRT_DRIFTCIRCLECONTAINER_H
#define TRT_DRIFTCIRCLECONTAINER_H

#include "SGTools/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"

// Containers
// numbers obtained using clid.db
namespace InDet {
typedef Trk::PrepRawDataContainer< TRT_DriftCircleCollection > TRT_DriftCircleContainer;
typedef EventContainers::IdentifiableCache< TRT_DriftCircleCollection > TRT_DriftCircleContainerCache;
}

CLASS_DEF(InDet::TRT_DriftCircleContainer,1128766825,1)
CONTAINER_IS_IDENTCONT(InDet::TRT_DriftCircleContainer)

CLASS_DEF( InDet::TRT_DriftCircleContainerCache , 1226029651 , 1 )

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H

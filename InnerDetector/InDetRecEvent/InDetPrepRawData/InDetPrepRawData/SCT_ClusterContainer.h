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

#ifndef SCT_CLUSTERCONTAINER_H
#define SCT_CLUSTERCONTAINER_H

#include "SGTools/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"

// Containers
// numbers obtained using clid.db
namespace InDet {
  typedef Trk::PrepRawDataContainer< SCT_ClusterCollection > SCT_ClusterContainer;
  typedef EventContainers::IdentifiableCache< SCT_ClusterCollection > SCT_ClusterContainerCache;
}

CLASS_DEF(InDet::SCT_ClusterContainer,2561,1)
CONTAINER_IS_IDENTCONT(InDet::SCT_ClusterContainer)

CLASS_DEF( InDet::SCT_ClusterContainerCache , 1302859648 , 1 )

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H

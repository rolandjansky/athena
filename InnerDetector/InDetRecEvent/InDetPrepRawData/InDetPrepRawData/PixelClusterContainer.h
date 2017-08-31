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

#ifndef PIXELCLUSTERCONTAINER_H
#define PIXELCLUSTERCONTAINER_H

#include "SGTools/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "EventContainers/IdentifiableCache.h"

// Containers
// numbers obtained using clid.db
namespace InDet {
typedef Trk::PrepRawDataContainer< PixelClusterCollection > PixelClusterContainer;
typedef EventContainers::IdentifiableCache< PixelClusterCollection > PixelClusterContainerCache;  
}

CLASS_DEF(InDet::PixelClusterContainer,2560,1)
CONTAINER_IS_IDENTCONT(InDet::PixelClusterContainer)

CLASS_DEF( InDet::PixelClusterContainerCache , 1224316430 , 1 )

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H

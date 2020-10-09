/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataCLASS_DEF.h
//   Header file for class PrepRawDataCLASS_DEF
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the CLASS_DEF for Containers and Collections
///////////////////////////////////////////////////////////////////
// Version 0.1 15/09/2020 James Smith
///////////////////////////////////////////////////////////////////

#ifndef ITK_STRIP_CLUSTERCONTAINER_H
#define ITK_STRIP_CLUSTERCONTAINER_H

#include "SGTools/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "InDetPrepRawData/ITk_Strip_ClusterCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"

// Containers
// numbers obtained using clid.db
namespace InDet {
  typedef Trk::PrepRawDataContainer< ITk_Strip_ClusterCollection > ITk_Strip_ClusterContainer; 
}

CLASS_DEF(InDet::ITk_Strip_ClusterContainer,1273663998,1)
CONTAINER_IS_IDENTCONT(InDet::ITk_Strip_ClusterContainer)

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H

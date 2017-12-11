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

#ifndef ISF_FATRASEVENT_PLANARCLUSTERCONTAINER_H
#define ISF_FATRASEVENT_PLANARCLUSTERCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "ISF_FatrasEvent/PlanarClusterCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"

// Containers
// numbers obtained using clid.db

namespace iFatras {
  typedef Trk::PrepRawDataContainer< PlanarClusterCollection > PlanarClusterContainer; 
}

CLASS_DEF(iFatras::PlanarClusterContainer, 1175446284, 1)
CONTAINER_IS_IDENTCONT(iFatras::PlanarClusterContainer)

#endif // ISF_FATRASEVENT_PLANARCLUSTERCONTAINER_H

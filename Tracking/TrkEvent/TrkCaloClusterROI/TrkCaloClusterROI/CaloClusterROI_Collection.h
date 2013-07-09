/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKCALOCLUSTERROI_COLLECTION_H
#define TRKCALOCLUSTERROI_COLLECTION_H


#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrkCaloClusterROI/CaloClusterROI.h"


typedef DataVector<Trk::CaloClusterROI> CaloClusterROI_Collection;
CLASS_DEF( CaloClusterROI_Collection , 1161284415 , 1 )

#endif //TRKCALOCLUSTERROI_COLLECTION_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSISPACEPOINTCOLLECTION_H
#define TRIGSISPACEPOINTCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
//#include "InDetIdentifier/InDetDetElemHash.h"

typedef Trk::PrepRawDataCollection< TrigSiSpacePoint > TrigSiSpacePointCollection;

CLASS_DEF( TrigSiSpacePointCollection , 1333891984 , 1 )

typedef Trk::PrepRawDataContainer< TrigSiSpacePointCollection> TrigSiSpacePointContainer;

CLASS_DEF( TrigSiSpacePointContainer , 1291201278 , 1 )


#endif 

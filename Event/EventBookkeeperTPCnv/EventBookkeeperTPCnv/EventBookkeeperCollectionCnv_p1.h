/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTBOOKKEEPERCOLLECTIONCNV_P1_H
#define EVENTBOOKKEEPERCOLLECTIONCNV_P1_H

///////////////////////////////////////////////////////////////////////////
//  Header file for class EventBookkeeperCollectionCnv_p1
//  Author: David Cote <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////////////

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p1.h"
#include "EventBookkeeperCnv_p1.h"


typedef T_AthenaPoolTPCnvVector< EventBookkeeperCollection, EventBookkeeperCollection_p1, EventBookkeeperCnv_p1 >  EventBookkeeperCollectionCnv_p1;

template<>
class T_TPCnv<EventBookkeeperCollection, EventBookkeeperCollection_p1>
  : public EventBookkeeperCollectionCnv_p1
{
public:
};

#endif  //EVENTBOOKKEEPERCOLLECTIONCNV_P1_H

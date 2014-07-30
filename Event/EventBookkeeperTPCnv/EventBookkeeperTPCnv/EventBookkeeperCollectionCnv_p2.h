/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTBOOKKEEPERCOLLECTIONCNV_p2_H
#define EVENTBOOKKEEPERCOLLECTIONCNV_p2_H

///////////////////////////////////////////////////////////////////////////
//  Header file for class EventBookkeeperCollectionCnv_p2
//  Author: David Cote <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////////////

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p2.h"
#include "EventBookkeeperCnv_p2.h"


typedef T_AthenaPoolTPCnvVector< EventBookkeeperCollection, EventBookkeeperCollection_p2, EventBookkeeperCnv_p2 >  EventBookkeeperCollectionCnv_p2;

template<>
class T_TPCnv<EventBookkeeperCollection, EventBookkeeperCollection_p2>
  : public EventBookkeeperCollectionCnv_p2
{
public:
};

#endif  //EVENTBOOKKEEPERCOLLECTIONCNV_p2_H

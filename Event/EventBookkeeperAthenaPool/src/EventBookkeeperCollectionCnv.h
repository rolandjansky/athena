/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for EventBookkeeperCollectionCnv
//  Author: David Cote, September 2008. <david.cote@cern.ch>
//  This class defines which persitent version is used for writing, and how to handle various pN versions when reading. 
///////////////////////////////////////////////////////////////////

#ifndef EVENTBOOKKEEPERCOLLECTIONCNV__H
#define EVENTBOOKKEEPERCOLLECTIONCNV__H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p2.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollectionCnv_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollectionCnv_p2.h"

// the latest persistent representation type of EventBookkeeperCollection
typedef  EventBookkeeperCollection_p2  EventBookkeeperCollection_PERS;
typedef  T_AthenaPoolCustomCnv<EventBookkeeperCollection, EventBookkeeperCollection_PERS >  EventBookkeeperCollectionCnvBase;

class EventBookkeeperCollectionCnv : public EventBookkeeperCollectionCnvBase 
{
  friend class CnvFactory<EventBookkeeperCollectionCnv >;
 protected:
  EventBookkeeperCollectionCnv (ISvcLocator* svcloc) : EventBookkeeperCollectionCnvBase(svcloc) {}
  virtual EventBookkeeperCollection_PERS*  createPersistent (EventBookkeeperCollection* transCont);
  virtual EventBookkeeperCollection*     createTransient ();

  //  virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }

 private:
  EventBookkeeperCollectionCnv_p1   m_TPConverterP1;
  EventBookkeeperCollectionCnv_p2   m_TPConverter;
};

#endif  // EVENTBOOKKEEPERCOLLECTIONCNV__H


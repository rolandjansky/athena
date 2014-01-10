/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for EventBookkeeperCnv
//  Author: David Cote, September 2008. <david.cote@cern.ch>
//  This class defines which persitent version is used for writing, and how to handle various pN versions when reading. 
///////////////////////////////////////////////////////////////////

#ifndef EVENTBOOKKEEPERCNV__H
#define EVENTBOOKKEEPERCNV__H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "EventBookkeeperTPCnv/EventBookkeeper_p2.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p2.h"

// the latest persistent representation type of EventBookkeeper
typedef  EventBookkeeper_p2  EventBookkeeper_PERS;
typedef  T_AthenaPoolCustomCnv<EventBookkeeper, EventBookkeeper_PERS >  EventBookkeeperCnvBase;

class EventBookkeeperCnv : public EventBookkeeperCnvBase 
{
  friend class CnvFactory<EventBookkeeperCnv >;
 protected:
  EventBookkeeperCnv (ISvcLocator* svcloc) : EventBookkeeperCnvBase(svcloc) {}
  virtual EventBookkeeper_PERS*  createPersistent (EventBookkeeper* transCont);
  virtual EventBookkeeper*     createTransient ();

 private:
  EventBookkeeperCnv_p1   m_TPConverterP1;
  EventBookkeeperCnv_p2   m_TPConverter;
};

#endif  // EVENTBOOKKEEPERCNV__H


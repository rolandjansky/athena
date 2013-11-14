/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_TrackStoreTDS_H
#define MB_TrackStoreTDS_H

#include "GaudiKernel/DataObject.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthEvt/MboyAthEvtMisc.h"

#include "MboyAthEvt/MB_TrackStoreAth.h"

static const CLID CLID_MB_TrackStoreTDS = 4026;

  /**
   @class MB_TrackStoreTDS

   This class allows to store a MB_TrackStoreAth in TDS
   
  @author samusog@cern.ch
  
  */

class MB_TrackStoreTDS:public DataObject {
public:
   MB_TrackStoreTDS();
   virtual ~MB_TrackStoreTDS();

public:
///////////////////////////////////

   static const CLID& classID() { return CLID_MB_TrackStoreTDS; }
   virtual const CLID& clID() const { return CLID_MB_TrackStoreTDS; }

public:
///////////////////////////////////
//Data

   MB_TrackStoreAth m_MB_TrackStore ; //!< the MB_TrackStoreAth

};
#endif


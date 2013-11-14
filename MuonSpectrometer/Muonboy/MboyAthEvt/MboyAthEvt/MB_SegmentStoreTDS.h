/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_SegmentStoreTDS_H
#define MB_SegmentStoreTDS_H

#include "GaudiKernel/DataObject.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthEvt/MboyAthEvtMisc.h"

#include "MboyAthEvt/MB_SegmentStoreAth.h"

static const CLID CLID_MB_SegmentStoreTDS = 4027;

  /**
   @class MB_SegmentStoreTDS

   This class allows to store a MB_SegmentStoreAth in TDS
   
  @author samusog@cern.ch
  
  */

class MB_SegmentStoreTDS:public DataObject {
public:
   MB_SegmentStoreTDS();
   virtual ~MB_SegmentStoreTDS();

public:
///////////////////////////////////

   static const CLID& classID() { return CLID_MB_SegmentStoreTDS; }
   virtual const CLID& clID() const { return CLID_MB_SegmentStoreTDS; }

public:
///////////////////////////////////
//Data

   MB_SegmentStoreAth m_MB_SegmentStore ; //!< the MB_SegmentStoreAth

};
#endif

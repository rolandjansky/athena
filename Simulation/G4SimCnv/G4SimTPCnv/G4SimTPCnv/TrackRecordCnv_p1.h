/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKRECORD_TRACKRECORDCNV_P1_H
#define TRACKRECORD_TRACKRECORDCNV_P1_H

/*
Transient/Persistent converter for TrackRecord class
Author: Davide Costanzo
*/

class TrackRecord;
class TrackRecord_p1;

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrackRecordCnv_p1  : public T_AthenaPoolTPCnvBase<TrackRecord, TrackRecord_p1>
{
public:
  TrackRecordCnv_p1() {}

  virtual void          persToTrans(const TrackRecord_p1* persObj, TrackRecord* transObj, MsgStream &log);
  virtual void          transToPers(const TrackRecord* transObj, TrackRecord_p1* persObj, MsgStream &log);
};


#endif


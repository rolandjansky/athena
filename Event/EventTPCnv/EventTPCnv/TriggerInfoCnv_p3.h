/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_TRIGGERINFOCNV_P3_H
#define EVENTATHENAPOOL_TRIGGERINFOCNV_P3_H

class MsgStream;
class TriggerInfoCnv_p3 {
public:
  void   persToTrans(std::vector<unsigned int>::const_iterator &persObj, TriggerInfo* transObj, bool bugcompat);
  void   transToPers(const TriggerInfo* transObj, std::vector<unsigned int> &persObj);
};

#endif


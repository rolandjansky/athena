/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_TRIGGERINFOCNV_P3_H
#define EVENTATHENAPOOL_TRIGGERINFOCNV_P3_H
class TriggerInfo;
#include <vector>

class MsgStream;
class TriggerInfoCnv_p3 {
public:
  TriggerInfoCnv_p3() {}
  void   persToTrans(std::vector<unsigned int>::const_iterator &persObj, TriggerInfo* transObj, bool bugcompat) const;
  void   transToPers(const TriggerInfo* transObj, std::vector<unsigned int> &persObj) const;
};

#endif


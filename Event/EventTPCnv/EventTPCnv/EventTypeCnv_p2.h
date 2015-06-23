/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTTYPECNV_P2_H
#define EVENTATHENAPOOL_EVENTTYPECNV_P2_H
#include <vector>

class EventTypeCnv_p2{
public:
  void   persToTrans(std::vector<unsigned int>::const_iterator &persObj, EventType* transObj, int version, bool bugcompat);
  void   transToPers(const EventType* transObj, std::vector<unsigned int> &persObj);
};

#endif

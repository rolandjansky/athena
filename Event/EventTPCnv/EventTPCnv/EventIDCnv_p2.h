/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTIDCNV_P2_H
#define EVENTATHENAPOOL_EVENTIDCNV_P2_H
#include <vector>
class EventIDCnv_p2 {
public:
  EventIDCnv_p2() {}
  virtual void   persToTrans(std::vector<unsigned int>::const_iterator & nnnnpersObj, EventID* transObj) ;
  virtual void   transToPers(const EventID* transObj, std::vector<unsigned int> &persObj) ;
};

#endif

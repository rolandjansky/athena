/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENT_COLLECTION_P1_H
#define TRIGMON_EVENT_COLLECTION_P1_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonEventCollection;
*/

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMonEventCollection_p1 : public std::vector<TPObjRef>
{  
 public:

  TrigMonEventCollection_p1() {}
  virtual ~TrigMonEventCollection_p1() {}

};

#endif

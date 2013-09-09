/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIG_COLLECTION_P1_H
#define TRIGMON_CONFIG_COLLECTION_P1_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonConfigCollection;
*/

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMonConfigCollection_p1 : public std::vector<TPObjRef>
{  
 public:

  TrigMonConfigCollection_p1() {}
  virtual ~TrigMonConfigCollection_p1() {}

};

#endif

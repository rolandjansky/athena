/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TTPVALTRIGDUMP_H_
#define  TTPVALTRIGDUMP_H_
//
// Class to dump out trigger bits
//
#include "TString.h"
#include "TTree.h"
#include <list>

class TTPValTrigDump {
 public:
  TTPValTrigDump();
  virtual ~TTPValTrigDump();

  void dump(const TString& filename);
  void dump(TTree* tree);

  void setNEvents(long nev) {_maxNEvents = nev;}

  // Dump specific events rather than just everything
  // Can be called multiple times before calling dump()
  void selectEvent(long evNumber);

 private:
  long _maxNEvents;
  std::list<long> _eventList;
};

#endif

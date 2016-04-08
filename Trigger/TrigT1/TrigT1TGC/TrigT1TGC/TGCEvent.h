/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCEvent.h
                                      QCJP, 1999
*/
// ====================================================================
#ifndef TGC_EVENT_H
#define TGC_EVENT_H

#include <vector>
#include "TrigT1TGC/TGCReadoutIndex.h"

namespace LVL1TGCTrigger {

class TGCASDOut;

// ====================================================================
//
// class definition
//
// ====================================================================
 
class TGCEvent {
protected:
  int eventNumber;
  std::vector<TGCASDOut*> vecASDOut;

public:
//  double eta;//for test
  TGCEvent();

  ~TGCEvent();
 
  TGCEvent(const TGCEvent& right)
  {
    *this= right;
  }
 
  const TGCEvent& operator=(const TGCEvent& right)
  {
    eventNumber= right.eventNumber;
    vecASDOut= right.vecASDOut;

    return *this;
  }
 
  int operator==(const TGCEvent& right) const
  {
    return (this==&right);
  }
 
  int operator!=(const TGCEvent& right) const
  {
    return (this!=&right);
  }

  // set functions
  void SetEventNumber(int num) { eventNumber= num; }

  TGCASDOut* NewASDOut(TGCIndex tgcindex, int ilyr, 
		       TGCSignalType sigtype=WIREGROUP, int id=-1, 
		       double tof=0);

  TGCASDOut* NewASDOut(TGCReadoutIndex tgcindex, 
		       TGCSignalType sigtype=WIREGROUP, int id=-1, 
		       double tof=0);

  // get functions
  int GetEventNumber() const { return eventNumber; }

  // ASDOut ...
  int GetNASDOut() const { return vecASDOut.size(); }

  const TGCASDOut* GetASDOut(int index) const
  {
    if(index<=0 || (unsigned int)index> vecASDOut.size()) return 0;
    else return vecASDOut[index-1];
  }

  std::vector<TGCASDOut*>& GetASDOutVector() const
  {
    return (std::vector<TGCASDOut*>&)vecASDOut;
  }

  // operations
  void Clear();          // clear event
  void Print() const;    // print out event information
};


} //end of namespace bracket

#endif

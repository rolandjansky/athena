/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  int m_eventNumber;
  std::vector<TGCASDOut*> m_vecASDOut;

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
    m_eventNumber= right.m_eventNumber;
    m_vecASDOut= right.m_vecASDOut;

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
  void SetEventNumber(int num) { m_eventNumber= num; }

  TGCASDOut* NewASDOut(TGCIndex tgcindex, int ilyr, 
		       TGCSignalType sigtype=WIREGROUP, int id=-1, 
		       double tof=0);

  TGCASDOut* NewASDOut(TGCReadoutIndex tgcindex, 
		       TGCSignalType sigtype=WIREGROUP, int id=-1, 
		       double tof=0);

  // get functions
  int GetEventNumber() const { return m_eventNumber; }

  // ASDOut ...
  int GetNASDOut() const { return m_vecASDOut.size(); }

  const TGCASDOut* GetASDOut(int index) const
  {
    if(index<=0 || (unsigned int)index> m_vecASDOut.size()) return 0;
    else return m_vecASDOut[index-1];
  }

  std::vector<TGCASDOut*>& GetASDOutVector() const
  {
    return (std::vector<TGCASDOut*>&)m_vecASDOut;
  }

  // operations
  void Clear();          // clear event
  void Print() const;    // print out event information
};


} //end of namespace bracket

#endif

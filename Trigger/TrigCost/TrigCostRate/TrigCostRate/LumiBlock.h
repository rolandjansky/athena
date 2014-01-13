/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_LUMIBLOCK_H
#define ANP_LUMIBLOCK_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : LumiBlock
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Helper class for this class computes properties of lumi blocks in data
 *  - count number of events in lumi block
 *  - earliest and latest event time within lumiblock
 *  - number of trigger processes reported for lumi block?
 *  - luminosity sum for event
 *
 **********************************************************************************/

#include <set>

namespace Anp
{
  class LumiBlock {
  public:

    LumiBlock();
    LumiBlock(int run, int lb);
    ~LumiBlock() {}

    void SetRun(int run) { fRun = run; }
    void SetLB (int lb)  { fLB  = lb; }

    void AddLumi(double lumi)  { fLuminosity += lumi; }
    void SetLumi(double lumi)  { fLuminosity  = lumi; }

    void SetFirst(unsigned sec, unsigned nsec) { fFirstSec = sec; fFirstNSec = nsec; }
    void SetLast (unsigned sec, unsigned nsec) { fLastSec  = sec; fLastNSec  = nsec; }
    void SetNext (unsigned sec, unsigned nsec) { fNextSec  = sec; fNextNSec  = nsec; }
    
    void AddEvent(unsigned nevent = 1) { fNEvent += nevent; }
    void SetEvent(unsigned nevent)     { fNEvent  = nevent; }
    
    void AddReadEvent(unsigned nevent = 1) { fNEventRead += nevent; }
    void AddEstmEvent(unsigned nevent = 1) { fNEventEstm += nevent; }
    
    void AddAppHist(unsigned app_id) { fAppHist.insert(app_id); }
    void AddAppRead(unsigned app_id) { fAppRead.insert(app_id); }

    int GetRun() const { return fRun; }
    int GetLB()  const { return fLB; }

    unsigned GetFirstSec()  const { return fFirstSec; }
    unsigned GetFirstNSec() const { return fFirstNSec; }

    unsigned GetLastSec()  const { return fLastSec; }
    unsigned GetLastNSec() const { return fLastNSec; }

    unsigned GetNextSec()  const { return fNextSec; }
    unsigned GetNextNSec() const { return fNextNSec; }
    
    unsigned GetCount()   const { return fNEvent; }
    unsigned GetNEvents() const { return fNEvent; }
    
    unsigned GetNReadEvents() const { return fNEventRead; }
    unsigned GetNEstmEvents() const { return fNEventEstm; }
    
    unsigned GetNAppHist() const { return fAppHist.size(); }
    unsigned GetNAppRead() const { return fAppRead.size(); }

    double GetLumi() const { return fLuminosity; }

    long double GetDeltaT() const;
    
  private:
    
    int                   fRun;        // Run number
    int                   fLB;         // Luminosity block number
    double                fLuminosity; // Luminosity in arbitrary units

    unsigned              fFirstSec;   // sec for earliest (first) event
    unsigned              fFirstNSec;  // ns  for earliest (first) event
    
    unsigned              fLastSec;    // sec for latest (last) event
    unsigned              fLastNSec;   // ns  for latest (last) event

    unsigned              fNextSec;    // sec for earliest (first) event in next lumi block
    unsigned              fNextNSec;   // ns  for earliest (first) event in next lumi block

    unsigned              fNEvent;     // Count events
    unsigned              fNEventRead; // Count events - recorded by cost tool 
    unsigned              fNEventEstm; // Count events - estimated number of all L1 events

    std::set<unsigned>    fAppHist;    // Number of trigger applications from history
    std::set<unsigned>    fAppRead;    // Number of trigger applications with events
  };
}

#endif

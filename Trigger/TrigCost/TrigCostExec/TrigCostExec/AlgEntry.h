/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_ALGENTRY_H
#define ANP_ALGENTRY_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : AlgEntry
 * @Author : Rustem Ospanov
 *
 * @Brief  : Helper class for collecting algorithm timing and ROB request data for single RoI
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <set>

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

namespace Anp
{
  struct EventTime {
    EventTime() : event_timer(0.0), cpu_timer(0.0), rob_timer(0.0), tproc_timer(0.0), 
	 icost_timer(0.), run(0), lumi(0), bxid(0), sec(0), nsec(0) {}
    
    void ClearTimers() 
    {
      event_timer = 0.0; // event time from cost
      cpu_timer   = 0.0; // event CPU time from cost
      rob_timer   = 0.0; // event ROB time from cost
      tproc_timer = 0.0; // event timer measured by steering
      icost_timer = 0.0; // time taken by TrigCostTool itself
    }
    
    double   event_timer;
    double   cpu_timer;
    double   rob_timer;
    float    tproc_timer;
    float    icost_timer;
    
    unsigned run;
    unsigned lumi;
    unsigned bxid;
    unsigned sec;
    unsigned nsec;
  };

  class ROBEntry {
  public:
    
    struct ROB {
      ROB() : nrob(0), size(0) {}

      uint64_t  nrob;
      uint64_t  size;
    };
    
    struct Ratio {
      Ratio() : 
	nids(0), nids_cache(0), nids_retrv(0), nids_other(0),
	size(0), size_cache(0), size_retrv(0), size_other(0) {}

      double nids;
      double nids_cache;
      double nids_retrv;
      double nids_other;

      double size;
      double size_cache;
      double size_retrv;
      double size_other;
    };

  private:

    unsigned  nreq;
    double    time;
    Ratio     ratio;

    std::map<TrigMonROBData::History, ROB> robs;
    std::vector<uint32_t>                  rids;
  public:

    ROBEntry();
    ~ROBEntry() {}

    void Clear();
    void Print();

    void AddMonROB(const TrigMonROB &rob);
    void AddOneROB(const ROBEntry &rhs);

    double   GetTime() const { return time; }
    unsigned GetNReq() const { return nreq; }
    unsigned GetNROB() const;
    unsigned GetNIds() const { return GetNROB(); }
    double   GetSize() const;

    unsigned GetNROB(TrigMonROBData::History hist) const;
    double   GetSize(TrigMonROBData::History hist) const;

    unsigned GetNROBOther() const;
    double   GetSizeOther() const;

    const Ratio& GetRatio() const { return ratio; }

    const std::vector<uint32_t>& GetROBIds() const { return rids; }
  };

  class AlgEntry {
  public:

    AlgEntry();
    explicit AlgEntry(const TrigMonAlg &alg, float eta_ = -1.0e6, float phi_ = -1.0e6);
    ~AlgEntry() {}

    void Clear();

    bool FillRoiROB(const TrigMonROB &mon);
    
    const ROBEntry& GetROB() const { return rob; }
    
    double GetAlgTime() const { return alg_stop.getElapsed(alg_start); }
    double GetRobTime() const { return rob.GetTime(); }

  public:
    
    float             eta;
    float             phi;

    TrigMonTimer      alg_start;
    TrigMonTimer      alg_stop;

    std::set<uint8_t>      roi;
    ROBEntry               rob;
  };
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SLOWEVENT_H
#define ANP_SLOWEVENT_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : SlowEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Helper class for storing N slowest trigger events
 *
 **********************************************************************************/

// C/C++
#include <set>

// Local
#include "TrigCostBase/HtmlNode.h"

// Local
#include "TrigCostExec/AlgEntry.h"
#include "TrigCostBase/HtmlABC.h"

class TrigMonEvent; 
class TrigMonConfig;

namespace Anp
{
  //
  // Helper class for sorting slowest events
  //
  class SlowEvent {
  public:
    
    SlowEvent();
    SlowEvent(const TrigMonEvent &e, double t);
    SlowEvent(const EventTime &e, double timer);
    
    double GetTimer() const { return fTimer; }
    const TrigMonEvent& GetEvent() const { return fEvent; }

  private: 

    TrigMonEvent fEvent;
    double       fTimer;
  };

  inline bool operator<(const SlowEvent &lhs, const SlowEvent &rhs) {
    return lhs.GetTimer() < rhs.GetTimer();
  }
  inline bool operator==(const SlowEvent &lhs, const SlowEvent &rhs) {
    return lhs.GetTimer() == rhs.GetTimer();
  }

  //
  // Class for storing N slowest events
  //
  class SlowEvents {
  public:
   
    SlowEvents();
    ~SlowEvents() {}
    
    void SetNEvent(unsigned nevent) { fSlowNEvent = nevent; }

    void AddSlowEvent(const TrigMonEvent &event, double timer);

    void AddSlowEvent(const EventTime &event, double timer);

    unsigned NSlowEvents() const { return fSlowNEvent; }

    const std::multiset<SlowEvent>&  GetSlowEvents() const { return fSlowEvents; }
    const std::multiset<SlowEvent>&  GetSlowDetail() const { return fSlowDetail; }

    void WriteSlow(Handle<HtmlNode> node,
		   const HtmlMenu &menu,
		   const TrigMonConfig &config,
		   const std::multiset<SlowEvent> &events,
		   const std::string &key) const;

    void WriteSlow(Handle<HtmlNode> node,
		   const std::multiset<SlowEvent> &events,
		   const std::string &key) const;

    void PrintAlgTimers(std::ostream &os,
			const HtmlMenu &menu,
			const TrigMonEvent &event,
			const TrigMonConfig &config) const;

    void PrintAlgTimers(std::ostream &os,
			const HtmlMenu &menu,
			const TrigMonEvent &event,
			const TrigMonConfig &config,
			const unsigned int &fLevel) const;
    
  private:

    unsigned                   fSlowNEvent; // Number of recorded slow events
    std::multiset<SlowEvent>   fSlowEvents; // List of N slowest events
    std::multiset<SlowEvent>   fSlowDetail; // List of N slowest events with cost data
  };
}

#endif

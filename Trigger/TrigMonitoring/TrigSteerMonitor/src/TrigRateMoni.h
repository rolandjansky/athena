/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteerMonitor_TrigRateMoni_h
#define TrigSteerMonitor_TrigRateMoni_h

#include <vector>
#include <map>
#include <time.h>
#include <boost/thread/mutex.hpp>

#include "AthenaKernel/AlgorithmTimer.h"
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TimeDivider.h"



/**
 * @brief Tool for monitoring rates. 
 * The tools makes 2 histograms 2D. One which is filled every event @see m_buffer.
 * And another that gets published @see m_published which is copy of former made every time the configured interval is passed. 
 * The tool has 2 configurable options:
 * Number of intervals the cycle is repeated and also number interval between the copy from the buffer histogram to published one.
 * The intervals are aligned with UTC.
 * 
 * The rate is monitored for total events, streams & groups and also for chains.
 * The last bins of the published histogram mark the interval number. 
 * Assume setup like this m_duration=10, m_intervals=6. That means tool will collect statistics
 * for 10 seconds and then copy it to the published histogram. When this is done the buffer histograms gets reset 
 * and operation is cycled.
 * For period 17:00:00 - 17:00:10 the marker in the last histograms bins will be filled indicating interval 0.
 * for period 17:00:10 - 17:00:20 it will be 1 etc. until it cycles 
 * at 17:01:10.
 *
 * This should allow with the default gathering policy to find out about the provenance of the histograms entering the sum.
 */


class TH2F;
namespace HLT {
  class TrigSteer;
  class SteeringChain;
}

class TrigRateMoni : public TrigMonitorToolBase {
 public:
  TrigRateMoni(const std::string & type, const std::string & name,
		  const IInterface* parent);
  ~TrigRateMoni();

  virtual StatusCode initialize();
  virtual StatusCode bookHists();
  virtual StatusCode fillHists();
  virtual StatusCode finalHists();

 private:

  void updatePublished(unsigned int interval, unsigned int forceUpdateInterval, const time_t& t); 
  void fillInterval(unsigned int interval);
  void fillAbsoluteTime(const time_t& t);
  void fillTotalAndStreamTagBins();
  void fillChainAndGroupBins();
  void SetUpStreamSets();

  // properties
  unsigned int m_duration;
  unsigned int m_intervals;

  bool m_doStreams;
  bool m_doGroups;
  bool m_doChains;

  // helper to know when to switch to new interval  
  TimeDivider* m_timeDivider;
  

  // helper objects for the speedup of Fill methods
  std::map<std::string, int> m_stream_map;

  // These are for the special stream sets requested by Brian (Savannah 94703)
  std::string m_specialStream_OtherName;
  std::map<std::string, std::vector<std::string > > m_specialStreamSets;
  std::vector<std::string > m_genericStreamSets;

  typedef std::map<const HLT::SteeringChain*, std::set<int> > ChainToGroupBins;
  ChainToGroupBins m_groups_map; //!< map for bins
  
  class GroupDecisions {
  public:
    bool input;
    bool not_ps;
    bool raw;
    bool output;
    bool rerun;    
    GroupDecisions() { clear(); }
    void clear() {
      input =false; 
      not_ps=false;
      raw   =false;
      output=false; 
      rerun =false;
    }
  };
  
  typedef std::map<int, GroupDecisions> GroupHelper; 
  GroupHelper m_group_helper;  //!< decisions for groups  

  
  std::map<const HLT::SteeringChain*, int> m_chains_map; 
  

  TH2F* m_published{0}; //!< histogram which is published
  TH2F* m_buffer{0};    //!< buffer to collect statistics

  const HLT::TrigSteer* m_parentAlg{0};
  std::vector<std::string> m_specialStreamSetProperties;// for undecoded specials

  enum ybins { input=0, prescale=1, raw=2, output=3, rerun=4, algoIn=5, passedrerun=6 };

  Athena::AlgorithmTimer* m_timer{0};
  void callback();
  boost::mutex m_update_mutex;
  bool m_in_running;
};



#endif

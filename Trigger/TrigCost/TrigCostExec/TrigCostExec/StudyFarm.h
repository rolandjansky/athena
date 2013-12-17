/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_STUDYFARM_H
#define ANP_STUDYFARM_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : StudyFarm
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Algorithm to study trigger timing
 *  - collect total event processing time for L2 and/or EF from steering and cost
 *  - timing analysis for the farm nodes
 *
 **********************************************************************************/

// C/C++
#include <string>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostExec/TrigTimeAlg.h"
#include "TrigCostExec/TrigTimeChn.h"
#include "TrigCostExec/TrigTimeSeq.h"
#include "TrigCostBase/LumiABC.h"

class TH1;
class TH2;

namespace Anp
{
    //
    // helper class which stores the cpu usage and number of farm nodes used in a lumi block
    //
    struct LumiCpuUse {
    LumiCpuUse() : n_nodes(0),
	           n_algs(0),
	           cpu_time(0.0),
	           level(0),
	           alg_time(0.0),
	           evt_time(0.0),
	           ievt_time(0.0),
	           n_events(0),
 	           req_time(0.0) {}
               	   //startTime(0) {}
	           
      void         Clear();
      unsigned int n_nodes;
      unsigned int n_algs;
      double       cpu_time;
      std::set<unsigned> farmNodes;
      unsigned int level;
      double       alg_time;
      float        evt_time;
      float        ievt_time;
      unsigned int n_events;
      double       req_time;
    };
  //
  // Main algorithm for analyzing timing data
  //
  class StudyFarm: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    StudyFarm();
    virtual ~StudyFarm();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    void Set(const TrigMonConfig &event);
    void Run(const TrigMonEvent &event);
    typedef std::map<uint32_t, Anp::LumiCpuUse > LumiMap;
  private:
   
    typedef std::map<uint32_t,std::string> FarmNodeMap;
    typedef std::vector<Anp::Handle<Anp::TrigTimeChn> > ChnVec;
    typedef std::map<uint32_t, Anp::Handle<Anp::TrigTimeAlg> > AlgMap;

    //uint32_t
    Anp::LumiCpuUse &GetLumiCpuUse(uint32_t lumiId);
    

  private:
    
    std::string level() const;

  private:

    Anp::Registry          fReg;          // Registry configuration
    Anp::Registry          fRegIndex;     // Registry configuration for index page
    
    Branch<TrigMonConfig>  fConfg;        // Trigger configuration
    Branch<TrigMonEvent>   fEntry;        // Trigger data
    Handle<ConfigABC>      fConf;         // Configuration algorithm

    TrigMonConfig          fConfig;       // Local configuration copy

    // Properties:
    bool                   fDebug;        // Print debugging info
    bool                   fIsSubAlg;     // Is sub-algorithm? Parent will manage setup
    bool                   fhasL2;        // Alg level to write webpage
    bool                   fhasEF;        // Alg level to write webpage

    unsigned               fLevel;        // Algorithm level

    // Variables:
    bool                   fIsConfig;     // Is trigger configuration already read?
    
    unsigned int           fnEvents;      // Number of L2 events
    unsigned int           fnNodes;       // Number of farm nodes
    unsigned int           fRun;          // Run Number

    FarmNodeMap            fFarmNodeMap;  // Map from farm node to id number
    LumiMap                fLumiMap;      // Map from lumi block id to computer usage
    Handle<LumiABC>        fCoolLumi;     // Scaling algorithm for bunch group scaling
    std::string            fCoolLumiName; // Scaling algorithm for bunch group scaling
    Handle<LumiABC>        fCostLumi;     // Scaling algorithm for bunch group scaling
    std::string            fCostLumiName; // Scaling algorithm for bunch group scaling
  };
}

#endif

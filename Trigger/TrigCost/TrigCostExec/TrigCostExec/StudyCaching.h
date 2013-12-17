/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_STUDYCACHING_H
#define ANP_STUDYCACHING_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : StudyCaching
 * @Author : Rustem Ospanov, Doug Schaefer
 *
 * @Brief  : 
 *
 * Algorithm to study trigger timing
 *  - collect total event processing time for L2 and/or EF from steering and cost
 *  - timing analysis for chains, sequences and algorithms
 *  - produce html summary pages and histograms
 *  - use lookup table (vector of raw pointers) to find algorithm,
 *    sequence and chain objects
 *  - write html pages with timing summary for: chains, algorithm classes and instances
 *  - can be executed as sub-algorithm of MakePuTimes
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
#include "TrigCostExec/SlowEvent.h"
#include "TrigCostExec/TimeHist.h"
#include "TrigCostExec/TrigTimeAlg.h"
#include "TrigCostExec/TrigTimeChn.h"
#include "TrigCostExec/TrigTimeROB.h"
#include "TrigCostExec/TrigTimeSeq.h"

class TH1;
class TH2;

namespace Anp
{
  //
  // Main algorithm for analyzing timing data
  //
  class StudyCaching: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    StudyCaching();
    virtual ~StudyCaching();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    void Save(TDirectory *dir);

    bool Init();

    void Exec();

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    void Set(const TrigMonConfig &event);
    void Run(const TrigMonEvent &event);

    const std::vector<TrigConfChain>& GetChnVec() { return fConfig.getVec<TrigConfChain>(); }
    const std::vector<TrigConfSeq>&   GetSeqVec() { return fConfig.getVec<TrigConfSeq>(); }
    
  private:

    typedef std::map<unsigned, std::set<unsigned> > SecMap;

  private:
    
    TH1* GetTH1(const std::string &key) const;    
    TH2* GetTH2(const std::string &key) const;

    void FillHist(TH1 *h, double v);    

    std::string level() const;

    void ReadEvents(const std::string &fpath, SecMap &smap);

    bool AddEvent(unsigned sec, unsigned ns, SecMap &smap) const;

    bool PassEvent(const TrigMonEvent &event) const;

  private:

    TDirectory            *fDir;          // Directory pointer
    Anp::Registry          fReg;          // Registry configuration
    Anp::Registry          fRegIndex;     // Registry configuration for index page
    
    Branch<TrigMonEvent>   fEntry;        // Trigger data
    Handle<ConfigABC>      fConf;         // Configuration algorithm
    
    TrigMonConfig          fConfig;       // Local configuration copy

    // Properties:
    std::string            fDirName;      // Directory name
    std::string            fHistOpts;     // Histogram fill and draw options
    std::string            fRootName;     // Name of root file where plots are saved

    bool                   fDebug;        // Print debugging info

    unsigned               fShowNAlg;     // Print only N slowest algorithms
    unsigned               fLevel;        // Algorithm level
    unsigned               fMinLB;        // Min LB (inclusive)
    unsigned               fMaxLB;        // Max LB (inclusive)
   
    std::set<unsigned>     fLBs;          // Selected LBs
    SecMap                 fTimes;        // Selected  event times
    SecMap                 fEvents;       // Processed event times

    // Variables:
    bool                   fIsConfig;     // Is trigger configuration already read?
    double                 fTimeEvent;    // Total processing time from algorithm timer
    double                 fTimeSteer;    // Total processing time from TrigSteer
    
    long                   fCountL2;      // Number of active L2 cost events
    long                   fCountEF;      // Number of active EF cost events
    long                   fNEvent;       // Number of events
    EventTime              fEvent;        // Struct to hold event summary

    long                   fTotalAlgNCache; // Number of total alg caches
    long                   fTotalAlgNCalls; // Number of total alg calls

    // Histograms:
    TH1                   *hTimeAlgs;
    TH1                   *hTimeCost;
    TH1                   *hTimeEvent;
    TH1                   *hTimeTproc;
    TH1                   *hTimeTexec;

    TH1                   *hAlgNExec;
    TH1                   *hAlgNCache;
    TH1                   *hAlgFCache;
    TH1                   *hAlgNTotal;

    TH1                   *hSeqNExec;
    TH1                   *hSeqNTotal;

    TH1                   *hChnPass;
    TH1                   *hEventLB;
  };
}

#endif

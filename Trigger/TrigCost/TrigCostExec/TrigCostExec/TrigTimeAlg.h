/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_TRIGTIMEALG_H
#define ANP_TRIGTIMEALG_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : TrigTimeAlg
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Timing plots for algorithms
 *  - cpu and ROB request timing per call and per event
 *  - plot separately cached and retreived ROB requests
 *  - additional plots
 *
 **********************************************************************************/

// C/C++
#include <set>
#include <map>
#include <string>
#include <vector>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/HtmlNode.h"

// Local
#include "TrigCostExec/AlgEntry.h"
#include "TrigCostExec/SlowEvent.h"
#include "TrigCostExec/TimeHist.h"

class TDirectory;
class TH1;
class TH2;
class TCanvas;

namespace Anp
{
  class TrigTimeAlg: public virtual Base {
  public:
    //
    // Count number of active and cached algorithm calls
    //
    struct Count {
      Count() : ncalls(0), ncache(0) {}

      uint64_t ncalls;
      uint64_t ncache;
    };

  public:

    TrigTimeAlg();
    virtual ~TrigTimeAlg();

    void Config(TDirectory *dir) { fDir = dir; }
    void Config(const Registry &reg);
    void Config(const TrigConfAlg &alg);

    void AddRobEntry(const TrigMonROB &rob);
    void AddAlgEntry(const TrigMonSeq &seq,
		     const TrigMonAlg &alg,
		     const TrigConfSeq &conf_seq,
		     const TrigConfAlg &conf_alg,
		     const float eta, const float phi);

    void ResetEvent(const EventTime &event, unsigned level);

    void Finalize();

    const std::map<std::string, double>& GetVarMap() const { return fVarMap; }
    
    double GetVar(const std::string &key) const;

    std::string GetVarTitle(const std::string &key) const;

    const std::string GetName() const;

    unsigned GetNameId() const;
    unsigned GetTypeId() const;

    const std::vector<AlgEntry>& GetAlgEntry() const { return fEvent.rois; }
    
    const std::map<unsigned, Count>&  GetChnCount() const { return fCountChn; }
    const std::map<unsigned, Count>&  GetSeqCount() const { return fCountSeq; }

    void WriteHtml(Handle<HtmlNode> node, TCanvas *canvas = 0); //const;   
    
  private:
    
    //
    // Execution summary for one event or all events
    //
    struct Event {
      Event();

      void AddEvent(const Event &rhs);
      void Clear();

      uint64_t n_event;
      uint64_t n_alg_calls;
      uint64_t n_alg_cache;
      uint64_t n_alg_timeout;

      double   timer_1st;
      double   timer_2nd;
      double   timer_sum;
      double   timer_sum2;

      ROBEntry rob_reques;   // All ROB requests
      ROBEntry rob_active;   // Requests with at least one non-cached ROB
      ROBEntry rob_cached;   // Requests with all ROBs cached

      std::vector<AlgEntry> rois;
    };

    //
    // Algorithm information
    //
    struct AlgCorrelation {
    AlgCorrelation() : name(""), nCalls(0), avgTimeCalls(0.0), avgTimeAll(0.0), robCachedReq(0), robCalledReq(0), avgRobTime(0.0) {}
      std::string name;
      unsigned    nCalls;
      float       avgTimeCalls;
      float       avgTimeAll;
      	          
      unsigned    robCachedReq;
      unsigned    robCalledReq;
      float       avgRobTime;
    };

    //
    // Count number of ROB requests and size
    //
    struct CountROB {
      CountROB() : size_ret(0), size_chd(0), nreq(0), nreq_ret(0) {}

      uint64_t size_ret;
      uint64_t size_chd;

      uint64_t nreq;
      uint64_t nreq_ret;
    };

    typedef std::map<uint32_t, CountROB> CountROSMap;

    //
    // Count number of ROB requests and size
    //
    struct CountDet {
      
      CountROB                     subdet;      
      std::map<uint32_t, CountROB> rosmap;
    };
    
    typedef std::map<uint32_t, CountDet> CountDetMap;    

    struct Hists {
      Hists();

      void Set(TDirectory *dir, const Registry &reg);

      void FillAllHist(const ROBEntry &rob, 
		       unsigned run, 
		       unsigned lumi, 
		       double alg_time,
		       float eta = -1.0e6, 
		       float phi = -1.0e6);
      
      void FillROBHist(const ROBEntry &rob);

      void Finalize();

      bool isConfig;

      TH1 *hAlgTime;
      TH1 *hCpuTime;
      TH1 *hROBTime;
      TH1 *hROBNreq;      

      TH1 *hROBSize;       // All ROBs
      TH1 *hROBSizeRetrv;  // Retrieved ROBS
      TH1 *hROBSizeCache;  // Cached robs

      TH1 *hROBNids;       // All ROBs
      TH1 *hROBNidsRetrv;  // Retrieved ROBS
      TH1 *hROBNidsCache;  // Cached robs

      TH1 *hAlgTeta;
      TH1 *hAlgTphi;

      TH2 *hCpuLumi;
      TH2 *hRobLumi;

      TH1 *hROBRatioCache;
      TH1 *hROBRatioRetrv;

      TH1 *hNROBRatioCache;
      TH1 *hNROBRatioRetrv;

      TimeHist *hAlgTimeDyn;
      TimeHist *hCpuTimeDyn;
      TimeHist *hROBTimeDyn;

      enum Type { AlgTimeDyn, CpuTimeDyn, ROBTimeDyn };

      TH1* GetTH1(Type type) const;
    };

    typedef std::map<std::string, double> VarMap;

  private:

    void PrintVar(std::ostream &os,
		  const std::string &key, 
		  const std::string &prf,
		  const std::string &suf) const;
    
    double ConvertWord2kB(unsigned nwords) const;

    void WriteDetCount(HtmlNode &node, const CountDetMap &cmap, const std::string &opt = "") const;
    void WriteROSCount(HtmlNode &node, const CountDetMap &cmap, const std::string &opt = "") const;
   
    void GetROSCount(uint64_t &nreq, uint64_t &nret) const;

  private:

    TDirectory                *fDir;

    bool                       fDebug;       // Print debug info
    bool                       fCanRebin;    // Rebin timing hists
    bool                       fMakeHists;   // Make histograms?
    bool                       fUseType;     // Use algorithm type for comparisons 
    double                     fTimeout;     // Algorithm timeout cutoff

    std::string                fCanvasType;  // Canvas print type: eps or png
    std::string                fHistOpts;    // Histogram options
    std::string                fPrefix;      // Plot name prefix

    std::vector<TrigConfAlg>   fAlgVec;      // Algorithm configuration(s)
    std::set<uint32_t>         fIds;         // Ids of related instances
    
    bool                       fIsConfig;
    bool                       fFillROBIds;  // Fill ROB ids for every event
    unsigned                   fLevel;       // Trigger level for parent chain
    Event                      fEvent;       // Current event measurements
    Event                      fTotal;       // Total run measurements

    VarMap                     fVarMap;      // Variables computed by Finalize()
    std::map<unsigned, Count>  fCountChn;    // Count chns which requested this algorithm
    std::map<unsigned, Count>  fCountSeq;    // Count seqs which requested this algorithm

    SlowEvents                 fSlow;        // List of slow events
    CountDetMap                fSubDet;      // Data requests per sub-detectors/ROS

    TH1                       *hEvtFract;
    TH1                       *hCacheROBNreq;
    TH1                       *hCacheROBSize;
    
    Hists                      fHistEvent;
    Hists                      fHistCalls;
  };
  
  //
  // Helper structure to sort by total time, mean event time, etc
  //
  struct TimeAlgSort {

    TimeAlgSort() :fKey() {}
    explicit TimeAlgSort(const std::string &key) :fKey(key) {}

    bool operator()(const Anp::Handle<Anp::TrigTimeAlg> &lhs,
		    const Anp::Handle<Anp::TrigTimeAlg> &rhs);
    
    private:

    std::string fKey;
  };
}

#endif

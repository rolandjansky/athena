/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_TRIGTIMEROB_H
#define ANP_TRIGTIMEROB_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : TrigTimeROB
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Timing plots for subdetector
 *  - cpu and ROB request timing per call and per event
 *  - plot 0 time for cached objects
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
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/LumiABC.h"

// Local
#include "TrigCostExec/AlgEntry.h"
#include "TrigCostExec/TimeHist.h"

class TDirectory;
class TH1;
class TH2;
class TCanvas;

namespace Anp
{
  class TrigTimeROB: public virtual Base {
  public:
    
    struct Count {
      Count() : ncalls(0), ncache(0) {}
      uint64_t ncalls;
      uint64_t ncache;
    };

  public:

    TrigTimeROB();
    virtual ~TrigTimeROB();

    void Config(TDirectory *dir) { fDir = dir; }
    void Config(const Registry &reg, const uint32_t subdetId);

    void AddROBEntry(const TrigMonROB &rob);

    void AddROSEntry(const TrigMonROB &rob, const std::string algName, const uint32_t rosId);
    
    void ResetEvent();

    const std::string GetName() const;
    const std::string GetROSName() const;
    bool GetReset() const { return fReset; }

    void Finalize();

    const std::map<std::string, float>& GetVarMap() const { return fVarMap; }

    double GetVar(const std::string &key) const;

    std::string GetString(const std::string &key) const;

    void WriteHtml(Handle<HtmlNode> node, TCanvas *canvas = 0) const;
    void WriteROSHtml(Handle<HtmlNode> node, HtmlMenu &hmenu, TCanvas *canvas = 0, const unsigned nEvts =0, const std::set<unsigned> *lbs = 0, const unsigned nROSEvts = 0);
    
  private:

    void PrintVar(std::ostream &os,
		  const std::string &key, 
		  const std::string &pRefreshrf,
		  const std::string &suf) const;
   
    void PrintVar(std::ostream &os,
		  const std::string &key, 
		  const std::string &pRefreshrf,
		  const std::string &suf,
		  const int nEvts) const;
    
  public:

    struct SubDet {
    SubDet() : time(0.0), 
	       requestSize(0.0), 
	       requestSizeCached(0.0), 
       	       retrievSize(0.0),
	       nrobs(0), nrobs_cached(0), 
	       nrobs_retrieved(0), 
	       nrobs_ignored(0), 
	       nrobs_disabled(0), 
	       nTrigMonRobRequests(0), 
	       nTrigMonROB_FullyCached(0),
	       nTrigMonROB_FullyRetriev(0),
	       n_event(0),
	       alg_name("UNKNOWN_ALG") {}

      void Clear();
      void AddSubDet(const SubDet &rhs);
      void AddEvent() { ++n_event; }
      const std::string GetAlgName() {return alg_name; }
      double time;
      float  requestSize;
      float  requestSizeCached;
      float  retrievSize;

      unsigned int nrobs;
      unsigned int nrobs_cached;
      unsigned int nrobs_retrieved;
      unsigned int nrobs_ignored;
      unsigned int nrobs_disabled;
      unsigned int nTrigMonRobRequests;
      unsigned int nTrigMonROB_FullyCached;
      unsigned int nTrigMonROB_FullyRetriev;
      unsigned int n_event;
      std::string alg_name;
    };
    typedef std::map<std::string,float>       VarMap;
    typedef std::map<uint32_t,SubDet>         AlgMap;
    typedef std::map<std::string,std::string> ROSInfoMap;

    SubDet &GetAlg(int32_t algId);
    void calcROS(const SubDet &data, const unsigned nEvts, const float runCoolTime, const float runCoolEvts, const float scale, const float nROSEvts);
    void clearROSVars();
    //bool SubDetSort(const SubDet &lhs,const SubDet &rhs) { if(lhs.time < rhs.time) return true; else return false;}

  private:

    struct Hists {
      Hists();

      void Set(TDirectory *dir, const Registry &reg);

      void Fill(const SubDet &info);

      TH1* GetTH1(const std::string &hname);

      bool        isConfig;      
      TDirectory *dir;

      TH1 *hRobTime;
      TH1 *hCachedSize;
      TH1 *hRequestSize;
      TH1 *hnumRobRequests;
    };

  private:

    TDirectory                *fDir;

    bool                       fDebug;       // Print debug info
    bool                       fCanRebin;    // Rebin timing hists
    bool                       fMakeHists;   // Make histograms?
    bool                       fUseType;     // Use algorithm type for comparisons    
    double                     fTimeout;     // Algorithm timeout cutoff
    uint32_t                   fSubdetId;    // Subdetector ID #

    std::string                fCanvasType;  // Canvas print type: eps or png
    std::string                fHistOpts;    // Histogram options
    std::string                fPrefix;      // Plot name prefix
    std::string                fCoolLumiName;// Scaling algorithm for bunch group scaling
    std::string                fCostLumiName;// Scaling algorithm for bunch group scaling
    Handle<LumiABC>            fCoolLumi;    // Scaling algorithm for bunch group scaling
    Handle<LumiABC>            fCostLumi;    // Scaling algorithm for bunch group scaling

    bool                       fIsConfig;
    bool                       fisGlobalROS; // All ros(es)
    bool                       fReset;       // Need to hit reset
    VarMap                     fVarMap;      // Map to the variables
    AlgMap                     fAlgMap;      // Map Alg to ros request info
    ROSInfoMap                 fROSInfoMap;  // Map ROS request info string to variable name

    
    
    SubDet                     fSubDet;      // Data requests 
    SubDet                     fSubDetTotal; // Total data for all events
    SubDet                     fSubDetEvt;   // Total data for one event
    
    std::set<SubDet *>         fRosReset;    // Reset ros(es) after event is processed

    Hists                      fHistEvent;
    Hists                      fHistCalls;
  };
  
  //
  // Helper structure to sort by total time, mean event time, etc
  //
  struct TimeRobSort {

    TimeRobSort() :fKey() {}
    explicit TimeRobSort(const std::string &key) :fKey(key) {}

    bool operator()(const Anp::Handle<Anp::TrigTimeROB> &lhs,
		    const Anp::Handle<Anp::TrigTimeROB> &rhs);
    
    private:

    std::string fKey;
  };
  //
  // Inlined Functions
  //
  bool operator<(const Anp::TrigTimeROB::SubDet &lhs, const Anp::TrigTimeROB::SubDet &rhs);
}

#endif

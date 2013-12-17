/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_TRIGTIMECHN_H
#define ANP_TRIGTIMECHN_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : TrigTimeChn
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Timing plots for chains
 *  - cpu and ROB request timing per call and per event
 *  - plot separately cached and retreived ROB requests
 *  - additional plots
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <string>
#include <vector>

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/HtmlNode.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostExec/TrigTimeAlg.h"

class TCanvas;
class TDirectory;
class TH1;

namespace Anp
{
  class TrigTimeChn: public virtual Base {
  public:
    
    TrigTimeChn();
    virtual ~TrigTimeChn();
      
    void Config(TDirectory *dir) { fDir = dir; }
    void Config(const Registry &reg, const TrigConfChain &seq);
    
    void AddEntry(const TrigMonSeq &entry);
    void AddEvent(double event_time);

    void Finalize();

    const std::vector<TrigConfChain>& GetChnVec() const { return fChnVec; }
    const std::vector<TrigConfSeq>&   GetSeqVec() const { return fSeqVec; }

    const TrigConfChain&   GetTrigConfChain() const { return fChn; }
    
    const std::map<std::string, double>& GetVarMap() const { return fVarMap; }

    unsigned int GetNEvent() const { return fTotal.n_event; }
    double       GetVar(const std::string &key) const;

    const std::string& GetName() const { return fChn.getName(); }
    const std::string& GetPath() const { return fPath; }
    unsigned int       GetId()   const { return fChn.getId(); }
    
    void WriteHtml(Handle<HtmlNode> node, TCanvas *canvas) const;
    
  private:
    
    void PrintVar(const std::string &key, 
		  const std::string &prf,
		  const std::string &suf,
		  std::ostream &os) const;

    TH1 *FindTH1(const std::string &key);
    
  private:
    
    struct Event {
      Event();
      void AddEvent(const Event &rhs);
      void Clear();

      uint64_t n_event;
      uint64_t n_alg_calls;
      uint64_t n_alg_cache;
      uint64_t n_seq_execs;

      double   timer_1st;
      double   timer_2nd;
      double   timer_sum;
      double   timer_sum2;
    };

    typedef std::map<std::string, TH1 *> HistMap;
    typedef std::map<std::string, double> VarMap;
    typedef std::vector<Anp::TrigTimeAlg> TimeAlgVec;
    
    friend struct TimeAlgSort;

  private:
    
    TDirectory                 *fDir;
    TrigConfChain               fChn;       // Sequence configuration

    std::vector<TrigConfChain>  fChnVec;    // Chain configurations
    std::vector<TrigConfSeq>    fSeqVec;    // Sequence configurations
    
    std::string        fName;      // =seq_name
    std::string        fPath;      // Unique path to instance of this class
    unsigned int       fId;        // =seq_id

    Event              fEvent;     // Current event measurements
    Event              fTotal;     // Total run measurements

    VarMap             fVarMap;    // Variables computed by Finalize()
    TimeAlgVec         fTimeAlg;   // Timing measurements by algorithms

    bool               fDebug;     // Print debug info
    bool               fMakeHists; // Make histograms?
    std::string        fCanvasType;
    
    double             fMaxTimer;  // Longest chain execution time
    TH1               *hChnTimer;
    TH1               *hEvtFract;
    TH1               *hAlgNCalls;
    TH1               *hAlgNCache;

    HistMap            fHist;
  };
  
  //
  // Inlined functions
  //

  bool operator==(const Handle<TrigTimeChn> &lhs, const Handle<TrigTimeChn> &rhs);
  bool operator==(const Handle<TrigTimeChn> &chn, const TrigConfChain &key);
  bool operator==(const TrigConfChain &key, const Handle<TrigTimeChn> &chn);

  bool operator<(const Handle<TrigTimeChn> &lhs, const Handle<TrigTimeChn> &rhs);
  bool operator<(const Handle<TrigTimeChn> &chn, unsigned int key);
  bool operator<(unsigned int key, const Handle<TrigTimeChn> &chn);
  
  struct TimeChnSort {

    TimeChnSort() :fKey() {}
    explicit TimeChnSort(const std::string &key) :fKey(key) {}

    bool operator()(const Anp::Handle<Anp::TrigTimeChn> &lhs,
		    const Anp::Handle<Anp::TrigTimeChn> &rhs);
    
    private:

    std::string fKey;
  };
}

#endif

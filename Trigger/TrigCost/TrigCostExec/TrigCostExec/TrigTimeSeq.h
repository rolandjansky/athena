/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_TRIGTIMESEQ_H
#define ANP_TRIGTIMESEQ_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : TrigTimeSeq
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Timing plots for sequences
 *  - cpu timing per call and per event
 *  - additional plots
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <string>
#include <vector>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"

// Local
#include "TrigCostExec/TrigTimeAlg.h"

class TDirectory;
class TH1;

namespace Anp
{
  class TrigTimeAlg;

  class TrigTimeSeq: public virtual Base {
  public:
    
    TrigTimeSeq();
    virtual ~TrigTimeSeq();

    void Config(TDirectory *dir) { fDir = dir; }      
    void Config(const Registry &reg, const TrigConfSeq &seq);

    void AddEntry(const TrigMonSeq &entry);

    void ResetEvent(const EventTime &event);

    void Finalize();

    const TrigConfSeq& GetConfSeq() const { return fSeq; }
    
    const std::map<std::string, double>& GetVarMap() const { return fVarMap; }
    
    unsigned GetLevel() const { return fLevel; }

    unsigned int GetNEvent() const { return fTotal.n_event; }
    double       GetVar(const std::string &key) const;

    const std::string& GetName() const { return fSeq.getName(); }
    const std::string& GetPath() const { return fPath; }
    unsigned int       GetId()   const { return fSeq.getId(); }
    
    void WriteHtml(Handle<HtmlNode> node, const HtmlMenu &menu) const;
    
    TrigTimeAlg *GetAlgName(unsigned index);
    TrigTimeAlg *GetAlgType(unsigned index);
    
    bool SetAlgName(unsigned index, TrigTimeAlg *alg);
    bool SetAlgType(unsigned index, TrigTimeAlg *alg);

  private:
    
    void PrintVar(const std::string &key, 
		  const std::string &prf,
		  const std::string &suf,
		  std::ostream &os) const;

    TH1 *FindTH1(const std::string &key);
    
  private:
    
    struct Event
    {
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
    TrigConfSeq                 fSeq;       // Sequence configuration
    
    std::string                 fName;      // =seq_name
    std::string                 fPath;      // Unique path to instance of this class

    Event                       fEvent;     // Current event measurements
    Event                       fTotal;     // Total run measurements

    VarMap                      fVarMap;    // Variables computed by Finalize()
    TimeAlgVec                  fTimeAlg;   // Timing measurements by algorithms

    std::vector<TrigTimeAlg *>  fAlgName;
    std::vector<TrigTimeAlg *>  fAlgType;
    
    bool                        fDebug;     // Print debug info
    bool                        fMakeHists; // Make histograms?
    unsigned                    fLevel;     // Sequence level (based on chain level)

    TH1                        *hSeqTimer;
    TH1                        *hEvtFract;
    TH1                        *hSeqNExecs;
    TH1                        *hAlgNCalls;
    TH1                        *hAlgNCache;

    HistMap                    fHist;
  };
  
  //
  // Inlined functions
  //
  bool operator==(const Handle<TrigTimeSeq> &lhs, const Handle<TrigTimeSeq> &rhs);
  bool operator<(const Handle<TrigTimeSeq> &lhs, const Handle<TrigTimeSeq> &rhs);

  bool operator==(const Handle<TrigTimeSeq> &seq, const TrigConfSeq &key);
  bool operator==(const TrigConfSeq &key,         const Handle<TrigTimeSeq> &seq);

  bool operator<(const Handle<TrigTimeSeq> &seq, unsigned int key);
  bool operator<(unsigned int key,               const Handle<TrigTimeSeq> &seq);
  
  struct TimeSeqSort {

    TimeSeqSort() :fKey() {}
    explicit TimeSeqSort(const std::string &key) :fKey(key) {}

    bool operator()(const Anp::Handle<Anp::TrigTimeSeq> &lhs,
		    const Anp::Handle<Anp::TrigTimeSeq> &rhs);
    
    private:

    std::string fKey;
  };
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_REMOVEDCHN_H
#define ANP_REMOVEDCHN_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : RemovedChn
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Hold the sequences, algorithms, and robs deleted by Chain removal
 *  - store seq
       ~ algs
       ~ robs
 *  - write number and time of seq, algs, robs replaced
 *  - write amount not replaced
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
#include "TrigCostExec/TrigTimeAlg.h"
#include "TrigCostExec/TrigTimeChn.h"
#include "TrigCostExec/TrigTimeSeq.h"
#include "TrigCostExec/StudyTime.h"

class TDirectory;
class TH1;
class TH2;
class TCanvas;

namespace Anp
{
  class RemovedChn: public virtual Base {
  public:
    
    struct Count {
      Count() : ncalls(0), ncache(0) {}
      uint64_t ncalls;
      uint64_t ncache;
    };
  public:
    struct rmAlgData {
    rmAlgData() : type(""), name(""), nameId(0), typeId(0), roiId(0), algTime(0.0) {}
      ~rmAlgData() {}
      
      void     getalgdata(const TrigMonAlg &algPointer,TrigTimeSeq *&data, const unsigned roiKey);

      unsigned GetNameId() {return nameId;}
      unsigned GetTypeId() {return typeId;}
      std::string GetName() {return name;}
      std::string GetType() {return type;}

      std::string type;
      std::string name;
      unsigned int nameId;
      unsigned int typeId;
      TrigMonAlg alg;
      unsigned roiId;
      const std::vector<uint8_t>&  getByte() const { return m_byte; }
      const std::vector<uint32_t>& getWord() const { return m_word; }
      double   getTimer() const { return algTime; }
      double algTime;
      std::vector<uint8_t>      m_byte;     // Encoded bytes: position, caching state, rois
      std::vector<uint32_t>     m_word;     // Encoded words: first two entries are timers
    };
  public:
    struct rmRobData {
    rmRobData() : robId(0),start(0),stop(0) {}
      ~rmRobData() {}
      void addrobdata(TrigMonROB &robPointer, const unsigned robKey);
      unsigned   robId;
      unsigned   start;
      unsigned   stop;
      TrigMonROB rob;
    };
  public:
    struct rmSeqData{
    rmSeqData() : seqName(""), seqIndex(0), seqId(0), level(0) {}
      ~rmSeqData() {}
      std::string seqName;

      uint16_t seqIndex;      
      unsigned seqId;
      unsigned level;

      std::vector<TrigMonSeq>    seq;
      std::vector<rmRobData>     vecRmRobData;    // Removed Rob info
      std::vector<rmAlgData>     vecRmAlgData;    // Removed Alg info
      std::vector<rmRobData>     &getRobs() { return vecRmRobData; }
      std::vector<rmAlgData>     &getAlgs() { return vecRmAlgData; }
      
      std::set<uint32_t> depSeqIds;
      std::set<uint32_t> getDepSeqIds() const {return depSeqIds; }
      void Clear();
      void addAlgs(TrigTimeSeq *data);      // add algs from seq
      void addRobs(std::vector<TrigMonROB> &rob_list, TrigTimeSeq *&data); // add TrigMonROBData calls from seq
    };
  public:
    bool algMatchRoi(const TrigMonAlg &alg1, const rmAlgData &alg2, const std::vector<TimeRoiData> &RoiData) const;
    bool algMatchRob(std::vector<TrigMonROB> &rob_list, const TrigMonAlg &alg, const rmRobData &rmRob, TrigTimeSeq *&data) const;
    
    rmSeqData addSeqEntry(const TrigMonSeq &mSeq, TrigTimeSeq *seqData, std::vector<TrigMonROB> &rob_entry);
    void resetSeq(TrigMonSeq &seq, TrigTimeSeq *data); 
    void resetRobs(std::vector<TrigMonROB> &rob_list,const TrigMonAlg &entry, TrigTimeSeq *data); // add back the rob time
    void resetAlgs(std::vector<TrigMonAlg> &resetAlgs, const std::vector<TimeRoiData> &RoiData, TrigTimeSeq *data); // add back the alg time

  public:

    RemovedChn();
    virtual ~RemovedChn();

    void Config(TDirectory *dir) { fDir = dir; }
    void Config(const Registry &reg);
    //void Config(const TrigConfAlg &alg);

    //    void ResetEvent(const EventTime &event, unsigned level);

    void Finalize();

    const std::map<std::string, double>& GetVarMap() const { return fVarMap; }
    
    double GetVar(const std::string &key) const;

    std::string GetVarTitle(const std::string &key) const;

    const std::string GetName() const;

    unsigned GetNameId() const;
    unsigned GetTypeId() const;

    const std::vector<AlgEntry>& GetAlgEntry() const { return fEvent.rois; }
    
    const std::map<unsigned int, Count>&  GetChnCount() const { return fCountChn; }
    const std::map<unsigned int, Count>&  GetSeqCount() const { return fCountSeq; }

    void WriteHtml(Handle<HtmlNode> node, TCanvas *canvas = 0) const;
    
  private:

    void PrintVar(std::ostream &os,
		  const std::string &key, 
		  const std::string &prf,
		  const std::string &suf) const;

    double ConvertWord2kB(unsigned nwords) const;
  public:
    typedef std::map<uint32_t,rmSeqData > SeqMap;

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

      ROBEntry rob_active;
      ROBEntry rob_cached;

      std::vector<AlgEntry> rois;
    };

    struct Hists {
      Hists();

      void Set(TDirectory *dir, const Registry &reg);
      
      //      void Fill(const ROBEntry &rob);

      // void Finalize();

      bool isConfig;

      TH1 *hAlgTime;
      TH1 *hCpuTime;
      TH1 *hROBTime;
      TH1 *hROBNreq;
      TH1 *hROBNids;
      TH1 *hROBSize;

    };

    typedef std::map<std::string, double> VarMap;
    typedef std::map<unsigned, Count> CountMap;

  private:

    TDirectory                *fDir;

    bool                       fDebug;       // Print debug info
    bool                       fCanRebin;    // Rebin timing hists
    bool                       fMakeHists;   // Make histograms?
    double                     fTimeout;     // Algorithm timeout cutoff

    std::string                fCanvasType;  // Canvas print type: eps or png
    std::string                fHistOpts;    // Histogram options
    std::string                fPrefix;      // Plot name prefix

    std::vector<TrigConfAlg>   fAlgVec;      // Algorithm configuration(s)
    std::set<uint32_t>         fIds;         // Ids of related instances
    
    bool                       fIsConfig;
    unsigned                   fLevel;       // Trigger level for parent chain

    VarMap                     fVarMap;      // Variables computed by Finalize()
    CountMap                   fCountChn;    // Count chns which requested this algorithm
    CountMap                   fCountSeq;    // Count seqs which requested this algorithm

    Event                      fTotal;
    Event                      fEvent;
    
  public:
    SeqMap                     fSeqMap;    // Sequences removed by Chain
  };
  
}

#endif

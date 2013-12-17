/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_STUDYTIME_H
#define ANP_STUDYTIME_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : StudyTime
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
  // Helper class for correlating roi and algorithm calls
  //
  struct TimeRoiData {
    TimeRoiData() : valid(false), eta(0.0), phi(0.0), id(0) {}

    void SetRoi(const TrigMonRoi &roi);

    bool      valid;
    float     eta;

    float     phi;
    unsigned  id;
  };

  //
  // Main algorithm for analyzing timing data
  //
  class StudyTime: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    StudyTime();
    virtual ~StudyTime();
    
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
    
    const std::map<uint32_t, Handle<TrigTimeAlg> >& GetAlgName() const { return fAlgName; }
    const std::map<uint32_t, Handle<TrigTimeAlg> >& GetAlgType() const { return fAlgType; }

    const std::vector<Handle<TrigTimeChn> >& GetChn() const { return fChn; }
    const std::vector<Handle<TrigTimeSeq> >& GetSeq() const { return fSeq; }    

  private:

    typedef std::vector<Anp::Handle<Anp::TrigTimeSeq> > SeqVec;
    typedef std::vector<Anp::Handle<Anp::TrigTimeChn> > ChnVec;
    typedef std::map<int,      Anp::Handle<Anp::TrigTimeROB> > ROBMap;
    typedef std::map<uint32_t, Anp::Handle<Anp::TrigTimeAlg> > AlgMap;

  private:
    
    void FillEventHists(const TrigMonEvent &event);

    void WriteAlgs(Handle<HtmlNode> nodeS,
		   HtmlMenu &hmenu,
		   TCanvas *canvas, 
		   std::vector<Handle<TrigTimeAlg> > &algs,
		   const std::string &key, 
		   unsigned nalg);

    void WriteSubDetCalls(Handle<HtmlNode> nodeDET,
			  HtmlMenu &hmenu,
			  TCanvas *canvas,
			  std::vector<Handle<TrigTimeROB> > &algs,
			  const std::string &key);

    void WriteChns(Handle<HtmlNode> node,
		   HtmlMenu &hmenu,
		   TCanvas *canvas, 
		   std::vector<Handle<TrigTimeChn> > &chains,
		   unsigned nchn);

    void MakeAlg(const TrigConfSeq   &seq, const TrigConfAlg &alg);
    void MakeChn(const TrigConfChain &chn);
    void MakeSeq(const TrigConfSeq   &seq);

    std::vector<uint32_t> algMatchRoi(const TrigMonAlg &alg1, const TrigMonEvent &event, std::string key, std::vector<std::string> &algNames);
    
    Handle<TrigTimeROB> GetSubDetROB(int subdet);
    Handle<TrigTimeROB> GetROSROB(int rosid);

    TH1* GetTH1(const std::string &key) const;    
    TH2* GetTH2(const std::string &key) const;

    std::string level() const;

  private:

    TDirectory            *fDir;          // Directory pointer
    Anp::Registry          fReg;          // Registry configuration
    Anp::Registry          fRegIndex;     // Registry configuration for index page
    
    Branch<TrigMonEvent>   fEntry;        // Trigger data
    Handle<ConfigABC>      fConf;         // Configuration algorithm
    
    TrigTimeROB            fTimeROS;      // Total ROS info by alg
    TrigMonConfig          fConfig;       // Local configuration copy

    // Properties:
    std::string            fDirName;      // Directory name
    std::string            fHistOpts;     // Histogram fill and draw options
    std::string            fRootName;     // Name of root file where plots are saved

    bool                   fDebug;        // Print debugging info
    bool                   fIsSubAlg;     // Is sub-algorithm? Parent will manage setup
    bool                   fMakeHists;    // Make histograms?
    bool                   fRunCorr;      // Run the correlation of algorithms

    unsigned               fShowNAlg;     // Print only N slowest algorithms
    unsigned               fLevel;        // Algorithm level
   
    double                 fTimeout;      // Timeout limit for algorithm time
    double                 fTimeYMaxTH2;  // Time limit for TH2 histograms
    int                    fTimeNBinTH2;  // Number of bins for TH2 timing histograms

    // Variables:
    bool                   fIsConfig;     // Is trigger configuration already read?
    double                 fTimeEvent;    // Total processing time from algorithm timer
    double                 fTimeSteer;    // Total processing time from TrigSteer
    float                  fTimeSubDets;  // Total request time for all sub detectors
    
    long                   fCountL2;      // Number of active L2 cost events
    long                   fCountEF;      // Number of active EF cost events
    unsigned int           fnEvents;      // Number of L2 events
    unsigned               fROSEvts;      // Number of events with ROS requests

    AlgMap                 fAlgType;      // Timing for algorithms by type (class)
    AlgMap                 fAlgName;      // Timing for algorithms by name (instance)
    ROBMap                 fROB;          // Timing for ROB requests per sub-detector
    ROBMap                 fROS;          // Timing for ROS requests

    ChnVec                 fChn;          // Timing for chains
    SeqVec                 fSeq;          // Timing for sequences

    EventTime              fEvent;        // Struct to hold event summary
    SlowEvents             fSlow;         // List of slowests events

    std::vector<TrigTimeChn *> fChnData;    // Lookup table for chains
    std::vector<TrigTimeSeq *> fSeqData;    // Lookup table for sequences

    std::vector<TimeRoiData>   fRoiData;    // Store RoI data for event

    std::vector<TrigTimeChn *> fChnReset;   // Reset chains after event is processed
    std::vector<TrigTimeSeq *> fSeqReset;   // Reset   seqs after event is processed
    std::set<TrigTimeROB *>    fRobReset;   // Reset   robs after event is processed
    std::set<TrigTimeROB *>    fRosReset;   // Reset   ros(es) after event is processed
    std::set<unsigned>         flb;         // Store all lb accessed

    TH1 *hTimeSteer;
    TH1 *hTimeSteerOpi;
    TH1 *hTimeSteerOff;
    TH1 *hTimeCost;
    TH1 *hTimeCpus;
    TH1 *hTimeRobs;
    TH1 *hTimeCostTimer;

    struct LumiHist {
      LumiHist() : hLumiCost(0), hLumiCpus(0), hLumiRobs(0) {}
      
      void Config(const Registry &reg, TDirectory *dir);

      TH1 *hLumiCost;
      TH1 *hLumiCpus;
      TH1 *hLumiRobs;
    };
    
    std::map<unsigned, LumiHist> fLumis;
  };
}

#endif

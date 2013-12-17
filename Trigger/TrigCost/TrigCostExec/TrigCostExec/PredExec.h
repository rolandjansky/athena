/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_PREDEXEC_H
#define ANP_PREDEXEC_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : PredExec
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
#include "TrigCostExec/StudyTime.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostExec/RemovedChn.h"

class TH1;
class TH2;

namespace Anp
{
  //
  // Main algorithm for analyzing timing data
  //
  class PredExec: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    PredExec();
    virtual ~PredExec();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    void Set(const TrigMonConfig &event);
    void Run(TrigMonEvent &event);


    //
    // functions that may be needed
    //
    const std::vector<TrigConfChain>& GetChnVec() { return fConfig.getVec<TrigConfChain>(); }
    const std::vector<TrigConfSeq>&   GetSeqVec() { return fConfig.getVec<TrigConfSeq>(); }
    
    const std::map<uint32_t, Handle<TrigTimeAlg> >& GetAlgName() const { return fAlgName; }
    const std::map<uint32_t, Handle<TrigTimeAlg> >& GetAlgType() const { return fAlgType; }

    const std::vector<Handle<TrigTimeChn> >& GetChn() const { return fChn; }
    const std::vector<Handle<TrigTimeSeq> >& GetSeq() const { return fSeq; } 
  




  private:

    typedef std::vector<Anp::Handle<Anp::TrigTimeSeq> > SeqVec;
    typedef std::vector<Anp::Handle<Anp::TrigTimeChn> > ChnVec;
    typedef std::map<uint32_t, Anp::Handle<Anp::TrigTimeAlg> > AlgMap;

  private:
    //
    // Functions i may need
    //
    void MakeAlg(const TrigConfSeq   &seq, const TrigConfAlg &alg);
    void MakeChn(const TrigConfChain &chn);
    void MakeSeq(const TrigConfSeq   &seq);
    std::string getSeqStatus(const TrigMonSeq   &seq) const;
    //std::string getChnStatus(const TrigMonChain &chn) const;
    //std::string getChnStatus(const uint16_t encoded) const;
    std::string getAlgStatus(const TrigMonAlg   &alg) const;
    std::string level() const;

    void RemoveChain(const TrigConfChain &chain, 
		     std::vector<TrigMonSeq> &seq_list,
		     std::vector<TrigMonROB> &rob_list) const;
    void resetCachedSeq(std::vector<TrigMonSeq> &seq_list, std::vector<TrigMonSeq>::iterator &riter, RemovedChn &dat, std::vector<TrigMonSeq>::iterator curriter) const;
    std::vector<uint32_t> getDepSeqId(const unsigned index) const;

  private:

    Anp::Registry          fReg;          // Registry configuration
    Anp::Registry          fRegIndex;     // Registry configuration for index page
    
    Branch<TrigMonEvent>   fEntry;        // Trigger data
    Handle<ConfigABC>      fConf;         // Configuration algorithm

    TrigMonConfig                fConfig;       // Local configuration copy
    // Properties:
    std::vector<TrigConfChain>   frmChn;        // The chain to be removed
    std::vector<TrigConfChain>   actChn;        // List of Active chains for the Trigger Menu

    // Properties:
    bool                   fDebug;        // Print debugging info
    bool                   fIsSubAlg;     // Is sub-algorithm? Parent will manage setup
    bool                   fhasL2;        // Alg level to write webpage
    bool                   fhasEF;        // Alg level to write webpage

    unsigned               fLevel;        // Algorithm level
    std::string            frmChnName;    // removed chain
    std::set<std::string>  frmChnSet;     // removed chain set

    // Variables:
    bool                   fIsConfig;     // Is trigger configuration already read?
    
    unsigned int           fnEvents;      // Number of L2 events
    unsigned int           fRun;          // Run Number

    //
    // Vars I may need
    //

    AlgMap                 fAlgType;      // Timing for algorithms by type (class)
    AlgMap                 fAlgName;      // Timing for algorithms by name (instance)

    ChnVec                 fChn;          // Timing for chains
    SeqVec                 fSeq;          // Timing for sequences    

    std::vector<TrigTimeChn *> fChnData;  // Lookup table for chains
    std::vector<TrigTimeSeq *> fSeqData;  // Lookup table for sequences
    std::vector<TimeRoiData>   fRoiData;  // Store RoI data for event

    Handle<HtmlNode> WriteNode;           //(new HtmlNode("pred_exec_L2.html", HtmlNode::kHTML));

  };
}

#endif

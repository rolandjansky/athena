/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLESLOW_H
#define ANP_EXECTABLESLOW_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableSlow
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Timing information for slow events
 *  - chain and alg timing
 *  - 
 *
 **********************************************************************************/

// C/C++
#include <set>
#include <map>
#include <string>
#include <vector>
#include "TRandom2.h"

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/HtmlNode.h"

// Local
#include "TrigCostExec/ExecBase.h"

class TCanvas;

namespace Anp
{
  class ExecTableSlow: public virtual ExecClient {
  public:

    ExecTableSlow();
    virtual ~ExecTableSlow();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *);
    
    void ProcessAlg(const Exec::AlgCall &alg);

    void ProcessSlowAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);

    void SaveEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &anode, HtmlMenu &hmenu, const Exec::Summary &sum);    

  private:

    typedef std::map<uint32_t, Handle<ExecClient> >  ExecMap;
    typedef std::vector<Handle<ExecClient> >         EvtVec;
    typedef std::vector<Exec::AlgCall>               AlgVec;
    typedef std::vector<ExecClient *>                ClientVec;
    
  public:

    //
    // Store the Slow event information
    //
    struct SlowEvt{
      SlowEvt() {}

      void Clear();

      Exec::Event        sEvt;
      EvtVec             totEvt;
      AlgVec             AlgCalls;

      ExecMap            sROS;
      ExecMap            sChn;
      ExecMap            sAlg;
    };

    typedef std::vector<SlowEvt> SlowEvtVec;
    typedef std::map<uint32_t, Ptr<TrigConfSeq> > SeqConfMap;

    void ProcessEvent(SlowEvt &se);

  private:

    void WriteEvt(     HtmlNode &nodeE, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);
    void WriteMonEvent(HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);
    void WriteTxtEvent(HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);
    void WriteXMLEvent(HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);
    void WriteAlgCall( HtmlNode &anode, HtmlMenu &hmenu, const Exec::AlgCall &alg, const Exec::Summary &sum);
    void WriteTxtCall( HtmlNode &anode, HtmlMenu &hmenu, const Exec::AlgCall &alg, const Exec::Summary &sum);
    void WriteXMLCall( HtmlNode &anode, HtmlMenu &hmenu, const Exec::AlgCall &alg, const Exec::Summary &sum);
    void WriteChn(     HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);
    void WriteAlg(     HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);
    void WriteDet(     HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);
    void WriteROS(     HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt,      const Exec::Summary &sum);

    void GetEvtClient(const Exec::Event &event);
    ExecClient* GetAlgClient(const TrigConfAlg &alg_conf);
    ExecClient* GetChnClient(const TrigConfChain &chn_conf);
    ExecClient* GetROSClient(unsigned ros_id, const Exec::AlgCall &alg);
    Exec::AlgCall GetAlgCall(const TrigMonAlg &alg,const TrigMonSeq &seq, const SlowEvt &sevt);

    void resetCachedSeq(std::vector<TrigMonSeq> &seq_list, unsigned &seq_pos, TrigMonSeq currSeq, const Exec::Summary &sum);
    std::vector<uint32_t> getDepSeqId(const unsigned index, const Exec::Summary &sum) const;
    std::string getROSCalls(const Exec::AlgCall &alg, const Exec::Summary &sum) const;

  private:

    TDirectory                *fDir;
    TCanvas                   *fCanvas;
    Registry                   fReg;

    bool                       fDebug;
    bool                       fFillNode;
    bool                       fMakeHist;
    bool                       fWriteSeq;
    bool                       fCollectSlow;
    bool                       fDetailSpecific;
    bool                       fWriteTxt;    
    bool                       fWriteXML;
    double                     fTimeout;
    std::string                fHistOpt;
    std::string                fDirName;
    unsigned                   fnSlow;
    unsigned                   fnProc;
    unsigned                   fnEvent;

    TRandom2                   fRanSel;

    Exec::Count                fCount;

    SlowEvtVec                 fSlowVec;
    SlowEvt                    fSlowEvt;
    AlgVec                     fAlg; 
    ClientVec                  fActiveClients;
    SeqConfMap                 fSeqConfMap;
  };

  //
  // Sorting functions
  //
  struct SortExecSlow {

    SortExecSlow(const std::string &key = "") :fKey(key) {}

    bool operator()(const ExecTableSlow::SlowEvt &lhs, const ExecTableSlow::SlowEvt &rhs) const;
    
    public:

    std::string fKey;
  };
}

#endif

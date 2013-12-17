/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLESEQ_H
#define ANP_EXECTABLESEQ_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableSeq
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Timing plots for sequences
 *  - cpu and ROB request timing per call and per event
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
#include "TrigCostExec/ExecBase.h"

class TCanvas;

namespace Anp
{
  class ExecTableSeq: public virtual ExecClient {
  public:

    ExecTableSeq();
    virtual ~ExecTableSeq();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &, HtmlMenu &hmenu, const Exec::Summary &sum);    

  private:

    void WriteSeq(HtmlNode &anode, const ExecClient &seq, const Exec::Summary &sum);

    ExecClient* GetSeqClient(const TrigConfSeq &seq);

  private:

    typedef std::map<uint32_t, Handle<ExecClient> > SeqMap;
    typedef std::vector<ExecClient *> ClientVec;
    
  private:

    TDirectory                *fDir;
    TCanvas                   *fCanvas;
    Registry                   fReg;

    bool                       fDebug;
    bool                       fFillNode;
    bool                       fMakeHist;
    double                     fTimeout;
    std::string                fHistOpt;
    std::string                fDirName;

    Exec::Count                fCount;

    SeqMap                     fSeq;    
    ClientVec                  fActiveClients;
  };
}

#endif

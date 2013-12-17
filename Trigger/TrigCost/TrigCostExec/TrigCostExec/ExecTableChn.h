/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLECHN_H
#define ANP_EXECTABLECHN_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableChn
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Timing plots for chains and sequences
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
  class ExecTableChn: public virtual ExecClient {
  public:

    ExecTableChn();
    virtual ~ExecTableChn();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &, HtmlMenu &hmenu, const Exec::Summary &sum);    

  private:

    void WriteChn(HtmlNode &anode, const ExecClient &chn, const Exec::Summary &sum);
    void WriteROS(HtmlNode &rnode, const ExecClient &chn, const Exec::Summary &sum);

    ExecClient* GetChnClient(const TrigConfChain &chn);

  private:

    typedef std::map<uint32_t, Handle<ExecClient> > ChnMap;
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

    ChnMap                     fChn;    
    ClientVec                  fActiveClients;
  };
}

#endif

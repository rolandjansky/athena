/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEALG_H
#define ANP_EXECTABLEALG_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableAlg
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
#include "TrigCostExec/ExecBase.h"

class TCanvas;

namespace Anp
{
  class ExecTableAlg: public virtual ExecClient {
  public:

    ExecTableAlg();
    virtual ~ExecTableAlg();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &, HtmlMenu &hmenu, const Exec::Summary &sum);    

  private:

    void WriteAlg(HtmlNode &anode, const ExecClient &alg, const Exec::Summary &sum);
    void WriteDet(HtmlNode &dnode, const ExecClient &alg, const Exec::Summary &sum);
    void WriteROS(HtmlNode &rnode, const ExecClient &alg, const Exec::Summary &sum);
    void WriteChn(HtmlNode &cnode, const ExecClient &alg, const Exec::Summary &sum);

    ExecClient* GetAlgClient(const TrigConfAlg &alg);

  private:

    typedef std::map<uint32_t, Handle<ExecClient> > AlgMap;
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

    AlgMap                     fAlg;    
    ClientVec                  fActiveClients;
  };
}

#endif

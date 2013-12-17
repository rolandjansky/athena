/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEEVT_H
#define ANP_EXECTABLEEVT_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableEvt
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Study the CPU Usage of the HLT Farms
 *  - collect total event processing time for L2 and/or EF from steering and cost
 *  - timing analysis for the farm nodes
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
#include "TrigCostBase/ScaleABC.h"

// Local
#include "TrigCostExec/ExecBase.h"

class TCanvas;

namespace Anp
{
  class ExecTableEvt: public virtual ExecClient {
  public:

    ExecTableEvt();
    virtual ~ExecTableEvt();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &, HtmlMenu &hmenu, const Exec::Summary &sum);    

  private:

    TH1* GetTH1(const std::string &key, const std::string &name = "") const;
    
  private:

    TDirectory                 *fDir;
    TCanvas                    *fCanvas;
    Registry                    fReg;

    Handle<ScaleABC>           fScaleBG;        // BG scaling algorithm

    bool                        fDebug;
    unsigned                    fLevel;
    unsigned                    fnROBRet;
    bool                        fMakeHist;
    double                      fTimeout;

    std::string                 fHistOpt;
    std::string                 fDirName;
    std::string                 fHistKey;

    TH1*                        fSteerTime;
    TH1*                        fAlgTime;
    TH1*                        fROBTime;
    TH1*                        fCPUTime;
    TH1*                        fROBRet;
    TH1*                        fAlgCall;
    TH1*                        fAlgCach;

    Exec::Count                 fCount;
  };
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SURVEYLBS_H
#define ANP_SURVEYLBS_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : SurveyLBs
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Timing plots for ROBs and ROSes
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
#include "TrigCostBase/LumiABC.h"

// Local
#include "TrigCostExec/ExecBase.h"

class TCanvas;

namespace Anp
{
  class SurveyLBs: public virtual ExecClient {
  public:

    SurveyLBs();
    virtual ~SurveyLBs();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum);

  private:
    
    //    TH1* MakeHist(const std::string &hname, const std::string &yaxis, 
    //		  unsigned min_lb, unsigned max_lb);
    
    std::ostream& log() const;

  private:

    TDirectory                *fDir;
    TCanvas                   *fCanvas;

    // Properties:
    bool                       fDebug;
    bool                       fMakeHist;

    unsigned                   fLevel;  

    std::string                fDirName;
    std::string                fHistOpt;
  };
}

#endif

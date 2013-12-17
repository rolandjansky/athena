/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEFARM_H
#define ANP_EXECTABLEFARM_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableFarm
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

// Local
#include "TrigCostExec/ExecBase.h"

class TCanvas;

namespace Anp
{
  class ExecTableFarm: public virtual ExecClient {
  public:

    ExecTableFarm();
    virtual ~ExecTableFarm();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &, HtmlMenu &hmenu, const Exec::Summary &sum);    

  private:

    struct LumiTime{
    LumiTime() : timer_steer(0.0),
	timer_alg(0.0),
	timer_rob(0.0),
	run(0),
	lb(0),
	nCost_evt(0) {}
      
      void AddEvent(const Exec::Event &event);
    
      double timer_steer;
      double timer_alg;
      double timer_rob;
      
      unsigned run;
      unsigned lb;
      unsigned nCost_evt;
    };

    typedef std::map<uint32_t, LumiTime>       LumiMap;
    typedef std::map<uint32_t, LumiMap>        RunMap;
    typedef std::map<uint32_t, Exec::FarmHist> FarmHistMap;

  private:

    LumiTime* GetLumiTime(const Exec::Event &event);

    TH1* MakeHist(const std::string &hname, const std::string &yaxis, 
		  const unsigned min_lb, const unsigned max_lb);
    
  private:

    TDirectory                 *fDir;
    TCanvas                    *fCanvas;
    Registry                    fReg;

    bool                        fDebug;
    unsigned                    fLevel;
    bool                        fMakeHist;
    double                      fTimeout;

    std::string                 fHistOpt;
    std::string                 fDirName;

    Exec::Count                 fCount;
    FarmHistMap                 hFarmMap;

    RunMap                      fRun;    
  };
}

#endif

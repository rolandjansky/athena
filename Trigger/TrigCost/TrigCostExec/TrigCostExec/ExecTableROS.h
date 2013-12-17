/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEROS_H
#define ANP_EXECTABLEROS_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableROS
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
  class ExecTableROS: public virtual ExecClient {
  public:

    ExecTableROS();
    virtual ~ExecTableROS();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum);

  private:
    
    ExecClient* GetROSClient(uint32_t ros_id, const Exec::AlgCall &alg);

    void WriteROS(HtmlNode &rnode, const ExecClient  &ros, const Exec::Summary &sum);

    void WriteAlg(HtmlNode &anode, const Exec::Count &alg, const Exec::Summary &sum);
    
    void WriteLBs(HtmlNode &rnode, const ExecClient &exec, const Exec::Summary &sum);
    
    TH1* MakeHist(const std::string &hname, const std::string &yaxis, 
		  unsigned min_lb, unsigned max_lb, TDirectory *dir);
    
    std::ostream& log() const;

  private:

    typedef std::map<uint32_t, Handle<ExecClient> > ROSMap;
    typedef std::vector<ExecClient *> ClientVec;
    
  private:

    TDirectory                *fDir;
    TCanvas                   *fCanvas;
    Registry                   fReg;

    // Properties:
    bool                       fDebug;
    bool                       fFillNode;
    bool                       fMakeHist;
    bool                       fPlots;
    double                     fTimeout;

    std::string                fDirName;
    std::string                fHistOpt;
    
    // Variables:
    Exec::Count                fCount;

    ROSMap                     fROS;    
    ClientVec                  fActiveClients;

  };
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEROBIN_H
#define ANP_EXECTABLEROBIN_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableROBIN
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Timing plots for ROBINs
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
  class ExecTableROBIN: public virtual ExecClient {
  public:

    ExecTableROBIN();
    virtual ~ExecTableROBIN();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum);

  private:
    
    ExecClient* GetROBINClient(uint32_t robin_id, const Exec::AlgCall &alg);

    void WriteROBIN(HtmlNode &rnode, const ExecClient  &robin, const Exec::Summary &sum);

    void WriteAlg(HtmlNode &anode, const Exec::Count &alg, const Exec::Summary &sum);
    
    std::ostream& log() const;

  private:

    typedef std::map<uint32_t, Handle<ExecClient> > ROBINMap;
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

    ROBINMap                   fRobin;    
    ClientVec                  fActiveClients;

  };
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEDET_H
#define ANP_EXECTABLEDET_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableDet
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Read out information for the sub detectors
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
  class ExecTableDet: public virtual ExecClient {
  public:

    ExecTableDet();
    virtual ~ExecTableDet();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum);

  private:

    typedef std::map<uint32_t, Exec::Count> group_map;

  private:

    void AddGroup(uint32_t group_id, const Exec::Count &count);

    void WriteDet(HtmlNode &anode, const group_map &det_map, const Exec::Summary &sum, const std::string &KEY);

    TH1* MakeHist(const std::string &hname, const std::string &yaxis, 
		  unsigned min_lb, unsigned max_lb, TDirectory *dir);
    
    std::ostream& log() const;
    
  private:

    TDirectory                *fDir;
    TCanvas                   *fCanvas;
    Registry                   fReg;

    // Properties:
    bool                       fDebug;
    bool                       fFillNode;
    bool                       fMakeHist;

    std::string                fDirName;
    std::string                fHistOpt;
    
    unsigned                   fEvents;

    // Variables:
    Exec::Count                fCount;

    Handle<ExecClient>         fExecDet;
    group_map                  fGroupMap;
    
  };
}

#endif

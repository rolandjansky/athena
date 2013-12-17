/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

	#ifndef ANP_EXECTABLETE_H
#define ANP_EXECTABLETE_H

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
  class ExecTableTE: public virtual ExecClient {
  public:

    ExecTableTE();
    virtual ~ExecTableTE();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &/*event*/);

    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &, HtmlMenu &hmenu, const Exec::Summary &/*sum*/);    
  private:
    typedef std::vector<ExecClient *> ClientVec;

   
  private:

    TDirectory                          	*fDir;
    TCanvas                              	*fCanvas;
    Registry                            	fReg;
    bool                                	fDebug;
    unsigned                            	fLevel;
    bool                                	fMakeHist;
    double                              	fTimeout;
    std::string                        	 	fHistOpt;
    std::string                         	fDirName;

    Exec::Count                         	fCount;
    std::vector<Exec::AlgCall>          	fTEs;
    std::map<unsigned,unsigned>         	fCountTEs;
    std::vector<std::vector<std::string> >	fCombinedTriggers;
    std::vector<int>           		        fPassCount;
//    Handle<ScaleABC>          	 		fScaleBG;        // BG scaling algorithm
    int 					fSimplecounter;
    ClientVec                  			fActiveClients;
    //vector<TrigConfChain>			fTEConfChains;
	
  };
}

#endif

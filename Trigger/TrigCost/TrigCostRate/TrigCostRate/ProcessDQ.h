/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_PROCESSDQ_H
#define ANP_PROCESSDQ_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : ProcessDQ
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Algorithm for selecting events which pass data quality info:
 *  - good run number
 *  - lumi block range
 *  - bunch crossing ids
 *  - data quality info is read from XML files
 *
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostRate/LumiBlock.h"
#include "TrigCostBase/HtmlABC.h"

namespace Anp
{
  struct LumiRange {
    LumiRange() : run(-1), lb_beg(-1), lb_end(-1) {}
    
    bool MatchLB(int lb) const;
    
    int run;
    int lb_beg;
    int lb_end;        
  };
  
  struct RunDQ {
    RunDQ() : run(-1) {}
    
    void Print() const;

    int run;
    
    std::vector<LumiRange> lumi;
    std::set<unsigned>     bxid;
  };

  class ProcessDQ: 
    public virtual AlgEvent, 
    public virtual HtmlABC {
  public:

    ProcessDQ();
    virtual ~ProcessDQ();
    
    void Config(const Registry &reg);

    bool Init();

    void Exec();

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

  private:

    bool Run(const TrigMonEvent  &event);

    void ReadXML(const std::string &path);

  private:

    typedef std::map<unsigned, RunDQ> LumiMap;

  private:    
    
    Anp::Registry               fRegIndex;      // Registry configuration for index page
    Branch<TrigMonEvent>        fEntry;         // Trigger data
    
    // Properties
    bool                        fDebug;         // Print debugging info
    bool                        fIsActive;      // Active state with successful configuration
    bool                        fIsData;        // Running on data events - nothing to do for MC
    bool                        fSelectBX;      // Select bunch crossing ids
    bool                        fSelectLB;      // Select lumi blocks

    // Variables
    unsigned                    fCountPass;     // Count passed events
    unsigned                    fCountFail;     // Count failed events
    unsigned                    fCountFailBX;   // Count failed events with bad bunch id 
    unsigned                    fCountFailLB;   // Count failed events with bad lumi block
    unsigned                    fCountFailRun;  // Count failed events with bad run
    unsigned                    fCountTotal;    // Count all events

    std::string                 fFileXML;       // Name of input XML file with data quality
    LumiMap                     fLumi;

    double                      fuseSubset;
  };
}

#endif

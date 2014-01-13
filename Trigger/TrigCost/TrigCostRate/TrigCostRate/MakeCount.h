/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_MAKECOUNT_H
#define ANP_MAKECOUNT_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : MakeCount
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Algorithm to count trigger decisions per lumi block
 *  - collect counts for L1 items and HLT chains for each lb
 *  - normalize by the highest rate chain to the cool rates
 *
 **********************************************************************************/

// C/C++
#include <string>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostBase/NtupleSvc.h"

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

class TH1;

namespace Anp
{
  //
  // Main algorithm for analyzing timing data
  //
  class MakeCount: public virtual AlgEvent, public virtual HtmlABC, public virtual LumiABC {
  public:
    
    MakeCount();
    virtual ~MakeCount();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);
    
    void Save(TDirectory *dir);

    bool Init();

    void Exec();

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    long double GetIntegratedTime() const { return 0.0; }
    long double GetIntegratedLumi() const { return 0.0; }
    long double GetMeanInstanLumi() const { return 0.0; }
    long double GetRateCorrection(unsigned level = 0) const;

  private:

    void Set(const TrigMonConfig &event);
    void Run(const TrigMonEvent &event);
    
    Lumi::BlockInfo& GetCurrentLumiBlock(unsigned run, unsigned lb);

    void FillRateScaleCorr(const std::vector<Lumi::BlockInfo> &cool_infos);

    void FillRateScaleCorr(Lumi::BlockInfo &this_info, const Lumi::BlockInfo &cool_info);

    const Lumi::LV1Count* GetHighestRateLV1Count(const Lumi::BlockInfo &info) const;
    const Lumi::HLTCount* GetHighestRateHLTCount(const Lumi::BlockInfo &info, unsigned level) const;

    void FillHistBCID(HtmlNode &bnode, const std::vector<TH1 *> &hists);
    
    char* VarName(const char* var, const std::string &chn_name);
    void WriteNtuple();
    void InitNtuple ();

    TH1* FindHistBCID(unsigned level, unsigned counter);
    TH1* MakeHistBCID(unsigned level, unsigned counter, std::vector<TH1 *> &hists);

  private:

    TDirectory                 *fDir;
    Anp::Registry               fRegIndex;        // Registry configuration for index page
    			     
    Branch<TrigMonEvent>        fEntry;           // Trigger data
    Handle<ConfigABC>           fConf;            // Configuration algorithm
    Handle<LumiABC>             fCoolLumi;        // Scaling algorithm for bunch group scaling
			     
    // Properties:	     
    bool                        fDebug;           // Print debugging info
    bool                        fFillHistBCID;    // Write trigger counts per BCID?
    std::string                 fCoolLumiName;    // Scaling algorithm for bunch group scaling
    std::string                 fHistKey;

    // Variables:
    TrigMonConfig               fConfig;          // Local configuration copy
    bool                        fIsConfig;        // Did we read trigger configuration?
    bool                        fMakeRatesNtup;   // Make Output rate ntuple

    unsigned                    fNEvents;         // Number of processed events
    unsigned                    fNSwitch;         // Number of index switches
    unsigned                    fCurrIndex;       // Current index for fInfos vector       

    std::vector<TH1 *>          fBCIDHistL1;
    std::vector<TH1 *>          fBCIDHistL2;
    std::vector<TH1 *>          fBCIDHistEF;


    double                      fCountCoolL1;
    double                      fCountCoolL2;
    double                      fCountCoolEF;
    double                      fCountThisL1;
    double                      fCountThisL2;
    double                      fCountThisEF;
    double                      fTotalDuration;

    //
    // Writing a flat ntuple for trigger rates
    //
    TFile                      *fRatesNtup;         // Trigger rates file
    TTree                      *fBL1;               // L1 trigger tree
    TTree                      *fBL2;               // L2 trigger tree
    TTree                      *fBEF;               // EF trigger tree
    float                       fEventVars[13];     // Event variables
    std::vector<TBranch*>       fBranches;
  };  
}

#endif

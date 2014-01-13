/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_READCOOL_H
#define ANP_READCOOL_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : ReadCool
 * @Author : Rustem Ospanov, Doug Schaefer
 *
 * @Brief  : 
 *
 * Algorithm for reading COOL data for rates job
 *  - read time length of lumi blocks
 *  - read number of processed L1, L2 and EF events per lumi block
 *  - read luminosity information
 *  - write summary data to web pages
 *
 **********************************************************************************/

// C/C++
#include <string>

// Trigger
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostRate/CoolDB.h"

class TCanvas;

namespace Anp
{
  class ReadCool: public virtual AlgEvent, public virtual LumiABC,  public virtual HtmlABC {
  public:

    ReadCool();
    virtual ~ReadCool();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    void Save(TDirectory *dir);

    bool Init();

    void Exec();

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    long double GetIntegratedTime() const;
    long double GetIntegratedLumi() const;
    long double GetMeanInstanLumi() const;
    long double GetRateCorrection(unsigned level = 0) const; 

  private:

    void Run(unsigned run, unsigned lumi);

    void ReadRunData(unsigned run, std::map<unsigned, Lumi::BlockInfo> &infos);
    
    void ReadLV1Counts(cool::IObjectIteratorPtr &it, 
		       std::map<unsigned, Lumi::BlockInfo> &infos);

    void ReadHLTCounts(unsigned level, 
		       cool::IObjectIteratorPtr &it, 
		       std::map<unsigned, Lumi::BlockInfo> &infos);
    
    void ReadLumiPerBX(cool::IObjectIteratorPtr &it);

    void WriteSummary(std::ostream &os) const;

    void WriteTable(HtmlNode &inode) const;

    void FillLifetimeFraction(const Lumi::BlockInfo &b, HtmlNode &node) const;

    bool MatchRunLB(unsigned run, unsigned lb) const;

    TH1 *GetTH1(const std::string &key, const std::string &name = "") const;

    std::string GetItemBG(unsigned ctpid) const;

  private:

    typedef std::map<unsigned, std::set<unsigned> >  RunLumiMap;

  private:

    TCanvas                    *fCanvas;
    TDirectory                 *fDir;

    Anp::Registry               fRegIndex;       // Registry configuration for index page

    Branch<TrigMonEvent>        fEntry;          // Trigger data
    CoolDB                      fCoolTrigDB;     // Wrapper for COOL database pointer
    CoolDB                      fCoolTDAQDB;     // Wrapper for COOL database pointer
    
    Handle<ConfigABC>           fConfAlg;        // Configuration algorithm

    // Properties:
    bool                        fDebug;          // Print debugging info?
    bool                        fFillBGHist;     // Fill histograms per bunch group?
    bool                        fReadCounts;     // Read trigger counts from cool
    bool                        fReadBCIDs;      // Read BCID information

    std::string                 fCoolTrigId;     // COOL connection id
    std::string                 fCoolTDAQId;     // COOL connection id for TDAQ
    std::string                 fDirName;
    std::string                 fHistOpt;
    
    unsigned                    fLumiChan;
    unsigned                    fLumiChanBCID;

    unsigned                    fConfigRun;      // Run number from configuration
    unsigned                    fConfigLB;       // Lumi block number from configuration
    unsigned                    fMinCount;       // Minimum number of L1 events for lifetime calc

    // Variables
    unsigned                    fCountEvents;    // Count number of processed TrigMonEvent
    unsigned                    fRun;            // Current run number        
    unsigned                    fLumi;           // Current lumi block
    std::vector<unsigned>       fRunList;        // List of all run number previously called        

    RunLumiMap                  fRunMap;         // Map run number -> set of lumi     
  };
}

#endif

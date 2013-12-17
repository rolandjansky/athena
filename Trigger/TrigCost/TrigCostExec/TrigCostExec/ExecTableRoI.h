/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEROI_H
#define ANP_EXECTABLEROI_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTableRoI
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Plots for ROB requests and other variables per RoI
 *  - cpu and ROB request times per RoI
 *  - ROB request size per RoI and event
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
  class ExecTableRoI: public virtual ExecClient {
  public:

    ExecTableRoI();
    virtual ~ExecTableRoI();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum);

  private:

    typedef std::pair<double,  unsigned> RoIPair;
    typedef std::pair<unsigned,unsigned> RoICount;

  public:

    struct RoICounts {
      RoICounts() : counts(0),size(0.0) {}
      ~RoICounts() {}
      
      unsigned counts;
      float    size;
    };

    struct THClients {

      // Vector of the RoI threshold counts with an entry per LBx
      std::vector<RoICounts> RoIThreshCounts;
      Handle<ExecClient>     roi_client;
      TH1*                   hRoISize;
      TH1*                   hRoICounts;
    };

    struct Clients {
      Handle<ExecClient>                      roi_client;
      std::map<unsigned, Handle<ExecClient> > det_clients;
      // Vector of Thresholds
      std::vector<THClients>                  th_clients;
    };

    struct TrigTimeSum {

      TrigTimeSum();

      void AddRoITime(const unsigned &lb,const double &time, const bool MakeRoIPlot);

      // vector of LBs with the alg time per RoI and the number of RoI in the pair
      std::vector<RoIPair> LBTimeAndRoI;

      std::string name;

      TH1* hRoITime;
      TH1* hLBAvgRoITime;
      TH1* hLBTime;

    };

  private:
    
    ExecClient* GetRoIClient(const TrigConfChain &chn);

    Clients& FindClients(const std::set<uint32_t> &rois, const Exec::AlgCall &alg);
    
    Handle<ExecClient> BookClient(std::string name, unsigned detector, TDirectory *dir) const;

    void AddRoICounts(const unsigned &lb, const unsigned nRoI);
    
    TH1* GetTH1(   const std::string &key, const std::string &name = "") const;
    TH1* GetTrigTH1(const std::string &type="", const std::string &name="") const;

    TrigTimeSum* GetTrigTimeSum(const unsigned &trigid, const std::string &type="", const std::string &name="");

    std::ostream& log() const;    

  private:

    typedef std::map<std::set<uint32_t>, std::vector<Exec::AlgCall> > RoIMap;
    typedef std::map<uint32_t, Clients> ClientMap;
    typedef std::map<uint32_t, std::vector<Exec::AlgCall> > SingleRoIMap;
    typedef std::map<uint32_t, TrigTimeSum> SingleRoITrigMap;
    typedef std::map<uint32_t, std::pair<std::string,double> > RoITimeMap;

  private:

    TDirectory                *fDir;
    TDirectory                *fDirAlg;
    TDirectory                *fDirChn;
    TCanvas                   *fCanvas;
    Registry                   fReg;

    // Properties:
    bool                       fDebug;
    bool                       fMakeHist;
    bool                       fMakeRoIPlot;

    std::string                fDirName;
    std::string                fHistKey;
    std::string                fHistOpt;
    std::string                fAlgLink;
    std::string                fChnLink;
    
    // Variables:
    Exec::Count                fCount;
    RoIMap                     fRoIMap;
    SingleRoIMap               fSingleRoIMap;

    SingleRoITrigMap           fAlgRoIMap;
    SingleRoITrigMap           fChnRoIMap;
    RoITimeMap                 fAlgRoItMap;
    RoITimeMap                 fChnRoItMap;

    Handle<ExecClient>         fRoI;

    std::vector<RoICount>      fRoICount;        // Number of (events,RoIs) per Lumiblock

    // Histograms
    TH1                       *hNRoIPerCall;     // Number of RoIs per call
    TH1                       *hNRoIPerEvent;    // Number of RoIs per event    
    TH1                       *hAlgTimePerRoI;   // Alg time per RoI    
    TH1                       *hLBnRoI;          // Number of active HLT RoIs 
    
    ClientMap                  fClients;
  };
}

#endif

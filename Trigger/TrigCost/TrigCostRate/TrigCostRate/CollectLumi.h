/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_COLLECTLUMI_H
#define ANP_COLLECTLUMI_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : CollectLumi
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *
 * Simple algorithm for collecting luminosity block information from data events:
 *  - computes a number of events
 *  - computes a duration of read lumi blocks
 *
 * Algorithm is used for trigger rates normalization
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
  class HostInfo {
  public:
    
    HostInfo();
    HostInfo(const std::string &name, unsigned id);
    ~HostInfo() {}

    struct Lumi {
      Lumi() : nevent_cost(0), nevent_hist(0) {}

      unsigned nevent_cost;
      unsigned nevent_hist;
    };
    
    std::vector<Lumi> fLumi;
    std::string       fName;
    unsigned          fId;
  };

  class CollectLumi: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    CollectLumi();
    virtual ~CollectLumi();
    
    void Config(const Registry &reg);

    void Save(TDirectory *dir);

    bool Init();

    void Exec();

    void Done();

    void Set(const TrigMonConfig &config);
    void Run(const TrigMonConfig &config);
    void Run(const TrigMonEvent  &event);

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    long double GetTotalTime() const { return fTotalTime; }
    long double GetTotalLumi() const { return fTotalLumi; }
    long double GetScaleCorr() const;
    long double GetPredLumi() const { return 0.0; }


  private:
    
    struct Count {
      Count() : count_read(0), count_estm(0) {}

      unsigned count_read;
      unsigned count_estm;
    };

    struct Host {
      Host() {}
      explicit Host(const std::string &h) :host(h) {}

      std::string        host;  // host/application name
      std::vector<Count> lumi;  // luminosity block count
    };

    struct RunData {
      std::map<uint32_t, Host>      hosts;
      std::map<uint32_t, HostInfo>  infos;
      std::vector<Anp::LumiBlock>   lumis;
    };

    typedef std::map<uint32_t, Host>     HostMap;
    typedef std::map<uint32_t, HostInfo> InfoMap;
    typedef std::map<unsigned, RunData>  RunMap;
    typedef std::set<std::pair<uint32_t, uint32_t> > TimeMap;
    
  private:

    void CountHost(unsigned host_id, const TrigMonEvent &event);

  private:    
    
    Anp::Registry               fRegIndex;      // Registry configuration for index page
    TDirectory                 *fDir;           // Output directory

    Branch<TrigMonConfig>       fConfg;         // Trigger configuration
    Branch<TrigMonEvent>        fEntry;         // Trigger data
    
    // Properties:
    bool                        fDebug;         // Print debugging info
    bool                        fPrintLumi;     // Print summary
    bool                        fWriteHtml;     // Write html page?

    std::string                 fLumiChain;     // Chain name for luminosity measurement
    unsigned                    fLumiEncodedId; // Chain encoded id for luminosity measurement
    
    // Variables:
    bool                        fIsConfig;      // Run configuration step?
    long double                 fTotalLumi;     // Total luminosty for data collection period
    long double                 fTotalTime;     // Total time period for data collection period    
    long                        fCountEvents;   // Total number of events
    long                        fCountRead;     // Total number of read events
    long                        fCountEstm;     // Total number of estimated events 
    long                        fCountBuffers;  // Total number of SFO events

    RunMap                      fRuns;          // Lumi block and host data for runs
    TimeMap                     fTime;          // All L1 time stamps    

    TH1                        *hDiffTime;
    TH1                        *hDiffTimeC;
  };
}

#endif

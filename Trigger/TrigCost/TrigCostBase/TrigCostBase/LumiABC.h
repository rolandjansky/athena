/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_LUMIABC_H
#define ANP_LUMIABC_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : LumiABC
 * @Author : Rustem Ospanov, Doug Schaefer
 *
 * @Brief  : Interface class for accessing luminosity and rate normalization data
 * 
 **********************************************************************************/

// C/C+
#include <map>
#include <vector>

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/HtmlNode.h"
#include "TrigCostBase/Ptr.h"

class TrigMonConfig;

namespace Anp
{
  namespace Lumi {

    struct LV1Count {
      LV1Count();

      unsigned    ctp_id;
      float       ctp_ps;

      uint64_t count_tbp;
      uint64_t count_tap;
      uint64_t count_tav;
    };

    struct HLTCount {
      HLTCount();

      unsigned    chain_counter;
      unsigned    chain_level;
      float       chain_ps;
      float       chain_pt;

      unsigned    count_after_ps;
      unsigned    count_input;
      unsigned    count_total;
      unsigned    count_raw;
    };

    struct BlockInfo {

      BlockInfo();
      
      double GetLBDuration() const;

      std::string GetBegTimeAsString() const;
      std::string GetEndTimeAsString() const;
      
      const Lumi::LV1Count* GetL1Count(unsigned ctp_id)  const;
      const Lumi::HLTCount* GetL2Count(unsigned counter) const;
      const Lumi::HLTCount* GetEFCount(unsigned counter) const;
      
      void RemoveNullCountPointers();

      unsigned run;            // Run number 
      unsigned lb;             // Lumi block number
      unsigned bcid;           // BCID

      uint64_t lumiBegTime;    // LB start timestamp
      uint64_t lumiEndTime;    // LB end   timestamp
      
      uint32_t lumiValid;      // Luminosity measurement status 
      double   lumiInstan;     // Average instantenous luminosity

      double   lifetimeFrac;   // Live data taking fraction = 1-deadtime fraction
      double   scaleCorrL1;    // Scale correction to convert L1 counts per lb to Hz
      double   scaleCorrL2;    // Scale correction to convert L2 counts per lb to Hz
      double   scaleCorrEF;    // Scale correction to convert EF counts per lb to Hz
      
      uint64_t L1accept;       // Number of L1 accepted events
      uint64_t L2accept;       // Number of L2 accepted events
      uint64_t EFaccept;       // Number of EF accepted events

      std::vector<Ptr<LV1Count> > countsL1;
      std::vector<Ptr<HLTCount> > countsL2;
      std::vector<Ptr<HLTCount> > countsEF;
    };   
  }

  //
  // Abstract base class for luminosity and normalization data
  //
  class LumiABC: public virtual Base {
  public:
    
    LumiABC() {}
    virtual ~LumiABC() {}   

    //
    // New interface functions
    //
    virtual long double GetIntegratedTime() const = 0;
    virtual long double GetIntegratedLumi() const = 0;
    virtual long double GetMeanInstanLumi() const = 0;
    virtual long double GetRateCorrection(unsigned level = 0) const = 0;
    
    virtual const std::vector<Lumi::BlockInfo>& GetInfos() const { return fInfos; }

    //
    // Base class helper functions
    //
    bool FillBlockInfo(unsigned run, unsigned lb, Lumi::BlockInfo &info) const;

    void WriteInfoTable(HtmlNode &inode) const;
    void WriteHLTCounts(HtmlNode &cnode, const Lumi::BlockInfo &b, unsigned level) const;
    void WriteLV1Counts(HtmlNode &cnode, const Lumi::BlockInfo &b) const;

    bool FindTrigName(unsigned level, unsigned counter, std::string &name) const;    

  protected:

    void FillTrigNames(const TrigMonConfig &config);

  protected:
        
    std::vector<Lumi::BlockInfo>     fInfos;
    std::map<uint16_t, std::string>  fNameMap;
  };
}

#endif

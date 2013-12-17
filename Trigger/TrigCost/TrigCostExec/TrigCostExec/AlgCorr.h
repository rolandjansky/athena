/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_ALGCORR_H
#define ANP_ALGCORR_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : AlgCorr
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Correlations between algorithms run on the same ROI
 *  - cpu and ROB request timing per ROI
 *  - write correlations to a file to be read in by PredExec
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

// Local
#include "TrigCostExec/AlgEntry.h"

class TDirectory;

namespace Anp
{
  class AlgCorr: public virtual Base {
  public:

    AlgCorr();
    virtual ~AlgCorr();

    void Config(TDirectory *dir) { fDir = dir; }
    void Config(const Registry &reg);
    void Config(const TrigConfAlg &alg);


    void AddAlgsRunOnRoi(const std::vector<uint32_t> &algIds, const TrigMonAlg &alg, const std::vector<std::string> &algnames);

    const std::string GetName() const;

    unsigned GetNameId() const;
    unsigned GetTypeId() const;

    void WriteHtml(Handle<HtmlNode> node, TCanvas *canvas = 0); //const;   
    
  private:
    //
    // Algorithm information
    //
    struct AlgCorrelation {
    AlgCorrelation() : name(""), nCalls(0), avgTimeCalls(0.0), avgTimeAll(0.0), robCachedReq(0), robCalledReq(0), avgRobTime(0.0) {}
      std::string name;
      unsigned    nCalls;
      float       avgTimeCalls;
      float       avgTimeAll;
      	          
      unsigned    robCachedReq;
      unsigned    robCalledReq;
      float       avgRobTime;
    };
    typedef std::map<uint32_t, AlgCorrelation> CpuAlgMap;

  private:

    TDirectory                *fDir;

    bool                       fDebug;       // Print debug info
    bool                       fUseType;     // Use algorithm type for comparisons 
    bool                       fRunCorr;     // Run Correlation between algorithms

    bool                       fIsConfig;
    unsigned                   fLevel;       // Trigger level for parent chain

    std::vector<TrigConfAlg>   fAlgVec;      // Algorithm configuration(s)
    std::set<uint32_t>         fIds;         // Ids of related instances

    CpuAlgMap                  fCpuAlgMap;   // maps the average cpu time of this algorithm when the mapped alg also runs on the roi

  };
}

#endif


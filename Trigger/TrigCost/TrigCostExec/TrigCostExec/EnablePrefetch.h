/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_ENABLEPREFETCH_H
#define ANP_ENABLEPREFETCH_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : EnablePrefetch
 * @Author : Pieter
 *
 * @Brief  : 
 *
 * Algorithm for Disabling Prefetching and trying new and improved Prefetching
 *
 **********************************************************************************/

// C/C++
#include <string>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostBase/ConfigABC.h"

namespace Anp
{

  //
  // Main algorithm for analyzing timing data
  //
  class EnablePrefetch: public virtual AlgEvent {
  public:
    
    EnablePrefetch();
    virtual ~EnablePrefetch();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    void Save(TDirectory *dir);

    bool Init();

    void Exec();

    void Done();

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    void Set(const TrigMonConfig &event);
    void Run(TrigMonEvent &event);

    const std::vector<TrigConfChain>& GetChnVec() { return fConfig.getVec<TrigConfChain>(); }
    const std::vector<TrigConfSeq>&   GetSeqVec() { return fConfig.getVec<TrigConfSeq>(); }
    
  private:
    
    typedef std::map<unsigned,std::vector<TrigMonROBData> > MapPFetROS_ROB;

  private:

    std::string level() const;

    void FillPFetMap(const TrigMonROB& pfetROB);

    const std::vector<TrigMonROBData> *GetROBidsPFet(const unsigned ros_id);

    bool CheckAndRemoveROBPFet(const unsigned rob_id);

    void RemovePrefetching(TrigMonEvent &event);

    void NewPrefetching(TrigMonEvent &event);

    void PrefetchImprovement(TrigMonEvent &Event, const std::string &name);
         
  private:

    TDirectory            *fDir;          // Directory pointer
    Anp::Registry          fReg;          // Registry configuration
    Anp::Registry          fRegIndex;     // Registry configuration for index page
    
    Branch<TrigMonEvent>   fEntry;        // Trigger data
    Handle<ConfigABC>      fConf;         // Configuration algorithm
    
    TrigMonConfig          fConfig;       // Local configuration copy

    // Properties:
    std::string            fDirName;      // Directory name
    std::string            fRootName;     // Name of root file where plots are saved
    std::string            fType;         // Flag to disable or try new ROS prefetching 

    bool                   fDebug;        // Print debugging info

    // Variables:
    bool                   fIsConfig;     // Is trigger configuration already read?
    unsigned               fLevel;        // Triggering level

    MapPFetROS_ROB         fPreFetMap;    // Map of prefetched ROB ids to the ROS
  };
}

#endif

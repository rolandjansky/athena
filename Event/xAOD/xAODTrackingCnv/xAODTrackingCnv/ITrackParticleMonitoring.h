/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _IMON_TrigInDetTrackMonitoringTool_H
#define _IMON_TrigInDetTrackMonitoringTool_H

// Normal STL and physical vectors
#include <vector>
// Gaudi includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//xAODs 
#include "xAODTracking/TrackParticleContainer.h"



/* Interface to the inner detector trigger track monitoring */
class ITrackParticleMonitoring : virtual public IAlgTool{
public:
   DeclareInterfaceID (ITrackParticleMonitoring, 1, 0);


   /* Monitor track parameters via mon tool, prefix and suffix allows to switch between different Track collections if needed */
   virtual void monitor_tracks(const std::string &prefix, const std::string &suffix, const xAOD::TrackParticleContainer &tracks ) const = 0;

};


#endif

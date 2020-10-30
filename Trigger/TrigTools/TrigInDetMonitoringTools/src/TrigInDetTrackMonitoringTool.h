/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MON_TrigInDetTrackMonitoringTool_H
#define _MON_TrigInDetTrackMonitoringTool_H

#include "AthenaBaseComps/AthAlgTool.h"
//New athenaMT monitoring
#include "AthenaMonitoringKernel/Monitored.h"
//Implementation of the interface
#include "xAODTrackingCnv/ITrackParticleMonitoring.h"

class TrigInDetTrackMonitoringTool : public AthAlgTool, virtual public ITrackParticleMonitoring {
/**
 * This class implements the monitoring for the online track (xAODTrackParticles to be precise) parameters  
 *
 * @author Matous Vozak     <matous.vozak@cern.ch>
 *
 */
public:

  TrigInDetTrackMonitoringTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigInDetTrackMonitoringTool();

  virtual StatusCode initialize() final;
  virtual StatusCode finalize()   final;

  void monitor_tracks(const std::string &prefix, const std::string &suffix, const xAOD::TrackParticleContainer &tracks ) const;

private:
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonitoringTool","","Monitoring tool"};

};


#endif

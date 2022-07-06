/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalTrackMonAlg.h
 * Implementation of inner detector global track monitoring tool
 *
 *@author
 * Leonid Serkin <lserkin@cern.ch> @n
 * 
 * based on InDetGlobalTrackMonTool.h 
 * 
 ****************************************************************************/

#ifndef InDetGlobalTrackMonAlg_H
#define InDetGlobalTrackMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "GaudiKernel/EventContext.h"

#include "PixelGeoModel/IBLParameterSvc.h"


//Detector Managers
#include "StoreGate/ReadHandleKey.h"


//------------TrackMon------------
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticleContainer.h"

//Standard c++
#include <string>
#include <memory>

//------------------------------

namespace InDet {
  class IInDetTrackSelectionTool;
}


//namespace Trk {
//	  class ITrackSummaryTool;
//}


class InDetGlobalTrackMonAlg : public AthMonitorAlgorithm {
  
 public:
  
  InDetGlobalTrackMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~InDetGlobalTrackMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;
  
 private:
  
  ToolHandle <InDet::IInDetTrackSelectionTool> m_trackSelTool; // baseline
  ToolHandle <InDet::IInDetTrackSelectionTool > m_tight_trackSelTool; //tightw
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleName{this, "TrackParticleContainerName", "InDetTrackParticles","TrackParticle Collection for Global Monitoring"};
  
  ServiceHandle <IBLParameterSvc> m_IBLParameterSvc;

  bool m_doIBL;
  
};
#endif

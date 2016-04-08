/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_TRT_DETELROAD_TOOL_H__
#define __TRIG_TRT_DETELROAD_TOOL_H__

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class TrkTrackState;
}

namespace InDetDD {
  class TRT_DetectorManager;
}

class TRT_ID;
class TrigTRT_DetElementRoad;
class TrigTRT_TrackExtensionGeometry;
class TrigMagneticFieldTool;

class TrigTRT_DetElementRoadTool : public AthAlgTool {
public:

  TrigTRT_DetElementRoadTool( const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent );
  ~TrigTRT_DetElementRoadTool();

  static const InterfaceID& interfaceID();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  TrigTRT_DetElementRoad* m_buildTRT_Road(Trk::TrkTrackState*);

private:
  TrigTRT_TrackExtensionGeometry* m_trtGeo;
  const InDetDD::TRT_DetectorManager* m_trtMgr;
  const TRT_ID* m_trtHelper;
  double m_noiseProbabilityBarrel,m_noiseProbabilityEndcap,
    m_hitEfficiency,m_hitResolution, m_gateBarrel,m_gateEndcap,
    m_associationThreshold;
  bool m_makeWireHits;
  ToolHandle<TrigMagneticFieldTool> m_trigFieldTool;
};

#endif

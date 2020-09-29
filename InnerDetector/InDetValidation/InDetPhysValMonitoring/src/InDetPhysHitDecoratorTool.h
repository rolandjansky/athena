/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETPHYSVALMONITORING_InDetPhysHitDecoratorTool_H
#define INDETPHYSVALMONITORING_InDetPhysHitDecoratorTool_H
/**
 * @file InDetPhysHitDecoratorTool.h
 * header file for class of same name
 * @author shaun roe
 * @date 27 March 2014
 **/
// STL includes
#include <string>
#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

class Identifier;
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace Trk {
  class IUpdator;
  class ITrackHoleSearchTool;
  class IResidualPullCalculator;
}



// class to decorate xAOD::TruthParticles with additional information required by validation
class InDetPhysHitDecoratorTool: virtual public IInDetPhysValDecoratorTool, public AthAlgTool {
public:
  // L0PIXBARR : layer0 of pixel barrel (IBL in Run2)
  // PIXEL : in barrel all layers appart from layer0. In end-caps all disks/rings.
  enum Subdetector {
    INVALID_DETECTOR=-1, L0PIXBARR, PIXEL, SCT, TRT, DBM, N_SUBDETECTORS
  };
  enum Region {
    INVALID_REGION=-1, BARREL, ENDCAP
  };
  enum LayerType {
    INVALID_LAYER=-1, INNERMOST, NEXT_TO_INNERMOST, OTHER, N_TYPES
  };
  
  InDetPhysHitDecoratorTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual
  ~InDetPhysHitDecoratorTool ();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual bool decorateTrack(const xAOD::TrackParticle& particle, const std::string& prefix);
private:
  ToolHandle<Trk::ITrackHoleSearchTool>    m_holeSearchTool;
  ToolHandle<Trk::IUpdator>    m_updatorHandle; // !< Tool handle of updator for unbiased states
  ToolHandle<Trk::IResidualPullCalculator>  m_residualPullCalculator;   // !< The residual and pull calculator tool
                                                                        // handle
  const double m_ptThreshold;
  bool m_isUnbiased;
  bool m_doUpgrade;
  bool m_useNewITkLayerNumbering;
  // the following help identify a surface in the detector
  const AtlasDetectorID* m_idHelper;
  const PixelID* m_pixelID;
  const SCT_ID* m_sctID;
  const TRT_ID* m_trtID;

  // private member functions
  bool decideDetectorRegion(const Identifier& id, Subdetector& det, Region& r, int& layer, LayerType& lType);
  const Trk::TrackParameters*
  getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement);
};

#endif

/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigFTKFastSimTruth_h
#define TrigFTKFastSimTruth_h

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/WriteHandleKey.h"

#include "TrigFTKSim/FTK_SGHitInput.h"
#include "TrigFTKSim/FTKClusteringEngine.h"

#include "TrigFTKFastSimTruth/FTKTruthTrajectory.h"

#include "TrigFTKToolInterfaces/ITrigFTKSectorMatchTool.h"
#include "TrigFTKToolInterfaces/ITrigFTKTrackBuilderTool.h"

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

#include <memory>
#include <string>


class FTK_RawTrack;

class TrigFTKFastSimTruth : public AthAlgorithm
{
public:
  TrigFTKFastSimTruth (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigFTKFastSimTruth() = default;
  StatusCode initialize() override;
  StatusCode execute() override;

private:
  std::unique_ptr<FTK_RawTrack> SimToRaw(const FTKTrack &track) const;
  std::vector<FTKTruthTrajectory> buildTruthTrajectories(const std::vector<FTKRawHit>& hits) const;

private:
  // Raw Data output (RDO)
  SG::WriteHandleKey<FTK_RawTrackContainer> m_ftkRawTrackContainerKey;

  ToolHandle<FTK_SGHitInputI> m_hitInputTool;

  std::unique_ptr<FTKClusteringEngine> m_clusteringEngine;

  ToolHandle<ITrigFTKSectorMatchTool> m_sectorMatchTool;
  ToolHandle<ITrigFTKTrackBuilderTool> m_trackBuilderTool;

  const PixelID *m_pixelId;
  const SCT_ID *m_sctId;

  // variables to manage the distribution of the hits
  int m_IBLMode;                               // global FTK setup variable to handle IBL
  std::string m_pmap_path_8L;                  // path of the PMAP file
  std::unique_ptr<FTKPlaneMap> m_pmap_8L;      // pointer to the pmap object
  std::string m_pmap_path_12L;                 // path of the PMAP file
  std::unique_ptr<FTKPlaneMap> m_pmap_12L;     // pointer to the pmap object


  // clustering parameters
  bool m_clustering;                  // flag to enable the clustering
  bool m_saveClusterContent;          // flag to allow the debug of the cluster content
  bool m_diagClustering;              // true if diagonal clustering is used
  bool m_sctClustering;               // true if SCT clustering is performed
  unsigned int m_pixelClusteringMode; // 1 means ToT information is used
                                      //   && 400/600um pixels are accounted for
                                      // 0 is default used up to 2013
  bool m_ibl3DRealistic;              // If true, use HWlike clustering in IBL 3D mod
  bool m_duplicateGanged;
  bool m_gangedPatternRecognition;

  bool m_splitBlayerModules; // true if the BLayer modules are splitted

  float m_trainingBeamspotX;
  float m_trainingBeamspotY;
  float m_trainingBeamspotTiltX;
  float m_trainingBeamspotTiltY;
  bool m_reverseIBLlocx;

};

#endif // TrigFTKFastSimTruth_h

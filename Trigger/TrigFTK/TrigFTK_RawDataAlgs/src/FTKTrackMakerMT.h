/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigFTK_RawDataAlgs_FTKTrackMakerMT_h
#define TrigFTK_RawDataAlgs_FTKTrackMakerMT_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkTrack/TrackCollection.h" 

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"
#include "FTK_RecToolInterfaces/IFTK_DuplicateTrackRemovalTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"

/////////////////////////////////////////////////////////////////////////////

class PixelID;
class SCT_ID;

class FTKTrackMakerMT: public AthAlgorithm {
public:
  FTKTrackMakerMT (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKTrackMakerMT ();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  // Private methods
  const Trk::RIO_OnTrack* createSCT_Cluster(const FTK_RawSCT_Cluster& raw_cluster, const Trk::TrackParameters& trkPerigee, InDet::SCT_ClusterContainer* sctClusterContainer, const InDetDD::SiDetectorElementCollection* sctElements, const SCT_ID* sctidHelper) const;
  const Trk::RIO_OnTrack*  createPixelCluster(const FTK_RawPixelCluster& raw_pixel_cluster,  const Trk::TrackParameters& trkPerigee,InDet::PixelClusterContainer* pixelClusterContainer, const InDetDD::SiDetectorElementCollection* pixelElements, const PixelID* pixidHelper) const;
  InDet::PixelClusterCollection* getPixelClusterCollection(IdentifierHash hashId, InDet::PixelClusterContainer* pixelClusterContainer, const PixelID* pixidHelper) const;
  InDet::SCT_ClusterCollection* getSCT_ClusterCollection(IdentifierHash hashId,InDet::SCT_ClusterContainer* sctClusterContainer, const SCT_ID* pixidHelper) const;

  //  static inline bool compareFTK_Clusters (const Trk::RIO_OnTrack* cl1, const Trk::RIO_OnTrack* cl2) {
    
  // return (cl1->globalPosition().mag()<cl2->globalPosition().mag());
    
  //}



 private:

  // Read, Write & Cond Handles as Properties
  SG::ReadCondHandleKey<PixelCalib::PixelOfflineCalibData> m_clusterErrorKey{this, "PixelOfflineCalibData", "PixelOfflineCalibData", "Output key of pixel cluster"};

  SG::ReadHandleKey<FTK_RawTrackContainer> m_rdoContainerKey{this, "DataObjectName", "FTK_RDO_RawTrack", "FTK RDOs"};
  SG::WriteHandleKey<InDet::PixelClusterContainer> m_pixelClusterContainerKey{this, "PixelClustersName", "FTK_PixelClusterContainer", "Pixel cluster container"};
  SG::WriteHandleKey<InDet::SiClusterContainer> m_pixClusterContainerLinkKey{this, "PixelClustersLinkName_", "FTK_PixelClusterContainer", "Pixel cluster container link name (don't set this)"};

  SG::WriteHandleKey<InDet::SCT_ClusterContainer> m_sctClusterContainerKey{this, "SCTClustersName", "FTK_SCT_ClusterContainer", "SCT cluster container"};
  SG::WriteHandleKey<InDet::SiClusterContainer> m_sctClusterContainerLinkKey{this, "SCTClustersLinkName_", "FTK_SCT_ClusterContainer", "SCT cluster container link name (don't set this)"};
  SG::WriteHandleKey<TrackCollection> m_trackCollectionKey{this, "TracksName", "Tracks", "Track container"};

// ToolHandles as Properties
  ToolHandle<IFTK_UncertaintyTool> m_uncertaintyTool{this, "UncertaintyTool", "UncertaintyTool","Tool to provide track parameter errors"};
  ToolHandle< Trk::IRIO_OnTrackCreator > m_ROTcreator{this,"ROTcreatorTool","Trk::IRIO_OnTrackCreator/FTK_ROTcreatorTool","Tool to create RIO On Track"};
  ToolHandle< IFTK_DuplicateTrackRemovalTool > m_DuplicateTrackRemovalTool{this,"DuplicateTrackRemovalTool","DuplicateTrackRemovalTool","Remove track duplicates in overlap region of FTK towers"};
  ToolHandle<ISiLorentzAngleTool> m_pixelLorentzAngleTool{this, "PixelLorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retrieve Lorentz angle of Pixel"};
  ToolHandle<ISiLorentzAngleTool> m_sctLorentzAngleTool{this, "SCTLorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retrieve Lorentz angle of SCT"};

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};



  // Algorithm properties

  Gaudi::Property<double> m_trainingBeamspotX{this,"TrainingBeamspotX", 0., "x coordinate of beamline used for FTK track parameters"};
  Gaudi::Property<double> m_trainingBeamspotY{this,"TrainingBeamspotY", 0., "y coordinate of beamline used for FTK track parameters"};
  Gaudi::Property<double> m_trainingBeamspotZ{this,"TrainingBeamspotZ", 0., "z coordinate of beamline used for FTK track parameters"};
  Gaudi::Property<double> m_trainingBeamspotTiltX{this,"TrainingBeamspotTiltX", 0., "x Tilt of beamline used for FTK track parameters"};
  Gaudi::Property<double> m_trainingBeamspotTiltY{this,"TrainingBeamspotTiltY", 0., "y Tilt of beamline used for FTK track parameters"};
  Gaudi::Property<double> m_pTscaleFactor{this,"pTscaleFactor", 1., "ScaleFactor for FTK pT"};
  Gaudi::Property<std::vector<float> > m_pixelBarrelPhiOffsets{this,"PixelBarrelPhiOffsets",{0.,0.,0.,0.}," Pixel Barrel Phi Offsets in mm" };
  Gaudi::Property<std::vector<float> > m_pixelBarrelEtaOffsets{this,"PixelBarrelEtaOffsets",{0.,0.,0.,0.}," Pixel Barrel Eta Offsets in mm" };
  Gaudi::Property<std::vector<float> > m_pixelEndCapPhiOffsets{this,"PixelEndCapPhiOffsets",{0.,0.,0.}," Pixel EndCap Phi Offsets in mm" };
  Gaudi::Property<std::vector<float> > m_pixelEndCapEtaOffsets{this,"PixelEndCapEtaOffsets",{0.,0.,0.}," Pixel EndCap Eta Offsets in mm" };
  Gaudi::Property<bool> m_correctPixelClusters {this,"CorrectPixelClusters",true,"Correct Pixel cluster positions using the ROTcreatorTool"};
  Gaudi::Property<bool> m_correctSCTClusters {this,"CorrectSCTClusters",true,"Correct SCT cluster positions using the ROTcreatorTool"};
  Gaudi::Property<bool> m_broadPixelErrors{this,"setBroadPixelClusterOnTrackErrors",false," set isBroad as parameter of PixelClusterOnTrack"};
  Gaudi::Property<bool> m_broadSCT_Errors{this,"setBroadSCT_ClusterOnTrackErrors",false," set isBroad as parameter of SCT_ClusterOnTrack"};
  Gaudi::Property<bool> m_reverseIBLlocx{this,"ReverseIBLlocX",false,"reverse the direction of IBL locX from FTK"};
  Gaudi::Property<bool> m_remove_duplicates{this,"RemoveDuplicates",true,"Remove suplicate tracks"};

};


#endif // TrigFTK_RawDataAlgs_FTKTrackMakerMT_h

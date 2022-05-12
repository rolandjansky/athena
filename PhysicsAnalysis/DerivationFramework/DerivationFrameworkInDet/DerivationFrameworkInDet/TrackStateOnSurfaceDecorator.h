/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToVertexWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKSTATEONSURFACEDECORATOR_H
#define DERIVATIONFRAMEWORK_TRACKSTATEONSURFACEDECORATOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthLinks/ElementLink.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "TRT_ElectronPidTools/ITRT_ToT_dEdx.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "CommissionEvent/ComTime.h"

#include "TrkEventUtils/PRDtoTrackMap.h"

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace Trk {
  class IUpdator;
  class PrepRawData;
}

namespace DerivationFramework {

  class TrackStateOnSurfaceDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      TrackStateOnSurfaceDecorator(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      
      
      ElementLink< xAOD::TrackMeasurementValidationContainer > buildElementLink( const Trk::PrepRawData*, 
                                                                  const std::vector<unsigned int>*, 
                                                                  const xAOD::TrackMeasurementValidationContainer* ) const;

      // --- Steering and configuration flags
      bool    m_isSimulation;
      
      bool    m_storeHoles;
      bool    m_storeOutliers;
      bool    m_storeTRT;
      bool    m_storeSCT;
      bool    m_storePixel;
      bool    m_addPulls;
      bool    m_addSurfaceInfo;
      bool    m_addPRD;
      bool    m_addExtraEventInfo;

      // --- Configuration keys
      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey
        { this, "EventInfoKey", "EventInfo", "" };
      Gaudi::Property<std::string> m_sgName
         { this, "DecorationPrefix", "IDDET1_",""};
      SG::ReadHandleKey<xAOD::TrackParticleContainer> m_containerName
         { this, "ContainerName", "InDetTrackParticles", "" };
      SG::ReadHandleKey<ComTime> m_trtPhaseKey
         { this,"TRTPhaseKey","TRT_Phase", ""};

      SG::ReadHandleKey<std::vector<unsigned int> > m_pixelMapName
         { this, "PixelMapName", "PixelClustersOffsets" , ""};
      SG::ReadHandleKey<std::vector<unsigned int> >  m_sctMapName
         { this, "SctMapName",   "SCT_ClustersOffsets" , ""};
      SG::ReadHandleKey<std::vector<unsigned int> >  m_trtMapName
         { this, "TrtMapName",   "TRT_DriftCirclesOffsets" , ""};

      SG::ReadHandleKey<xAOD::TrackMeasurementValidationContainer > m_pixelClustersName
         {this, "PixelClustersName", "PixelClusters" ,"" };
      SG::ReadHandleKey<xAOD::TrackMeasurementValidationContainer > m_sctClustersName
        {this, "SctClustersName", "SCT_Clusters" ,"" };
      SG::ReadHandleKey<xAOD::TrackMeasurementValidationContainer> m_trtDCName
         {this, "TrtDriftCirclesName", "TRT_DriftCircles" ,"" };

      SG::ReadHandleKey<Trk::PRDtoTrackMap> m_prdToTrackMap
         { this,"PRDtoTrackMap","","option PRD-to-track association"};

      SG::WriteHandleKey<xAOD::TrackStateValidationContainer> m_pixelMsosName
         { this, "PixelMsosName", "PixelMSOSs", "" };
      SG::WriteHandleKey<xAOD::TrackStateValidationContainer> m_sctMsosName
         { this, "SctMsosName", "SCT_MSOSs", "" };
      SG::WriteHandleKey<xAOD::TrackStateValidationContainer> m_trtMsosName
         { this, "TrtMsosName",  "TRT_MSOSs", ""};

      // --- Read Cond Handle Key
      // For P->T converter of SCT_Clusters
      SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

     
      // --- Services and tools
      const AtlasDetectorID* m_idHelper;
      const PixelID*         m_pixId; 
      const SCT_ID*          m_sctId;
      const TRT_ID*          m_trtId;

      ToolHandle<Trk::IUpdator>                 m_updator;
      ToolHandle<Trk::IResidualPullCalculator>  m_residualPullCalculator;
      ToolHandle<Trk::ITrackHoleSearchTool>     m_holeSearchTool;
      ToolHandle<Trk::IExtrapolator>            m_extrapolator;
      ToolHandle<ITRT_CalDbTool>                m_trtcaldbTool;
	  
      ToolHandle<ITRT_ToT_dEdx>    m_TRTdEdxTool;
      // --- Private other members
      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_trtPhaseDecorKey;
      enum ETRTFloatDecor {kTRTdEdxDecor,
                           kTRTusedHitsDecor,
                           kTRTdEdx_noHT_divByLDecor,
                           kTRTusedHits_noHT_divByLDecor,
                           kNTRTFloatDecor};
     std::vector<SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> > m_trackTRTFloatDecorKeys;
     enum EPixFloatDecorKeys {kTrkIBLXDecor, kTrkIBLYDecor, kTrkIBLZDecor,
                              kTrkBLXDecor,  kTrkBLYDecor,  kTrkBLZDecor,
                              kTrkL1XDecor,  kTrkL1YDecor,  kTrkL1ZDecor,
                              kTrkL2XDecor,  kTrkL2YDecor,  kTrkL2ZDecor,
                              kNPixFloatDecor };
     std::vector<SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> > m_trackPixFloatDecorKeys;
     SG::WriteDecorHandleKey<xAOD::TrackParticleContainer>               m_trackTSOSMOSLinkDecorKey;
     Gaudi::Property< std::vector<float> > m_pixelLayerRadii {this, "PixelLayerRadii", {29.5,50.5,88.5,122.5}, "Radii to extrapolate to for estimating track position on layers" };


  }; 
}

#endif // DERIVATIONFRAMEWORK_TRACKSTATEONSURFACEDECORATOR_H

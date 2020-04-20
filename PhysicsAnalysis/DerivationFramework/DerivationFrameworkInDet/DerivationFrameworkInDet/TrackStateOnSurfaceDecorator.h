/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      std::string m_sgName;
      std::string m_containerName;
    
      std::string m_pixelMapName;
      std::string m_sctMapName;
      std::string m_trtMapName;
      
      std::string m_pixelClustersName;
      std::string m_sctClustersName;
      std::string m_trtDCName;      
      
      std::string m_pixelMsosName;
      std::string m_sctMsosName;
      std::string m_trtMsosName;

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
      ToolHandle< Trk::IPRD_AssociationTool >  m_assoTool;
      // --- Private other members
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRACKSTATEONSURFACEDECORATOR_H

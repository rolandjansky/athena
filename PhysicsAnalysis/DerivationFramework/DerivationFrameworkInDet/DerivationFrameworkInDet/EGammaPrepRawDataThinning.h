/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGammaPrepRawDataThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGAMMAPREPRAWDATATHINNING_H
#define DERIVATIONFRAMEWORK_EGAMMAPREPRAWDATATHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"

#include "TLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

// Needed for the pixel clusters
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetIdentifier/PixelID.h"

#include "Identifier/IdentifierHash.h"

// Needed for RegSelSvc
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "IRegionSelector/RegSelEnums.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "RoiDescriptor/RoiDescriptor.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"


//class IRegSelSvc;

//class I4Momentum;

//class IParticle;

namespace DerivationFramework {

  class EGammaPrepRawDataThinning:public AthAlgTool, public IThinningTool{

  public:

    EGammaPrepRawDataThinning(const std::string& type, 
                        const std::string& name, 
                        const IInterface* parent);
    
    ~EGammaPrepRawDataThinning(){}
    StatusCode initialize();
    StatusCode finalize();
    StatusCode doThinning() const ;
    
  private:
    
    ServiceHandle<IThinningSvc> m_thinningSvc;
    ServiceHandle<IRegSelSvc> m_regionSelector;
    
    std::string m_electronsContainerName;
    std::string m_photonsContainerName;
    std::string m_pixelClusterKey;
    std::string m_sctClusterKey;
    std::string m_trtDriftCircleKey;

    double m_dr;
    double m_minEtEg;

    double m_maxDeltaEtaPRD ;
    double m_maxDeltaPhiPRD ;
    double m_maxDeltaZPRD ;

    // The method to thin the PixelClusters 
    StatusCode thinPixelsClusters(const InDet::PixelClusterContainer* pixelCont, 
                                  const std::vector<IdentifierHash> & goodPixelIDs) const ;
    
    // The method to thin the SCT_Clusters 
    StatusCode thinSCTClusters(const InDet::SCT_ClusterContainer* sctCont,
                               const std::vector<IdentifierHash> & goodSCTClustersIDs ) const ;
    
    // The method to thin the TRT_DriftCircles 
    StatusCode thinTRTDriftCircles(const InDet::TRT_DriftCircleContainer* trtCont,
                                   const std::vector<IdentifierHash> & goodTRTDriftCirclesIDs ) const;
    
    // The method to find good Inner Detector Objects near interesting objects
    // using the region selector service                                                               
    std::vector<IdentifierHash> findGoodIDObjects(DETID detectorID,                               
                                                  TLorentzVector candHepLorentz,
                                                  CLHEP::Hep3Vector primaryVertex,
                                                  double maxDeltaEta,
                                                  double maxDeltaPhi,
                                                  double maxDeltaZ ) const ;
    
    
  };  
}

#endif // DERIVATIONFRAMEWORK_EGAMMAPREPRAWDATATHINNING_H

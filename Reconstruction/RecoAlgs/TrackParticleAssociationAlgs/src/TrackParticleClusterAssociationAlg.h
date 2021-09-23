/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECLUSTERASSOCIATIONALG_H
#define TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECLUSTERASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include <string>

//////////////////////////////////////////////////////////
/// \class TrackParticleClusterAssociationAlg
/// Associate clusters to each track based on its extrapolation to the calo.
///
/// This alg reads in a CaloExtensionCollection corresponding to a TrackParticleContainer.
/// It then decorates each TrackParticleContainer with a vector<EL<CaloCluster>> containing
/// the clusters close enough (DeltaR property) w.r.t CaloExtension corresponding to the track.
/// Optionnaly tracks are also decorated with a const Trk::TrackParameters* obtain from
/// caloExtension->caloEntryLayerIntersection().
///
/// Note: In earlier versions this alg was creating 2 maps a xAODAssociations/TrackParticleClusterAssociationContainer
/// and a Rec::ParticleClusterAssociation and was using ParticleCaloClusterAssociationTool to retrieve the extrapolation
/// and perform the clusters association.
/// All of this was suppress or replaced in order to simplify the procedure.
class TrackParticleClusterAssociationAlg : public AthAlgorithm
{
 public:
  TrackParticleClusterAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);


  StatusCode initialize() override ;
  StatusCode execute() override ;


 private:

  /// returns the clusters from allClusters which are close enough to caloExtensions
  std::vector<const xAOD::CaloCluster* > associatedClusters(const Trk::CaloExtension & caloExtensions, const xAOD::CaloClusterContainer & allClusters) ;
  
  

  Gaudi::Property<float> m_dr {this, "DeltaR", 0.1, "max deltaR to match track and cluster" };
  Gaudi::Property<bool> m_useCovariance {this, "UseCovariance", true, "use covariance from TrkParameters to evaluate angular uncertainties"};
  Gaudi::Property<double> m_ptCut {this, "PtCut", 25000., ""};

  // input containers
  SG::ReadHandleKey<CaloExtensionCollection> m_caloExtKey {this, "CaloExtensionName", "", ""};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleCollectionHandle {this,"TrackParticleContainerName",  "InDetTrackParticles", "input tracks" };
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusters {this, "CaloClusterLocation", "CaloCalTopoClusters","input calo clusters"};

  // Whether or not to use the DetectorEta attribute of the clusters, which is important if the input cluster container has had the origin correction applied
  // Default assumes no origin correction, must be configured if desired
  SG::ReadDecorHandleKey<xAOD::CaloClusterContainer> m_detectorEtaDecor { this, "DetectorEtaName", "", "Decoration for CaloCluster DetectorEta" };
  bool m_doDetEta;

  // vertex handling
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContHandle {this, "VertexContainerName", "", "if empty all tracks will be decorated. if not only those corresponding to the PV[0] will."};
  ToolHandle<CP::ITrackVertexAssociationTool> m_trackvertexassoTool {this, "TrackVertexAssoTool", "" };


  // decorations 
  SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_assocClustersDecor {this, "AssociatedClusterDecorKey" , "", "Decoration key to store associated clusters. IMPORTANT  must be consistent with TrackParticleContainer" };
  SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_caloEntryParsDecor {this, "CaloEntryParsDecorKey" , "", "Decoration name to store trk parameters to calo entry (if non blank). IMPORTANT : must be consistent with TrackParticleContainer" };
};

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERINFOALG_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERINFOALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" 
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODPFlow/FlowElementContainer.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"


////////////////////////////////////////////////////////
/// \class TrackCaloClusterInfoAlg
///
/// Builds a TrackCaloClusterInfo map which contains the weights needed to build TrackCaloCluster objects.
/// This TrackCaloClusterInfo is then expected to be used by a TrackCaloClusterAlg and its tools.
///
/// \authors : Naomie Calace, P-A Delsart

class TrackCaloClusterInfoAlg: public ::AthAlgorithm { 
public: 
  TrackCaloClusterInfoAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackCaloClusterInfoAlg() {}
  
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override ;
  
  
protected:
  virtual  StatusCode fillInfo(SG::WriteHandle<TrackCaloClusterInfo> & tccInfo) const ;
  
  ///TrackCaloClusterInfo created by this alg
  SG::WriteHandleKey<TrackCaloClusterInfo> m_tccInfoHandle {this, "TCCInfoName","TCCInfo","Name of TrackCaloClusterInfo to be created" };


  
  ///Tracks  used by this alg
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_inputTracksHandle {this, "InputTracks", "InDetTrackParticles", ""};
  /// Decoration of tracks expected by this alg (set by TrackParticleClusterAssociationAlg)
  SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> m_assoClustersKey{this, "AssoClustersDecor", "", "Key to access clusters associated to a track. IMPORTANT must be consistent with TrackParticleContainer "};

  ///Original clusters from which the m_inputObjectName are build ()
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClustersHandle {this, "InputClusters", "CaloCalTopoClusters","input calo clusters"};
  
  SG::ReadHandleKey<xAOD::VertexContainer> m_inputVertexHandle {this, "VertexContainer", "PrimaryVertices", "IMPORTANT : if blank all tracks will be considered, else only those from the PV[0] of VertexContainer"};
  
  /// use cluster energy or pt?
  Gaudi::Property<bool> m_useEnergy {this, "UseEnergy", false,"wether to cut on E or pT"};

};





////////////////////////////////////////////////////////
/// \class TrackCaloClusterInfoUFOAlg
///
/// Builds a TrackCaloClusterInfo map using PFO as input.
///
/// \authors : P-A Delsart, Jennifer Roloff

class TrackCaloClusterInfoUFOAlg: public TrackCaloClusterInfoAlg { 
public: 
  TrackCaloClusterInfoUFOAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  
protected:

  virtual StatusCode fillInfo(SG::WriteHandle<TrackCaloClusterInfo> & tccInfo) const override;


  SG::ReadHandleKey< xAOD::FlowElementContainer > m_inputPFOHandle {this, "InputPFO", "JetEtMissChargedParticleFlowObjects_DEFAULT", "Key"};
  SG::ReadDecorHandleKey<xAOD::FlowElementContainer> m_orig_pfo{this, "OriginPFO", "Origin_pfo", "Key to access original un-modified pfo"};

  
  /// cluster with E below this cut won't be considered in the TCC alg. WARNING cut must be configured as in UFOTool
  Gaudi::Property<float> m_clusterEcut{this , "ClusterECut", 0, " Important !! : must be the same value as in UFOTool"};

  ToolHandle<CP::ITrackVertexAssociationTool> m_trackVertexAssoTool {this, "TrackVertexAssoTool", "" };

};

#endif //

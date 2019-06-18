/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERINFOALG_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERINFOALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"


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
  virtual ~TrackCaloClusterInfoAlg(); 
  
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  
protected:
  virtual  StatusCode fillInfo(TrackCaloClusterInfo & tccInfo);
  
  ///TrackCaloClusterInfo created by this alg
  std::string m_tccInfoName;

  /// the TrackParticleClusterAssociationContainer created by this alg
  std::string m_inputTrackCaloAssocName;
  
  ///Tracks  used by this alg
  std::string m_inputTracksName;

  ///Original clusters from which the m_inputObjectName are build ()
  std::string m_inputClustersName;
  
  std::string m_vertexContname;
  
  /// use cluster energy or pt?
  bool m_useEnergy;


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
  virtual ~TrackCaloClusterInfoUFOAlg(); 

  virtual StatusCode  initialize();
  
protected:

  virtual StatusCode fillInfo(TrackCaloClusterInfo & tccInfo);

  /// Prefix to the input PFO container needed (m_pfoPrefix+"ChargedParticleFlowObjects" and m_pfoPrefix+"NeutralParticleFlowObjects")
  std::string m_pfoPrefix;

  /// cluster with E below this cut won't be considered in the TCC alg. WARNING cut must be configured as in UFOTool
  float m_clusterEcut;

  ToolHandle<CP::ITrackVertexAssociationTool> m_loosetrackvertexassoTool;
  
};

#endif //

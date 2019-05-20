/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERINFOALG_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERINFOALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
// #include "xAODBase/IParticle.h"
// #include "xAODPFlow/TrackCaloClusterContainer.h"
// #include "xAODTracking/TrackParticleContainer.h"

#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"


class TrackCaloClusterInfoAlg: public ::AthAlgorithm { 
public: 
  TrackCaloClusterInfoAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackCaloClusterInfoAlg(); 
  
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  /// Definition of the 4-momentum type.
  typedef xAOD::IParticle::FourMom_t FourMom_t;
  
protected:
  virtual  StatusCode fillInfo(TrackCaloClusterInfo & tccInfo);
  
  /// fill the maps
  //void fillMaps(std::multimap <const xAOD::CaloCluster*, const xAOD::TrackParticle*>& clusterToTracksMap, std::map <const xAOD::TrackParticle*, FourMom_t>& TrackTotalClusterPt, std::map <const xAOD::CaloCluster*, FourMom_t>& clusterToTracksWeightMap    );
    
  
  
  
  ///TrackCaloClusterInfo created by this alg
  std::string m_tccInfoName;

  ///
  std::string m_inputTrackCaloAssocName;
  
  ///Clusters or PFO  used as source for the weights
  //std::string m_inputObjectName;

  ///Tracks  used by this alg
  std::string m_inputTracksName;

  ///Original clusters from which the m_inputObjectName are build ()
  std::string m_inputClustersName;
  
  std::string m_vertexContname;

  
  /// use cluster energy or pt?
  bool m_useEnergy;


};


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

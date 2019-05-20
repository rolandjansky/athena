/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERALG_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "xAODBase/IParticle.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrackCaloClusterRecInterfaces/ITrackCaloClusterTool.h"

class TrackCaloClusterAlg: public ::AthAlgorithm { 
public: 
  TrackCaloClusterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackCaloClusterAlg(); 
    
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  /// Definition of the 4-momentum type.
  typedef xAOD::IParticle::FourMom_t FourMom_t;
 
private:
  
  /// fill the maps
  //void fillMaps(std::multimap <const xAOD::CaloCluster*, const xAOD::TrackParticle*>& clusterToTracksMap, std::map <const xAOD::TrackParticle*, FourMom_t>& TrackTotalClusterPt, std::map <const xAOD::CaloCluster*, FourMom_t>& clusterToTracksWeightMap    );
    
  ToolHandleArray <ITrackCaloClusterTool> m_tccTools;
  
  
  
  ///TrackCaloClusterInfo used by this alg
  std::string m_tccInfoName;
  
  ///Cluster Collection container
  std::string m_outputTCCName;

    ///Cluster Collection container
  std::string m_copiedTCCName;

};

#endif //

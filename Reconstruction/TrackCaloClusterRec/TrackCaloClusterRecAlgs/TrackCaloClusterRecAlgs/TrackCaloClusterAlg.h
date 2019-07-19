/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERALG_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODBase/IParticle.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrackCaloClusterRecInterfaces/ITrackCaloClusterTool.h"

////////////////////////////////////////////////////////
/// \class TrackCaloClusterAlg
///
/// Builds TrackCaloCluster after a TrackCaloClusterInfo object has been prepared and is available in
/// the event store.
///
/// The 'taste' or type (TCC or UFO) of the TrackCaloCluster objects being build depends on the
/// TCC creator tools (ITrackCaloClusterTool) attached to this alg. See the TCCTools property.
/// 
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
  
  /// The tools implementting the creation of TCC
  ToolHandleArray <ITrackCaloClusterTool> m_tccTools;
     
  ///TrackCaloClusterInfo used by this alg
  std::string m_tccInfoName;
  
  ///Cluster Collection container
  std::string m_outputTCCName;

    ///Cluster Collection container
  std::string m_copiedTCCName;

};

#endif //

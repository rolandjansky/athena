/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERALG_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERALGS_TRACKCALOCLUSTERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"


#include "xAODPFlow/FlowElementContainer.h"
#include "TrackCaloClusterRecInterfaces/ITrackCaloClusterTool.h"

#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"


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
    
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  
 
private:
  
  /// The tools implementting the creation of TCC
  ToolHandleArray <ITrackCaloClusterTool> m_tccTools {this, "TCCTools", {},"List of TrackCaloClusterTool"};
     
  ///TrackCaloClusterInfo used by this alg
  SG::ReadHandleKey<TrackCaloClusterInfo> m_tccInfoHandle {this, "TCCInfo", "", "the TrackCaloClusterInfo which contain the energy weights needed to split clusters/PFO onto tracks" };
  
  ///Cluster Collection container
  SG::WriteHandleKey<xAOD::FlowElementContainer>  m_outputTCCHandle {this, "OutputTCCName", "TrackCaloClusters", "Name of output container to be created" };

  SG::ReadHandleKey<xAOD::FlowElementContainer> m_copiedTCCHandle {this, "AppendToTCCName", "", "optionnaly, read-in an existing container, recopy and append to it with new FlowElements build by this alg."};

};

#endif //

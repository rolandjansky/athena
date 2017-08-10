/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file D3PDMaker/TrackD3PDMaker/src/TrackParticleToSCTClusterAssociationTool.cxx
* @author <yuriy.pylypchenko@cern.ch>
* @date Oct, 2010
* @brief setting up an association of tracks to clusters
*        represented by indices. 
*/
#ifndef TRACKPARTICLETOSCTCLUSTERASSOCIATIONTOOL_H
#define TRACKPARTICLETOSCTCLUSTERASSOCIATIONTOOL_H
 

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "Identifier/Identifier.h"
#include "DataModel/DataVector.h"

class AtlasDetectorID;
class SCT_ID;


namespace Rec{
  class TrackParticle;
}


namespace Trk{
  class Track;
  class TrackStateOnSurface;
}

namespace InDet {
  class SCT_Cluster;
  class SiCluster;
}

 
namespace D3PD {
 
 

  ///
  ///@brief Associate all clusters which belong to given track.
  ///
  /// This is a multiple association tool.
  /// Given an @c InDet::SCT_Cluster object and a container of such objects,
  /// return the all objects in the container wich identified with TrackStateOnSurface 
  /// (Measurements) of the input track.
  ///


typedef D3PD::MultiAssociationTool<Rec::TrackParticle, InDet::SCT_Cluster> TrackParticleToSCTClusterToTrackBase;
  
class TrackParticleToSCTClusterAssociationTool
   : public TrackParticleToSCTClusterToTrackBase 
{
public:
 
  ///
  /// @brief Standard Gaudi tool constructor.
  /// @param type The name of the tool type.
  /// @param name The tool name.
  /// @param parent The tool's Gaudi parent.
  ///
  TrackParticleToSCTClusterAssociationTool (const std::string& type,
				  const std::string& name,
				  const IInterface* parent);
 

 
  /// Standard initialize method.
  virtual StatusCode initialize();
  ///
  /// @brief Create any needed tuple variables.
  /// In principle we do not intend to get anything else then index 
  ///(=we don't create) variables
  /// but for debugging and validation puproses it is nice to have this.
  /// This is called at the start of the first event.
  ///
  virtual StatusCode book();
 
  ///
  /// @brief Start the iteration for a new association.
  /// @param p The object from which to associate.
  ///
  virtual StatusCode reset (const Rec::TrackParticle& p);
 
 
  ///
  /// @brief Return a pointer to the next element in the association.
  ///
  /// Return 0 when the association has been exhausted.
  ///
  virtual const InDet::SCT_Cluster* next();
 


 
private:

  const AtlasDetectorID* m_idHelper;
  const SCT_ID* m_sctId;

  std::vector<const Trk::TrackStateOnSurface*> m_sctClusterForAssociation; 
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator m_clusItr;
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator m_clusEnd;

  /// extra ntuple variable 
  float                 *m_clGlobX;
  float                 *m_clGlobY;
  float                 *m_clGlobZ;
  float                 *m_clLocX;
  float                 *m_clLocY;
  float                 *m_locX;
  float                 *m_locY;
  float                 *m_err_locX;
  float                 *m_err_locY;
  float                 *m_cov_locXY;
  float                 *m_x;
  float                 *m_y;
  float                 *m_z;
  float                 *m_trkLocX;
  float                 *m_trkLocY;
  float                 *m_err_trkLocX;
  float                 *m_err_trkLocY;
  float                 *m_cov_trkLocXY;
  int                   *m_isCompetingRIO;
  int                   *m_sizePhi;
  int                   *m_sideModule;
  float                 *m_locPhi;
  float                 *m_locTheta;
  int                   *m_tsosType;
 
  Identifier::value_type *m_id; // unique identifier for a cluster
  Identifier::value_type *m_detElementId; // unique identifier for a detector element
  int *m_detType;
  int *m_bec;
  int *m_layer;

  /*
  float                 *m_locPhiOld;
  float                 *m_locThetaOld;
  float                 *m_locPhiOldNotCor;
  float                 *m_locThetaOldNotCor;
  */



}; // class TrackParticleToSCTClusterAssociationTool

 
} // namespace D3PD
 
#endif // not TRACKPARTICLETOSCTCLUSTERASSOCIATIONTOOL_H

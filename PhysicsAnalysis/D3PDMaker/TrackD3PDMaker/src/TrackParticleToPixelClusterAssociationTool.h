/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file D3PDMaker/TrackD3PDMaker/src/TrackParticleToPixelClusterAssociationTool.cxx
* @author <yuriy.pylypchenko@cern.ch>
* @date Oct, 2010
* @brief setting up an association of tracks to clusters
*        represented by indices. 
*/
#ifndef TRACKPARTICLETOPIXELCLUSTERASSOCIATIONTOOL_H
#define TRACKPARTICLETOPIXELCLUSTERASSOCIATIONTOOL_H
 

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "Identifier/Identifier.h"
#include "AthContainers/DataVector.h"

class AtlasDetectorID;
class PixelID;


namespace Rec{
  class TrackParticle;
}


namespace Trk{
  class Track;
  class TrackStateOnSurface;
}

namespace InDet {
  class PixelCluster;
  class SiCluster;
}

 
namespace D3PD {
 
 

  ///
  ///@brief Associate all clusters which belong to given track.
  ///
  /// This is a multiple association tool.
  /// Given an @c InDet::PixelCluster object and a container of such objects,
  /// return the all objects in the container wich identified with TrackStateOnSurface 
  /// (Measurements) of the input track.
  ///


typedef D3PD::MultiAssociationTool<Rec::TrackParticle, InDet::PixelCluster> TrackParticleToPixelClusterToTrackBase;
  
class TrackParticleToPixelClusterAssociationTool
   : public TrackParticleToPixelClusterToTrackBase 
{
public:
 
  ///
  /// @brief Standard Gaudi tool constructor.
  /// @param type The name of the tool type.
  /// @param name The tool name.
  /// @param parent The tool's Gaudi parent.
  ///
  TrackParticleToPixelClusterAssociationTool (const std::string& type,
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
  virtual const InDet::PixelCluster* next();
 


 
private:

  const AtlasDetectorID* m_idHelper;
  const PixelID* m_pixelId;

  std::vector<const Trk::TrackStateOnSurface*> m_pixelClusterForAssociation; 
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
  int			*m_ToT;
  //int                   *m_sideModule;
  float                 *m_locPhi;
  float                 *m_locTheta;
  float 		*m_Pt;
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



}; // class TrackParticleToPixelClusterAssociationTool

 
} // namespace D3PD
 
#endif // not TRACKPARTICLETOPIXELCLUSTERASSOCIATIONTOOL_H

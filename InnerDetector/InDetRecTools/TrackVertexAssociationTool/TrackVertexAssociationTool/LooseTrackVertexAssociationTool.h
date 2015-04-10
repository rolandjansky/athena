/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LOOSETRACKVERTEXASSOCIATIONTOOL_H
#define LOOSETRACKVERTEXASSOCIATIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "TrackVertexAssociationTool/TrackVertexAssociationMap.h"

#include <string>

namespace CP
{

  /// LooseTrackVertexAssociationTool: using loose track-vertex association method.
  ///  
  class LooseTrackVertexAssociationTool : virtual public asg::AsgTool, virtual public ITrackVertexAssociationTool
  {

    ASG_TOOL_CLASS( LooseTrackVertexAssociationTool, ITrackVertexAssociationTool)
    public:
      LooseTrackVertexAssociationTool(std::string name);
//      ~TrackToVertexTool(){};

      StatusCode initialize();

      void print() const;

      /// This function will return the decision of whether the track is matched to the Vertex after matching method
      /// Not sure whether the track is matched to best choice
      /// Note: Only apply to good vertex(vx->vertexType()!=xAOD::VxType::NoVtx), return false if vertex is a dummy one
      bool isCompatible( const xAOD::TrackParticle &trk, const xAOD::Vertex &vx ) const;  // return the decision of with the track match to the vertex
      xAOD::TrackVertexAssociationMap getMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list, std::vector< const xAOD::Vertex *> &vx_list) const;
      xAOD::TrackVertexAssociationMap getMatchMap( const xAOD::TrackParticleContainer &trkCont, const xAOD::VertexContainer &vxCont ) const;
 

      /// This functions will return the best matched vertex
      /// Match method is same as isCompatible. but will only keep the best vertex(with mini_dz) with more than one vertices match to the same track
      ElementLink< xAOD::VertexContainer> getUniqueMatchVertexLink(const xAOD::TrackParticle &, const xAOD::VertexContainer &vxCont) const;
      
      const xAOD::Vertex* getUniqueMatchVertex( const xAOD::TrackParticle &trk, std::vector< const xAOD::Vertex *> &vx_list) const;  // return the  vertex matched with the tracks in the vx_list

      /// This functions related to the previous functions, will return a map<const xAOD::Vertex*, std::vector<const xAOD::TrackParticle*> > (defined as TrackVertexAssociationMap) to store the best matched tracks for each vertex. 
      /// Note: the dummy vertex will be also stored in the map but the track stored vector which assigned to this vertex will be empty
      xAOD::TrackVertexAssociationMap getUniqueMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list) const;  // return the map of vertex matched with the tracks in the 

      xAOD::TrackVertexAssociationMap getUniqueMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont) const;  // return the map of vertex matched with the tracks in the 

    private:

      /// main function for doing loose matching process
      bool isMatch(const xAOD::TrackParticle &trk, const xAOD::Vertex &vx, float & dz) const;
      const xAOD::Vertex* getUniqueMatchVx( const xAOD::TrackParticle &trk, const xAOD::VertexContainer &vxCont) const;  // return the  vertex matched with the tracks in the vx_list

      float m_dz_cut;
      float m_d0_cut;
  };
}

#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TIGHTTRACKVERTEXASSOCIATIONTOOL_H
#define TIGHTTRACKVERTEXASSOCIATIONTOOL_H

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

  /// TightTrackVertexAssociationTool: apply tight track-vertex association process, the main idea is base on track used in vertex fit and recover the rest by loose method 
  ///
  class TightTrackVertexAssociationTool : virtual public asg::AsgTool, virtual public ITrackVertexAssociationTool
  {
    enum MatchStatus{UsedInFit=1, Matched=2, UnMatch=0};  // MatchCode for the matching, UsedInFit for the tracks used in the vertex fitting process, Matched for the tracks not used in the fitting process. UnMatch for the tracks not match to the vertex.

    ASG_TOOL_CLASS( TightTrackVertexAssociationTool, ITrackVertexAssociationTool)
    public:
      TightTrackVertexAssociationTool(std::string name);
//      ~TrackToVertexTool(){};

      StatusCode initialize();

      void print() const;

      /// This function will return the decision of whether the track is matched to the Vertex 
      /// Not sure whether the track is matched to best choice
      /// Note: Only apply to good vertex(vx->vertexType()!=xAOD::VxType::NoVtx), return false if vertex is a dummy one
      bool isCompatible( const xAOD::TrackParticle &trk, const xAOD::Vertex &vx ) const;  // return the decision of with the track match to the vertex
      xAOD::TrackVertexAssociationMap getMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list, std::vector< const xAOD::Vertex *> &vx_list) const;
      xAOD::TrackVertexAssociationMap getMatchMap( const xAOD::TrackParticleContainer &trkCont, const xAOD::VertexContainer &vxCont ) const;
 

      /// This functions will return the best matched vertex
      ///  For ambiguity resolve: 1) If this track is used in vertex fit, always pick up the vertex fitted using this track. 
      /// 2) If this track is not used for any vertex fit, pick up the vertex with mini_dz 
      ElementLink< xAOD::VertexContainer> getUniqueMatchVertexLink(const xAOD::TrackParticle &, const xAOD::VertexContainer &vxCont) const;

      const xAOD::Vertex* getUniqueMatchVertex( const xAOD::TrackParticle &trk, std::vector< const xAOD::Vertex *> &vx_list) const;  // return the  vertex matched with the tracks in the vx_list

      /// This functions related to the previous functions, will return a map<const xAOD::Vertex*, std::vector<const xAOD::TrackParticle*> > (defined as TrackVertexAssociationMap) to store the best matched tracks for each vertex. 
      /// Note: the dummy vertex will be also stored in the map but the track stored vector which assigned to this vertex will be empty
      xAOD::TrackVertexAssociationMap getUniqueMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list) const;  // return the map of vertex matched with the tracks in the 

      xAOD::TrackVertexAssociationMap getUniqueMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont) const;  // return the map of vertex matched with the tracks in the 

    private:

      /// Main function for doing tight matching.
      /// will return three status: UsedInFit(the track is used in this vertex fit), Matched(the track is not used in this vertex fit, but can be associated to this vertex)
      /// UnMatch(the track is not used for vertex fit and can not be association to this vertex)
      MatchStatus doTightMatch(const xAOD::TrackParticle &trk, const xAOD::Vertex &vx, float & dz) const;
      const xAOD::Vertex* getUniqueMatchVx( const xAOD::TrackParticle &trk, const xAOD::VertexContainer &vxCont) const;  // return the  vertex matched with the tracks in the vx_list

      float m_dz_cut;
      float m_d0_cut;
      bool m_doPV;
      bool m_dod0sel;
  };
}

#endif


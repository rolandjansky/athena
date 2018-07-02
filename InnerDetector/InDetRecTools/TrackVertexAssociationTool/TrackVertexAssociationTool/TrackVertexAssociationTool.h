/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKVERTEXASSOCIATIONTOOL_H
#define TRACKVERTEXASSOCIATIONTOOL_H

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

namespace CP {

  /// General track-vertex association tool
  ///
  /// Supported properties (set via setProperty):
  ///
  /// To apply the recommendations, please visit
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPRecsEarly2018#Track_to_Vertex_Association_Tool
  class TrackVertexAssociationTool : virtual public asg::AsgTool, virtual public ITrackVertexAssociationTool
  {
    ASG_TOOL_CLASS(TrackVertexAssociationTool, ITrackVertexAssociationTool)
    public:
      TrackVertexAssociationTool(std::string name);

      StatusCode initialize();

      void print() const;

      bool isCompatible( const xAOD::TrackParticle &trk, const xAOD::Vertex &vx ) const;

      xAOD::TrackVertexAssociationMap getMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list, std::vector< const xAOD::Vertex *> &vx_list) const;
      xAOD::TrackVertexAssociationMap getMatchMap( const xAOD::TrackParticleContainer &trkCont, const xAOD::VertexContainer &vxCont ) const;

      ElementLink< xAOD::VertexContainer> getUniqueMatchVertexLink(const xAOD::TrackParticle &, const xAOD::VertexContainer &vxCont) const;
      const xAOD::Vertex* getUniqueMatchVertex( const xAOD::TrackParticle &trk, std::vector< const xAOD::Vertex *> &vx_list) const;

      xAOD::TrackVertexAssociationMap getUniqueMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list) const;
      xAOD::TrackVertexAssociationMap getUniqueMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont) const;

    private:
      /// Checks if a track-vertex pair passes the cuts.  Returns
      /// Δz * sin θ of the pair in `dzSinTheta` if successful.
      bool isMatch(const xAOD::TrackParticle &trk, const xAOD::Vertex &vx,
                   float &dzSinTheta) const;

      template <typename T, typename U>
      xAOD::TrackVertexAssociationMap getMatchMapImpl(T &trk_list,
                                                      U &vx_list) const;

      template <typename T, typename U>
      xAOD::TrackVertexAssociationMap getUniqueMatchMapImpl(T &trk_list,
                                                            U &vx_list) const;

      template <typename T>
      const xAOD::Vertex *getUniqueMatchVertexImpl(const xAOD::TrackParticle &trk,
                                                   T &vx_list) const;

      /// Cut on dz*sin theta
      float m_dzSinTheta_cut;
      /// Cut on d0 significance
      float m_d0sig_cut;
  };

} // namespace CP

#endif // TRACKVERTEXASSOCIATIONTOOL_H
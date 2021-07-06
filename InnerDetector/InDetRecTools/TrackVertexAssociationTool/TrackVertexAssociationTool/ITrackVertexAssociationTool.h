/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRACKVERTEXASSOCIATIONTOOL_H
#define ITRACKVERTEXASSOCIATIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "TrackVertexAssociationTool/TrackVertexAssociationMap.h"

#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

#include <vector>

namespace CP
{

  class ITrackVertexAssociationTool : virtual public asg::IAsgTool
  {
    ASG_TOOL_INTERFACE(ITrackVertexAssociationTool)
    public:
      /// This function just return the decision of whether the track is matched to the Vertex 
      /// Not sure whether the track is matched to best choice
      virtual bool isCompatible(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx) const = 0;  // return the decision of with the track match to the vertex
      virtual bool isCompatible(const xAOD::TrackParticle& trk) const = 0;  // return the decision of with the track match to the predetermined hardscatter vertex
      virtual xAOD::TrackVertexAssociationMap getMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const = 0;
      virtual xAOD::TrackVertexAssociationMap getMatchMap(const xAOD::TrackParticleContainer& trkCont, const xAOD::VertexContainer& vxCont) const = 0;
      
      /// This functions will return the best matched vertex
      virtual ElementLink<xAOD::VertexContainer> getUniqueMatchVertexLink(const xAOD::TrackParticle& trk, const xAOD::VertexContainer& vxCont) const = 0;
      virtual const xAOD::Vertex* getUniqueMatchVertex(const xAOD::TrackParticle& trk, std::vector<const xAOD::Vertex*> & vx_list) const = 0;

      /// This functions related to the previous functions, will return a 2D vector to store the best matched tracks for each vertex
      virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const = 0;  // return the map of vertex matched with the tracks in the 
      virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap(const xAOD::TrackParticleContainer& trkCont, const xAOD::VertexContainer& vxCont) const = 0 ;  // return the map of vertex matched with the tracks in the 

  };
}

#endif

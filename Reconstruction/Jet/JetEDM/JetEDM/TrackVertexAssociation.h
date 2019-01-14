/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackVertexAssociation.h

#ifndef JETEDM_TRACKVERTEXASSOCIATION_H
#define JETEDM_TRACKVERTEXASSOCIATION_H

#include <vector>

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AsgTools/CLASS_DEF.h"

namespace jet {
  
/// Class to hold N-to-one aassociations between tracks and vertices.
/// It is assumed the track container is the one that directly holds
/// the tracks, i.e. is *not* a view container.

class TrackVertexAssociation {
  
public:

  // Ctor.
  TrackVertexAssociation(const xAOD::TrackParticleContainer* trkCont=0) ;

  // Associate a track with a vertex.
  void associate(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx);

  // Return the vertex associated to a track.
  const xAOD::Vertex* associatedVertex(const xAOD::TrackParticle* trk) const;

  // Return the tracks associatged with a vertex.
  std::vector<const xAOD::TrackParticle*> associatedTracks(const xAOD::Vertex* vtx) const;

protected:
    
  std::vector<const xAOD::Vertex*> m_vertex;
  const xAOD::TrackParticleContainer* m_trackContainer;

};
  
}  // end jet namespace

CLASS_DEF(jet::TrackVertexAssociation , 34721930 , 1)


#endif

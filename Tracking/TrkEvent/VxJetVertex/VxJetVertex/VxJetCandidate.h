/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          VxJetCandidate.h  -  Description
                             -------------------

    begin : Autumn 2006
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch
    changes: new!

This class represents a Jet Candidate with all information about all 
the vertices which were fitted on the JET AXIS.

It is a Multi Vertex Class for fitting vertices on the Jet Axis and is 
used by the new JetFitter algorithm.

Compatibility with the original VxCandidate class is kept for compatibility reasons (and to 
provide a base class for secondary vertex finders used by the standard B-Tagging algorithms).

     
 ***************************************************************************/

#ifndef VXJETVERTEX_VXJETCANDIDATE_H
#define VXJETVERTEX_VXJETCANDIDATE_H

#include <vector>
#include <utility>
#include "VxVertex/VxCandidate.h"
#include "VxJetVertex/RecVertexPositions.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"

namespace Trk
{


  class RecVertex;
  class VxTrackAtVertex;
  class VxJetFitterDebugInfo;
  class VxClusteringTable;
  class Track;
  class VxVertexOnJetAxis;

  class VxJetCandidate : public VxCandidate
{
  public:// standard constructor for persistency
    VxJetCandidate();
    VxJetCandidate(const Trk::RecVertex& recVertex,
		   const std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex);
    VxJetCandidate(const std::vector<std::pair<const Trk::TrackParticleBase*,const Trk::TrackParticleBaseCollection*> > &);
    //VxJetCandidate(const std::vector<std::pair<const Trk::TrackParticleBase*,B*> > &);
    VxJetCandidate(const std::vector<std::pair<const Track*,const TrackCollection*> > &);
    VxJetCandidate(const std::vector<const Trk::TrackParticleBase*> &);
    VxJetCandidate(const std::vector<const Track*> &);
    
    virtual VxJetCandidate* clone() const;
    virtual ~VxJetCandidate();
    VxJetCandidate(const VxJetCandidate& rhs);
    VxJetCandidate &operator= (const VxJetCandidate &);  //!< Assignement operator
    
    void setTracksOrigin(const TrackCollection * );
    void setTrackParticlesOrigin(const Trk::TrackParticleBaseCollection*);

    const Trk::RecVertexPositions & getRecVertexPositions() const;

    void setRecVertexPositions(const Trk::RecVertexPositions &);

    const Trk::VertexPositions & getLinearizationVertexPositions() const;
    Trk::VertexPositions & getLinearizationVertexPositions();//this is not a persistency object, so accessing it directly is permitted

    void setLinearizationVertexPositions(const Trk::VertexPositions &);
    
    const std::vector<VxVertexOnJetAxis*> & getVerticesOnJetAxis(void) const;

    void setVerticesOnJetAxis(const std::vector<VxVertexOnJetAxis*> &);

    const VxVertexOnJetAxis* getPrimaryVertex(void) const;
    VxVertexOnJetAxis* getPrimaryVertex(void);

    void setPrimaryVertex(const VxVertexOnJetAxis*);

    Trk::VxJetFitterDebugInfo * & getDebugInfo(void);

    Trk::VxClusteringTable* & getClusteringTable(void);

    const Trk::RecVertexPositions & getConstraintVertexPositions() const;
    void setConstraintVertexPositions(const Trk::RecVertexPositions &);



  protected:
    Trk::VxVertexOnJetAxis* m_primaryVertex;
    Trk::RecVertexPositions m_fittedPositions;
    Trk::VertexPositions m_linearizationPositions;
    Trk::RecVertexPositions m_constraintPositions;
    std::vector<VxVertexOnJetAxis*> m_verticesOnJetAxis;
    Trk::VxJetFitterDebugInfo* m_debugInfo;
    Trk::VxClusteringTable* m_clusteringTable;

};

  inline Trk::VxJetCandidate* Trk::VxJetCandidate::clone() const
    {
      return new Trk::VxJetCandidate(*this);
    }

}
#endif














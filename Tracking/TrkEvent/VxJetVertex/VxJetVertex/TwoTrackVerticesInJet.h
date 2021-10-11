/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          TwoTrackVerticesInJet.h  -  Description
                             -------------------

    created: January 2008
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    e-mail: giacinto.piacquadio@physik.uni-freiburg.de
    changes:

    (c) ATLAS Collaboration 2008

    This class is intended to store the two track vertices in a Jet.
    These are used for a first seed of the B Vertex or to flag conversions,Ks,Lambdas 
    or material interactions.
    The possible neutral particles created out of these two track vertices are 
    also stored, but only for those two track vertices which pass the criteria 
    to be a neutral track (mass,prob_chi2,...).
    Please give a look for the JetFitter seeder for more info.

  ***************************************************************************/

#ifndef VXJETVERTEX_TwoTrackVerticesInJet_H
#define VXJETVERTEX_TwoTrackVerticesInJet_H

#include <vector>
#include <map>

#include "xAODTracking/Vertex.h"

namespace Trk
{
  class TrackParticleBase;
  class VxCandidate;
}

namespace Trk 
{
  
  class TwoTrackVerticesInJet
  {
  public:
    

    /**
     * Default constructor, if called initializes a VxVertexOnJetAxis
     * with all data members set to 0.
     */
    TwoTrackVerticesInJet();
    

    /**
     * Destructor
     */
    ~TwoTrackVerticesInJet();

    /**
     * Constructs a TwoTrackVerticesInJet from a vector of the two track vertices 
     * + a map between the single neutral tracks (TrackParticleBase with NeutralPerigee) 
     *  and the corresponding two track vertex
     * (every neutral track in the second vector MUST correspond to a two track vertex 
     * in the first vector)
     */                               
    TwoTrackVerticesInJet(std::vector<const xAOD::Vertex*> twoTrackVertices,
                          std::vector<const TrackParticleBase*>  neutralTrackOfVertex);
    
    /**
     * Copy constructor and assignement operator
     */  
    TwoTrackVerticesInJet(const TwoTrackVerticesInJet& ); 
    TwoTrackVerticesInJet &operator= (const TwoTrackVerticesInJet &);  

    /**
     * Move constructor and assignement operator
     */  
    TwoTrackVerticesInJet(TwoTrackVerticesInJet&& ) noexcept = default;
    TwoTrackVerticesInJet &operator= (TwoTrackVerticesInJet &&) noexcept = default;

    /**
     * Set the two track vertices (takes ownership of pointers)
     */
    void setTwoTrackVertices(std::vector<const xAOD::Vertex*>  twoTrackVertices);


    /**
     * Set the neutral tracks of Vertices (takes ownership also of TrackParticleBase pointers, VxCandidates must be already in the twoTrackVertices list)
     */
    void setNeutralTrackOfVertices(std::vector<const TrackParticleBase*>  neutralTrackOfVertex);

    /**
     * Get the two track vertices (please do not delete the pointers)
     */
    const std::vector<const xAOD::Vertex *> & getTwoTrackVertice() const ;


    /**
     * Get the  neutral tracks of Vertices (please do not delete the pointers)
     */
    const std::vector<const TrackParticleBase*> & getNeutralTrackOfVertices() const ;
    
   private:

    static void deleteAll(std::vector<const TrackParticleBase*> & neutralTrackOfVertex) noexcept;
    static void deleteAll(std::vector<const xAOD::Vertex*> & twoTrackVertices) noexcept;

    std::vector<const xAOD::Vertex*> m_twoTrackVertices;
    std::vector<const TrackParticleBase*> m_neutralTrackOfVertex;
  };
  
}//end namespace

#endif    
    
    
    



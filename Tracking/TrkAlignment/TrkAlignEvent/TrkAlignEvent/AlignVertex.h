/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNVERTEX_H
#define TRKALIGNEVENT_ALIGNVERTEX_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "DataModel/DataVector.h"
  

#include "TrkTrack/Track.h"
#include "VxVertex/VxCandidate.h"
#include "xAODTracking/Vertex.h"
#include "TrkAlignEvent/AlignTrack.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "DataModel/DataVector.h"
#include <vector>
#include <fstream>

/**
   @file AlignVertex.h
   @class AlignVertex

   @brief AlignVertex is a simple structure to store objects belonging to a vertex. Contains a pointer to the original vertex, the matrix and the vector to be solved for the vertex, vertex position, its covariance matrix and a collection of AlignTrack's belonging to it.

   @author Pawel Bruckman
   @date 14/03/2011
*/


namespace Trk {

  typedef std::vector<AlignTrack**> AlignTrackCollection;
  typedef std::vector<AlignTrack**>::const_iterator AlignTrackIt;
  typedef std::pair<const AlignModule*, std::vector<Amg::VectorX> > AlignModuleVertexDerivatives;
  
  class AlignVertex {

  public:

    enum AlignVertexType{
        Unknown = 0,               //!< default type
        Primary = 1,               //!< not refitted, just copy constructed from original Track
        Refitted = 2,              //!< normally refitted, without adding any pseudo-measurement 
        Accumulated = 3,           //!< accumulated by the GX algorithm.  

        NVertexTypes               //!< total number of track types
    };
    
    /** default constructor  */
    AlignVertex();

    /** constructor takes the original vertex candidate */
    AlignVertex(const xAOD::Vertex* originalVertex);
    
    /** copy constructor **/
    AlignVertex( const AlignVertex & rhs);
    
    /** assignment **/
    AlignVertex & operator=( const AlignVertex & rhs);

    /** destructor */
    ~AlignVertex();


    /** returns collection of alignTracks */
    const AlignTrackCollection* alignTrackCollection() const;

    /** retrieve iterator pointer to first element in collection */
    AlignTrackCollection::const_iterator firstATrack() const;

    /** returns iterator pointer to last element in collection */
    AlignTrackCollection::const_iterator lastATrack() const;

    /** retrieve pointer to original vertex and its position*/
    const xAOD::Vertex * originalVertex() const { return  m_original; }
    inline Amg::Vector3D*  originalPosition() const {return m_originalPosition;}

    /** set pointer to original vertex */
    void setOriginalVertex(const xAOD::Vertex* vertex) { m_original = vertex; }

    /**get and set the refit type */
    inline AlignVertexType type() const {return m_type;}
    inline void setType(AlignVertexType type) {m_type = type;}

    /**get the vertex position and covariance */
    inline Amg::Vector3D*   position()   const {return m_position;}
    inline AmgSymMatrix(3)* covariance() const {return m_covariance;}

    /** The Amg::VectorX is a vector of first-derivatives of the alignTSOS on the alignTrack 
        w.r.t. a particular alignment parameter times W*F.  There is one Amg::VectorX in the vector for 
        each alignment parameter of each align module. Set by accumulateVTX. */
    std::vector<AlignModuleVertexDerivatives>* derivatives() const { return m_derivatives; }
    void setDerivatives(std::vector<AlignModuleVertexDerivatives>* vec) { m_derivatives=vec; }
    void addDerivatives(std::vector<AlignModuleVertexDerivatives>* vec);


    /** dump align vertex information */
    void dump(MsgStream& msg) ;

    /** add a track pointer to the collection of this verterx: */
    void addAlignTrack(AlignTrack* alignTrack);

    /** increment algebra objects for this verterx: */
    void incrementVector(const Amg::Vector3D& vtxV)     { (*m_vVector)+=vtxV; }
    void incrementMatrix(const AmgSymMatrix(3) vtxM)   { (*m_matrix)+=vtxM; }


    /** fit the vertex internally */
    void fitVertex();

    /** get the number of contributing tracks */
    int  Ntracks() { return m_nTracks; }

    /** set and get the constraint on VTX position */
    void setConstraint(AmgSymMatrix(3)*, Amg::Vector3D*); 

    AmgSymMatrix(3)* Qmatrix() { return m_qMatrix; }

    Amg::Vector3D* Vvector() { return m_vVector; }

    void setUnconstrained() { m_constrained=false; }

    bool Constrained() { return m_constrained; }

       
   private:
    int                    m_nTracks;
    
    AmgSymMatrix(3)*       m_matrix;
    Amg::Vector3D*         m_vector;

    const xAOD::Vertex*    m_original;
    Amg::Vector3D*         m_originalPosition;

    AlignTrackCollection   m_alignTracks;

    Amg::Vector3D*         m_position;
    AmgSymMatrix(3) *      m_covariance;

    std::vector<AlignModuleVertexDerivatives > * m_derivatives;

    AmgSymMatrix(3)*       m_qMatrix;
    Amg::Vector3D*         m_vVector;
    bool                   m_constrained;


    AlignVertexType        m_type;
    
  }; // end class


  inline const AlignTrackCollection*  AlignVertex::alignTrackCollection() const { return &m_alignTracks; }

  inline AlignTrackCollection::const_iterator AlignVertex::firstATrack() const { return m_alignTracks.begin(); }
  inline AlignTrackCollection::const_iterator AlignVertex::lastATrack() const { return m_alignTracks.end(); }

} // end namespace

#endif // TRKALIGNEVENT_ALIGNVERTEX_H

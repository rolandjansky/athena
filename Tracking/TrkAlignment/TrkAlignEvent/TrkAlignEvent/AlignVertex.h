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
    AlignVertex(const VxCandidate* originalVertex);

    /** destructor */
    ~AlignVertex();


    /** returns collection of alignTracks */
    const AlignTrackCollection* alignTrackCollection() const;

    /** retrieve iterator pointer to first element in collection */
    AlignTrackCollection::const_iterator firstATrack() const;

    /** returns iterator pointer to last element in collection */
    AlignTrackCollection::const_iterator lastATrack() const;

    /** retrieve pointer to original vertex and its position*/
    const VxCandidate * originalVertex() const { return  theOriginal; }
    inline Amg::Vector3D*  originalPosition() const {return theOriginalPosition;}

    /** set pointer to original vertex */
    void setOriginalVertex(const VxCandidate* vertex) { theOriginal = vertex; }

    /**get and set the refit type */
    inline AlignVertexType type() const {return theType;}
    inline void setType(AlignVertexType type) {theType = type;}

    /**get the vertex position and covariance */
    inline Amg::Vector3D*   position()   const {return thePosition;}
    inline AmgSymMatrix(3)* covariance() const {return theCovariance;}

    /** The Amg::VectorX is a vector of first-derivatives of the alignTSOS on the alignTrack 
        w.r.t. a particular alignment parameter times W*F.  There is one Amg::VectorX in the vector for 
        each alignment parameter of each align module. Set by accumulateVTX. */
    std::vector<AlignModuleVertexDerivatives>* derivatives() const { return theDerivatives; }
    void setDerivatives(std::vector<AlignModuleVertexDerivatives>* vec) { theDerivatives=vec; }
    void addDerivatives(std::vector<AlignModuleVertexDerivatives>* vec);


    /** dump align vertex information */
    void dump(MsgStream& msg) ;

    /** add a track pointer to the collection of this verterx: */
    void addAlignTrack(AlignTrack* alignTrack);

    /** increment algebra objects for this verterx: */
    void incrementVector(const Amg::Vector3D& vtxV)     { (*theVector)+=vtxV; }
    void incrementMatrix(const AmgSymMatrix(3) vtxM)   { (*theMatrix)+=vtxM; }


    /** fit the vertex internally */
    void fitVertex();

    /** get the number of contributing tracks */
    int  Ntracks() { return theNtracks; }

    /** set and get the constraint on VTX position */
    void setConstraint(AmgSymMatrix(3)*, Amg::Vector3D*); 

    AmgSymMatrix(3)* Qmatrix() { return theQmatrix; }

    Amg::Vector3D* Vvector() { return theVvector; }

    void setUnconstrained() { theConstrained=false; }

    bool Constrained() { return theConstrained; }

       
   private:
    // methods


    // members

    int                    theNtracks;
    
    AmgSymMatrix(3)*       theMatrix;
    Amg::Vector3D*         theVector;

    const VxCandidate*     theOriginal;
    Amg::Vector3D*         theOriginalPosition;

    AlignTrackCollection   theAlignTracks;

    Amg::Vector3D*         thePosition;
    AmgSymMatrix(3) *      theCovariance;

    std::vector<AlignModuleVertexDerivatives > * theDerivatives;

    AmgSymMatrix(3)*       theQmatrix;
    Amg::Vector3D*         theVvector;
    bool                   theConstrained;


    AlignVertexType        theType;
    
  }; // end class


  inline const AlignTrackCollection*  AlignVertex::alignTrackCollection() const { return &theAlignTracks; }

  inline AlignTrackCollection::const_iterator AlignVertex::firstATrack() const { return theAlignTracks.begin(); }
  inline AlignTrackCollection::const_iterator AlignVertex::lastATrack() const { return theAlignTracks.end(); }

} // end namespace

#endif // TRKALIGNEVENT_ALIGNVERTEX_H

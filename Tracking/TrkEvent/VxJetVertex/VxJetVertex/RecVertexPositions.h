/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           RecVertexPositions.h  -  Description
                             -------------------
    begin   : Autumn 2006
    authors : Giacinto Piacquadio (Freiburg University)
    email   : Giacinto.Piacquadio@cern.ch
    changes : 


 ***************************************************************************/

#ifndef VxJetVertex_RecVertexPositions_H
#define VxJetVertex_RecVertexPositions_H

#include "VxJetVertex/VertexPositions.h" //!< include headers from the base class
#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Trk {

  class RecVertexPositions : public VertexPositions {
  public:
    RecVertexPositions(); //!< default constructor, if called initializes a RecVertexPositions with all data members set to 0.
    RecVertexPositions(const VertexPositions& pos); //!< construct a RecVertexPositions from a Vertex. ErrorMatrix will be 0,0,0.
    RecVertexPositions(const Amg::VectorX &pos, const Amg::MatrixX &cov,
		       const double ndf = 0., 
		       const double chi2 = 0.,bool isWeightTimesPosition=false); //!< constructs a RecVertexPositions with position, error, ndf, dhi2 and ntrk
    //-> added support for non integer degrees of freedom
    // Additional constructor for truth ?
    RecVertexPositions(const Amg::VectorX &pos, const double ndf = 0., const double chi2 =0.);
    RecVertexPositions(const RecVertexPositions& );  //!< copy constructor
    RecVertexPositions &operator= (const RecVertexPositions &);  //!< Assignement operator

    /** Output Method for MsgStream, to be overloaded by child classes */
    virtual MsgStream& dump(MsgStream& sl) const;
    /** Output Method for std::ostream, to be overloaded by child classes */
    virtual std::ostream& dump(std::ostream& sl) const;

    virtual ~RecVertexPositions(); //!< default destructor

    Amg::MatrixX const &covariancePosition() const; //!< return the covDeltaV matrix of the vertex fit

    void setCovariancePosition(const Amg::MatrixX &);

    void setFitQuality(const Trk::FitQuality &);

    /**
     * Fit quality access method
     */
    const Trk::FitQuality& fitQuality() const;

    const Amg::VectorX& weightTimesPosition();

    // Use storing weight matrix * position instead of position 
    // saves you from inverting the weight matrix at every fit iteration 
    // Then transparently, asking the position for the first time, 
    // the m_position will be restored to the correct value of position 
    // (using the invertion of the weight matrix only once...)
    void setWeightTimesPosition(const Amg::VectorX &);

    //the finalizePosition() method is used to ask the RecVertexPositions class 
    //to invert the weight matrix and fill the m_position vector with the real 
    //position, in order to finalize the position (and make it available)
    void finalizePosition();

  private:
    Amg::MatrixX m_positionError; //!< cov matrix on vertex position (primary vtx+jet dir+distance)
    Trk::FitQuality m_fitQuality; //chi2 and number of degrees of freedom of the fit
    //bool m_positionErrorInverseIsComputed;
  };


} // end of namespace
#endif

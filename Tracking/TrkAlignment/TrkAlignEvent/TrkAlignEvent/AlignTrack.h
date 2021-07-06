/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNTRACK_H
#define TRKALIGNEVENT_ALIGNTRACK_H

#include "GaudiKernel/MsgStream.h"

#include "AthContainers/DataVector.h"
#include "CxxUtils/CachedUniquePtr.h"
#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkTrack/Track.h"

#include <vector>
#include <fstream>

/**
   @file AlignTrack.h
   @class AlignTrack

   @brief AlignTrack is a generalization of a Trk::Track, used as the basic object to contain track information and other quantities useful for alignment.  Created by an implementation of IAlignTrackCreator, and other quantities added for a particular alignment method using an implementation of IAlignTrackDresser.

   @author Robert Harrington <roberth@bu.edu>
   @author Daniel Kollar <daniel.kollar@cern.ch>
   @date 10/14/2008
*/

namespace CLHEP {
  class HepSymMatrix;
}

namespace Trk {

  class AlignVertex;

  typedef DataVector<AlignTSOS> AlignTSOSCollection;
  typedef DataVector<AlignTSOS>::const_iterator AlignTSOSIt;
  typedef std::pair<const AlignModule*, std::vector<Amg::VectorX> > AlignModuleDerivatives;
  
  class AlignTrack : public Track {

  public:

    enum AlignTrackType {
        Unknown = 0,               //!< default type
        Original = 1,              //!< not refitted, just copy constructed from original Track
        NormalRefitted = 2,        //!< normally refitted, without adding any pseudo-measurement 
        BeamspotConstrained = 3,   //!< refitted with beamspot constraint
        VertexConstrained = 4,     //!< refitted with vertex constraint
        VertexMember = 5,          //!< Refitted with 3DoF from the VTX

        NTrackTypes                //!< total number of track types
    };
    
    /** constructor takes the track containing the original TrackStateOnSurface objects */
    AlignTrack(const Track& associatedTrack);

    /** constructor takes collection of alignTSOS and the track containing the original TrackStateOnSurface objects */
    AlignTrack(const Track& associatedTrack, AlignTSOSCollection * alignTSOSCollection);

    /** copy constructor */
    AlignTrack(const AlignTrack& alignTrack);

    /** destructor */
    virtual ~AlignTrack();

    /** assignment operator*/
    AlignTrack& operator= (const AlignTrack& alignTrack);

    /** sets collection of AlignTSOS */
    void setAlignTSOSCollection(AlignTSOSCollection * atsosColl);

    /** returns collection of alignTSOS */
    const AlignTSOSCollection* alignTSOSCollection() const;

    /** retrieve iterator pointer to first element in collection */
    AlignTSOSCollection::const_iterator firstAtsos() const;

    /** returns iterator pointer to last element in collection */
    AlignTSOSCollection::const_iterator lastAtsos() const;

    /** retrieve non-const iterator pointer to first element in collection */
    AlignTSOSCollection::iterator firstAtsos();

    /** returns non-const iterator pointer to last element in collection */
    AlignTSOSCollection::iterator lastAtsos();

    /** retrieve pointer to original track */
    const Track * originalTrack() const { return  m_originalTrack; }

    /** set pointer to original track */
    void setOriginalTrack(const Track* track) { m_originalTrack = track; }

    /**get and set the refit type */
    inline AlignTrackType type() const {return m_type;}
    inline void setType(AlignTrackType type) {m_type = type;}

    /** For the AlignTrack, if there is pseudo-measurement in the TSOS collection, 
        the perigee will not alway be in the first position. We should explicitly do the perigee
        finding. Fortunately, base Class already has a protected function which loop over the TSOS
        colleciton to do perigee search and assigin the perigee parameters at the perigee. 
        Just call it */
    void doFindPerigee() const { findPerigee();}

    /** method to determine whether a straight line track or not */
    bool isSLTrack() const;

    /** static method to determine whether a track is straight line or not */
    static bool isSLTrack(const Track* track);

    /** dump track information */
    static void dumpTrackInfo(const Track& track, MsgStream& msg) ;

    /** dump less track information */
    static void dumpLessTrackInfo(const Track& track, MsgStream& msg) ;
    
    /** dump align track information */
    void dump(MsgStream& msg) ;

    /** local error matrix, calculated by AlignTrack by calling atsos->hitDistanceVar() */
    const Amg::SymMatrixX * localErrorMatrix() const  { return m_localErrorMat; }

    /** inverse local error matrix, calculated by AlignTrack by calling atsos->hitDistanceVar() */
    const Amg::SymMatrixX * localErrorMatrixInv() const  { return m_localErrorMatInv; }
    
    /** number of alignTSOS (including scatterers if included on AlignTrack */
    int nAlignTSOSMeas() const { return m_nAlignTSOSMeas; }

    /** The Amg::VectorX is a vector of first-derivatives of the alignTSOS on the alignTrack 
        w.r.t. a particular alignment parameter.  There is one Amg::VectorX in the vector for 
        each alignment parameter of each align module. Set by DerivCalcTool. */
    std::vector<AlignModuleDerivatives>* derivatives() const { return m_derivatives; }
    void setDerivatives(std::vector<AlignModuleDerivatives>* vec) { m_derivatives=vec; }

    /** The Amg::VectorX is a vector of errors in first-derivatives of the alignTSOS on 
        the alignTrack w.r.t. a particular alignment parameter.  There is one Amg::VectorX in 
        the vector for each alignment parameter of each align module. Set by 
        DerivCalcTool. */
    std::vector<AlignModuleDerivatives>* derivativeErr() const { return m_derivativeErr; }
    void setDerivativeErr(std::vector<AlignModuleDerivatives>* vec) { m_derivativeErr=vec; }

    /** The Amg::VectorX is a vector of first-derivatives of the alignTSOS on the alignTrack 
        w.r.t. a particular alignment parameter.  There is one Amg::VectorX in the vector for 
        each alignment parameter of each align module. Set by DerivCalcTool. */
    std::vector<std::pair<const AlignModule*,std::vector<double> > >* actualSecondDerivatives() const { return m_actualSecondDerivatives; }
    void setActualSecondDerivatives(std::vector<std::pair<const AlignModule*,std::vector<double> > >* vec) { m_actualSecondDerivatives=vec; }
    
    /** Vector of residuals of the alignTSOS on the alignTrack, to be set by AlignTrackDresser.  Owned by AlignTrack. */
    Amg::VectorX* residualVector() const { return m_residuals; }
    void setResidualVector(Amg::VectorX* vec) { m_residuals=vec;}
    
    /** Weight matrix is W from Si alignment (see Eqn. 10 of INDET-PUB-2005-002).  
        It is related to the residual covariance matrix from TRT alignment by 
        W= V^{-1}RV (R is defined in Eqn. 18 of ATL-INDET-PUB-2007-009).*/ 
    Amg::SymMatrixX* weightMatrix() const  { return m_weights; } 
    void setWeightMatrix( Amg::SymMatrixX* mat) { m_weights=mat; }

    /** First deriv weight matrix can be either W from Si alignment 
        (see Eqn. 10 of INDET-PUB-2005-002) or the inverse local error matrix */  
    Amg::SymMatrixX* weightMatrixFirstDeriv() const  { return m_weightsFirstDeriv; }
    void setWeightMatrixFirstDeriv( Amg::SymMatrixX* mat) { m_weightsFirstDeriv=mat; }

    /** locally calculated chi2 */
    double chi2() const { return m_chi2; }
    void setChi2(double chi2) { m_chi2=chi2; }

    /** locally calculated chi2 */
    double chi2dof() const { return m_chi2dof; }
    void setChi2dof(double chi2dof) { m_chi2dof=chi2dof; }

    /** set and get derivative matrix */    
    const Amg::MatrixX* derivativeMatrix() const { return m_derivativeMatrix; }
    void setDerivativeMatrix(const Amg::MatrixX* matrix);

    /** set and get full covariance matrix */    
    const Amg::SymMatrixX* fullCovarianceMatrix() const { return m_fullCovarianceMatrix; }
    void setFullCovarianceMatrix(const Amg::SymMatrixX* matrix);

    /** set and get array (one entry per alignment parameter) of fit chi2 of chi2 vs. alignment params (used for numerical derivatives)*/
    double trackAlignParamQuality(int i) const { return m_trackAlignParamQuality[i]; }
    void setTrackAlignParamQuality(int i, double val) { m_trackAlignParamQuality[i]=val; }

    /** returns track with ScatteringAngle pointers all set to zero (used for refit by iPat) */
    const Trk::Track* trackWithoutScattering() const;

    /** set and get pointer to the associated vertex */
    AlignVertex*    getVtx() const { return m_vtx; }
    void setVtx(AlignVertex * const vtx)  { m_vtx = vtx; }

    /** get refit flags */
    bool refitD0()    const { return m_refitD0; }
    bool refitZ0()    const { return m_refitZ0; }
    bool refitPhi()   const { return m_refitPhi; }
    bool refitTheta() const { return m_refitTheta; }
    bool refitQovP()  const { return m_refitQovP; }
       
    /** set refit flags */
    void setRefitD0(bool flag)     { m_refitD0    = flag; }
    void setRefitZ0(bool flag)     { m_refitZ0    = flag; }
    void setRefitPhi(bool flag)    { m_refitPhi   = flag; }
    void setRefitTheta(bool flag)  { m_refitTheta = flag; }
    void setRefitQovP(bool flag)   { m_refitQovP  = flag; }
       
   private:
    // methods
    AlignTrack();
    
    /** sets derivatives of alignment parameters */
    void setAlignParameters(); 

    /** point to original track */
    const Track * m_originalTrack;

    /**refit type of this alignTrack */
    AlignTrackType m_type;

    /** variables **/
    int m_nAlignTSOSMeas;

    /** collection of alignTSOS that make up the alignTrack */
    AlignTSOSCollection * m_alignTSOSCollection{};

    /** matrix containing local error matrices along diagonal */
    Amg::SymMatrixX * m_localErrorMat;               //Symmetric Matrix

    /** inverse matrix containing local error matrices along diagonal */
    Amg::SymMatrixX * m_localErrorMatInv;            //Symmetric Matrix

    /** matrix containing derivative matrix from track fitter */
    const Amg::MatrixX * m_derivativeMatrix;      

    /** matrix containing full covariance matrix from track fitter */
    const Amg::SymMatrixX * m_fullCovarianceMatrix;  //Symmetric Matrix

    std::vector<AlignModuleDerivatives > * m_derivatives;  
    std::vector<AlignModuleDerivatives > * m_derivativeErr; 
    std::vector<std::pair<const AlignModule *,std::vector<double> > > * m_actualSecondDerivatives;

    Amg::VectorX *        m_residuals;
    Amg::MatrixX *        m_weights;              //Symmetric Matrix
    Amg::MatrixX *        m_weightsFirstDeriv;    //Symmetric Matrix

    double                m_chi2;
    double                m_chi2dof;

    /** pointer to the associated vertex */
    AlignVertex*          m_vtx;

    bool                  m_refitD0;
    bool                  m_refitZ0;
    bool                  m_refitPhi;
    bool                  m_refitTheta;
    bool                  m_refitQovP;
   
    double              * m_trackAlignParamQuality; //!> describes the quality of contribution to track to alignment parameter (used by shifting derivatives)

    CxxUtils::CachedUniquePtr<Trk::Track> m_trackWithoutScattering; //!> the original track with ScatteringAngle pointers set to 0 (called for refit so scattering will be recalculated)

  }; // end class

  MsgStream&    operator<< (MsgStream& sl, const AlignTrack::AlignTrackType type);
  std::ostream& operator<< (std::ostream& ol, const AlignTrack::AlignTrackType type);
  std::string dumpAlignTrackType(const AlignTrack::AlignTrackType type);

  inline const AlignTSOSCollection * AlignTrack::alignTSOSCollection() const { return m_alignTSOSCollection; }

  inline void AlignTrack::setDerivativeMatrix(const Amg::MatrixX * matrix) { 
    if (!m_derivativeMatrix) delete m_derivativeMatrix; 
    m_derivativeMatrix=new Amg::MatrixX(*matrix); 
  }

  inline void AlignTrack::setFullCovarianceMatrix(const Amg::SymMatrixX * matrix) { 
    if (!m_fullCovarianceMatrix) delete m_fullCovarianceMatrix; 
    m_fullCovarianceMatrix=new Amg::SymMatrixX(*matrix); 
  }

  inline AlignTSOSCollection::const_iterator AlignTrack::firstAtsos() const { return m_alignTSOSCollection->begin(); }
  inline AlignTSOSCollection::const_iterator AlignTrack::lastAtsos()  const { return m_alignTSOSCollection->end(); }

  inline AlignTSOSCollection::iterator AlignTrack::firstAtsos() { return m_alignTSOSCollection->begin(); }
  inline AlignTSOSCollection::iterator AlignTrack::lastAtsos()  { return m_alignTSOSCollection->end(); }

} // end namespace

#endif // TRKALIGNEVENT_ALIGNTRACK_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// GXFTrackState.h
//   Class grouping all fitter relevant info on a surface-by-surface
//   basis
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_GXFTRACKSTATE_H
#define TRK_GXFTRACKSTATE_H

#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

namespace Trk {

  class MeasurementBase;
  class TrackStateOnSurface;
  class GXFMaterialEffects;
  class TransportJacobian;
  
  class GXFTrackState
  {
  public:

    //! default constructor
    GXFTrackState();
    //! copy constructor
    GXFTrackState(GXFTrackState&);
    //! destructor
    ~GXFTrackState();

    //! Constructor for measurement
    GXFTrackState(const MeasurementBase*,const TrackParameters *trackpar=0,bool ownmb=false);
   
    //! constructor for hole on track
    GXFTrackState(const TrackParameters*);

    //! constructor for material on track
    GXFTrackState(GXFMaterialEffects*, const TrackParameters *trackpar=0);


    //! assignment operator
    GXFTrackState& operator=(GXFTrackState& rhs);

    
    
    //! replace measurement by new, eg recalibrated one
    void setMeasurement(const MeasurementBase*);
    
    //! read access for data member: Measurement
    const MeasurementBase* measurement(bool takeownership=false);
    
    TrackState::TrackStateType trackStateType();
    void setTrackStateType(TrackState::TrackStateType);    
    
    //! set method for data member: TPs
    void setTrackParameters(const TrackParameters*);
    //! read access method for data member: TPs
    const TrackParameters* trackParameters(bool takeownership=false);
    
    
    void setMaterialEffects(GXFMaterialEffects*);
    //! read access method for data member: MaterialEffects
    GXFMaterialEffects* materialEffects();

    //! read access method for data member: surface
    const Surface* surface();

    //! set method for data member: TransportJacobian
    void setJacobian(TransportJacobian*);
    //! read access method for data member: TransportJacobian
    double (*(jacobian)())[5];

    CLHEP::HepMatrix& derivatives();
    void setDerivatives(CLHEP::HepMatrix&);

    AmgSymMatrix(5)* trackCovariance(bool takeownership=false);
    void setTrackCovariance(AmgSymMatrix(5)*);    

    const FitQualityOnSurface *fitQuality(bool takeownership=false);
    void setFitQuality(const FitQualityOnSurface *);

    //! retrieve the measurement/detector type
    TrackState::MeasurementType measurementType();
    //! set the measurement/detector type
    void setMeasurementType(TrackState::MeasurementType);

    double sinStereo();
    void setSinStereo(double);

    double *measurementErrors();
    void setMeasurementErrors(double*);

    int numberOfMeasuredParameters();

    bool isRecalibrated();
    void setRecalibrated(bool);

    const Amg::Vector3D &position();
    void setPosition(Amg::Vector3D &);

    bool measuresPhi();
    void setMeasuresPhi(bool);

  private:
    //const Surface*             m_surface;         //!< The surface corresponding to the track state
    const MeasurementBase*     m_measurement;     //!< The measurement defining the track state
    TrackState::TrackStateType m_tsType;          //!< type of track state, eg Fittable, Outlier, Scatterer, Brem, Hole
    const TrackParameters*     m_trackpar;    //!< track parameters
    GXFMaterialEffects* m_materialEffects;  //!< Material effects on track (ie scatterer, brem)
    double      m_jacobian[5][5];     //!< Transport jacobian wrt previous state
    CLHEP::HepMatrix           *m_derivs;           //!< Derivatives of local parameters wrt fit parameters
    AmgSymMatrix(5)*    m_covariancematrix;    //!< Covariance matrix of track parameters at this surface
    const FitQualityOnSurface* m_fitqual;
    double           m_measerror[5];     //!< Measurement errors (corrected for stereo angle)       
    double                     m_sinstereo;       //!< stereo angle
    TrackState::MeasurementType m_mType;      //!< Measurement type, eg pixel, SCT, ...
    bool m_recalib;                        //!< Has this measurement already been recalibrated?
    bool m_owntp;
    bool m_ownmb;
    bool m_ownfq;
    bool m_owncov;
    bool m_measphi;
    Amg::Vector3D m_globpos;
  };


inline double (*(GXFTrackState::jacobian)())[5]  { return m_jacobian; }

inline CLHEP::HepMatrix& GXFTrackState::derivatives()  { return *m_derivs; }

inline AmgSymMatrix(5)* GXFTrackState::trackCovariance(bool takeownership)  { 
  AmgSymMatrix(5)* tmpcov=m_covariancematrix;
  if (takeownership) {
    m_owncov=false; 
    //m_covariancematrix=0;
  }
  return tmpcov; 
}

inline const TrackParameters* GXFTrackState::trackParameters(bool takeownership)   {if (takeownership) m_owntp=false; return m_trackpar; }

inline GXFMaterialEffects* GXFTrackState::materialEffects() {
  return m_materialEffects; 
}

inline TrackState::MeasurementType GXFTrackState::measurementType() {
  return m_mType; 
}

inline TrackState::TrackStateType GXFTrackState::trackStateType()  {
  return m_tsType; 
}

inline void GXFTrackState::setMeasurementType(TrackState::MeasurementType mt){
  m_mType = mt; 
}

inline bool GXFTrackState::isRecalibrated(){
  return m_recalib;  
}

}
#endif

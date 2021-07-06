/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ProtoTrackStateOnSurface.h
//   Class grouping all fitter relevant info on a surface-by-surface
//   basis
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_PROTOTRACKSTATE_H
#define TRK_PROTOTRACKSTATE_H

#include "Identifier/Identifier.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h" // typedef
#include "TrkParameters/TrackParameters.h"          // typedef
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkExUtils/TransportJacobian.h"
#include "TrkFitterUtils/TrackCalibDefs.h"
#include <vector>
#include "CxxUtils/CachedUniquePtr.h"
namespace Trk {

  class MeasurementBase;
  class TrackStateOnSurface;
  class DNA_MaterialEffects;
  class ProtoMaterialEffects;
  /** @brief This class is a container to group all fitter internal
  information (input, temp and final results) for each surface.

  Contrary to the TrkTrack/TrackStateOnSurface it is less strict than
  the EDM counterpart, allowing a more flexible ownership management.
  Consequently this class is not meant for transmitting information
  between algorithms or handling by StoreGate.

  There are three types of methods handling each data member:
  - checkin methods set a pointer in the ProtoTrackStateOnSurface
    class and transfer ownership to it.
    There shall not be a pointer to this kind of data member already set.
    To overwrite a member, use eg. "delete ptsos.checkoutForwardPar();" first.
  - checkout methods get a pointer from this class, reset it to 0 and the
    ownership goes to the client (which will have to delete/storegatify it)
  - simple access methods return the pointer, but do not modify it and
    the ownership stays with this class

  @author Wolfgang Liebig <http://consult.cern.ch/xwho>
  */
  class ProtoTrackStateOnSurface
  {
  public:

    //! default constructor - needed to make a vector of these objects
    ProtoTrackStateOnSurface();
    //! copy constructor
    ProtoTrackStateOnSurface(const ProtoTrackStateOnSurface&);
    //! default desctructor
    ~ProtoTrackStateOnSurface();

    //! simple constructor with a MBase which the ProtoTrackState will either own or not
    ProtoTrackStateOnSurface(const MeasurementBase*,
                             bool, bool,
                             int positionOnTrajectory = -1,
                             const TransportJacobian* = 0,
                             const TrackParameters *  = 0);

    //! assignment operator
    ProtoTrackStateOnSurface& operator=(const ProtoTrackStateOnSurface& rhs);

    //! swap function: avoids new and delete of owned objects
    void swap(ProtoTrackStateOnSurface& rhs) throw (); // No throw exception guarantee

    //! checkin for data member: Measurement
    void checkinMeasurement(const MeasurementBase*,
                            bool  classShallOwnMbase=true);

    //! replace measurement by new, eg recalibrated one
    void replaceMeasurement(const MeasurementBase*,
                            TrackState::CalibrationType=Trk::TrackState::CalibrationNotKnown);

    //! checkout for data member: Measurement
    const MeasurementBase* checkoutMeasurement();

    //! read access for data member: Measurement
    const MeasurementBase* measurement() const;

    //! retrieve flag regarding outlier status
    bool isOutlier() const;
    //! retrieve flag if the outlier was set when the fitter works with the PTSoS
    bool isNewOutlier() const;
    //! retrieve type of outlier
    TrackState::TrackStateType trackStateType() const;

    //! set flag regarding outlier status
    void isOutlier(bool, int i=0);
    //! set flag regarding outlier status
    void isOutlier(TrackState::TrackStateType, int i=0);
    //! query the iteration number at which this outlier was spotted
    int  iterationShowingThisOutlier() const;

    //! checkin for data member: TransportJacobian
    void checkinTransportJacobian(const TransportJacobian*,
				  bool  classShallOwnJacobian=true);
    //! checkout for data member: TransportJacobian
    const TransportJacobian* checkoutTransportJacobian();
    //! read access for data member: TransportJacobian
    const TransportJacobian* jacobian() const;

    //! checkin for data member: ReferenceParameters (TrkParameters)
    void checkinReferenceParameters(const TrackParameters*,
                                    bool  classShallOwnRefPars=true);
    //! checkout for data member: TrackParameters
    const TrackParameters* checkoutReferenceParameters();
    //! read access for data member: referenceParameters
    const TrackParameters* referenceParameters() const;

    //! checkin for data member: ParametersDifference FIXME review if pointer of simply number
    void checkinParametersDifference(const AmgVector(5)*,
				     bool  classShallOwnDifference=true);
    //! checkout for data member: HepVector
    const AmgVector(5)* checkoutParametersDifference();
    //! read access for data member: ParametersDifference
    const AmgVector(5)* parametersDifference() const;

    //! checkin for data member: Covariance of Track Parameters as simple matrix
    void checkinParametersCovariance(const AmgSymMatrix(5)*,
                    bool  classShallOwnCovariance=true);
    //! checkout for data member: Covariance
    const AmgSymMatrix(5)* checkoutParametersCovariance();
    //! read access for data member: Covariance
    const AmgSymMatrix(5)* parametersCovariance() const;

    //! checkin method for data member: TPs from forward fit
    void checkinForwardPar(const TrackParameters*);
    //! checkout method for data member: TPs from forward fit
    const TrackParameters* checkoutForwardPar();
    //! read access method for data member: TPs from forward fit
    const TrackParameters* forwardTrackParameters() const;

    //! checkin method for data member: TPs from smoothing
    void checkinSmoothedPar(const TrackParameters*);
    //! checkout method for data member: TPs from smoothing
    const TrackParameters* checkoutSmoothedPar();
    //! read access method for data member: TPs from smoothing
    const TrackParameters* smoothedTrackParameters() const;

    //! checkin method for data member: FitQualityOnSurface (smoothed state)
    void checkinFitQuality(const FitQualityOnSurface*);
    //! checkout method for data member: FitQualityOnSurface (smoothed state)
    const FitQualityOnSurface* checkoutFitQuality();
    //! read access method for data member: FitQualityOnSurface (smoothed state)
    const FitQualityOnSurface* fitQuality() const;

    //! set fit quality numbers (not FQ object ownership) for forward filter's chi2
    void setForwardStateFitQuality(const FitQualityOnSurface&);
    //! set fit quality numbers directly for forward filter's chi2
    void setForwardStateFitQuality(double, double);
    //! get forward filter's fit quality numbers
    double forwardStateChiSquared() const;
    double forwardStateNumberDoF() const;
    void backwardStateChiSquared(double);
    double backwardStateChiSquared() const;

    //! checkin method for data member: ProtoMaterialEffects
    void checkinMaterialEffects(const ProtoMaterialEffects*);
    //! checkout method for data member: ProtoMaterialEffects
    const ProtoMaterialEffects* checkoutMaterialEffects();
    //! read access method for data member: ProtoMaterialEffects
    const ProtoMaterialEffects* materialEffects() const;

    //! checkin method for data member: DNA_MaterialEffects
    void checkinDNA_MaterialEffects(const DNA_MaterialEffects*);
    //! checkout method for data member: DNA_MaterialEffects
    const DNA_MaterialEffects* checkoutDNA_MaterialEffects();
    //! read access method for data member: DNA_MaterialEffects
    const DNA_MaterialEffects* dnaMaterialEffects() const;

    //! retrieve a serial position ID useful for debugging
    int   positionOnTrajectory()    const;
    //! set a serial position ID useful for debugging
    void  positionOnTrajectory(int);

    //! retrieve the Identifier
    const Identifier identify() const;
    //! set the Identifier (default is no ID present, ID=0)
    void  identifier(Identifier);

    //! say if the measurement it holds is a cluster (Pixel, SCT, trigger chambers)
    bool  isCluster() const;
    //! say if the measurement it holds is a drift-circle (TRT, MDT)
    bool  isDriftCircle() const;

    //! retrieve the measurement/detector type
    TrackState::MeasurementType measurementType() const;
    //! retrieve the calibration type
    TrackState::CalibrationType calibrationType() const;
    //! set the measurement/detector type
    void setMeasurementType(TrackState::MeasurementType,
                            TrackState::CalibrationType);

    //! create an EDM TrackStateOnSurface object from the internal information
    const Trk::TrackStateOnSurface* createState ATLAS_NOT_THREAD_SAFE (bool makeSlimOutlier=true);

    //! get surface (either from measurement or from material effects)
    const Trk::Surface *surface() const;

    //! get the measurement position as difference to reference parameters
    const Amg::VectorX* measurementDifference() const;

  private:
    const MeasurementBase*     m_measurement;     //!< The measurement defining the track state
    bool                       m_iOwnMeasurement; //!< flag to decide if the MB may be checked out/deleted
    TrackState::TrackStateType m_tsType;          //!< flag if outlier and which
    int                        m_iterationShowingOutlier; //!< outlier from it #
    const TrackParameters*     m_forwardState;    //!< track state obtaine during forward filtering
    const TrackParameters*     m_smoothedState;   //!< possibly final state after smoothing
    const FitQualityOnSurface* m_fitQuality;      //!< fit quality on this surface
    const DNA_MaterialEffects* m_dnaMaterialEffects; //!< extended mefot for DNA
    const ProtoMaterialEffects* m_protoMaterialEffects; //!< material layer and scattering info
    int                        m_stateID;         //!< simple counter to ease state numbering in fitter
    Identifier                 m_identifier;      //!< identify measurement for debugging
    TrackState::MeasurementType m_mType;           //!< store result once Meas is identified
    TrackState::CalibrationType m_calib;           //!< describe quality of measurement
    double                     m_forwardStateChi2; //!< forward-filter's update chi2
    double                     m_forwardStateNdof; //!< forward-filter's update ndof
    double                     m_backwardStateChi2;//!< backward-filter's update chi2
    const TransportJacobian*   m_transportJacobian;//!< Jacobian from propagator
    bool                       m_iOwnJacobian;     //!< flag to steer ownership of Jacobian
    const TrackParameters*     m_referenceParameters; //!< reference TrkParameters from propagator
    bool                       m_iOwnRefPars;      //!< flag to steer ownership of ref pars
    const AmgVector(5)*        m_parametersDifference; //!< vector of diff reference minus filtered pars
    bool                       m_iOwnParametersDifference; //!< flag to steer ownership on difference
    const AmgSymMatrix(5)*     m_parametersCovariance; //!< covariance matrix of fitted parameters
    bool                       m_iOwnParametersCovariance; //!< flag for ownership of covarinace matrix of fitted parameters
    CxxUtils::CachedUniquePtr<Amg::VectorX>   m_measurementDifferenceVector; //!< cached difference between reference TrkParameters and measurement
  };

  /// general swap function for ProtoTrackStateOnSurface (to speed up sort, etc)
  void swap(ProtoTrackStateOnSurface& lhs, ProtoTrackStateOnSurface& rhs);
}

// hmm ... inline means this code is copied into each object -> takes more space in vector, but faster.

inline bool Trk::ProtoTrackStateOnSurface::isOutlier() const { return (m_tsType != TrackState::Fittable && m_tsType!=TrackState::AnyState); }
inline bool Trk::ProtoTrackStateOnSurface::isNewOutlier() const
{ return (m_tsType!=TrackState::Fittable
          && m_tsType!=TrackState::AnyState
          && m_tsType!=TrackState::ExternalOutlier); }
inline void Trk::ProtoTrackStateOnSurface::isOutlier(bool outlier, int i) {
  m_tsType = outlier? TrackState::GeneralOutlier : TrackState::Fittable;
  if (i!=0) m_iterationShowingOutlier = i;
}
inline void Trk::ProtoTrackStateOnSurface::isOutlier(TrackState::TrackStateType outlier,
                                                     int i) {
  m_tsType = outlier;
  if (i!=0) m_iterationShowingOutlier = i;
}
inline int  Trk::ProtoTrackStateOnSurface::iterationShowingThisOutlier() const
{ return m_iterationShowingOutlier; }
inline int  Trk::ProtoTrackStateOnSurface::positionOnTrajectory() const { return m_stateID; }
inline void Trk::ProtoTrackStateOnSurface::positionOnTrajectory(int ID) { m_stateID = ID; }
inline const Trk::MeasurementBase* Trk::ProtoTrackStateOnSurface::measurement() const { return m_measurement; }
inline const Trk::TransportJacobian* Trk::ProtoTrackStateOnSurface::jacobian() const { return m_transportJacobian; }
inline const Trk::TrackParameters* Trk::ProtoTrackStateOnSurface::referenceParameters() const { return m_referenceParameters; }
inline const AmgVector(5)* Trk::ProtoTrackStateOnSurface::parametersDifference() const { return m_parametersDifference; }
inline const AmgSymMatrix(5)* Trk::ProtoTrackStateOnSurface::parametersCovariance() const { return m_parametersCovariance; }
inline const Trk::TrackParameters* Trk::ProtoTrackStateOnSurface::forwardTrackParameters() const
  {return m_forwardState; }
inline const Trk::TrackParameters* Trk::ProtoTrackStateOnSurface::smoothedTrackParameters() const
  {return m_smoothedState; }
inline const Trk::FitQualityOnSurface* Trk::ProtoTrackStateOnSurface::fitQuality() const
{return m_fitQuality; }
inline const Trk::DNA_MaterialEffects* Trk::ProtoTrackStateOnSurface::dnaMaterialEffects() const
{return m_dnaMaterialEffects; }
inline const Trk::ProtoMaterialEffects* Trk::ProtoTrackStateOnSurface::materialEffects() const
{return m_protoMaterialEffects; }
inline const Identifier Trk::ProtoTrackStateOnSurface::identify() const
  {return m_identifier; }
inline void  Trk::ProtoTrackStateOnSurface::identifier(Identifier id)
  {m_identifier = id; }
inline Trk::TrackState::MeasurementType
Trk::ProtoTrackStateOnSurface::measurementType() const
  {return m_mType; }
inline Trk::TrackState::CalibrationType
Trk::ProtoTrackStateOnSurface::calibrationType() const
  {return m_calib; }
inline Trk::TrackState::TrackStateType
Trk::ProtoTrackStateOnSurface::trackStateType() const
  {return m_tsType; }
inline void Trk::ProtoTrackStateOnSurface::setMeasurementType(TrackState::MeasurementType mt,
                                                              TrackState::CalibrationType ct)
  {m_mType = mt; m_calib = ct;}
inline double Trk::ProtoTrackStateOnSurface::forwardStateChiSquared() const
  { return m_forwardStateChi2; }
inline double Trk::ProtoTrackStateOnSurface::forwardStateNumberDoF() const
  { return m_forwardStateNdof; }
inline double Trk::ProtoTrackStateOnSurface::backwardStateChiSquared() const
  { return m_backwardStateChi2; }

#endif

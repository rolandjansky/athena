/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ProtoTrackStateOnSurface.cxx
// see header file for documentation.
//////////////////////////////////////////////////////////////////
//
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "TrkFitterUtils/ProtoMaterialEffects.h"

//#include "GaudiKernel/MsgStream.h"
//#include <string>
#include <algorithm>

Trk::ProtoTrackStateOnSurface::ProtoTrackStateOnSurface()
  :
  m_measurement(nullptr),
  m_iOwnMeasurement(false),
  m_tsType(Trk::TrackState::Fittable),
  m_iterationShowingOutlier(0),
  m_forwardState(nullptr),
  m_smoothedState(nullptr),
  m_fitQuality(nullptr),
  m_dnaMaterialEffects(nullptr),
  m_protoMaterialEffects(nullptr),
  m_stateID(0),
  m_identifier(),
  m_mType(Trk::TrackState::unidentified),
  m_calib(Trk::TrackState::CalibrationNotKnown),
  m_forwardStateChi2(0.0),
  m_forwardStateNdof(0.0),
  m_backwardStateChi2(0.0),
  m_transportJacobian ( nullptr ),
  m_iOwnJacobian ( true ),
  m_referenceParameters ( nullptr ),
  m_iOwnRefPars( false ),
  m_parametersDifference ( nullptr ),
  m_iOwnParametersDifference ( true ),
  m_parametersCovariance(nullptr),
  m_iOwnParametersCovariance(true),
  m_measurementDifferenceVector(nullptr)
{
  m_identifier.clear();
}

Trk::ProtoTrackStateOnSurface::ProtoTrackStateOnSurface
(const Trk::ProtoTrackStateOnSurface& rhs
 ):
  m_measurement(rhs.m_iOwnMeasurement
                ? (rhs.m_measurement ? rhs.m_measurement->clone() : nullptr)
                : rhs.m_measurement),
  m_iOwnMeasurement(rhs.m_iOwnMeasurement),
  m_tsType(rhs.m_tsType),
  m_iterationShowingOutlier(rhs.m_iterationShowingOutlier),
  m_forwardState(rhs.m_forwardState ? rhs.m_forwardState->clone() : nullptr),
  m_smoothedState(rhs.m_smoothedState ? rhs.m_smoothedState->clone() : nullptr),
  m_fitQuality(rhs.m_fitQuality ? new Trk::FitQualityOnSurface(*rhs.m_fitQuality) : nullptr ),
  m_dnaMaterialEffects(rhs.m_dnaMaterialEffects ? new Trk::DNA_MaterialEffects(*rhs.m_dnaMaterialEffects) : nullptr ),
  m_protoMaterialEffects(rhs.m_protoMaterialEffects ? new Trk::ProtoMaterialEffects(*rhs.m_protoMaterialEffects) : nullptr ),
  m_stateID(rhs.m_stateID),
  m_identifier(rhs.m_identifier),
  m_mType(rhs.m_mType),
  m_calib(rhs.m_calib),
  m_forwardStateChi2(rhs.m_forwardStateChi2),
  m_forwardStateNdof(rhs.m_forwardStateNdof),
  m_backwardStateChi2(rhs.m_backwardStateChi2),
  m_transportJacobian ( rhs.m_iOwnJacobian ? ( ( rhs.m_transportJacobian?  new Trk::TransportJacobian ( *rhs.m_transportJacobian ) :nullptr ) ) : rhs.m_transportJacobian ),
  m_iOwnJacobian ( rhs.m_iOwnJacobian ),
  m_referenceParameters ( rhs.m_referenceParameters ? rhs.m_referenceParameters->clone() : nullptr ),
  m_iOwnRefPars ( rhs.m_iOwnRefPars ),
  m_parametersDifference ( rhs.m_iOwnParametersDifference?
    ( rhs.m_parametersDifference? new AmgVector(5) ( *rhs.m_parametersDifference ) : nullptr ) : rhs.m_parametersDifference),
  m_iOwnParametersDifference ( rhs.m_iOwnParametersDifference ),
  m_parametersCovariance ( rhs.m_iOwnParametersCovariance?
    ( rhs.m_parametersCovariance? new AmgSymMatrix(5) ( *rhs.m_parametersCovariance ) : nullptr ) : rhs.m_parametersCovariance),
  m_iOwnParametersCovariance ( rhs.m_iOwnParametersCovariance ),
  m_measurementDifferenceVector(nullptr)
{
}

Trk::ProtoTrackStateOnSurface::ProtoTrackStateOnSurface
(
 const Trk::MeasurementBase* measurement,
 bool  is_outlier,
 bool  iOwnMeasurement,
 int   positionOnTrajectory,
 const Trk::TransportJacobian* inTransportJacobian,
 const Trk::TrackParameters* inReferenceParameters
 ):
  m_measurement(measurement),
  m_iOwnMeasurement(iOwnMeasurement),
  m_tsType(is_outlier ? Trk::TrackState::ExternalOutlier : 
           Trk::TrackState::Fittable),
  m_iterationShowingOutlier(0),
  m_forwardState(nullptr),
  m_smoothedState(nullptr),
  m_fitQuality(nullptr),
  m_dnaMaterialEffects(nullptr),
  m_protoMaterialEffects(nullptr),
  m_stateID(positionOnTrajectory),
  m_identifier(),
  m_mType(Trk::TrackState::unidentified),
  m_calib(Trk::TrackState::CalibrationNotKnown),
  m_forwardStateChi2(0.0),
  m_forwardStateNdof(0.0),
  m_backwardStateChi2(0.0),
  m_transportJacobian ( inTransportJacobian ),
  m_iOwnJacobian ( true ),
  m_referenceParameters ( inReferenceParameters ),
  m_iOwnRefPars ( true ),
  m_parametersDifference (nullptr),
  m_iOwnParametersDifference(true),
  m_parametersCovariance(nullptr),
  m_iOwnParametersCovariance(true),
  m_measurementDifferenceVector(nullptr)
{
  m_identifier.clear();
}

Trk::ProtoTrackStateOnSurface::~ProtoTrackStateOnSurface()
{
  if (m_iOwnMeasurement) delete m_measurement;
  delete m_forwardState;
  delete m_smoothedState;
  delete m_fitQuality;
  delete m_dnaMaterialEffects;
  delete m_protoMaterialEffects;
  if ( m_iOwnJacobian ) {
    delete m_transportJacobian;
    m_transportJacobian =nullptr;
  }
  if ( m_iOwnRefPars ) {
    delete m_referenceParameters;
    m_referenceParameters =nullptr;
  }
  if ( m_iOwnParametersDifference ) {
    delete m_parametersDifference;
    m_parametersDifference =nullptr;
  }
  
  if (m_iOwnParametersCovariance) {
      delete m_parametersCovariance;
  }
}

Trk::ProtoTrackStateOnSurface& Trk::ProtoTrackStateOnSurface::operator=(const Trk::ProtoTrackStateOnSurface& rhs)
{
  if (this!=&rhs){
    if (m_iOwnMeasurement) delete m_measurement;
    delete m_forwardState;
    delete m_smoothedState;
    delete m_fitQuality;
    if ( m_iOwnJacobian ) delete m_transportJacobian;
    if ( m_iOwnRefPars )  delete m_referenceParameters;
    if ( m_iOwnParametersDifference ) delete m_parametersDifference;
    if ( m_iOwnParametersCovariance ) delete m_parametersCovariance;
    m_measurement = rhs.m_iOwnMeasurement
      ? (rhs.m_measurement ? rhs.m_measurement->clone() : nullptr)
      : rhs.m_measurement;
    m_iOwnMeasurement = rhs.m_iOwnMeasurement;
    m_tsType = rhs.m_tsType;
    m_iterationShowingOutlier = rhs.m_iterationShowingOutlier;
    m_forwardState = rhs.m_forwardState ? rhs.m_forwardState->clone() : nullptr;
    m_smoothedState = rhs.m_smoothedState ? rhs.m_smoothedState->clone() : nullptr;
    m_fitQuality = rhs.m_fitQuality ? new Trk::FitQualityOnSurface(*rhs.m_fitQuality) : nullptr ;
    m_dnaMaterialEffects = rhs.m_dnaMaterialEffects ? new Trk::DNA_MaterialEffects(*rhs.m_dnaMaterialEffects) : nullptr ;
    m_protoMaterialEffects = rhs.m_protoMaterialEffects ? new Trk::ProtoMaterialEffects(*rhs.m_protoMaterialEffects) : nullptr ;
    m_stateID          = rhs.m_stateID;
    m_identifier       = rhs.m_identifier;
    m_mType            = rhs.m_mType;
    m_calib            = rhs.m_calib;
    m_forwardStateChi2 = rhs.m_forwardStateChi2;
    m_forwardStateNdof = rhs.m_forwardStateNdof;
    m_backwardStateChi2= rhs.m_backwardStateChi2;
    m_transportJacobian= rhs.m_iOwnJacobian
                         ? ( rhs.m_transportJacobian 
                             ? new Trk::TransportJacobian ( *rhs.m_transportJacobian ) : nullptr )
                         : rhs.m_transportJacobian ;
    m_iOwnJacobian  = rhs.m_iOwnJacobian;
    m_referenceParameters= rhs.m_iOwnRefPars
                         ? ( rhs.m_referenceParameters
                              ? rhs.m_referenceParameters->clone() : nullptr )
                         : rhs.m_referenceParameters ;
    m_iOwnRefPars   = rhs.m_iOwnRefPars;
    m_parametersDifference= rhs.m_iOwnParametersDifference
                            ? ( rhs.m_parametersDifference
                                ? new AmgVector(5)(*rhs.m_parametersDifference) : nullptr )
                            : rhs.m_parametersDifference;
    m_iOwnParametersDifference= rhs.m_iOwnParametersDifference;
    m_parametersCovariance= rhs.m_iOwnParametersCovariance
                            ? ( rhs.m_parametersCovariance
                                ? new AmgSymMatrix(5)(*rhs.m_parametersCovariance) : nullptr )
                            : rhs.m_parametersCovariance;
    m_iOwnParametersCovariance= rhs.m_iOwnParametersCovariance;
    m_measurementDifferenceVector.store(nullptr);
  }
  return *this;
}

void Trk::ProtoTrackStateOnSurface::swap(ProtoTrackStateOnSurface& rhs) throw () {
    if (this==&rhs){
        return;
    }
    std::swap(this->m_measurement, rhs.m_measurement);
    std::swap(this->m_iOwnMeasurement, rhs.m_iOwnMeasurement);
    std::swap(this->m_tsType, rhs.m_tsType);
    std::swap(this->m_iterationShowingOutlier, rhs.m_iterationShowingOutlier);
    std::swap(this->m_forwardState, rhs.m_forwardState);
    std::swap(this->m_fitQuality, rhs.m_fitQuality);
    std::swap(this->m_dnaMaterialEffects, rhs.m_dnaMaterialEffects);
    std::swap(this->m_protoMaterialEffects, rhs.m_protoMaterialEffects);
    std::swap(this->m_stateID, rhs.m_stateID);
    std::swap(this->m_identifier, rhs.m_identifier);
    std::swap(this->m_mType, rhs.m_mType);
    std::swap(this->m_calib, rhs.m_calib);
    std::swap(this->m_forwardStateChi2, rhs.m_forwardStateChi2);
    std::swap(this->m_forwardStateNdof, rhs.m_forwardStateNdof);
    std::swap(this->m_backwardStateChi2, rhs.m_backwardStateChi2);
    std::swap(this->m_transportJacobian, rhs.m_transportJacobian);
    std::swap(this->m_iOwnJacobian, rhs.m_iOwnJacobian);
    std::swap(this->m_referenceParameters, rhs.m_referenceParameters);
    std::swap(this->m_iOwnRefPars, rhs.m_iOwnRefPars);
    std::swap(this->m_parametersDifference, rhs.m_parametersDifference);
    std::swap(this->m_iOwnParametersDifference, rhs.m_iOwnParametersDifference);
    std::swap(this->m_parametersCovariance, rhs.m_parametersCovariance);
    std::swap(this->m_iOwnParametersCovariance, rhs.m_iOwnParametersCovariance);
    std::swap(this->m_measurementDifferenceVector, rhs.m_measurementDifferenceVector);
}

void Trk::ProtoTrackStateOnSurface::checkinMeasurement(const Trk::MeasurementBase* meas,
                                                       bool classShallOwnMbase) 
{
  if (m_measurement) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinMeasurement!" << std::endl;
    if (m_iOwnMeasurement) delete m_measurement;
  }
  m_measurement     = meas;
  m_iOwnMeasurement = classShallOwnMbase;
  m_mType           = Trk::TrackState::unidentified;
  m_measurementDifferenceVector.store(nullptr);
}

void Trk::ProtoTrackStateOnSurface::replaceMeasurement(const Trk::MeasurementBase* meas,
                                                       Trk::TrackState::CalibrationType ct) 
{
  if (meas!=nullptr && meas != m_measurement) { // chk that there *is* something to replace 
    if (m_iOwnMeasurement) delete m_measurement;
    m_measurement = meas;
    m_iOwnMeasurement = true;
    m_calib           = ct;
    m_measurementDifferenceVector.store(nullptr);
    // assume that replacements are done with the same detector type (recalibration)
  }
}

const Trk::MeasurementBase* Trk::ProtoTrackStateOnSurface::checkoutMeasurement() 
{
  if (!m_measurement) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutMeasurement!" << std::endl;
    return nullptr;
  }
  const Trk::MeasurementBase* helper = m_measurement;
  m_measurement = nullptr;
  m_measurementDifferenceVector.store(nullptr);
  if (!m_iOwnMeasurement) {
    return helper->clone();
  } else {
    //  not needed : m_iOwnMeasurement=false;
    return helper;
  }
}

void Trk::ProtoTrackStateOnSurface::checkinTransportJacobian ( const Trk::TransportJacobian* inJacobian,
      bool  classShallOwnJacobian ) 
{
  if ( m_transportJacobian ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinTransportJacobian!" << std::endl;
    if ( m_iOwnJacobian ) delete m_transportJacobian;
  }
  m_transportJacobian     = inJacobian;
  m_iOwnJacobian = classShallOwnJacobian;
}

const Trk::TransportJacobian* Trk::ProtoTrackStateOnSurface::checkoutTransportJacobian() 
{
  if ( !m_transportJacobian ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutTransportJacobian!" << std::endl;
    return nullptr;
  }
  const Trk::TransportJacobian* helper =  m_transportJacobian;
  m_transportJacobian = nullptr;
  if ( !m_iOwnJacobian ) return new Trk::TransportJacobian ( *m_transportJacobian );
  else {
    // not needed : m_iOwnJacobian=false;
    return helper;
  }
}

void Trk::ProtoTrackStateOnSurface::checkinReferenceParameters ( const Trk::TrackParameters* referenceParameters,
      bool  classShallOwnRefPars ) 
{
  if ( m_referenceParameters ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinReferenceParameters!" << std::endl;
    if ( m_iOwnRefPars ) delete m_referenceParameters;
  }
  m_referenceParameters = referenceParameters;
  m_iOwnRefPars         = classShallOwnRefPars;
  m_measurementDifferenceVector.store(nullptr);
}

const Trk::TrackParameters* Trk::ProtoTrackStateOnSurface::checkoutReferenceParameters() 
{
  if ( !m_referenceParameters ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutReferenceParameters!" << std::endl;
    return nullptr;
  }
  m_measurementDifferenceVector.store(nullptr);
  const Trk::TrackParameters* helper = m_referenceParameters;
  m_referenceParameters = nullptr;
  if ( !m_iOwnRefPars ) return helper->clone();
  else {
    // m_iOwnRefPars=false;
    return helper;
  }
}

void Trk::ProtoTrackStateOnSurface::checkinParametersDifference ( const AmgVector(5)* inParametersDifference,
        bool  classShallOwnParametersDifference )
{
  if ( m_parametersDifference ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinParametersDifference!" << std::endl;
    if ( m_iOwnParametersDifference ) delete m_parametersDifference;
  }
  m_parametersDifference     = inParametersDifference;
  m_iOwnParametersDifference = classShallOwnParametersDifference;
//   m_mType           = Trk::TrackState::unidentified;
}

const AmgVector(5)* Trk::ProtoTrackStateOnSurface::checkoutParametersDifference()
{
  if ( !m_parametersDifference ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutParametersDifference!" << std::endl;
    return nullptr;
  }
  const AmgVector(5)* helper = m_parametersDifference;
  m_parametersDifference = nullptr;
  if ( !m_iOwnParametersDifference ) {
    return new AmgVector(5) ( *helper );
  } else {
    // not needed : m_iOwnParametersDifference=false;
    return helper;
  }
}

void Trk::ProtoTrackStateOnSurface::checkinParametersCovariance ( const AmgSymMatrix(5)* inCovariance,
        bool  classShallOwnCovariance )
{
  if ( m_parametersCovariance ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinParametersCovariance!" << std::endl;
    if ( m_iOwnParametersCovariance ) delete m_parametersCovariance;
  }
  m_parametersCovariance     = inCovariance;
  m_iOwnParametersCovariance = classShallOwnCovariance;
}

const AmgSymMatrix(5)* Trk::ProtoTrackStateOnSurface::checkoutParametersCovariance()
{
  if ( !m_parametersCovariance ) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutParametersCovariance!" << std::endl;
    return nullptr;
  }
  const AmgSymMatrix(5)* helper = m_parametersCovariance;
  // m_iOwnParametersCovariance=false; // in principle not needed as the pointer is anyhow set to NULL
  m_parametersCovariance = nullptr;
  if ( !m_iOwnParametersCovariance ) {
    return new AmgSymMatrix(5) ( *helper );
  } else {
    // not needed :  m_iOwnParametersCovariance=false;
    return helper;
  }
}


void Trk::ProtoTrackStateOnSurface::checkinForwardPar(const Trk::TrackParameters* par) 
{
  if (m_forwardState) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinForwardPar!" << std::endl;
    delete m_forwardState;
  }
  m_forwardState = par;
}

const Trk::TrackParameters* Trk::ProtoTrackStateOnSurface::checkoutForwardPar() 
{
  if (!m_forwardState) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutForwardPar!" << std::endl;
    return nullptr;
  }
  const Trk::TrackParameters* helper = m_forwardState;
  m_forwardState = nullptr;
  return helper;
}

void Trk::ProtoTrackStateOnSurface::checkinSmoothedPar(const Trk::TrackParameters* par) 
{
  if (m_smoothedState) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinSmoothedPar!" << std::endl;
    delete m_smoothedState;
  }
  m_smoothedState = par;
}

const Trk::TrackParameters* Trk::ProtoTrackStateOnSurface::checkoutSmoothedPar() 
{
  if (!m_smoothedState) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutSmoothedPar!" << std::endl;
    return nullptr;
  }
  const Trk::TrackParameters* helper = m_smoothedState;
  m_smoothedState = nullptr;
  return helper;
}

void Trk::ProtoTrackStateOnSurface::checkinFitQuality(const Trk::FitQualityOnSurface* par) 
{
  if (m_fitQuality) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinFitQuality!" << std::endl;
    delete m_fitQuality;
  }
  m_fitQuality = par;
}

const Trk::FitQualityOnSurface* Trk::ProtoTrackStateOnSurface::checkoutFitQuality() 
{
  if (!m_fitQuality) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutFitQuality!" << std::endl;
    return nullptr;
  }
  const Trk::FitQualityOnSurface* helper = m_fitQuality;
  m_fitQuality = nullptr;
  return helper;
}

void Trk::ProtoTrackStateOnSurface::setForwardStateFitQuality(const FitQualityOnSurface& fq) 
{
  m_forwardStateChi2 = fq.chiSquared();
  m_forwardStateNdof = (double)fq.numberDoF();
}

void Trk::ProtoTrackStateOnSurface::setForwardStateFitQuality(double c2, double nd) 
{
  m_forwardStateChi2 = c2;
  m_forwardStateNdof = nd;
}

void Trk::ProtoTrackStateOnSurface::backwardStateChiSquared(double c2) 
{ m_backwardStateChi2 = c2; }


void Trk::ProtoTrackStateOnSurface::checkinDNA_MaterialEffects(const Trk::DNA_MaterialEffects* mef) 
{
  if (m_dnaMaterialEffects) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinDNA_MaterialEffects!" << std::endl;
    delete m_dnaMaterialEffects;
  }
  m_dnaMaterialEffects = mef;
}

const Trk::DNA_MaterialEffects* Trk::ProtoTrackStateOnSurface::checkoutDNA_MaterialEffects() 
{
  if (!m_dnaMaterialEffects) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutDNA_MaterialEffects!" << std::endl;
    return nullptr;
  }
  const Trk::DNA_MaterialEffects* helper = m_dnaMaterialEffects;
  m_dnaMaterialEffects = nullptr;
  return helper;
}


void Trk::ProtoTrackStateOnSurface::checkinMaterialEffects(const Trk::ProtoMaterialEffects* mef)
{
  if (m_protoMaterialEffects) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkinDNA_MaterialEffects!" << std::endl;
    delete m_protoMaterialEffects;
  }
  m_protoMaterialEffects = mef;
}

const Trk::ProtoMaterialEffects* Trk::ProtoTrackStateOnSurface::checkoutMaterialEffects()
{
  if (!m_protoMaterialEffects) {
    std::cout << "ProtoTrackStateOnSurface >>> WARNING possibly bad use of ::checkoutDNA_MaterialEffects!" << std::endl;
    return nullptr;
  }
  const Trk::ProtoMaterialEffects* helper = m_protoMaterialEffects;
  m_protoMaterialEffects = nullptr;
  return helper;
}

bool Trk::ProtoTrackStateOnSurface::isCluster() const
{
  if (!this->measurement()) return false;
  const Trk::TrackState::MeasurementType type = this->measurementType();
  if (type != Trk::TrackState::unidentified) {
    return (   type == Trk::TrackState::Pixel
            || type == Trk::TrackState::SCT
            || type == Trk::TrackState::RPC
            || type == Trk::TrackState::TGC
               || type == Trk::TrackState::CSC );
  } else {
    const Trk::Surface* sf = &(this->measurement()->associatedSurface());
    return ( (sf->associatedDetectorElement() != nullptr) 
             && (nullptr != dynamic_cast<const Trk::PlaneSurface*>(sf)));
  }
}

bool Trk::ProtoTrackStateOnSurface::isDriftCircle() const
{
  if (!this->measurement()) return false;
  const Trk::TrackState::MeasurementType type = this->measurementType();
  if (type != Trk::TrackState::unidentified) {
    return (   type == Trk::TrackState::TRT
            || type == Trk::TrackState::MDT );
  } else {
    const Trk::Surface* sf = &(this->measurement()->associatedSurface());
    return ( (sf->associatedDetectorElement() != nullptr) 
             && (nullptr != dynamic_cast<const Trk::StraightLineSurface*>(sf)));
  }
}

const Trk::TrackStateOnSurface* Trk::ProtoTrackStateOnSurface::createState(bool makeSlimOutlier)
{
  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
  if (m_measurement && isOutlier()) typePattern.set(TrackStateOnSurface::Outlier);
  else if (m_measurement && !isOutlier()) typePattern.set(TrackStateOnSurface::Measurement);
  else if (!m_measurement && m_protoMaterialEffects) typePattern.set(TrackStateOnSurface::Scatterer);
  else return nullptr;

  const Trk::MaterialEffectsBase* mefot = nullptr;
  if (m_dnaMaterialEffects) {
    mefot = m_dnaMaterialEffects->makeMEFOT();
    typePattern.set(TrackStateOnSurface::BremPoint);
  } else if (m_protoMaterialEffects)
    mefot = m_protoMaterialEffects->makeMEOT();
  if (isOutlier() && makeSlimOutlier) {
    return new TrackStateOnSurface(m_measurement ? checkoutMeasurement() : nullptr,
                                   nullptr,nullptr,mefot,typePattern);
  } else {
    return new TrackStateOnSurface(m_measurement ? checkoutMeasurement() : nullptr,
                                   checkoutSmoothedPar(),
// for testing                     checkoutForwardPar(),
                                   m_fitQuality ? checkoutFitQuality() : nullptr,
                                   mefot,typePattern);
  }
}


const Trk::Surface *Trk::ProtoTrackStateOnSurface::surface() const {
    if (m_measurement) {
        return &(m_measurement->associatedSurface());
    }
    if (m_protoMaterialEffects && m_protoMaterialEffects->surface()) {
        return m_protoMaterialEffects->surface();
    }
    if (m_referenceParameters) {
        return &(m_referenceParameters->associatedSurface());
    }
    return nullptr;
}

const Amg::VectorX* Trk::ProtoTrackStateOnSurface::measurementDifference() const {
    if (!m_measurement || !m_referenceParameters) {
        return nullptr;
    }
    if (m_measurementDifferenceVector) {
        return m_measurementDifferenceVector.get();
    }

    const LocalParameters& locPar = m_measurement->localParameters();
    std::unique_ptr<Amg::VectorX> tmp_measurementDifferenceVector=std::make_unique<Amg::VectorX>();
    (*tmp_measurementDifferenceVector)=locPar - locPar.expansionMatrix()*m_referenceParameters->parameters();
    m_measurementDifferenceVector.set(std::move(tmp_measurementDifferenceVector));
    return m_measurementDifferenceVector.get();
}

/// general swap function for ProtoTrackStateOnSurface (to speed up sort, etc)
void Trk::swap(ProtoTrackStateOnSurface& lhs, ProtoTrackStateOnSurface& rhs) {
    lhs.swap(rhs);
}




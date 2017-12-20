/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Mdt Calibration Input Event Data Model
 * --------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 18 February 2004
 ***************************************************************************/

#ifndef MUONCALIBEVENT_MDTCALIBHIT_H
#define MUONCALIBEVENT_MDTCALIBHIT_H
// std
#include <cassert>
#include <cmath>
#include <iostream>
// other packages

// this package
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonPrepRawData/MdtPrepData.h"

// pre-declarations
namespace MuonGM {
  class MdtReadoutElement;
}

namespace MuonCalib {
  class MdtCalibHitBase;
  class IIdToFixedIdTool;
}

class MsgStream;

/**
   @class MdtCalibHit
   An MdtCalibHit is an object containing all the intermediate information needed 
   in the conversion of raw Mdt data to a drift radius with its error.
   It is used by the MdtCalibrationSvc.  
   @author Martin Woudstra
   @date June 2004

   A MdtCalibHit is the underlying class which holds the calibration information. 
   @author Zdenko van Kesteren
   @date June 2007
*/

class MdtCalibHit {
public:
   
  static const float kNoValue;  //!< constant to set values to invalid 
  MdtCalibHit(); //!< default constructor
  
  MdtCalibHit( const Muon::MdtPrepData *dig, const Amg::Vector3D &globalPos, const MuonGM::MdtReadoutElement *geo ); //!< constructor initializing with a MdtPrepData, global posiition and a MdtReadoutElement
  MdtCalibHit( const Muon::MdtPrepData *dig, const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos, 
	       const MuonGM::MdtReadoutElement *geo );//!< constructor initializing with a MdtPrepData, both local (station) and global position and a MdtReadoutElement

  MdtCalibHit( const Identifier &id, int tdc, int adc, const Amg::Vector3D &globalPos, const MuonGM::MdtReadoutElement *geo ); //!< constructor initializing its Identifier, tdc/adc counts, global position and a mdtReadoutElement
  MdtCalibHit( const Identifier &id, int tdc, int adc, const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos, 
	       const MuonGM::MdtReadoutElement* geo ); //!< constructor initializing its Identifier, tdc/adc counts, both local (station) and global position and a mdtReadoutElement

  void setPrepData( const Muon::MdtPrepData *dig );         //!< sets mdtPrepData
  void setIdentifier( const Identifier &id );               //!< sets Identifier
  void setTdc( int tdc );                                   //!< sets TDC counts
  void setAdc( int adc );                                   //!< sets the ADC counts
  void setGlobalPos( const Amg::Vector3D &globalPos );      //!< sets the position in ATLAS coordinates
  void setLocalPos( const Amg::Vector3D &localPos );        //!< sets the position in the station coordinates
  void setGlobalPointOfClosestApproach( const Amg::Vector3D &point );//!< sets point of closest approach in global coordinates
  void setLocalPointOfClosestApproach( const Amg::Vector3D &point ); //!< sets point of closest approach in local coordinates
  void setGeometry( const MuonGM::MdtReadoutElement *geo ); //!< sets the MdtReadoutElement
  void setDriftTime( double t );                            //!< sets drift time
  void setDriftRadius( double r, double sigmaR );           //!< sets drift radius and drift radius error
  void setDistanceToTrack( double dist, double sigmaDist ); //!< sets the distance to the fitted track and its error
  void setTimeFromTrackDistance( double t, double sigmaT ); //!< sets the time of the distance to the fitted track (and its error)
  void setSlewingTime( double slew );                       //!< sets the time correction due to slewing
  void setLorentzTime( double lor );                        //!< sets the time correction due to the magnetic field
  void setPropagationTime( double prop );                   //!< sets the time correction due to the propagation of the signal over the wire
  void setTimeOfFlight( double tof );                       //!< sets the time correction due to the muon time of flight
  void setDistanceToReadout( double dist );                 //!< sets the distance to read out
  void setWiresagTime( float wsag );                        //!< sets the time correction due to wire sag
  inline void setTemperatureTime( float temp ) {            //!< sets the time correction due to temperature effects
    m_temperatureTime = temp;
  }
  void setBackgroundTime( float bkgr );                     //!< sets the time correction due to background signal
  void setBFieldPerp( float bfperp );                       //!< sets the B field perpendicular to the tube
  void setBFieldPara( float bfpara );                       //!< sets the B field parallel to the tube
  void setTemperature( float temp );                        //!< sets the temperature
  void setProjSag( float sag );                             //!< sets the projected wire sag 
  void setTubeT0( float t0 );                              //!< sets the tube t0
  void setTubeAdcCal( float adccal );                      //!< sets the tube mean ADC
  void setLocXtwin( float xtwin );                         //!< sets twin local position
  void setSigma2LocXtwin( float xtwin );                   //!< sets twin local position variance

  const MuonGM::MdtReadoutElement* geometry() const;        //!< retrieve the MdtReadoutElement	
  const Identifier& identify() const;                       //!< retrieve Identifier
  const Amg::Vector3D& globalPosition() const;              //!< retrieve the position in ATLAS coordinates
  const Amg::Vector3D& localPosition() const;               //!< retrieve the position in the station coordinates
  const Amg::Vector3D& globalPointOfClosestApproach() const;//!< retrieve the point of closest approach in global coordinates
  const Amg::Vector3D& localPointOfClosestApproach() const; //!< retrieve the point of closest approach in local coordinates
  int tdcCount() const;                              //!< retrieve TDC counts
  int adcCount() const;                              //!< retrieve the ADC counts
  double driftTime() const;                          //!< retrieve drift time
  double driftRadius() const;                        //!< retrieve drift radius
  double sigmaDriftRadius() const;                   //!< retrieve the error on the radius of the drift circle
  double sigma2DriftRadius() const;                  //!< retrieve the error squared on the radius of the drift circle
  double signedDistanceToTrack() const;              //!< retrieve the distance of the track to the wire
  double sigmaDistanceToTrack() const;               //!< retrieve the error on the distance of the track to the wire
  double sigma2DistanceToTrack() const;              //!< retrieve the error squared on the distance of the track to the wire
  double distanceToReadout() const;                  //!< retrieve the distance to readout
  double slewingTime() const;                        //!< retrieve the timing correction due to slewing
  double lorentzTime() const;                        //!< retrieve the timing correction due to the magnetic field (lorentz angle)
  double propagationTime() const;                    //!< retrieve the timing correction due to the propagation of the signal
  double timeOfFlight() const;                       //!< retrieve the timing correction due to time of flight of the muon
  double timeFromTrackDistance() const;              //!< retrieve the time corresponding to the distance to track
  double sigmaTimeFromTrackDistance() const;         //!< retrieve the error of the time corresponding to the distance to track
  double sigma2TimeFromTrackDistance() const;        //!< retrieve the error squared of the time corresponding to distance to track
  double radialResidual() const;                     //!< retrieve difference between driftcircle and distance to track (unsigned)
  double trackResidual() const;                      //!< retrieve difference between driftcircle and distance to track (signed)
  double timeResidual() const;                       //!< retrieve difference between drifttime and timeFromTrackDistance
  float WiresagTime() const;                         //!< retrieve the timing correction due to wiresag
  inline float TemperatureTime() const {             //!< retrieve the timing correction due to temperature
    return m_temperatureTime;
  }
  float BackgroundTime() const;
  float bFieldPerp() const;                          //!< retrieve the Bfield perpendicular to the tube
  float bFieldPara() const;                          //!< retrieve the Bfield parallel to the tube
  float temperature() const;                         //!< retrieve the temperature
  float projSag() const;                             //!< retrieve the projected sag
  float tubeT0() const;                              //!< retrieve the tube T0
  float tubeAdcCal() const;                          //!< retrieve the tube mean ADC
  float xtwin() const;                               //!< retrieve twin position 
  float sigmaXtwin() const;                          //!< retrieve twin position error
  float sigma2Xtwin() const;                         //!< retrieve twin position variance
    
  MuonCalib::MdtCalibHitBase* hitBase(const MuonCalib::IIdToFixedIdTool&) const;  //!< return a pointer to the MdtCalibHit object to be used in the calibration framework 
  
  std::ostream& dump(std::ostream &stream) const;    //!< ostream operator
  MsgStream& dump(MsgStream &sl) const;              //!< MsgStream operator
  
 private:
  Identifier m_id;                             //!< (ATLAS) Identifier
  int        m_tdc;                            //!< TDC counts
  int        m_adc;                            //!< ADC counts 
  const MuonGM::MdtReadoutElement *m_geometry; //!< MdtReadoutElement
  Amg::Vector3D m_globalPosition;              //!< position of the hit expressed in ATLAS global coordinates
  Amg::Vector3D m_localPosition;               //!< position of the hit expressed in local (station) coordinates
  Amg::Vector3D m_localPointOfClosestApproach; //!< point of closest approach of track to wire in local coordinates
  Amg::Vector3D m_globalPointOfClosestApproach;//!< point of closest approach of track to wire in global coordinates
  float      m_driftTime;                      //!< drift time after all corrections
  float      m_driftRadius;                    //!< drift radius 
  float      m_sigma2DriftRadius;              //!< drift radius variance 
  float      m_timeFromTrackDistance;          //!< for calibration or trackfit in time-domain
  float      m_sigma2TimeFromTrackDistance;    //!< (square of) sigma on m_timeFromTrackDistance
  float      m_slewingTime;                    //!< extra time due to slewing (electronics)
  float      m_lorentzTime;                    //!< extra time due to magnetic field
  float      m_propagationTime;                //!< along the wire
  float      m_timeOfFlight;                   //!< time of flight of the muon (CLHEP::ns)
  float      m_distanceToReadout;              //!< from hit to read-out side of tube
  float      m_signedDistanceToTrack;          //!< signed distance from track to wire
  float      m_sigma2DistanceToTrack;          //!< square of sigma on m_signedDistanceToTrack
  float      m_wiresagTime;                    //!< extra time due to wiresag
  float      m_temperatureTime;                //!< extra time due to temperature
  float      m_backgroundTime;                 //!< extra time due to background
  float      m_bFieldPerp;                     //!< local bfield perpendicular to the wire
  float      m_bFieldPara;                     //!< local bfield parallel to the wire
  float      m_temperature;                    //!< local temperature
  float      m_projSag;                        //!< projective sag
  float      m_tube_t0;                        //!< the t0 of the tube	
  float      m_tube_adccal;                    //!< mean ADC of the tube (calibration)
  float      m_xtwin;                          //!< the twin position 	
  float      m_sigma2XTwin;                    //!< the twin position variance	
};

inline MdtCalibHit::MdtCalibHit()
   : m_id(0), m_tdc(0),m_adc(0), m_geometry(0), m_globalPosition(kNoValue,kNoValue,kNoValue), 
     m_localPosition(kNoValue,kNoValue,kNoValue),
     m_driftTime(kNoValue), m_driftRadius(kNoValue), m_sigma2DriftRadius(1.0),
     m_timeFromTrackDistance(kNoValue),m_sigma2TimeFromTrackDistance(kNoValue),
     m_slewingTime(0.0),m_lorentzTime(0.0),m_propagationTime(0.0),
     m_timeOfFlight(0),m_distanceToReadout(kNoValue),
     m_signedDistanceToTrack(kNoValue),m_sigma2DistanceToTrack(kNoValue), m_wiresagTime(0), 
     m_temperatureTime(0), m_backgroundTime(0),  m_bFieldPerp(kNoValue), m_bFieldPara(kNoValue), m_temperature(kNoValue), 
     m_projSag(0), m_tube_t0(0), m_tube_adccal(0), m_xtwin(kNoValue), m_sigma2XTwin(kNoValue)
{}

inline MdtCalibHit::MdtCalibHit( const Muon::MdtPrepData *dig, const Amg::Vector3D &globalPos,
				 const MuonGM::MdtReadoutElement *geo )
   : m_id(dig->identify()), m_tdc(dig->tdc()),m_adc(dig->adc()), 
     m_geometry(geo), m_globalPosition(globalPos),
     m_localPosition(kNoValue,kNoValue,kNoValue),
     m_driftTime(kNoValue), m_driftRadius(kNoValue), m_sigma2DriftRadius(1.0),
     m_timeFromTrackDistance(kNoValue),m_sigma2TimeFromTrackDistance(kNoValue),
     m_slewingTime(0.0),m_lorentzTime(0.0),m_propagationTime(0.0),
     m_timeOfFlight(0),m_distanceToReadout(kNoValue), 
     m_signedDistanceToTrack(kNoValue),m_sigma2DistanceToTrack(kNoValue),m_wiresagTime(0), 
     m_temperatureTime(0), m_backgroundTime(0),  m_bFieldPerp(kNoValue), m_bFieldPara(kNoValue), m_temperature(kNoValue), 
     m_projSag(0), m_tube_t0(0), m_tube_adccal(0), m_xtwin(kNoValue), m_sigma2XTwin(kNoValue)
{}

inline MdtCalibHit::MdtCalibHit( const Muon::MdtPrepData *dig, const Amg::Vector3D &globalPos, 
				 const Amg::Vector3D &localPos, const MuonGM::MdtReadoutElement *geo )
   : m_id(dig->identify()), m_tdc(dig->tdc()),m_adc(dig->adc()), 
     m_geometry(geo), m_globalPosition(globalPos), m_localPosition(localPos),
     m_driftTime(kNoValue), m_driftRadius(kNoValue), m_sigma2DriftRadius(1.0),
     m_timeFromTrackDistance(kNoValue),m_sigma2TimeFromTrackDistance(kNoValue),
     m_slewingTime(0.0),m_lorentzTime(0.0),m_propagationTime(0.0),
     m_timeOfFlight(0),m_distanceToReadout(kNoValue),
     m_signedDistanceToTrack(kNoValue),m_sigma2DistanceToTrack(kNoValue),m_wiresagTime(0), 
     m_temperatureTime(0), m_backgroundTime(0), 
     m_bFieldPerp(kNoValue), m_bFieldPara(kNoValue), m_temperature(kNoValue), 
     m_projSag(0), m_tube_t0(0), m_tube_adccal(0), m_xtwin(kNoValue), m_sigma2XTwin(kNoValue)
{}

inline MdtCalibHit::MdtCalibHit( const Identifier &id, int tdc, int adc, const Amg::Vector3D &globalPos,
				 const MuonGM::MdtReadoutElement *geo )
   : m_id(id), m_tdc(tdc), m_adc(adc), 
     m_geometry(geo), m_globalPosition(globalPos), m_localPosition(kNoValue,kNoValue,kNoValue),
     m_driftTime(kNoValue), m_driftRadius(kNoValue), m_sigma2DriftRadius(1.0),
     m_timeFromTrackDistance(kNoValue),m_sigma2TimeFromTrackDistance(kNoValue),
     m_slewingTime(0.0),m_lorentzTime(0.0),m_propagationTime(0.0),
     m_timeOfFlight(0),m_distanceToReadout(kNoValue),
     m_signedDistanceToTrack(kNoValue),m_sigma2DistanceToTrack(kNoValue),m_wiresagTime(0), 
     m_temperatureTime(0), m_backgroundTime(0),  
     m_bFieldPerp(kNoValue), m_bFieldPara(kNoValue), m_temperature(kNoValue), m_projSag(0), 
     m_tube_t0(0), m_tube_adccal(0), m_xtwin(kNoValue), m_sigma2XTwin(kNoValue)
{}

inline MdtCalibHit::MdtCalibHit( const Identifier &id, int tdc, int adc, const Amg::Vector3D &globalPos, 
				 const Amg::Vector3D &localPos, const MuonGM::MdtReadoutElement *geo )
   : m_id(id), m_tdc(tdc), m_adc(adc), 
     m_geometry(geo), m_globalPosition(globalPos), m_localPosition(localPos),
     m_driftTime(kNoValue), m_driftRadius(kNoValue), m_sigma2DriftRadius(1.0),
     m_timeFromTrackDistance(kNoValue),m_sigma2TimeFromTrackDistance(kNoValue),
     m_slewingTime(0.0),m_lorentzTime(0.0),m_propagationTime(0.0),
     m_timeOfFlight(0),m_distanceToReadout(kNoValue),
     m_signedDistanceToTrack(kNoValue),m_sigma2DistanceToTrack(kNoValue), m_wiresagTime(0), 
     m_temperatureTime(0), m_backgroundTime(0), 
     m_bFieldPerp(kNoValue), m_bFieldPara(kNoValue), m_temperature(kNoValue), m_projSag(0), 
     m_tube_t0(0), m_tube_adccal(0), m_xtwin(kNoValue), m_sigma2XTwin(kNoValue)
{}

inline void MdtCalibHit::setPrepData( const Muon::MdtPrepData *dig ) {
  m_id  = dig->identify();
  m_tdc = dig->tdc(); 
  m_adc = dig->adc();
}

inline void MdtCalibHit::setIdentifier( const Identifier &id ) {
  m_id = id;
}

inline void MdtCalibHit::setTdc( int tdc ) {
  m_tdc = tdc; 
}

inline void MdtCalibHit::setAdc( int adc ) {
  m_adc = adc;
}

inline void MdtCalibHit::setGlobalPos( const Amg::Vector3D &globalPos ) {
  m_globalPosition = globalPos;
}

inline void MdtCalibHit::setLocalPos( const Amg::Vector3D &localPos ) {
  m_localPosition = localPos;
}

inline void MdtCalibHit::setGlobalPointOfClosestApproach( const Amg::Vector3D &point ) {
  m_globalPointOfClosestApproach=point;
}
  
inline void MdtCalibHit::setLocalPointOfClosestApproach( const Amg::Vector3D &point ) {
  m_localPointOfClosestApproach=point;
}

inline void MdtCalibHit::setGeometry( const MuonGM::MdtReadoutElement *geo ) {
  m_geometry = geo;
}

inline void MdtCalibHit::setDriftTime( double t ) {
   m_driftTime = t;
}

inline void MdtCalibHit::setDriftRadius( double r, double sigmaR ) {
   m_driftRadius = r;
   m_sigma2DriftRadius = sigmaR*sigmaR;
}

inline void MdtCalibHit::setDistanceToTrack( double dist, double sigmaDist ) {
   m_signedDistanceToTrack = dist;
   m_sigma2DistanceToTrack = sigmaDist*sigmaDist;
}

inline void MdtCalibHit::setTimeFromTrackDistance( double t, double sigmaT ) {
  m_timeFromTrackDistance = t;
  m_sigma2TimeFromTrackDistance = sigmaT*sigmaT; 
}

inline void MdtCalibHit::setSlewingTime( double slew ) {
  m_slewingTime = slew;
}

inline void MdtCalibHit::setLorentzTime( double lor ) {
  m_lorentzTime = lor;
}

inline void MdtCalibHit::setPropagationTime( double prop ) {
  m_propagationTime = prop;
}

inline void MdtCalibHit::setTimeOfFlight( double tof ) {
  m_timeOfFlight = tof;
}

inline void MdtCalibHit::setDistanceToReadout( double dist ) {
   m_distanceToReadout = dist;
}

inline void MdtCalibHit::setWiresagTime( float wsag ) {
  m_wiresagTime = wsag;
}
   
inline void MdtCalibHit::setBFieldPerp( float val ) {
  m_bFieldPerp = val;
}

inline void MdtCalibHit::setBFieldPara( float val ) {
  m_bFieldPara = val;
}

inline void MdtCalibHit::setTemperature( float val ) {
  m_temperature = val;
}

inline void MdtCalibHit::setProjSag( float val ) {
  m_projSag = val;
}

inline void MdtCalibHit::setTubeT0( float t0 ) {
  m_tube_t0 = t0;
}

inline void MdtCalibHit::setTubeAdcCal( float adccal ) {
  m_tube_adccal = adccal;
}

inline void MdtCalibHit::setLocXtwin( float xtwin ) {
  m_xtwin = xtwin;
}

inline void MdtCalibHit::setSigma2LocXtwin( float sigma2XTwin ) {
  m_sigma2XTwin = sigma2XTwin;
}

inline const MuonGM::MdtReadoutElement* MdtCalibHit::geometry() const {
  return m_geometry;
}

inline const Identifier& MdtCalibHit::identify() const {
  return m_id;
}

inline const Amg::Vector3D& MdtCalibHit::globalPosition() const {
  return m_globalPosition;
}

inline const Amg::Vector3D& MdtCalibHit::localPosition() const {
  return m_localPosition;
}

inline const Amg::Vector3D& MdtCalibHit::globalPointOfClosestApproach() const {
  return m_globalPointOfClosestApproach;
}

inline const Amg::Vector3D& MdtCalibHit::localPointOfClosestApproach() const {
  return m_localPointOfClosestApproach;
}

inline int MdtCalibHit::tdcCount() const {
  return m_tdc;
}

inline int MdtCalibHit::adcCount() const {
  return m_adc;
}

inline double MdtCalibHit::driftTime() const {
  return m_driftTime;
}

inline double MdtCalibHit::driftRadius() const {
   return m_driftRadius;
}

inline double MdtCalibHit::sigmaDriftRadius() const {
  return std::sqrt( m_sigma2DriftRadius );
}

inline double MdtCalibHit::sigma2DriftRadius() const {
  return m_sigma2DriftRadius;
}

inline double MdtCalibHit::signedDistanceToTrack() const {
  return m_signedDistanceToTrack;
}

inline double MdtCalibHit::sigmaDistanceToTrack() const {
  return std::sqrt( m_sigma2DistanceToTrack );
}

inline double MdtCalibHit::sigma2DistanceToTrack() const {
  return m_sigma2DistanceToTrack;
}

inline double MdtCalibHit::distanceToReadout() const {
  return m_distanceToReadout;
}

inline double MdtCalibHit::slewingTime() const {
  return m_slewingTime; 
}

inline double MdtCalibHit::lorentzTime() const {
  return m_lorentzTime;
}

inline double MdtCalibHit::propagationTime() const {
  return m_propagationTime;
}

inline double MdtCalibHit::timeOfFlight() const {
  return m_timeOfFlight;
}

inline double MdtCalibHit::timeFromTrackDistance() const {
  return m_timeFromTrackDistance;
}

inline double MdtCalibHit::sigmaTimeFromTrackDistance() const {
  return std::sqrt(m_sigma2TimeFromTrackDistance);
}

inline double MdtCalibHit::sigma2TimeFromTrackDistance() const {
  return m_sigma2TimeFromTrackDistance;
}

inline double MdtCalibHit::radialResidual() const {
  return std::abs( m_driftRadius ) - std::abs( m_signedDistanceToTrack );
}

inline double MdtCalibHit::trackResidual() const {
  return m_signedDistanceToTrack - m_driftRadius;
}

inline double MdtCalibHit::timeResidual() const {
   return m_driftTime - m_timeFromTrackDistance;
}

inline float MdtCalibHit::tubeT0() const {
  return m_tube_t0;
}

inline float MdtCalibHit::tubeAdcCal() const {
  return m_tube_adccal;
}

inline float MdtCalibHit::xtwin() const {
  return m_xtwin;
}

inline float MdtCalibHit::sigmaXtwin() const {
  return std::sqrt( m_sigma2XTwin );
}

inline float MdtCalibHit::sigma2Xtwin() const {
  return m_sigma2XTwin;
}

inline float MdtCalibHit::bFieldPerp() const {
  return m_bFieldPerp;
}

inline float MdtCalibHit::bFieldPara() const{
  return m_bFieldPara;
}

inline float MdtCalibHit::temperature() const{
  return m_temperature;
}

inline float MdtCalibHit::projSag() const{
  return m_projSag;
}

std::ostream& operator << (std::ostream &stream, const MdtCalibHit &hit);

MsgStream& operator << ( MsgStream &sl, const MdtCalibHit &hit);

#endif // MUONCALIBEVENT_MDTCALIBHIT_H

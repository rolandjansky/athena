/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Mdt Calibration Input Event Data Model
 * --------------------------------------
 *
 * Author       : Niels van Eldik, Zdenko van Kesteren
 * Creation Date: 8 April 2005
 ***************************************************************************/

#ifndef MUONCALIB_MDTCALIBHITBASE_H
#define MUONCALIB_MDTCALIBHITBASE_H
 
// std
#include <cassert>
#include <cmath>
#include <iostream>

// other packages
#include "GeoPrimitives/GeoPrimitives.h"

// this package
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

  /**
     @class MdtCalibHitBase
     Athena-independent part of the MdtCalibHit. As a basic building
     block, this class contains all the information needed to perform
     the calibration.
     @author Niels.Van.Eldik@cern.ch, Zdenko van Kesteren (zkestere@nikhef.nl)

  */

  class MdtCalibHitBase {

  public:

    static const double kNoValue; //!< constant to set values to invalid 
 
    MdtCalibHitBase(); //!< default constructor
    MdtCalibHitBase( int tdc, int adc, const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos );  //!< constructor setting the position (local and global) as well as the tdc and adc counts

    ~MdtCalibHitBase() {}; //!< destructor
   
    // setters 
    void setIdentifier( const MuonFixedId &id );           //!< sets the MuonFixedIdentifier of the hit
    void setTdc( unsigned short tdc );                     //!< sets the tdc counts
    void setAdc( unsigned short adc );                     //!< sets the adc counts
    void setGlobalPos( const Amg::Vector3D &globalPos );   //!< sets the position in ATLAS coordinates
    void setLocalPos( const Amg::Vector3D &localPos );     //!< sets the position in the station coordinates
    void setGlobalPointOfClosestApproach( const Amg::Vector3D &point );//!< sets point of closest approach in global coordinates
    void setLocalPointOfClosestApproach( const Amg::Vector3D &point ); //!< sets point of closest approach in local coordinates
    void setDriftTime( float t );                          //!< sets drift time
    void setDriftRadius( float r, float sigmaR );          //!< sets drift radius and drift radius error
    void setTubeRadius( float r );                         //!< sets tube inner radius 
    void setDistanceToTrack( float dist, float sigmaDist );//!< sets the distance to the fitted track and its error
    void setTimeFromTrackDistance( float t, float sigmaT );//!< sets the time of the distance to the fitted track (and its error)
    void setSlewingTime( float slew );                     //!< sets the time correction due to slewing
    void setBFieldTime( float lor );                       //!< sets the time correction due to the magnetic field
    void setPropagationTime( float prop );                 //!< sets the time correction due to the propagation of the signal over the wire
    void setTimeOfFlight( float tof );                     //!< sets the time correction due to the muon time of flight
    void setDistanceToReadout( float dist );               //!< sets the distance to read out
    void setWiresagTime( float wsag );                     //!< sets the time correction due to wire sag
    void setTemperatureTime( float temp );                 //!< sets the time correction due to temperature effects
    void setBackgroundTime( float bkgr );                  //!< sets the time correction due to background signal
    void setBFieldPerp( float bfperp );                    //!< sets the B field perpendicular to the tube
    void setBFieldPara( float bfpara );                    //!< sets the B field parallel to the tube
    void setTemperature( float temp );                     //!< sets the temperature
    void setProjSag( float sag );                          //!< sets the projected wire sag 
    void setTubeT0( float t0 );                            //!< sets the tube t0
    void setTubeAdcCal( float adccal );                    //!< sets the tube mean ADC
    void setLocXtwin( float xtwin );                       //!< sets the local twin position 
    void setSigmaLocXtwin( float xtwin );                  //!< sets the local twin position error 
    void setSegmentT0Applied(bool flag);                   //!< sets flag if Segment T0 has been applied to hit 
 
    /* getters */
    const MuonFixedId& identify() const;                   //!< retrieve the MuonFixedId of the hit
    const Amg::Vector3D& globalPosition() const;           //!< retrieve the position given in global coordinates
    const Amg::Vector3D& localPosition() const;            //!< retrieve the position expressed in local (station) coordinates
    const Amg::Vector3D& globalPointOfClosestApproach() const;//!< retrieve the point of closest approach in global coordinates
    const Amg::Vector3D& localPointOfClosestApproach() const; //!< retrieve the point of closest approach in local coordinates
    unsigned short tdcCount() const;                       //!< retrieve the tdc counts
    unsigned short adcCount() const;                       //!< retrieve the adc counts
    float driftTime() const;                               //!< retrieve the drift time
    float driftRadius() const;                             //!< retrieve the radius of the drift circle
    float sigmaDriftRadius() const;                        //!< retrieve the error on the radius of the drift circle
    float sigma2DriftRadius() const;                       //!< retrieve the error squared on the radius of the drift circle
    float signedDistanceToTrack() const;                   //!< retrieve the distance of the track to the wire
    float sigmaDistanceToTrack() const;                    //!< retrieve the error on the distance of the track to the wire
    float sigma2DistanceToTrack() const;                   //!< retrieve the error squared on the distance of the track to the wire
    float tubeRadius() const;                              //!< retrieve the inner radius of the drift tube
    float distanceToReadout() const;                       //!< retrieve the distance to readout
    float slewingTime() const;                             //!< retrieve the timing correction due to slewing
    float bFieldTime() const;                              //!< retrieve the timing correction due to the magnetic field (lorentz angle)
    float propagationTime() const;                         //!< retrieve the timing correction due to the propagation of the signal
    float timeOfFlight() const;                            //!< retrieve the timing correction due to time of flight of the muon
    float timeFromTrackDistance() const;                   //!< retrieve the time corresponding to the distance to track
    float sigmaTimeFromTrackDistance() const;              //!< retrieve the error of the time corresponding to the distance to track
    float sigma2TimeFromTrackDistance() const;             //!< retrieve the error squared of the time corresponding to distance to track
    float radialResidual() const;                          //!< retrieve difference between driftcircle and distance to track (unsigned)
    float trackResidual() const;                           //!< retrieve difference between driftcircle and distance to track (signed)
    float timeResidual() const;                            //!< retrieve difference between drifttime and timeFromTrackDistance
    float WiresagTime() const;                             //!< retrieve the timing correction due to wiresag
    float TemperatureTime() const;                         //!< retrieve the timing correction due to temperature
    float BackgroundTime() const;
    float bFieldPerp() const;                              //!< retrieve the Bfield perpendicular to the tube
    float bFieldPara() const;                              //!< retrieve the Bfield parallel to the tube
    float temperature() const;                             //!< retrieve the temperature
    float projSag() const;                                 //!< retrieve the projected sag
    float tubeT0() const;                                  //!< retrieve the tube T0
    float tubeAdcCal() const;                              //!< retrieve the tube mean ADC
    float xtwin() const;                                   //!< retrieve the local twin position 
    float sigmaXtwin() const;                              //!< retrieve the local twin position error
    bool  segmentT0Applied() const;                        //!< retrieve the segment T0 applied flag
   
    /* ostream operators */
    std::ostream& dump(std::ostream &stream) const;        //!< dump to be used for operator<<() to dump the MdtCalibHitBase
  
  private:
    /* Fixed, Athena-independent Identifier*/
    MuonFixedId m_id;                             //!< Identifier of the hit (not an ATLAS ID, rather a MuonFixedId)
    int m_tdc;                                    //!< TDC count
    int m_adc;                                    //!< ADC count
    Amg::Vector3D m_globalPosition;               //!< position of the hit in global coordinates
    Amg::Vector3D m_localPosition;                //!< position of the hit in local (station) coordinates
    Amg::Vector3D m_localPointOfClosestApproach;  //!< point of closest approach of track to wire in local coordinates
    Amg::Vector3D m_globalPointOfClosestApproach; //!< point of closest approach of track to wire in global coordinates
    float      m_driftTime;                       //!< drift time after all corrections
    float      m_driftRadius;                     //!< drift radius
    float      m_sigma2DriftRadius;               //!< error squared on the drift radius
    float      m_timeFromTrackDistance;           //!< time calculated from 'distance to track', for calibration or trackfit in time-domain
    float      m_sigma2TimeFromTrackDistance;     //!< (square of) sigma on m_timeFromTrackDistance
    float      m_slewingTime;                     //!< time correction due to slewing (electronics)
    float      m_bFieldTime;                      //!< time correction due to magnetic field
    float      m_propagationTime;                 //!< time correction due to signal propagation along the wire
    float      m_timeOfFlight;                    //!< time correction due to the time of flight in ns
    float      m_distanceToReadout;               //!< from hit to read-out side of tube
    float      m_signedDistanceToTrack;           //!< signed distance from track to wire
    float      m_sigma2DistanceToTrack;           //!< square of sigma on m_signedDistanceToTrack
    float      m_wiresagTime;                     //!< extra time due to wiresag
    float      m_temperatureTime;                 //!< extra time due to temperature
    float      m_backgroundTime;                  //!< extra time due to background
    float      m_bFieldPerp;                      //!< local bfield perpendicular to the wire
    float      m_bFieldPara;                      //!< local bfield parallel to the wire
    float      m_temperature;                     //!< local temperature
    float      m_projSag;                         //!< projective sag
    float      m_tube_t0;                         //!< tube t0 time offset
    float      m_tube_adccal;                     //!< tube mean ADC (calibration)
    float      m_xtwin;                           //!< twin position 
    float      m_sigmaXTwin;                      //!< twin position error
    bool       m_segmentT0Applied;                //!< true if segment t0 is substr. from drift time
    float m_tubeRadius;                           //!< inner tube radius
  };

  inline void MdtCalibHitBase::setIdentifier( const MuonFixedId &id ) {
    m_id = id;
  }

  inline void MdtCalibHitBase::setTdc( unsigned short tdc ) {
    m_tdc = tdc; 
  }

  inline void MdtCalibHitBase::setAdc( unsigned short adc ) {
    m_adc = adc;
  }

  inline void MdtCalibHitBase::setGlobalPos( const Amg::Vector3D &globalPos ) {
    m_globalPosition = globalPos;
  }

  inline void MdtCalibHitBase::setLocalPos( const Amg::Vector3D &localPos ) {
    m_localPosition = localPos;
  }

  inline void MdtCalibHitBase::setGlobalPointOfClosestApproach( const Amg::Vector3D &point ) {
   m_globalPointOfClosestApproach=point;
  }

  inline void MdtCalibHitBase::setLocalPointOfClosestApproach( const Amg::Vector3D &point ) {
   m_localPointOfClosestApproach=point;
  }

  inline void MdtCalibHitBase::setDriftTime( float t ) {
    m_driftTime = t;
  }

  inline void MdtCalibHitBase::setDriftRadius( float r, float sigmaR ) {
    m_driftRadius = r;
    m_sigma2DriftRadius = sigmaR*sigmaR;
  }

  inline void MdtCalibHitBase::setTubeRadius( float r ) {
    m_tubeRadius = r;
  }

  inline void MdtCalibHitBase::setDistanceToTrack( float dist, float sigmaDist ) {
    m_signedDistanceToTrack = dist;
    m_sigma2DistanceToTrack = sigmaDist*sigmaDist;
  }

  inline void MdtCalibHitBase::setTimeFromTrackDistance( float t, float sigmaT ) {
    m_timeFromTrackDistance = t;
    m_sigma2TimeFromTrackDistance = sigmaT*sigmaT; 
  }
  
  inline void MdtCalibHitBase::setWiresagTime( float wsag ) {
    m_wiresagTime = wsag;
  }
   
  inline void MdtCalibHitBase::setSlewingTime( float slew ) {
    m_slewingTime = slew;
  }

  inline void MdtCalibHitBase::setBFieldTime( float lor ) {
    m_bFieldTime = lor;
  }

  inline void MdtCalibHitBase::setPropagationTime( float prop ) {
    m_propagationTime = prop;
  }

  inline void MdtCalibHitBase::setTimeOfFlight( float tof ) {
    m_timeOfFlight = tof;
  }

  inline void MdtCalibHitBase::setDistanceToReadout( float dist ) {
    m_distanceToReadout = dist;
  }
 
  inline void MdtCalibHitBase::setTemperatureTime( float temp ) {
    m_temperatureTime=temp;
  }
	
  inline void MdtCalibHitBase::setBackgroundTime( float bkgr ) {
    m_backgroundTime=bkgr;
  }
	
  inline void MdtCalibHitBase::setBFieldPerp( float val ) {
    m_bFieldPerp = val;
  }

  inline void MdtCalibHitBase::setBFieldPara( float val ) {
    m_bFieldPara = val;
  }

  inline void MdtCalibHitBase::setTemperature( float val ) {
    m_temperature = val;
  }

  inline void MdtCalibHitBase::setProjSag( float val ) {
    m_projSag = val;
  }

  inline void MdtCalibHitBase::setTubeT0( float t0 ) {
    m_tube_t0 = t0;
  }

  inline void MdtCalibHitBase::setTubeAdcCal( float adccal ) {
    m_tube_adccal = adccal;
  }

  inline void MdtCalibHitBase::setLocXtwin( float xtwin ) {
    m_xtwin = xtwin;
  }
  
  inline void MdtCalibHitBase::setSigmaLocXtwin( float sigmaXTwin ) {
    m_sigmaXTwin = sigmaXTwin;
  }

  inline const MuonFixedId& MdtCalibHitBase::identify() const {
    return m_id;
  }

  inline const Amg::Vector3D& MdtCalibHitBase::globalPosition() const {
    return m_globalPosition;
  }

  inline const Amg::Vector3D& MdtCalibHitBase::localPosition() const {
    return m_localPosition;
  }

  inline const Amg::Vector3D& MdtCalibHitBase::globalPointOfClosestApproach() const {
   return m_globalPointOfClosestApproach;
  }

  inline const Amg::Vector3D& MdtCalibHitBase::localPointOfClosestApproach() const {
   return m_localPointOfClosestApproach;
  }
  
  inline unsigned short MdtCalibHitBase::tdcCount() const {
    return m_tdc;
  }

  inline unsigned short MdtCalibHitBase::adcCount() const {
    return m_adc;
  }

  inline float MdtCalibHitBase::driftTime() const {
    return m_driftTime;
  }

  inline float MdtCalibHitBase::driftRadius() const {
    return m_driftRadius;
  }

  inline float MdtCalibHitBase::sigmaDriftRadius() const {
    return std::sqrt( m_sigma2DriftRadius );
  }

  inline float MdtCalibHitBase::sigma2DriftRadius() const {
    return m_sigma2DriftRadius;
  }

  inline float MdtCalibHitBase::signedDistanceToTrack() const {
    return m_signedDistanceToTrack;
  }

  inline float MdtCalibHitBase::sigmaDistanceToTrack() const {
    return std::sqrt( m_sigma2DistanceToTrack );
  }

  inline float MdtCalibHitBase::sigma2DistanceToTrack() const {
    return m_sigma2DistanceToTrack;
  }

  inline float MdtCalibHitBase::distanceToReadout() const {
    return m_distanceToReadout;
  }

  inline float MdtCalibHitBase::slewingTime() const {
    return m_slewingTime; 
  }

  inline float MdtCalibHitBase::bFieldTime() const {
    return m_bFieldTime;
  }

  inline float MdtCalibHitBase::bFieldPerp() const {
    return m_bFieldPerp;
  }

  inline float MdtCalibHitBase::bFieldPara() const{
    return m_bFieldPara;
  }

  inline float MdtCalibHitBase::temperature() const{
    return m_temperature;
  }

  inline float MdtCalibHitBase::TemperatureTime() const {
    return m_temperatureTime;
  }

  inline float MdtCalibHitBase::WiresagTime() const {
    return m_wiresagTime;
  }

  inline float MdtCalibHitBase::projSag() const{
    return m_projSag;
  }

  inline float MdtCalibHitBase::propagationTime() const {
    return m_propagationTime;
  }

  inline float MdtCalibHitBase::timeOfFlight() const {
    return m_timeOfFlight;
  }

  inline float MdtCalibHitBase::timeFromTrackDistance() const {
    return m_timeFromTrackDistance;
  }

  inline float MdtCalibHitBase::sigmaTimeFromTrackDistance() const {
    return std::sqrt(m_sigma2TimeFromTrackDistance);
  }

  inline float MdtCalibHitBase::sigma2TimeFromTrackDistance() const {
    return m_sigma2TimeFromTrackDistance;
  }

  inline float MdtCalibHitBase::radialResidual() const {
    return std::abs( m_driftRadius ) - std::abs( m_signedDistanceToTrack );
  }

  inline float MdtCalibHitBase::trackResidual() const {
    return m_signedDistanceToTrack - m_driftRadius;
  }

  inline float MdtCalibHitBase::timeResidual() const {
    return m_driftTime - m_timeFromTrackDistance;
  }
  
  inline float  MdtCalibHitBase::tubeT0() const {
    return m_tube_t0;
  }

  inline float  MdtCalibHitBase::tubeAdcCal() const {
    return m_tube_adccal;
  }

  inline float  MdtCalibHitBase::xtwin() const {
    return m_xtwin;
  }

  inline float  MdtCalibHitBase::sigmaXtwin() const {
    return m_sigmaXTwin;
  }

  inline bool MdtCalibHitBase::segmentT0Applied() const {
    return m_segmentT0Applied;
  }
  
  inline void MdtCalibHitBase::setSegmentT0Applied(bool flag) {
    m_segmentT0Applied=flag;
  }

  inline float MdtCalibHitBase::tubeRadius() const {
    return m_tubeRadius;
  }

}  //namespace MuonCalib

std::ostream& operator << (std::ostream &stream, const MuonCalib::MdtCalibHitBase &hit);

#endif //MUONCALIB_MDTCALIBHITBASE_H

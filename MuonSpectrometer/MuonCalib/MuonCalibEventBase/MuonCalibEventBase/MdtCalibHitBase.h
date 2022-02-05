/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Mdt Calibration Input Event Data Model
 ***************************************************************************/

#ifndef MUONCALIB_MDTCALIBHITBASE_H
#define MUONCALIB_MDTCALIBHITBASE_H

// std
#include <float.h>

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
        static constexpr float kNoValue{FLT_MAX};
        struct defineParams {
            /* Fixed, Athena-independent Identifier*/
            MuonFixedId id{0};                                    //!< Identifier of the hit (not an ATLAS ID, rather a MuonFixedId)
            int tdc{0};                                           //!< TDC count
            int adc{0};                                           //!< ADC count
            Amg::Vector3D globalPosition{0, 0, 0};                //!< position of the hit in global coordinates
            Amg::Vector3D localPosition{0, 0, 0};                 //!< position of the hit in local (station) coordinates
            Amg::Vector3D localPointOfClosestApproach{0, 0, 0};   //!< point of closest approach of track to wire in local coordinates
            Amg::Vector3D globalPointOfClosestApproach{0, 0, 0};  //!< point of closest approach of track to wire in global coordinates
            float driftTime{FLT_MAX};                             //!< drift time after all corrections
            float driftRadius{FLT_MAX};                           //!< drift radius
            float sigma2DriftRadius{FLT_MAX};                     //!< error squared on the drift radius
            float timeFromTrackDistance{FLT_MAX};  //!< time calculated from 'distance to track', for calibration or trackfit in time-domain
            float sigma2TimeFromTrackDistance{FLT_MAX};  //!< (square of) sigma on timeFromTrackDistance
            float slewingTime{FLT_MAX};                  //!< time correction due to slewing (electronics)
            float bFieldTime{FLT_MAX};                   //!< time correction due to magnetic field
            float propagationTime{FLT_MAX};              //!< time correction due to signal propagation along the wire
            float timeOfFlight{FLT_MAX};                 //!< time correction due to the time of flight in ns
            float distanceToReadout{FLT_MAX};            //!< from hit to read-out side of tube
            float signedDistanceToTrack{FLT_MAX};        //!< signed distance from track to wire
            float sigma2DistanceToTrack{FLT_MAX};        //!< square of sigma on signedDistanceToTrack
            float wiresagTime{FLT_MAX};                  //!< extra time due to wiresag
            float temperatureTime{FLT_MAX};              //!< extra time due to temperature
            float backgroundTime{FLT_MAX};               //!< extra time due to background
            float bFieldPerp{FLT_MAX};                   //!< local bfield perpendicular to the wire
            float bFieldPara{FLT_MAX};                   //!< local bfield parallel to the wire
            float temperature{FLT_MAX};                  //!< local temperature
            float projSag{FLT_MAX};                      //!< projective sag
            float tube_t0{FLT_MAX};                      //!< tube t0 time offset
            float tube_adccal{FLT_MAX};                  //!< tube mean ADC (calibration)
            float xtwin{FLT_MAX};                        //!< twin position
            float sigmaXTwin{FLT_MAX};                   //!< twin position error
            bool segmentT0Applied{false};                //!< true if segment t0 is substr. from drift time
            float tubeRadius{FLT_MAX};                   //!< inner tube radius
        };

        MdtCalibHitBase() = default;                                       //!< default constructor
        MdtCalibHitBase &operator=(const MdtCalibHitBase &rhs) = default;  //!< assignment operator
        MdtCalibHitBase(const MdtCalibHitBase &event) = default;           //!< Copy constructor
        ~MdtCalibHitBase() = default;                                      //!< destructor

        MdtCalibHitBase(const MdtCalibHitBase::defineParams &pars);
        MdtCalibHitBase(
            int tdc, int adc, const Amg::Vector3D &globalPos,
            const Amg::Vector3D &localPos);  //!< constructor setting the position (local and global) as well as the tdc and adc counts

        // setters
        void setIdentifier(const MuonFixedId &id);                         //!< sets the MuonFixedIdentifier of the hit
        void setTdc(unsigned short tdc);                                   //!< sets the tdc counts
        void setAdc(unsigned short adc);                                   //!< sets the adc counts
        void setGlobalPos(const Amg::Vector3D &globalPos);                 //!< sets the position in ATLAS coordinates
        void setLocalPos(const Amg::Vector3D &localPos);                   //!< sets the position in the station coordinates
        void setGlobalPointOfClosestApproach(const Amg::Vector3D &point);  //!< sets point of closest approach in global coordinates
        void setLocalPointOfClosestApproach(const Amg::Vector3D &point);   //!< sets point of closest approach in local coordinates
        void setDriftTime(float t);                                        //!< sets drift time
        void setDriftRadius(float r, float sigmaR);                        //!< sets drift radius and drift radius error
        void setTubeRadius(float r);                                       //!< sets tube inner radius
        void setDistanceToTrack(float dist, float sigmaDist);              //!< sets the distance to the fitted track and its error
        void setTimeFromTrackDistance(float t, float sigmaT);  //!< sets the time of the distance to the fitted track (and its error)
        void setSlewingTime(float slew);                       //!< sets the time correction due to slewing
        void setBFieldTime(float lor);                         //!< sets the time correction due to the magnetic field
        void setPropagationTime(float prop);    //!< sets the time correction due to the propagation of the signal over the wire
        void setTimeOfFlight(float tof);        //!< sets the time correction due to the muon time of flight
        void setDistanceToReadout(float dist);  //!< sets the distance to read out
        void setWiresagTime(float wsag);        //!< sets the time correction due to wire sag
        void setTemperatureTime(float temp);    //!< sets the time correction due to temperature effects
        void setBackgroundTime(float bkgr);     //!< sets the time correction due to background signal
        void setBFieldPerp(float bfperp);       //!< sets the B field perpendicular to the tube
        void setBFieldPara(float bfpara);       //!< sets the B field parallel to the tube
        void setTemperature(float temp);        //!< sets the temperature
        void setProjSag(float sag);             //!< sets the projected wire sag
        void setTubeT0(float t0);               //!< sets the tube t0
        void setTubeAdcCal(float adccal);       //!< sets the tube mean ADC
        void setLocXtwin(float xtwin);          //!< sets the local twin position
        void setSigmaLocXtwin(float xtwin);     //!< sets the local twin position error
        void setSegmentT0Applied(bool flag);    //!< sets flag if Segment T0 has been applied to hit

        /* getters */
        const MuonFixedId &identify() const;                        //!< retrieve the MuonFixedId of the hit
        const Amg::Vector3D &globalPosition() const;                //!< retrieve the position given in global coordinates
        const Amg::Vector3D &localPosition() const;                 //!< retrieve the position expressed in local (station) coordinates
        const Amg::Vector3D &globalPointOfClosestApproach() const;  //!< retrieve the point of closest approach in global coordinates
        const Amg::Vector3D &localPointOfClosestApproach() const;   //!< retrieve the point of closest approach in local coordinates
        unsigned short tdcCount() const;                            //!< retrieve the tdc counts
        unsigned short adcCount() const;                            //!< retrieve the adc counts
        float driftTime() const;                                    //!< retrieve the drift time
        float driftRadius() const;                                  //!< retrieve the radius of the drift circle
        float sigmaDriftRadius() const;                             //!< retrieve the error on the radius of the drift circle
        float sigma2DriftRadius() const;                            //!< retrieve the error squared on the radius of the drift circle
        float signedDistanceToTrack() const;                        //!< retrieve the distance of the track to the wire
        float sigmaDistanceToTrack() const;                         //!< retrieve the error on the distance of the track to the wire
        float sigma2DistanceToTrack() const;                        //!< retrieve the error squared on the distance of the track to the wire
        float tubeRadius() const;                                   //!< retrieve the inner radius of the drift tube
        float distanceToReadout() const;                            //!< retrieve the distance to readout
        float slewingTime() const;                                  //!< retrieve the timing correction due to slewing
        float bFieldTime() const;                   //!< retrieve the timing correction due to the magnetic field (lorentz angle)
        float propagationTime() const;              //!< retrieve the timing correction due to the propagation of the signal
        float timeOfFlight() const;                 //!< retrieve the timing correction due to time of flight of the muon
        float timeFromTrackDistance() const;        //!< retrieve the time corresponding to the distance to track
        float sigmaTimeFromTrackDistance() const;   //!< retrieve the error of the time corresponding to the distance to track
        float sigma2TimeFromTrackDistance() const;  //!< retrieve the error squared of the time corresponding to distance to track
        float radialResidual() const;               //!< retrieve difference between driftcircle and distance to track (unsigned)
        float trackResidual() const;                //!< retrieve difference between driftcircle and distance to track (signed)
        float timeResidual() const;                 //!< retrieve difference between drifttime and timeFromTrackDistance
        float WiresagTime() const;                  //!< retrieve the timing correction due to wiresag
        float TemperatureTime() const;              //!< retrieve the timing correction due to temperature
        float BackgroundTime() const;
        float bFieldPerp() const;       //!< retrieve the Bfield perpendicular to the tube
        float bFieldPara() const;       //!< retrieve the Bfield parallel to the tube
        float temperature() const;      //!< retrieve the temperature
        float projSag() const;          //!< retrieve the projected sag
        float tubeT0() const;           //!< retrieve the tube T0
        float tubeAdcCal() const;       //!< retrieve the tube mean ADC
        float xtwin() const;            //!< retrieve the local twin position
        float sigmaXtwin() const;       //!< retrieve the local twin position error
        bool segmentT0Applied() const;  //!< retrieve the segment T0 applied flag

        /* ostream operators */
        std::ostream &dump(std::ostream &stream) const;  //!< dump to be used for operator<<() to dump the MdtCalibHitBase

    private:
        defineParams m_pars{};
    };

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MdtCalibHitBase &hit);

#endif  // MUONCALIB_MDTCALIBHITBASE_H

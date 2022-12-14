/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 * --------------------------------------
 *
 * Author       : Zdenko van Kesteren
 * Creation Date: 17 October 2005
 * Last Update  : 17 October 2005
 ***************************************************************************/

#ifndef MUONCALIBEVENT_RPCCALIBHIT_BASE_H
#define MUONCALIBEVENT_RPCCALIBHIT_BASE_H

// std
#include <iostream>

// other packages
#include "GeoPrimitives/GeoPrimitives.h"

// this package
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    /**
       @class RpcCalibHitBase
       Athena independent part of the RpcCalibHit. A RpcCalibHitBase contains the basic information of a RPC hit:
       - identifier, stored in the MuonFixedId format (not an Identifier)
       - strip width and length
       - number of strips used to cluster the hit
       - time value
       - the error
       - position in both local (station) and global coordinates
       - distance of the hit to readout of the strip

    */
    class RpcCalibHitBase {
    public:
        RpcCalibHitBase() = default;                                       //!< default constructor
        RpcCalibHitBase& operator=(const RpcCalibHitBase& rhs) = default;  //!< assignment operator
        RpcCalibHitBase(const RpcCalibHitBase& event) = default;           //!< Copy constructor
        ~RpcCalibHitBase() = default;                                      //!< destructor

        RpcCalibHitBase(int nStrips, double stripWidth, double time, double error, const Amg::Vector3D& globalPos,
                        const Amg::Vector3D& localPos);  //!< constructor initializing most of its members.
        RpcCalibHitBase(int nStrips, double stripWidth, double stripLength, double time, double error, const Amg::Vector3D& globalPos,
                        const Amg::Vector3D& localPos);  //!< constructor initializing most of its members.

        void setIdentifier(const MuonFixedId& id);          //!< sets the MuonFixedId of the hit
        void setNStrips(int nStrips);                       //!< sets the number of strips used to construct the hit
        void setStripWidth(double stripWidth);              //!< sets the strip width
        void setStripLength(double stripLength);            //!< sets the strip length
        void setTime(double time);                          //!< sets the time
        void setError(double error);                        //!< sets the error
        void setGlobalPos(const Amg::Vector3D& globalPos);  //!< sets the position in global coordinates
        void setLocalPos(const Amg::Vector3D& localPos);    //!< sets the position in local (station) coordinates
        void setDistanceToRO(const double distance);        //!< sets the distance of the hit to readout

        const MuonFixedId& identify() const;          //!< retrieve the MuonFixedId of the hit
        int nStrips() const;                          //!< retrieve the number of strips used to construct the hit
        double stripWidth() const;                    //!< retrieve the strip width
        double stripLength() const;                   //!< retrieve the strip length
        double time() const;                          //!< retrieve the time
        double error() const;                         //!< retrieve the error
        const Amg::Vector3D& globalPosition() const;  //!< retrieve the position in global coordinates
        const Amg::Vector3D& localPosition() const;   //!< retrieve the position in local (station) coordinates
        double distanceToRO() const;                  //!< retrieve the distance of the hit to readout

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the RpcCalibHitBase

    private:
        MuonFixedId m_id{0};                         //!< Indentifier of the RpcCalibHit (is not of the type Identifier but MuonFixedId )
        int m_nStrips{0};                            //!< number of strips
        double m_stripWidth{0.};                     //!< strip width
        double m_stripLength{0.};                    //!< strip length
        double m_time{0.};                           //!< time
        double m_error{0.};                          //!< error
        Amg::Vector3D m_globalPosition{0., 0., 0.};  //!< position expressed in global coordinates
        Amg::Vector3D m_localPosition{0., 0., 0.};   //!< position expressed in local (station) coordinates
        double m_distanceToRO{0.};                   //!< distance from hit to readout of the strip
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::RpcCalibHitBase& hit);

#endif  // MUONCALIBEVENT_RPCCALIBHITBASE_H

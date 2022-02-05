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

#ifndef MUONCALIBEVENT_TGCCALIBHIT_BASE_H
#define MUONCALIBEVENT_TGCCALIBHIT_BASE_H

// std
#include <iostream>

// other packages
#include "GeoPrimitives/GeoPrimitives.h"

// this package
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class TgcCalibHitBase
       Athena independent part of the TgcCalibHit. A TgcCalibHitBase contains the basic information of a RPC hit:
       - position, both in global and local (station) coordinates
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - strip information
       - error on the hit
    */

    class TgcCalibHitBase {
    public:
        TgcCalibHitBase() = default;                                       //!< default constructor
        TgcCalibHitBase& operator=(const TgcCalibHitBase& rhs) = default;  //!< assignment operator
        TgcCalibHitBase(const TgcCalibHitBase& event) = default;           //!< Copy constructor

        ~TgcCalibHitBase() = default;  //!< destructor

        TgcCalibHitBase(int nStrips, double stripWidth, double error, const Amg::Vector3D& globalPos,
                        const Amg::Vector3D& localPos);  //!< constructor initializing most of its members

        void setIdentifier(const MuonFixedId& id);          //!< sets the identifier (MuonFixedId)
        void setNStrips(int nStrips);                       //!< sets the number of strips used to cluster the hit
        void setStripWidth(double stripWidth);              //!< sets the width of the strip
        void setStripLength(double stripLength);            //!< sets the length of the strip
        void setError(double error);                        //!< sets the error on the hit
        void setGlobalPos(const Amg::Vector3D& globalPos);  //!< sets the position in global coordinates
        void setLocalPos(const Amg::Vector3D& localPos);    //!< sets the position in local (station) coordinates

        const MuonFixedId& identify() const;          //!< retrieve the identifier (MuonFixedId)
        int nStrips() const;                          //!< retrieve the number of strips used to cluster the hit
        double stripWidth() const;                    //!< retrieve the width of the strip
        double stripLength() const;                   //!< retrieve the length of the strip
        double error() const;                         //!< retrieve the error on the hit
        const Amg::Vector3D& globalPosition() const;  //!< retrieve the position in global coordinates
        const Amg::Vector3D& localPosition() const;   //!< retrieve the position in local (station) coordinates

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the TgcCalibHitBase

    private:
        MuonFixedId m_id{0};                         //!< Identifier of the hit (a MuonFixedId, not an Identifier)
        int m_nStrips{0};                            //!< number of strips used to cluster the hit
        double m_stripWidth{0.};                     //!< strip width
        double m_stripLength{0.};                    //!< strip length
        double m_error{0.};                          //!< error on the hit
        Amg::Vector3D m_globalPosition{0., 0., 0.};  //!< position of the hit expressed in global coordinates
        Amg::Vector3D m_localPosition{0., 0., 0.};   //!< position of the hit expressed in local (station) coordinates
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::TgcCalibHitBase& hit);

#endif  // MUONCALIBEVENT_TGCCALIBHIT_H

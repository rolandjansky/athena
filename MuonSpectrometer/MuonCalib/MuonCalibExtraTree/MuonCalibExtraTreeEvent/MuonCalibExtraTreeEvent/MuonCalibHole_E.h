/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBHOLE_E_H
#define MUONCALIBHOLE_E_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibHole_E
       class describing a Hole on track. A hole is the position/ID of a MDT where a Hit was expected, though not recorded.
    */
    class MuonCalibHole_E {
    public:
        MuonCalibHole_E() = default;                                       //!< default constructor
        MuonCalibHole_E(const MuonFixedId& id, const Amg::Vector3D& pos);  //!< constructor initializing all members
        MuonCalibHole_E(const MuonCalibHole_E& hit) = default;             //!< copyconstructor
        ~MuonCalibHole_E() = default;                                      //!< destructor

        MuonCalibHole_E& operator=(const MuonCalibHole_E& rhs) = default;  //!< assignment operator

        //!< retrieve hole ID (MuonCalib::MuonFixedId)
        const MuonFixedId& identify() const;
        //!< retrieve hole position
        const Amg::Vector3D& position() const;
        //!< sets hole ID (MuonCalib::MuonFixedId)
        void setId(const MuonFixedId& id);
        //!< sets hole position
        void setPosition(const Amg::Vector3D& pos);

    private:
        MuonFixedId m_id{0};              //!< hole ID (MuonCalib::MuonFixedId)
        Amg::Vector3D m_pos{0., 0., 0.};  //!< hole position
    };

}  // namespace MuonCalib

#endif  // MUONCALIBHOLE_E_H

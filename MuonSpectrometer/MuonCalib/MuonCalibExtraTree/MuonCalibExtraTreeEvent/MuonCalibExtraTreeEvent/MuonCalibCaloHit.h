/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBCALOHIT_H
#define MUONCALIBCALOHIT_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {

    /**
       Simplified class designed to store information of a MBTS hit. It has :
       - a position
       - a Identifier (int)
       - time and charge

       @author Niels van Eldik
    */
    class MuonCalibCaloHit {
    public:
        MuonCalibCaloHit() = default;  //!< default constructor
        MuonCalibCaloHit(int id, const Amg::Vector3D& pos, float time, float charge);
        ~MuonCalibCaloHit() = default;  //!< destructor

        int identify() const;                   //!< return the identifier of the Hit
        const Amg::Vector3D& position() const;  //!< return the (global) position of the MuonCalibHit
        float time() const;                     //!< return the time
        float charge() const;                   //!< return the charge

    private:
        int m_id{0};                      //!< Identifier
        Amg::Vector3D m_pos{0., 0., 0.};  //!< Global position
        float m_time{0.};                 //!< time
        float m_charge{0.};               //!< charge
    };

}  // namespace MuonCalib

#endif

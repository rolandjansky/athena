/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibExtraEvt_MUONCALIBHIT_E_H
#define MuonCalibExtraEvt_MUONCALIBHIT_E_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
namespace MuonCalib {
    /**
       @class MuonCalibHit_E
       Simplified class designed to store information of a MDT hit. It has :
       - a position
       - a MuonCalib::MuonFixedId identifier
       - driftradius and its error
    */
    // using MuonCalibHit_E = MuonCalibRawMdtHit;

    class MuonCalibHit_E {
    public:
        struct definePars {
            float driftRadius{0.};          //!< driftRadius
            float error{0.};                //!< error on the driftRadius
            float resi{-9999.};             //!< residual
            float pull{-9999.};             //!< pull including track error
            int measType{-9999};            //!< error on the driftRadius
            Amg::Vector3D pos{0., 0., 0.};  //!< Global position
        };

        MuonCalibHit_E() = default;                             //!< default constructor
        MuonCalibHit_E(const MuonFixedId& id, definePars pos);  //!< constructor initializing its ID and position

        virtual ~MuonCalibHit_E() = default;  //!< destructor

        const MuonFixedId& identify() const;    //!< return the MuonCalib::MuonFixedId of the MuonCalibHit
        const Amg::Vector3D& position() const;  //!< return the (global) position of the MuonCalibHit
        float driftRadius() const;              //!< return the driftradius of the MuonCalibHit
        float error() const;                    //!< return the error on the driftradius of the MuonCalibHit
        float residual() const;                 //!< return the residual
        float pull() const;                     //!< return the pull
        int type() const;                       //!< return the type of measurement

        //!< sets the MuonCalib::MuonFixedId of the MuonCalibHit
        void setId(const MuonFixedId& id);
        //!< sets the (global) position of the MuonCalibHit
        void setPosition(const Amg::Vector3D& pos);
        //!< sets the driftradius of the MuonCalibHit

        void setDriftRadius(float driftRadius);
        //!< sets the error on the driftradius of the MuonCalibHit
        void setError(float error);
        //!< sets the error on the driftradius of the MuonCalibHit
        void setResidual(float res);
        //!< sets the error on the driftradius of the MuonCalibHit

        void setPull(float pull);
        //!< sets  the error on the driftradius of the MuonCalibHit
        void setType(int type);

    private:
        MuonFixedId m_id{0};  //!< Identifier of the (athena-independent) type MuonCalib::MuonFixedId
        definePars m_pars{};
    };
}  // namespace MuonCalib

#endif  // MUONCALIBHIT_E_H

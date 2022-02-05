/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibMdtTruthHit_h
#define MuonCalibMdtTruthHit_h

#include <iostream>

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibMdtTruthHit
       Class with truth information of a MDT hit, containing:
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - barcode
       - driftradius and position along the tube
    */
    class MuonCalibMdtTruthHit {
    public:
        MuonCalibMdtTruthHit() = default;                                            //!< default constructor
        MuonCalibMdtTruthHit(const MuonCalibMdtTruthHit &truth) = default;           //!< copyconstructor
        ~MuonCalibMdtTruthHit() = default;                                           //!< destructor
        MuonCalibMdtTruthHit &operator=(const MuonCalibMdtTruthHit &rhs) = default;  //!< assignment operator

        MuonCalibMdtTruthHit(MuonFixedId id, int barCode, double driftRadius, double positionAlongTube, Amg::Vector3D globPos,
                             double time);  //<! constructor initializing all attributes of the MuonCalibMdtTruthHit

        std::ostream &dump(std::ostream &stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibMdtTruthHit

        MuonFixedId identify() const;                         //!< retrieve the MuonFixedId
        int barCode() const;                                  //!< retrieve the barcode
        double driftRadius() const;                           //!< retrieve the driftradius
        double positionAlongTube() const;                     //!< retrieve the position along tube
        double gpositionX() const;                            //!< retrieve the global X position
        double gpositionY() const;                            //!< retrieve the global Y position
        double gpositionZ() const;                            //!< retrieve the global Z position
        double time() const;                                  //!< retrieve the time
        void setIdentifier(MuonFixedId id);                   //!< sets the MuonFixedId
        void setBarCode(int barCode);                         //!< sets the barcode
        void setDriftRadius(double driftRadius);              //!< sets the driftradius
        void setPositionAlongTube(double positionAlongTube);  //!< sets the position along tube
        void setgPositionX(double gpositionX);                //!< sets the global X position
        void setgPositionY(double gpositionY);                //!< sets the global Y position
        void setgPositionZ(double gpositionZ);                //!< sets the global Z position
        void setgPosition(Amg::Vector3D newPos);
        void setTime(double time);  //!< sets the time

    private:
        MuonFixedId m_ID{0};             //!< identifier of the truth hit, (a MuonFixedId, not an Identifier)
        int m_barCode{0};                //!< barcode
        double m_driftRadius{0.};        //!< drift radius
        double m_positionAlongTube{0.};  //!< position along tube
        Amg::Vector3D m_pos{0., 0., 0.};
        double m_time{0.};  //!< time
    };

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibMdtTruthHit &truth);

#endif  // MuonCalibTruth_h

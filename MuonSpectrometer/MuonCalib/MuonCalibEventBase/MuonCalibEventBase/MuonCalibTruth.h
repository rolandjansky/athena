/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store truth information in                     //
// Author: Zdenko van Kesteren                             //
// Date  : 2 December 2005                                 //
//*********************************************************//

#ifndef MuonCalibTruth_h
#define MuonCalibTruth_h
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {
    /**
       @class MuonCalibTruth
       Calib-EDM version of TrackRecord, given at the MuonSpectrometer entrance. It contains the following:
       - position and momentum
       - kinetic energy
       - PDG code (13 for a muon) and the BarCode
    */

    class MuonCalibTruth {
    public:
        MuonCalibTruth() = default;                             //!< default constructor
        MuonCalibTruth(const MuonCalibTruth& truth) = default;  //!< copy constructor
        MuonCalibTruth& operator=(const MuonCalibTruth& rhs);   //!< assignement operator
        ~MuonCalibTruth() = default;                            //!< destructor

        MuonCalibTruth(const Amg::Vector3D& pos, const Amg::Vector3D& momentum, double kinEnergy, int code,
                       int barCode);  //!< constructor initializing all attributes

        void setPosition(const Amg::Vector3D& pos);  //!< sets position
        void setMomentum(const Amg::Vector3D& mom);  //!< sets momentum
        void setKinEnergy(double kinEnergy);         //!< sets kinetic energy
        void setPDGCode(int code);                   //!< sets PDG code
        void setBarCode(int barCode);                //!< sets barcode

        const Amg::Vector3D& position() const;  //!< retrieve position
        const Amg::Vector3D& momentum() const;  //!< retrieve momentum
        double kinEnergy() const;               //!< retrieve kinetic energy
        int PDGCode() const;                    //!< retrieve PDG code
        int barCode() const;                    //!< retrieve barcode

        std::ostream& dump(std::ostream& stream) const;  //!< dump method to be used in operator<<() of the MuonCalibTruth for output
    private:
        Amg::Vector3D m_position{0., 0., 0.};  //!< position
        Amg::Vector3D m_momentum{0., 0., 0.};  //!< momentum
        double m_kinEnergy{0.};                //!< kinetic energy
        int m_PDGCode{0};                      //!< PDG code
        int m_barCode{0};                      //!< bar code
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibTruth& truht);

#endif  // MuonCalibTruth_h

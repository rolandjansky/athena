/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibRawMdtHit_h
#define MuonCalibRawMdtHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibRawMdtHit
       Calib-EDM version of MdtPrepData class, containing uncalibrated information.
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - position in both local (station) and global coordinates
       - occupancy: value denoting how many times this raw hit is assigned to a MuonCalibSegment. When this value yields -1, the raw hit is
       not assinged to any MuonCalibSegment
       - tdc and adc counts
       - drift time, drift radius and its error
     */
    class MuonCalibRawMdtHit {
    public:
        MuonCalibRawMdtHit() = default;                                          //!< default constructor
        MuonCalibRawMdtHit(const MuonCalibRawMdtHit &hit) = default;             //!< copyconstructor
        MuonCalibRawMdtHit &operator=(const MuonCalibRawMdtHit &rhs) = default;  //!< assignment operator
        ~MuonCalibRawMdtHit() = default;                                         //!< destructor

        MuonCalibRawMdtHit(const MuonFixedId &id, const Amg::Vector3D &loc, const Amg::Vector3D &glob,
                           int occ);  //!< constructor initializing the identifier, the position(s) and occupancy.

        std::ostream &dump(std::ostream &stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawMdtHit

        const MuonFixedId &identify() const;          //!< retrieve the MuonFixedId
        const Amg::Vector3D &localPosition() const;   //!< retrieve the position expressed in local (station) coordinates
        const Amg::Vector3D &globalPosition() const;  //!< retrieve the position expressed in global coordinates
        int adcCount() const;                         //!< retrieve adc counts
        int tdcCount() const;                         //!< retrieve tdc counts
        int occupancy() const;                        //!< retrieve the occupancy (-1 == not assigned to any segment)
        double driftTime() const;                     //!< retrieve the drift time
        double driftRadius() const;                   //!< retrieve the drift radius
        double driftRadiusError() const;              //!< retrieve the error on the drift radius

        void setId(MuonFixedId Id);                         //!< sets the MuonFixedId
        void setLocalPosition(const Amg::Vector3D &loc);    //!< sets the position expressed in local (station) coordinates
        void setGlobalPosition(const Amg::Vector3D &glob);  //!< sets the position expressed in global coordinates
        void setAdc(int adc);                               //!< sets adc counts
        void setTdc(int tdc);                               //!< sets tdc counts
        void setOccupancy(int occ);                         //!< sets the occupancy
        void setDriftTime(double t);                        //!< sets the drift time
        void setDriftRadius(double r);                      //!< sets the drift radius
        void setDriftRadiusError(double dr);                //!< sets the error on the drift radius

    private:
        MuonFixedId m_id{0};               //!< identifier of the raw hit, (is a MuonFixedId, not an Identifier)
        Amg::Vector3D m_Lpos{0., 0., 0.};  //!< position of the hit in local (station) coordinates
        Amg::Vector3D m_Gpos{0., 0., 0.};  //!< position of the hit in global coordinates
        int m_adc{0};                      //!< ADC count
        int m_tdc{0};                      //!< TDC count
        int m_occupancy{0};                //!< occupancy (-1 == not assigned to any segment)
        double m_t{0.};                    //!< drift time
        double m_r{0.};                    //!< drift radius
        double m_dr{0.};                   //!< error on the drift radius
    };

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawMdtHit &rawHit);

#endif  // MuonCalibRawMdtHit_h

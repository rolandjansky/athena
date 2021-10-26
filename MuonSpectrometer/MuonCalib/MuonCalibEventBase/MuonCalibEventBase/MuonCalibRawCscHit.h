/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Csc information in                   //
//*********************************************************//

#ifndef MuonCalibRawCscHit_h
#define MuonCalibRawCscHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    /**
       @class MuonCalibRawCscHit
       Calib-EDM version of CscPrepData class, containing uncalibrated information.
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - position in global coordinates
       - occupancy: value denoting how many times this raw hit is
         assigned to a MuonCalibSegment. A value of -1 indicates the
         raw hit is not assinged to any MuonCalibSegment
       - charge and time
       - strip width
     */

    class MuonCalibRawCscHit {
    public:
        MuonCalibRawCscHit() = default;                                //!< default constructor
        MuonCalibRawCscHit(const MuonCalibRawCscHit &hit) = default;   //!< copy constructor
        MuonCalibRawCscHit &operator=(const MuonCalibRawCscHit &rhs);  //!< assingment operator

        ~MuonCalibRawCscHit() = default;  //!< destructor

        MuonCalibRawCscHit(const MuonFixedId &id, const Amg::Vector3D &glob, int occ, float t, float width,
                           float charge);  //!< constructor setting all attributes

        std::ostream &dump(std::ostream &stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawCscHit

        const MuonFixedId &identify() const;          //!< retrieve the MuonFixedId
        const Amg::Vector3D &globalPosition() const;  //!< retrieve the position expressed in global coordinates
        int occupancy() const;                        //!< retrieve the occupancy (-1 == not assigned to any segment)
        float t() const;                              //!< retrieve the time
        float width() const;                          //!< retrieve the strip width
        float charge() const;                         //!< retrieve the charge

        void setId(MuonFixedId Id);                         //!< sets the MuonFixedId
        void setGlobalPosition(const Amg::Vector3D &glob);  //!< sets the position expressed in global coordinates
        void setOccupancy(int occ);                         //!< sets the occupancy
        void setT(float t);                                 //!< sets the time
        void setWidth(float width);                         //!< sets the strip width
        void setCharge(float charge);                       //!< sets the charge

    private:
        MuonFixedId m_id{0};               //!< identifier of the raw hit, (a MuonFixedId, not an Identifier)
        Amg::Vector3D m_Gpos{0., 0., 0.};  //!< position of the hit in global coordinates
        int m_occupancy{0};                //!< occupancy (-1 == not assigned to any segment)
        float m_t{0.};                     //!< time
        float m_width{0.};                 //!< strip width
        float m_charge{0.};                //!< charge
    };

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawCscHit &rawHit);

#endif  // MuonCalibRawCscHit_h

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Rpc information in                   //
//*********************************************************//

#ifndef MuonCalibRawRpcHit_h
#define MuonCalibRawRpcHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    /**
       @class MuonCalibRawRpcHit
       Calib-EDM version of RpcPrepData class, containing uncalibrated information.
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - position in global coordinates
       - occupancy: value denoting how many times this raw hit is assigned to a MuonCalibSegment. When this value yields -1, the raw hit is
       not assinged to any MuonCalibSegment
       - time
       - strip length and width
     */
    class MuonCalibRawRpcHit {
    public:
        MuonCalibRawRpcHit() = default;                                //!< default constructor
        MuonCalibRawRpcHit(const MuonCalibRawRpcHit& hit) = default;   //!< copy constructor
        MuonCalibRawRpcHit& operator=(const MuonCalibRawRpcHit& rhs);  //!< assingment operator
        virtual ~MuonCalibRawRpcHit() = default;                       //!< destructor

        struct defineParams {
            MuonFixedId id{0};                   //!< identifier of the raw hit, (is a MuonFixedId, not an Identifier)
            Amg::Vector3D glob_pos{0., 0., 0.};  //!< position of the hit in global coordinates
            int occupancy{0};                    //!< occupancy (-1 == not assigned to any segment)
            float t{0.};                         //!< time
            float width{0.};                     //!< strip width
            float length{0.};                    //!< strip length
        };

        MuonCalibRawRpcHit(const defineParams& pars);

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawRpcHit

        const MuonFixedId& identify() const;          //!< retrieve the MuonFixedId
        const Amg::Vector3D& globalPosition() const;  //!< retrieve the position expressed in global coordinates
        int occupancy() const;                        //!< retrieve the occupancy (-1 == not assigned to any segment)
        float t() const;                              //!< retrieve the time
        float width() const;                          //!< retrieve the strip width
        float length() const;                         //!< retrieve the strip length

        void setId(MuonFixedId Id);                         //!< sets the MuonFixedId
        void setGlobalPosition(const Amg::Vector3D& glob);  //!< sets the position expressed in global coordinates
        void setOccupancy(int occ);                         //!< sets the occupancy
        void setT(float t);                                 //!< sets the time
        void setWidth(float width);                         //!< sets the strip width
        void setLength(float length);                       //!< sets the strip length

    private:
        defineParams m_pars{};
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawRpcHit& rawHit);

#endif  // MuonCalibRawRpcHit_h

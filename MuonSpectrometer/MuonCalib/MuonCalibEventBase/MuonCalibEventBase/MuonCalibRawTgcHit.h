/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Tgc information in                   //
// Author: Zdenko van Kesteren                             //
// Date  : 21st june 2006                                  //
//*********************************************************//

#ifndef MuonCalibRawTgcHit_h
#define MuonCalibRawTgcHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibRawTgcHit
       Calib-EDM version of TgcPrepData class, containing uncalibrated information.
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - position in global coordinates
       - occupancy: value denoting how many times this raw hit is assigned to a MuonCalibSegment. When this value yields -1, the raw hit is
       not assinged to any MuonCalibSegment
       - strip length and width
     */

    class MuonCalibRawTgcHit {
    public:
        MuonCalibRawTgcHit() = default;                                          //!< default constructor
        MuonCalibRawTgcHit(const MuonCalibRawTgcHit& hit) = default;             //!< copy constructor
        MuonCalibRawTgcHit& operator=(const MuonCalibRawTgcHit& rhs) = default;  //!< assingment operator

        ~MuonCalibRawTgcHit() = default;  //!< destructor

        enum BunchCrossingTag { Previous = 1, Current = 2, Next = 3 };

        struct defineParams {
            MuonFixedId id{0};                     //!< identifier of the raw hit, (is a MuonFixedId, not an Identifier)
            Amg::Vector3D global_pos{0., 0., 0.};  //!< position of the hit in global coordinates
            int occupancy{0};                      //!< occupancy (-1 == not assigned to any segment)
            int station{0};
            int eta{0};
            int phi{0};
            int gasGap{0};
            int isStrip{0};
            int channel{0};
            int bcTag{0};

            float width{0};
            float shortWidth{0};
            float longWidth{0};  //!< strip width
            float length{0};     //!< strip length
        };

        MuonCalibRawTgcHit(const defineParams& pars);

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawTgcHit

        const MuonFixedId& identify() const;          //!< retrieve the MuonFixedId
        const Amg::Vector3D& globalPosition() const;  //!< retrieve the position expressed in global coordinates
        int occupancy() const;                        //!< retrieve the occupancy (-1 == not assigned to any segment)
        int station() const;
        int eta() const;
        int phi() const;
        int gasGap() const;
        int isStrip() const;
        int channel() const;
        int bcTag() const;

        float width() const;
        float shortWidth() const;
        float longWidth() const;
        float length() const;  //!< retrieve the strip length

        void setId(MuonFixedId Id);                         //!< sets the MuonFixedId
        void setGlobalPosition(const Amg::Vector3D& glob);  //!< sets the position expressed in global coordinates
        void setOccupancy(int occ);                         //!< sets the occupancy
        void setStation(int station);
        void setEta(int eta);
        void setPhi(int phi);
        void setGasGap(int gasGap);
        void setIsStrip(int isStrip);
        void setChannel(int channel);
        void setBcTag(int bcTag);

        void setWidth(float width);
        void setShortWidth(float shortWidth);
        void setLongWidth(float longWidth);  //!< sets the strip width
        void setLength(float length);        //!< sets the strip length

    private:
        defineParams m_pars{};
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawTgcHit& rawHit);

#endif  // MuonCalibRawTgcHit_h

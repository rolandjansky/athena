/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Tgc information in                   //
// Author: Takashi Kubota                                  //
// Date  : 31st July 2008                                  //
//*********************************************************//

#ifndef MuonCalibRawTgcCoin_h
#define MuonCalibRawTgcCoin_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibRawTgcCoin
       Calib-EDM version of TgcPrepData class, containing uncalibrated information.
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - position in global coordinates
       - occupancy: value denoting how many times this raw coin is assigned to a MuonCalibSegment. When this value yields -1, the raw coin
       is not assinged to any MuonCalibSegment
       - strip length and width
     */

    class MuonCalibRawTgcCoin {
    public:
        MuonCalibRawTgcCoin() = default;  //!< default constructor

        MuonCalibRawTgcCoin(const MuonCalibRawTgcCoin& coin) = default;            //!< copy constructor
        MuonCalibRawTgcCoin& operator=(const MuonCalibRawTgcCoin& rhs) = default;  //!< assingment operator
        ~MuonCalibRawTgcCoin() = default;                                          //!< destructor

        MuonCalibRawTgcCoin(const MuonFixedId& id, const Amg::Vector3D& gPosIn, const Amg::Vector3D& gPosOut, const int type, const int eta,
                            const int phi, const int sector, const int isForward, const int isStrip, const int trackletId,
                            const int trackletIdStrip, const int bcTag, const float widthIn, const float widthOut, const float widthR,
                            const float widthPhi, const int delta, const int roi, const int pt, const int sub, const int isPositiveDeltaR);

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawTgcCoin

        const MuonFixedId& identify() const;   //!< retrieve the MuonFixedId
        const Amg::Vector3D& gPosIn() const;   //!< retrieve the position expressed in global coordinates
        const Amg::Vector3D& gPosOut() const;  //!< retrieve the position expressed in global coordinates
        int type() const;
        int eta() const;
        int phi() const;
        int sector() const;
        int isForward() const;
        int isStrip() const;
        int trackletId() const;
        int trackletIdStrip() const;
        int bcTag() const;
        float widthR() const;
        float widthIn() const;
        float widthOut() const;
        float widthPhi() const;
        int delta() const;
        int roi() const;
        int pt() const;
        int sub() const;
        int isPositiveDeltaR() const;

        void setId(MuonFixedId Id);                               //!< sets the MuonFixedId
        void setGlobalPositionIn(const Amg::Vector3D& gPosIn);    //!< sets the position expressed in global coordinates
        void setGlobalPositionOut(const Amg::Vector3D& gPosOut);  //!< sets the position expressed in global coordinates
        void setType(int type);
        void setEta(int eta);
        void setPhi(int phi);
        void setSector(int sector);
        void setIsForward(int isForward);
        void setIsStrip(int isStrip);
        void setTrackletId(int trackletId);
        void setTrackletIdStrip(int trackletIdStrip);
        void setBcTag(int bcTag);
        void setWidthIn(float widthIn);
        void setWidthOut(float widthOut);
        void setWidthR(float widthR);
        void setWidthPhi(float widthPhi);
        void setDelta(int delta);
        void setRoi(int roi);
        void setPt(int pt);
        void setSub(int sub);
        void setIsPositiveDeltaR(int isPositiveDeltaR);

    private:
        MuonFixedId m_id;  //!< identifier of the raw coin, (is a MuonFixedId, not an Identifier)
        Amg::Vector3D m_gPosIn{0., 0., 0.};
        Amg::Vector3D m_gPosOut{0., 0., 0.};
        int m_type{0};
        int m_eta{0};
        int m_phi{0};
        int m_sector{0};
        int m_isForward{0};
        int m_isStrip{0};
        int m_trackletId{0};
        int m_trackletIdStrip{0};
        int m_bcTag{0};
        float m_widthIn{0};
        float m_widthOut{0};
        float m_widthR{0};
        float m_widthPhi{0};

        int m_delta{0};
        int m_roi{0};
        int m_pt{0};

        int m_sub{0};
        int m_isPositiveDeltaR{0};
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawTgcCoin& rawCoin);

#endif  // MuonCalibRawTgcCoin_h

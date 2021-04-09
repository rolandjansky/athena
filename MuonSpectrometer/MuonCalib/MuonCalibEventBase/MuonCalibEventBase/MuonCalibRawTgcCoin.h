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
        MuonCalibRawTgcCoin() :
            m_gPosIn(0., 0., 0.),
            m_gPosOut(0., 0., 0.),
            m_type(0),
            m_eta(0),
            m_phi(0),
            m_sector(0),
            m_isForward(0),
            m_isStrip(0),
            m_trackletId(0),
            m_trackletIdStrip(0),
            m_bcTag(0),
            m_widthIn(0.),
            m_widthOut(0.),
            m_widthR(0.),
            m_widthPhi(0.),
            m_delta(0),
            m_roi(0),
            m_pt(0),
            m_sub(0),
            m_isPositiveDeltaR(0){};  //!< default constructor

        MuonCalibRawTgcCoin(const MuonFixedId& id, const Amg::Vector3D& gPosIn, const Amg::Vector3D& gPosOut, const int type, const int eta,
                            const int phi, const int sector, const int isForward, const int isStrip, const int trackletId,
                            const int trackletIdStrip, const int bcTag, const float widthIn, const float widthOut, const float widthR,
                            const float widthPhi, const int delta, const int roi, const int pt, const int sub, const int isPositiveDeltaR);

        MuonCalibRawTgcCoin(const MuonCalibRawTgcCoin& coin);  //!< copy constructor
        ~MuonCalibRawTgcCoin(){};                              //!< destructor

        MuonCalibRawTgcCoin& operator=(const MuonCalibRawTgcCoin& rhs);  //!< assingment operator

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawTgcCoin

        const MuonFixedId& identify() const { return m_id; }        //!< retrieve the MuonFixedId
        const Amg::Vector3D& gPosIn() const { return m_gPosIn; }    //!< retrieve the position expressed in global coordinates
        const Amg::Vector3D& gPosOut() const { return m_gPosOut; }  //!< retrieve the position expressed in global coordinates
        int type() const { return m_type; }
        int eta() const { return m_eta; }
        int phi() const { return m_phi; }
        int sector() const { return m_sector; }
        int isForward() const { return m_isForward; }
        int isStrip() const { return m_isStrip; }
        int trackletId() const { return m_trackletId; }
        int trackletIdStrip() const { return m_trackletIdStrip; }
        int bcTag() const { return m_bcTag; }
        float widthIn() const { return m_widthIn; }
        float widthOut() const { return m_widthOut; }
        float widthR() const { return m_widthR; }
        float widthPhi() const { return m_widthPhi; }
        int delta() const { return m_delta; }
        int roi() const { return m_roi; }
        int pt() const { return m_pt; }
        int sub() const { return m_sub; }
        int isPositiveDeltaR() const { return m_isPositiveDeltaR; }

        void setId(MuonFixedId Id) { m_id = Id; }                                     //!< sets the MuonFixedId
        void setGlobalPositionIn(const Amg::Vector3D& gPosIn) { m_gPosIn = gPosIn; }  //!< sets the position expressed in global coordinates
        void setGlobalPositionOut(const Amg::Vector3D& gPosOut) {
            m_gPosOut = gPosOut;
        }  //!< sets the position expressed in global coordinates
        void setType(int type) { m_type = type; }
        void setEta(int eta) { m_eta = eta; }
        void setPhi(int phi) { m_phi = phi; }
        void setSector(int sector) { m_sector = sector; }
        void setIsForward(int isForward) { m_isForward = isForward; }
        void setIsStrip(int isStrip) { m_isStrip = isStrip; }
        void setTrackletId(int trackletId) { m_trackletId = trackletId; }
        void setTrackletIdStrip(int trackletIdStrip) { m_trackletIdStrip = trackletIdStrip; }
        void setBcTag(int bcTag) { m_bcTag = bcTag; }
        void setWidthIn(float widthIn) { m_widthIn = widthIn; }
        void setWidthOut(float widthOut) { m_widthOut = widthOut; }
        void setWidthR(float widthR) { m_widthR = widthR; }
        void setWidthPhi(float widthPhi) { m_widthPhi = widthPhi; }
        void setDelta(int delta) { m_delta = delta; }
        void setRoi(int roi) { m_roi = roi; }
        void setPt(int pt) { m_pt = pt; }
        void setSub(int sub) { m_sub = sub; }
        void setIsPositiveDeltaR(int isPositiveDeltaR) { m_isPositiveDeltaR = isPositiveDeltaR; }

    private:
        MuonFixedId m_id;  //!< identifier of the raw coin, (is a MuonFixedId, not an Identifier)
        Amg::Vector3D m_gPosIn;
        Amg::Vector3D m_gPosOut;
        int m_type;
        int m_eta;
        int m_phi;
        int m_sector;
        int m_isForward;
        int m_isStrip;
        int m_trackletId;
        int m_trackletIdStrip;
        int m_bcTag;
        float m_widthIn;
        float m_widthOut;
        float m_widthR;
        float m_widthPhi;

        int m_delta;
        int m_roi;
        int m_pt;

        int m_sub;
        int m_isPositiveDeltaR;
    };

    inline MuonCalibRawTgcCoin::MuonCalibRawTgcCoin(const MuonFixedId& id, const Amg::Vector3D& gPosIn, const Amg::Vector3D& gPosOut,
                                                    const int type, const int eta, const int phi, const int sector, const int isForward,
                                                    const int isStrip, const int trackletId, const int trackletIdStrip, const int bcTag,
                                                    const float widthIn, const float widthOut, const float widthR, const float widthPhi,
                                                    const int delta, const int roi, const int pt, const int sub = 0,
                                                    const int isPositiveDeltaR = 0) :
        m_id(id),
        m_gPosIn(gPosIn),
        m_gPosOut(gPosOut),
        m_type(type),
        m_eta(eta),
        m_phi(phi),
        m_sector(sector),
        m_isForward(isForward),
        m_isStrip(isStrip),
        m_trackletId(trackletId),
        m_trackletIdStrip(trackletIdStrip),
        m_bcTag(bcTag),
        m_widthIn(widthIn),
        m_widthOut(widthOut),
        m_widthR(widthR),
        m_widthPhi(widthPhi),
        m_delta(delta),
        m_roi(roi),
        m_pt(pt),
        m_sub(sub),
        m_isPositiveDeltaR(isPositiveDeltaR) {}

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawTgcCoin& rawCoin);

#endif  // MuonCalibRawTgcCoin_h

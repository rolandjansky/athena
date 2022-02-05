/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"

#include <iostream>

namespace MuonCalib {

    const MuonFixedId& MuonCalibRawTgcHit::identify() const { return m_pars.id; }
    const Amg::Vector3D& MuonCalibRawTgcHit::globalPosition() const { return m_pars.global_pos; }
    int MuonCalibRawTgcHit::occupancy() const { return m_pars.occupancy; }
    int MuonCalibRawTgcHit::station() const { return m_pars.station; }
    int MuonCalibRawTgcHit::eta() const { return m_pars.eta; }
    int MuonCalibRawTgcHit::phi() const { return m_pars.phi; }
    int MuonCalibRawTgcHit::gasGap() const { return m_pars.gasGap; }
    int MuonCalibRawTgcHit::isStrip() const { return m_pars.isStrip; }
    int MuonCalibRawTgcHit::channel() const { return m_pars.channel; }
    int MuonCalibRawTgcHit::bcTag() const { return m_pars.bcTag; }

    float MuonCalibRawTgcHit::width() const { return m_pars.width; }
    float MuonCalibRawTgcHit::shortWidth() const { return m_pars.shortWidth; }
    float MuonCalibRawTgcHit::longWidth() const { return m_pars.longWidth; }
    float MuonCalibRawTgcHit::length() const { return m_pars.length; }

    void MuonCalibRawTgcHit::setId(MuonFixedId Id) { m_pars.id = Id; }
    void MuonCalibRawTgcHit::setGlobalPosition(const Amg::Vector3D& glob) { m_pars.global_pos = glob; }
    void MuonCalibRawTgcHit::setOccupancy(int occ) { m_pars.occupancy = occ; }
    void MuonCalibRawTgcHit::setStation(int station) { m_pars.station = station; }
    void MuonCalibRawTgcHit::setEta(int eta) { m_pars.eta = eta; }
    void MuonCalibRawTgcHit::setPhi(int phi) { m_pars.phi = phi; }
    void MuonCalibRawTgcHit::setGasGap(int gasGap) { m_pars.gasGap = gasGap; }
    void MuonCalibRawTgcHit::setIsStrip(int isStrip) { m_pars.isStrip = isStrip; }
    void MuonCalibRawTgcHit::setChannel(int channel) { m_pars.channel = channel; }
    void MuonCalibRawTgcHit::setBcTag(int bcTag) { m_pars.bcTag = bcTag; }

    void MuonCalibRawTgcHit::setWidth(float width) { m_pars.width = width; }
    void MuonCalibRawTgcHit::setShortWidth(float shortWidth) { m_pars.shortWidth = shortWidth; }
    void MuonCalibRawTgcHit::setLongWidth(float longWidth) { m_pars.longWidth = longWidth; }
    void MuonCalibRawTgcHit::setLength(float length) { m_pars.length = length; }

    std::ostream& MuonCalibRawTgcHit::dump(std::ostream& stream) const {
        stream << "MuonCalibRawTgcHit with" << std::endl;
        stream << "   identifier " << identify() << std::endl;
        stream << "   global position " << globalPosition() << std::endl;
        stream << "   occupancy " << occupancy() << std::endl;
        stream << "   station " << station() << std::endl;
        stream << "   eta " << eta() << std::endl;
        stream << "   phi " << phi() << std::endl;
        stream << "   gasGap " << gasGap() << std::endl;
        stream << "   isStrip " << isStrip() << std::endl;
        stream << "   channel " << channel() << std::endl;
        stream << "   bcTag " << bcTag() << std::endl;
        stream << "   width " << width() << std::endl;
        stream << "   shortWidth " << shortWidth() << std::endl;
        stream << "   longWidth " << longWidth() << std::endl;
        stream << "   length " << length() << std::endl;
        return stream;
    }

    MuonCalibRawTgcHit::MuonCalibRawTgcHit(const MuonCalibRawTgcHit::defineParams& pars) : m_pars{pars} {}
}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawTgcHit& hit) { return hit.dump(stream); }

//} //namespace MuonCalib

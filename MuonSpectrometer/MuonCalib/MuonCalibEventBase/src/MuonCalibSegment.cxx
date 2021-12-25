/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibSegment.h"

#include <algorithm>
#include <iostream>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {

    MuonCalibSegment::MuonCalibSegment(double chi2, const Amg::Vector3D& pos, const Amg::Vector3D& dir, const Amg::Transform3D& locToGlo,
                                       unsigned int qualityFlag) :
        m_chi2{chi2}, m_localPosition{pos}, m_localDirection{dir}, m_localToGlobal{locToGlo}, m_qualityFlag{qualityFlag} {}

    MuonCalibSegment::~MuonCalibSegment() = default;
    void MuonCalibSegment::copy(const MuonCalibSegment& other) {
        m_chi2 = other.chi2();
        m_dy0 = other.error_dy0();
        m_dtheta = other.error_dtheta();
        m_localPosition = other.position();
        m_localDirection = other.direction();
        m_localToGlobal = other.localToGlobal();
        m_qualityFlag = other.qualityFlag();
        m_author = other.author();
        m_fittedT0 = other.m_fittedT0;  // don't use function as it would return 0 if m_fittedT0 == -99999.

        m_mdtHitsOnTrack.clear();
        for (const MdtHitPtr& mdt_it : other.mdtHOT()) { addHitOnTrack(new MdtCalibHitBase(*mdt_it)); }

        m_mdtCloseHits.clear();
        for (const MdtHitPtr& mdt_it : other.mdtClose()) { addCloseHit(new MdtCalibHitBase(*mdt_it)); }

        m_cscHitsOnTrack.clear();
        for (const CscHitPtr& csc_it : other.cscHOT()) { addHitOnTrack(new CscCalibHitBase(*csc_it)); }
        for (const CscHitPtr& csc_it : other.cscClose()) { addCloseHit(new CscCalibHitBase(*csc_it)); }

        m_rpcHitsOnTrack.clear();
        for (const RpcHitPtr& rpc_it : other.rpcHOT()) { addHitOnTrack(new RpcCalibHitBase(*rpc_it)); }

        m_rpcCloseHits.clear();
        for (const RpcHitPtr& rpc_it : other.rpcClose()) { addCloseHit(new RpcCalibHitBase(*rpc_it)); }

        m_tgcHitsOnTrack.clear();
        for (const TgcHitPtr& tgc_it : other.tgcHOT()) { addHitOnTrack(new TgcCalibHitBase(*tgc_it)); }

        m_tgcCloseHits.clear();
        for (const TgcHitPtr& tgc_it : other.tgcClose()) { addCloseHit(new TgcCalibHitBase(*tgc_it)); }
    }

    MuonCalibSegment::MuonCalibSegment(const MuonCalibSegment& other) { copy(other); }
    MuonCalibSegment& MuonCalibSegment::operator=(const MuonCalibSegment& other) {
        if (this != &other) { copy(other); }
        return *this;
    }
    unsigned int MuonCalibSegment::hitsPerML(int ML) const {
        int counter{0};
        for (const MdtHitPtr& hit_it : mdtHOT()) { counter += (hit_it->identify().mdtMultilayer() == ML); }
        return counter;
    }

    std::ostream& MuonCalibSegment::dump(std::ostream& stream) const {
        stream << "MuonCalibSegment with chi2 " << chi2() << std::endl;
        stream << " -- local position " << position() << " direction " << direction() << std::endl;
        stream << " -- HitsOnTrack " << hitsOnTrack() << std::endl;

        // Dump MdtCalibHit
        for (const MdtHitPtr& mdt_it : mdtHOT()) { mdt_it->dump(stream); }
        stream << " -- CloseHits " << mdtCloseHits() << std::endl;
        for (const MdtHitPtr& mdt_it : mdtClose()) { mdt_it->dump(stream); }

        // Dump CscCalibHit
        for (const CscHitPtr& csc_it : cscHOT()) { csc_it->dump(stream); }
        stream << " -- CloseHits " << cscCloseHits() << std::endl;
        for (const CscHitPtr& csc_it : cscClose()) { csc_it->dump(stream); }

        // Dump RpcCalibHit
        for (const RpcHitPtr& rpc_it : rpcHOT()) { rpc_it->dump(stream); }
        stream << " -- CloseHits " << rpcCloseHits() << std::endl;
        for (const RpcHitPtr& rpc_it : rpcClose()) { rpc_it->dump(stream); }

        // Dump TgcCalibHit
        for (const TgcHitPtr& tgc_it : tgcHOT()) { tgc_it->dump(stream); }
        stream << " -- CloseHits " << tgcCloseHits() << std::endl;
        for (const TgcHitPtr& tgc_it : tgcClose()) { tgc_it->dump(stream); }

        return stream;
    }  // end MuonCalibSegment::dump

    void MuonCalibSegment::refineMdtSelection(const std::vector<unsigned int>& new_selection) {
        if (new_selection.size() != m_mdtHitsOnTrack.size()) {
            MsgStream log(Athena::getMessageSvc(), "MuonCalibSegment");
            log << MSG::WARNING << "MuonCalibSegment::refineMdtSelection: Wrong size of vector!" << endmsg;
            return;
        }
        // copy old hit vector
        MdtHitVec old_hit_vec(m_mdtHitsOnTrack);
        m_mdtHitsOnTrack.clear();
        for (unsigned int i = 0; i < old_hit_vec.size(); i++) {
            if (!new_selection[i]) {
                m_mdtHitsOnTrack.emplace_back(old_hit_vec[i]);
            } else {
                m_mdtCloseHits.emplace_back(old_hit_vec[i]);
            }
        }
    }  // end MuonCalibSegment::refineMdtSelection

    bool MuonCalibSegment::hasFittedT0() const { return m_fittedT0 != s_dummy_ctor_dbl; }
    double MuonCalibSegment::fittedT0() const { return hasFittedT0() ? m_fittedT0 : 0; }

    void MuonCalibSegment::setSegment(const Amg::Vector3D& pos, const Amg::Vector3D& dir) {
        m_localPosition = pos;
        m_localDirection = dir;
    }
    void MuonCalibSegment::setErrors(double error_dy0, double error_dtheta) {
        m_dy0 = error_dy0;
        m_dtheta = error_dtheta;
    }
    void MuonCalibSegment::setAuthor(int author) { m_author = author; }
    void MuonCalibSegment::setFittedT0(double t0) { m_fittedT0 = t0; }

    int MuonCalibSegment::author() const { return m_author; }
    unsigned int MuonCalibSegment::qualityFlag() const { return m_qualityFlag; }

    void MuonCalibSegment::set(double chi2, const Amg::Vector3D& pos, const Amg::Vector3D& dir) {
        m_chi2 = chi2;
        m_localPosition = pos;
        m_localDirection = dir;
    }

    // member access
    unsigned int MuonCalibSegment::hitsOnTrack() const { return m_mdtHitsOnTrack.size() + m_cscHitsOnTrack.size(); }

    unsigned int MuonCalibSegment::closeHits() const { return m_mdtCloseHits.size(); }

    //  number of hits in segment
    unsigned int MuonCalibSegment::missedHits() const { return outOfTimeHits() + emptyTubes(); }
    unsigned int MuonCalibSegment::deltaHits() const { return m_qualityFlag % 10; }
    unsigned int MuonCalibSegment::emptyTubes() const { return (m_qualityFlag % 1000) / 100; }
    unsigned int MuonCalibSegment::outOfTimeHits() const { return (m_qualityFlag % 100) / 10; }

    // MDT specific
    unsigned int MuonCalibSegment::mdtHitsOnTrack() const { return m_mdtHitsOnTrack.size(); }
    const MuonCalibSegment::MdtHitVec& MuonCalibSegment::mdtHOT() const { return m_mdtHitsOnTrack; }
    MuonCalibSegment::MdtHitVec& MuonCalibSegment::mdtHOT() { return m_mdtHitsOnTrack; }

    unsigned int MuonCalibSegment::mdtCloseHits() const { return m_mdtCloseHits.size(); }
    const MuonCalibSegment::MdtHitVec& MuonCalibSegment::mdtClose() const { return m_mdtCloseHits; }
    MuonCalibSegment::MdtHitVec& MuonCalibSegment::mdtClose() { return m_mdtCloseHits; }

    // CSC specific
    unsigned int MuonCalibSegment::cscHitsOnTrack() const { return m_cscHitsOnTrack.size(); }
    const MuonCalibSegment::CscHitVec& MuonCalibSegment::cscHOT() const { return m_cscHitsOnTrack; }
    MuonCalibSegment::CscHitVec& MuonCalibSegment::cscHOT() { return m_cscHitsOnTrack; }

    unsigned int MuonCalibSegment::cscCloseHits() const { return m_cscCloseHits.size(); }
    const MuonCalibSegment::CscHitVec& MuonCalibSegment::cscClose() const { return m_cscCloseHits; }
    MuonCalibSegment::CscHitVec& MuonCalibSegment::cscClose() { return m_cscCloseHits; }

    // RPC specific
    unsigned int MuonCalibSegment::rpcHitsOnTrack() const { return m_rpcHitsOnTrack.size(); }
    const MuonCalibSegment::RpcHitVec& MuonCalibSegment::rpcHOT() const { return m_rpcHitsOnTrack; }
    MuonCalibSegment::RpcHitVec& MuonCalibSegment::rpcHOT() { return m_rpcHitsOnTrack; }

    unsigned int MuonCalibSegment::rpcCloseHits() const { return m_rpcCloseHits.size(); }
    const MuonCalibSegment::RpcHitVec& MuonCalibSegment::rpcClose() const { return m_rpcCloseHits; }
    MuonCalibSegment::RpcHitVec& MuonCalibSegment::rpcClose() { return m_rpcCloseHits; }

    // TGC specific
    unsigned int MuonCalibSegment::tgcHitsOnTrack() const { return m_tgcHitsOnTrack.size(); }
    const MuonCalibSegment::TgcHitVec& MuonCalibSegment::tgcHOT() const { return m_tgcHitsOnTrack; }
    MuonCalibSegment::TgcHitVec& MuonCalibSegment::tgcHOT() { return m_tgcHitsOnTrack; }

    unsigned int MuonCalibSegment::tgcCloseHits() const { return m_tgcCloseHits.size(); }
    const MuonCalibSegment::TgcHitVec& MuonCalibSegment::tgcClose() const { return m_tgcCloseHits; }
    MuonCalibSegment::TgcHitVec& MuonCalibSegment::tgcClose() { return m_tgcCloseHits; }

    // local track parameters
    double MuonCalibSegment::chi2() const { return m_chi2; }
    double MuonCalibSegment::error_dy0() const { return m_dy0; }
    double MuonCalibSegment::error_dtheta() const { return m_dtheta; }
    const Amg::Vector3D& MuonCalibSegment::position() const { return m_localPosition; }
    const Amg::Vector3D& MuonCalibSegment::direction() const { return m_localDirection; }
    const Amg::Transform3D& MuonCalibSegment::localToGlobal() const { return m_localToGlobal; }

    Amg::Vector3D MuonCalibSegment::globalPosition() const { return Amg::Vector3D(localToGlobal() * position()); }
    Amg::Vector3D MuonCalibSegment::globalDirection() const { return Amg::Vector3D(localToGlobal().linear() * direction()); }

    // methodes to add CalibHitBase's
    void MuonCalibSegment::addHitOnTrack(MdtCalibHitBase* hit) { m_mdtHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(MdtCalibHitBase* hit) { m_mdtCloseHits.emplace_back(hit); }

    void MuonCalibSegment::addHitOnTrack(CscCalibHitBase* hit) { m_cscHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(CscCalibHitBase* hit) { m_cscCloseHits.emplace_back(hit); }

    void MuonCalibSegment::addHitOnTrack(RpcCalibHitBase* hit) { m_rpcHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(RpcCalibHitBase* hit) { m_rpcCloseHits.emplace_back(hit); }

    void MuonCalibSegment::addHitOnTrack(TgcCalibHitBase* hit) { m_tgcHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(TgcCalibHitBase* hit) { m_tgcCloseHits.emplace_back(hit); }

    void MuonCalibSegment::addHitOnTrack(const MdtHitPtr& hit) { m_mdtHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(const MdtHitPtr& hit) { m_mdtCloseHits.emplace_back(hit); }

    void MuonCalibSegment::addHitOnTrack(const CscHitPtr& hit) { m_cscHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(const CscHitPtr& hit) { m_cscCloseHits.emplace_back(hit); }

    void MuonCalibSegment::addHitOnTrack(const RpcHitPtr& hit) { m_rpcHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(const RpcHitPtr& hit) { m_rpcCloseHits.emplace_back(hit); }

    void MuonCalibSegment::addHitOnTrack(const TgcHitPtr& hit) { m_tgcHitsOnTrack.emplace_back(hit); }
    void MuonCalibSegment::addCloseHit(const TgcHitPtr& hit) { m_tgcCloseHits.emplace_back(hit); }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibSegment& seg) { return seg.dump(stream); }

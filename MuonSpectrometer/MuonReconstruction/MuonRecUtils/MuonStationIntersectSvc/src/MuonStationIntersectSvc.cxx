/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"

#include <memory>

#include "MuonCondData/MdtCondDbData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonStationIntersectSvc/MdtIntersectGeometry.h"
#include "MuonStationIntersectSvc/MuonIntersectGeometry.h"

MuonStationIntersectSvc::MuonStationIntersectSvc(const std::string& name, ISvcLocator* sl) : AthService(name, sl) {}

// queryInterface
StatusCode MuonStationIntersectSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
    if (IID_IMuonStationIntersectSvc.versionMatch(riid)) {
        *ppvIF = (MuonStationIntersectSvc*)this;
    } else {
        return Service::queryInterface(riid, ppvIF);
    }
    return StatusCode::SUCCESS;
}

StatusCode MuonStationIntersectSvc::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
}

const std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> > MuonStationIntersectSvc::getStationGeometry(
    const Identifier& id, const MdtCondDbData* dbData, const MuonGM::MuonDetectorManager* detMgr) const {
    // get ids of geometry associated with identifier
    const std::vector<Identifier> chambers = binPlusneighbours(id);

    // vector to store result
    std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> > stations;

    // loop over bins, retrieve geometry
    for (const Identifier& chId : chambers) {
        if (dbData && !dbData->isGoodStation(chId)) {
            ATH_MSG_VERBOSE("chamber " << m_idHelperSvc->toString(chId) << " is dead");
            continue;
        }
        stations.emplace_back(std::make_unique<Muon::MdtIntersectGeometry>(chId, detMgr, dbData, &this->msgStream(), m_idHelperSvc.get()));
    }
    return stations;
}

const Muon::MuonStationIntersect MuonStationIntersectSvc::tubesCrossedByTrack(const Identifier& id, const Amg::Vector3D& pos,
                                                                              const Amg::Vector3D& dir, const MdtCondDbData* dbData,
                                                                              const MuonGM::MuonDetectorManager* detMgr) const {
    const std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> > stations = getStationGeometry(id, dbData, detMgr);

    ATH_MSG_DEBUG(" Calculating intersections for chamber " << m_idHelperSvc->toString(id) << " accounting for " << stations.size()
                                                            << " stations");

    Muon::MuonStationIntersect::TubeIntersects tubeIntersects;
    for (const std::unique_ptr<Muon::MdtIntersectGeometry>& it : stations) {
        const Muon::MuonStationIntersect intersect = it->intersection(pos, dir);
        tubeIntersects.insert(tubeIntersects.end(), intersect.tubeIntersects().begin(), intersect.tubeIntersects().end());
    }

    Muon::MuonStationIntersect intersection;
    intersection.setTubeIntersects(std::move(tubeIntersects));
    return intersection;
}

const std::vector<Identifier> MuonStationIntersectSvc::binPlusneighbours(const Identifier& id) const {
    std::vector<Identifier> chIds;
    int stName = m_idHelperSvc->mdtIdHelper().stationName(id);
    int stPhi = m_idHelperSvc->mdtIdHelper().stationPhi(id);
    int stEta = m_idHelperSvc->mdtIdHelper().stationEta(id);
    bool isBarrel = m_idHelperSvc->mdtIdHelper().isBarrel(id);
    int stEtaMin = m_idHelperSvc->mdtIdHelper().stationEtaMin(id);
    int stEtaMax = m_idHelperSvc->mdtIdHelper().stationEtaMax(id);

    int chEtaLeft = stEta - 1;
    int chEtaRight = stEta + 1;

    // chamber with smallest eta
    if (chEtaLeft < stEtaMin) chEtaLeft = -999;

    // chamber with largest eta
    if (chEtaRight > stEtaMax) chEtaRight = -999;

    Muon::MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(id);

    // special treatment of EOS chambers
    if (chIndex == Muon::MuonStationIndex::EOS) {
        chEtaRight = -999;
        chEtaLeft = -999;
    }

    if (isBarrel) {
        // eta = 0 doesn't exist take next
        if (chEtaLeft == 0) chEtaLeft -= 1;
        if (chEtaRight == 0) chEtaRight += 1;
    } else {
        // do not combined positive and negative endcaps
        if (chEtaLeft == 0) chEtaLeft = -999;
        if (chEtaRight == 0) chEtaRight = -999;
    }

    // no neighbours for BIS8
    if (chIndex == Muon::MuonStationIndex::BIS && std::abs(stEta) == 8) {
        chEtaLeft = -999;
        chEtaRight = -999;
    }

    // BIS 8 never neighbour of a chamber
    if (chIndex == Muon::MuonStationIndex::BIS) {
        if (std::abs(chEtaLeft) == 8) chEtaLeft = -999;
        if (std::abs(chEtaRight) == 8) chEtaRight = -999;
    }
    /// No CSC chambers -> No Mdt EI Station
    if ((chIndex == Muon::MuonStationIndex::EIS || chIndex == Muon::MuonStationIndex::EIL) && !m_idHelperSvc->hasCSC()) { return chIds; }
    if (chEtaLeft != -999 && m_idHelperSvc->mdtIdHelper().validElement(m_idHelperSvc->mdtIdHelper().elementID(stName, chEtaLeft, stPhi)))
        chIds.push_back(m_idHelperSvc->mdtIdHelper().elementID(stName, chEtaLeft, stPhi));
    chIds.push_back(m_idHelperSvc->mdtIdHelper().elementID(id));
    if (chEtaRight != -999 && m_idHelperSvc->mdtIdHelper().validElement(m_idHelperSvc->mdtIdHelper().elementID(stName, chEtaRight, stPhi)))
        chIds.push_back(m_idHelperSvc->mdtIdHelper().elementID(stName, chEtaRight, stPhi));

    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE(" returning chambers for id " << m_idHelperSvc->toString(id) << " ids " << chIds.size() << "   eta " << chEtaLeft
                                                      << "     " << stEta << "     " << chEtaRight << " chambers: ");
        for (unsigned int i = 0; i < chIds.size(); ++i) ATH_MSG_VERBOSE(m_idHelperSvc->toString(chIds[i]));
    }

    return chIds;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"

#include <sstream>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace {
    /// Lower threshold on the ADC
    constexpr int adc_lower_threshold = 70;

}  // namespace
namespace MuonCalib {

    // const std::vector<const MuonCalibExtendedTrack*>& MuonCalibExtendedSegment::associatedTracks() const { return m_associatedTracks; }

    /** add associated track */
    // void MuonCalibExtendedSegment::addTrack(const MuonCalibExtendedTrack* track) { m_associatedTracks.emplace_back(track); }

    MuonCalibExtendedSegment::MuonCalibExtendedSegment(const MuonCalibSegment& segment) : MuonCalibSegment(segment) {
        MuonFixedIdManipulator idManip;

        MuonCalibTrackSummary::ChamberHitSummary* currentChamberSummary = nullptr;

        for (const MdtHitPtr& mdt_hit : mdtHOT()) {
            const MuonFixedId& id = mdt_hit->identify();
            if (!id.isValid()) {
                MsgStream log(Athena::getMessageSvc(), "MuonCalibExtendedSegment");
                log << MSG::WARNING << "invalid MDT id!" << endmsg;
                continue;
            }

            m_idSet.insert(id);

            MuonFixedId chId = idManip.chamberIdentifier(id);

            bool measuresPhi = idManip.measuresPhi(id);
            bool isMdt = id.is_mdt();
            bool isFirst = isMdt ? id.mdtMultilayer() == 1 : !measuresPhi;

            // check whether first chamber or new chamber
            if (!currentChamberSummary || currentChamberSummary->chId != chId) {
                m_summary.chamberHitSummary.emplace_back(chId);
                currentChamberSummary = &m_summary.chamberHitSummary.back();
            }

            MuonCalibTrackSummary::ChamberHitSummary::Projection& proj =
                isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

            ++proj.nhits;

            ++m_summary.nhits;
            ++m_summary.nmdtHits;

            if (mdt_hit->adcCount() < adc_lower_threshold) ++m_summary.nmdtHitsBelowADCCut;

            MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);
            if (idManip.isEndcap(id)) {
                if (id.eta() < 0.)
                    m_summary.hasEndcapA = true;
                else
                    m_summary.hasEndcapC = true;
                if (stationIndex == MuonFixedIdManipulator::EIA || stationIndex == MuonFixedIdManipulator::EIC ||
                    stationIndex == MuonFixedIdManipulator::EMA || stationIndex == MuonFixedIdManipulator::EMC)
                    m_summary.hasEndcapLayersWithTGC = true;
            } else {
                m_summary.hasBarrel = true;
                if (stationIndex == MuonFixedIdManipulator::BM || stationIndex == MuonFixedIdManipulator::BO)
                    m_summary.hasBarrelLayersWithRPC = true;
            }

            m_summary.precisionStationLayers.insert(stationIndex);
            if (currentChamberSummary->nMdtHitsMl1() > 0 && currentChamberSummary->nMdtHitsMl2() && currentChamberSummary->nhits() > 3) {
                m_summary.goodPrecisionStationLayers.insert(stationIndex);
            }
        }
        for (const RpcHitPtr& rpc_hit : rpcHOT()) {
            MuonFixedId id = rpc_hit->identify();
            if (!id.isValid()) {
                MsgStream log(Athena::getMessageSvc(), "MuonCalibExtendedSegment");
                log << MSG::WARNING << "invalid RPC id!" << endmsg;
                continue;
            }

            m_idSet.insert(id);

            MuonFixedId chId = idManip.chamberIdentifier(id);

            bool measuresPhi = idManip.measuresPhi(id);
            bool isMdt = id.is_mdt();
            bool isFirst = isMdt ? id.mdtMultilayer() == 1 : measuresPhi;

            // check whether first chamber or new chamber
            if (!currentChamberSummary || currentChamberSummary->chId != chId) {
                m_summary.chamberHitSummary.emplace_back(chId);
                currentChamberSummary = &m_summary.chamberHitSummary.back();
            }

            MuonCalibTrackSummary::ChamberHitSummary::Projection& proj =
                isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

            ++proj.nhits;

            ++m_summary.nhits;
            if (measuresPhi)
                ++m_summary.nrpcPhiHits;
            else
                ++m_summary.nrpcEtaHits;

            MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
            if (measuresPhi) m_summary.phiStationLayers.insert(phiStationIndex);
            if (currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0) {
                m_summary.phiEtaStationLayers.insert(phiStationIndex);
            }
        }

        if (m_summary.nrpcEtaHits + m_summary.nrpcPhiHits > 0) {
            m_summary.hasBarrel = true;
            m_summary.hasBarrelLayersWithRPC = true;
        }

        for (const TgcHitPtr& tgc_hit : tgcHOT()) {
            const MuonFixedId& id = tgc_hit->identify();
            if (!id.isValid()) {
                MsgStream log(Athena::getMessageSvc(), "MuonCalibExtendedSegment");
                log << MSG::WARNING << "invalid TGC id!" << endmsg;
                continue;
            }

            m_idSet.insert(id);

            MuonFixedId chId = idManip.chamberIdentifier(id);

            bool measuresPhi = idManip.measuresPhi(id);
            bool isMdt = id.is_mdt();
            bool isFirst = isMdt ? id.mdtMultilayer() == 1 : measuresPhi;

            // check whether first chamber or new chamber
            if (!currentChamberSummary || currentChamberSummary->chId != chId) {
                m_summary.chamberHitSummary.push_back(MuonCalibTrackSummary::ChamberHitSummary(chId));
                currentChamberSummary = &m_summary.chamberHitSummary.back();
            }

            MuonCalibTrackSummary::ChamberHitSummary::Projection& proj =
                isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

            ++proj.nhits;

            ++m_summary.nhits;
            if (measuresPhi)
                ++m_summary.ntgcPhiHits;
            else
                ++m_summary.ntgcEtaHits;

            if (id.eta() < 0)
                m_summary.hasEndcapA = true;
            else
                m_summary.hasEndcapC = true;

            MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
            if (measuresPhi) m_summary.phiStationLayers.insert(phiStationIndex);
            if (currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0) {
                m_summary.phiEtaStationLayers.insert(phiStationIndex);
            }
        }

        for (const CscHitPtr& csc_hit : cscHOT()) {
            const MuonFixedId& id = csc_hit->identify();
            if (!id.isValid()) {
                MsgStream log(Athena::getMessageSvc(), "MuonCalibExtendedSegment");
                log << MSG::WARNING << "invalid CSC id!" << endmsg;
                continue;
            }

            m_idSet.insert(id);

            MuonFixedId chId = idManip.chamberIdentifier(id);

            bool measuresPhi = idManip.measuresPhi(id);
            bool isMdt = id.is_mdt();
            bool isFirst = isMdt ? id.mdtMultilayer() == 1 : measuresPhi;

            // check whether first chamber or new chamber
            if (!currentChamberSummary || currentChamberSummary->chId != chId) {
                m_summary.chamberHitSummary.emplace_back(chId);
                currentChamberSummary = &m_summary.chamberHitSummary.back();
            }

            MuonCalibTrackSummary::ChamberHitSummary::Projection& proj =
                isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

            ++proj.nhits;

            ++m_summary.nhits;
            if (measuresPhi)
                ++m_summary.ncscPhiHits;
            else
                ++m_summary.ncscEtaHits;

            MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
            if (measuresPhi) { m_summary.phiStationLayers.insert(phiStationIndex); }
            MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);
            m_summary.precisionStationLayers.insert(stationIndex);
            if (currentChamberSummary->netaHits() > 2 && currentChamberSummary->nphiHits() > 2) {
                m_summary.goodPrecisionStationLayers.insert(stationIndex);
                m_summary.phiEtaStationLayers.insert(phiStationIndex);
            }
        }
    }

    std::string MuonCalibExtendedSegment::dump() const {
        std::ostringstream sout;
        sout << dumpPars() << std::endl << dumpSummary();
        return sout.str();
    }

    std::string MuonCalibExtendedSegment::dumpPars() const {
        std::ostringstream sout;
        sout << "Extended Segment: " << std::setprecision(4) << "chi2 " << chi2() << " r " << (int)globalPosition().perp() << " z "
             << (int)globalPosition().z() << std::setprecision(5) << " phi " << globalDirection().phi() << " theta "
             << globalDirection().theta();  // << " nassociatedTracks " << m_associatedTracks.size();
        return sout.str();
    }

    std::string MuonCalibExtendedSegment::dumpSummary() const {
        std::ostringstream sout;
        sout << m_summary.dump();
        return sout.str();
    }

}  // namespace MuonCalib

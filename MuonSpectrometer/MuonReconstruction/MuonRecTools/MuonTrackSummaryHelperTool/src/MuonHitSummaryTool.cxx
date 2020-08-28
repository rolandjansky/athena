/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitSummaryTool.h"

#include <iostream>

#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrackSummary/TrackSummary.h"

namespace Muon {

MuonHitSummaryTool::MuonHitSummaryTool(const std::string& ty, const std::string& na, const IInterface* pa)
    : AthAlgTool(ty, na, pa),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_summaryHelperTool("Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool")
{
    declareInterface<IMuonHitSummaryTool>(this);
}

StatusCode
MuonHitSummaryTool::initialize()
{
    ATH_CHECK(m_idHelperSvc.retrieve());
    if (!m_summaryHelperTool.empty()) ATH_CHECK(m_summaryHelperTool.retrieve());
    if (!m_printer.empty()) ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    return StatusCode::SUCCESS;
}

void
MuonHitSummaryTool::getMuonTrackSummary(Trk::MuonTrackSummary& muonSummary, const Trk::Track& track) const
{

    // check whether helper tool set
    if (m_summaryHelperTool.empty()) return;

    Trk::TrackSummary tmpSummary;
    m_summaryHelperTool->addDetailedTrackSummary(track, tmpSummary);
    if (!tmpSummary.muonTrackSummary()) {
        ATH_MSG_WARNING("Could not create MuonTrackSummary, please enable the creation in " << m_summaryHelperTool);
        return;
    }
    muonSummary = *tmpSummary.muonTrackSummary();
}

IMuonHitSummaryTool::CompactSummary
MuonHitSummaryTool::summary(const Trk::Track& track) const
{

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(m_printer->print(track) << std::endl << m_printer->printStations(track));

    // check if the track already has a MuonTrackSummary, if so use it
    if (track.trackSummary() && track.trackSummary()->muonTrackSummary())
        return summary(*track.trackSummary()->muonTrackSummary());

    // calculate muon summary from track
    Trk::MuonTrackSummary muonSummary;
    getMuonTrackSummary(muonSummary, track);
    return summary(muonSummary);
}

IMuonHitSummaryTool::CompactSummary
MuonHitSummaryTool::summary(const Trk::TrackSummary& sum) const
{
    // check if the track summary has a MuonTrackSummary, if so use it
    if (sum.muonTrackSummary()) return summary(*sum.muonTrackSummary());
    return CompactSummary();
}

IMuonHitSummaryTool::CompactSummary
MuonHitSummaryTool::summary(const Trk::MuonTrackSummary& s) const
{

    CompactSummary sum;

    std::map<int, int> sectorLayerCounts;  // count hits per sector

    // loop over chambers
    std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit     = s.chamberHitSummary().begin();
    std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit_end = s.chamberHitSummary().end();
    for (; chit != chit_end; ++chit) {
        const Identifier& chId     = chit->chamberId();
        bool              isMdt    = m_idHelperSvc->isMdt(chId);
        bool              isCsc    = m_idHelperSvc->isCsc(chId);
        bool              isMM     = m_idHelperSvc->isMM(chId);
        bool              issTgc   = m_idHelperSvc->issTgc(chId);
        bool              isEIPrec = isCsc || isMM || issTgc;

        // only account for sectors if chamber has eta hits
        if ((isMdt && chit->nhits() > 0) || (isEIPrec && chit->netaHits() > 0)) {
            int sector = m_idHelperSvc->sector(chId);
            if (isMdt)
                sectorLayerCounts[sector] += chit->nhits();
            else
                sectorLayerCounts[sector] += chit->netaHits();
            sum.sectors.insert(sector);
        }

        // only account for phi layers if not an MDT and has phi hits
        if (!isMdt && chit->nphiHits() > 0) {
            MuonStationIndex::PhiIndex index = m_idHelperSvc->phiIndex(chId);
            sum.phiLayers.insert(index);
        }

        MuonStationIndex::StIndex index      = m_idHelperSvc->stationIndex(chId);
        HitSummary&               hitSummary = sum.stationLayers[index];
        hitSummary.isEndcap                  = m_idHelperSvc->isEndcap(chId);
        hitSummary.isSmall                   = m_idHelperSvc->isSmallChamber(chId);
        if (isMdt) {
            hitSummary.nprecisionHits += chit->nhits();
            hitSummary.nprecisionHoles += chit->nholes();
            hitSummary.nprecisionOutliers += chit->noutliers();
            hitSummary.nprecisionCloseHits += chit->ncloseHits();
            hitSummary.nprecisionGoodHits += chit->ngoodHits();
            hitSummary.noutBoundsHits += chit->noutBoundsHits();
        } else {
            if (isEIPrec) {
                hitSummary.nprecisionHits += chit->netaHits();
                hitSummary.nprecisionGoodHits += chit->netaHits();
                hitSummary.nprecisionHoles += chit->etaProjection().nholes;
                hitSummary.nprecisionOutliers += chit->etaProjection().noutliers;
                hitSummary.nprecisionCloseHits += chit->etaProjection().ncloseHits;
            } else {
                if (chit->netaHits() > 0) ++hitSummary.netaTriggerLayers;
            }
            if (chit->nphiHits() > 0) ++hitSummary.nphiLayers;
            if (chit->nphiHits() > 0 && chit->netaHits() > 0) ++hitSummary.netaPhiLayers;

            if (!isEIPrec && chit->etaProjection().nholes > 0 && chit->etaProjection().nhits == 0)
                ++hitSummary.netaTriggerHoleLayers;
            if (chit->phiProjection().nholes > 0 && chit->phiProjection().nhits == 0) ++hitSummary.nphiHoleLayers;
        }
    }

    int                          maxHits   = -1;
    int                          mainSec   = -1;
    std::map<int, int>::iterator secIt     = sectorLayerCounts.begin();
    std::map<int, int>::iterator secIt_end = sectorLayerCounts.end();
    for (; secIt != secIt_end; ++secIt) {
        if (secIt->second > maxHits) {
            maxHits = secIt->second;
            mainSec = secIt->first;
        }
    }
    sum.mainSector = mainSec;

    calculateSummaryCounts(sum);

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(sum.dump());

    return sum;
}

void
MuonHitSummaryTool::calculateSummaryCounts(IMuonHitSummaryTool::CompactSummary& sum) const
{
    sum.nprecisionLayers     = 0;
    sum.nprecisionGoodLayers = 0;
    sum.nprecisionHoleLayers = 0;
    sum.nphiLayers           = 0;
    sum.ntrigEtaLayers       = 0;
    sum.nphiHoleLayers       = 0;
    sum.ntrigEtaHoleLayers   = 0;

    std::map<MuonStationIndex::StIndex, Muon::IMuonHitSummaryTool::HitSummary>::const_iterator hsit =
        sum.stationLayers.begin();
    std::map<MuonStationIndex::StIndex, Muon::IMuonHitSummaryTool::HitSummary>::const_iterator hsit_end =
        sum.stationLayers.end();
    for (; hsit != hsit_end; ++hsit) {
        const Muon::IMuonHitSummaryTool::HitSummary& hitSummary = hsit->second;
        sum.ntrigEtaLayers += hitSummary.netaTriggerLayers;
        sum.nphiHoleLayers += hitSummary.nphiHoleLayers;
        sum.ntrigEtaHoleLayers += hitSummary.netaTriggerHoleLayers;
        if (hitSummary.nprecisionHits > 2) {
            ++sum.nprecisionLayers;
            if (hitSummary.nprecisionGoodHits > 2) {
                ++sum.nprecisionGoodLayers;
                if (hitSummary.isEndcap)
                    sum.isEndcap = true;
                else
                    sum.isEndcap = false;
                if (hitSummary.isSmall)
                    sum.isSmall = true;
                else
                    sum.isSmall = false;
            }
        } else if (hitSummary.nprecisionHoles > 2)
            ++sum.nprecisionHoleLayers;
    }
    sum.nphiLayers = sum.phiLayers.size();
}


IMuonHitSummaryTool::CompactSummary
MuonHitSummaryTool::summary(const Muon::MuonSegment& segment) const
{
    return summary(segment.containedMeasurements());
}

IMuonHitSummaryTool::CompactSummary
MuonHitSummaryTool::summary(const std::vector<const Muon::MuonSegment*>& segments) const
{
    std::vector<const Trk::MeasurementBase*>              rioVec;
    std::vector<const Muon::MuonSegment*>::const_iterator sit     = segments.begin();
    std::vector<const Muon::MuonSegment*>::const_iterator sit_end = segments.end();
    for (; sit != sit_end; ++sit)
        rioVec.insert(rioVec.end(), (*sit)->containedMeasurements().begin(), (*sit)->containedMeasurements().end());
    return summary(rioVec);
}

IMuonHitSummaryTool::CompactSummary
MuonHitSummaryTool::summary(const std::vector<const Trk::MeasurementBase*>& rioVec) const
{
    CompactSummary     sum;
    std::map<int, int> sectorLayerCounts;  // count hits per sector

    std::map<MuonStationIndex::StIndex, std::map<MuonStationIndex::PhiIndex, std::pair<int, int> > >
        countLayersPerStation;

    std::vector<const Trk::MeasurementBase*>::const_iterator it     = rioVec.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator it_end = rioVec.end();
    for (; it != it_end; ++it) {
        Identifier id = m_edmHelperSvc->getIdentifier(**it);
        if (!id.is_valid() || !m_idHelperSvc->isMuon(id)) continue;

        bool isMdt       = m_idHelperSvc->isMdt(id);
        bool isCsc       = m_idHelperSvc->isCsc(id);
        bool measuresPhi = m_idHelperSvc->measuresPhi(id);

        // only account for sectors if chamber has eta hits
        if (isMdt || (isCsc && !measuresPhi)) {
            int sector = m_idHelperSvc->sector(id);
            if (isMdt) ++sectorLayerCounts[sector];
            if (isCsc) ++sectorLayerCounts[sector];
            sum.sectors.insert(sector);
        }

        // only account for phi layers if not an MDT and has phi hits
        if (!isMdt && measuresPhi) {
            MuonStationIndex::PhiIndex index = m_idHelperSvc->phiIndex(id);
            sum.phiLayers.insert(index);
        }

        MuonStationIndex::StIndex index      = m_idHelperSvc->stationIndex(id);
        HitSummary&               hitSummary = sum.stationLayers[index];
        if (isMdt || (isCsc && !measuresPhi)) ++hitSummary.nprecisionHits;

        if (!isMdt) {
            MuonStationIndex::PhiIndex pindex      = m_idHelperSvc->phiIndex(id);
            std::pair<int, int>        etaPhiCount = countLayersPerStation[index][pindex];
            if (measuresPhi)
                ++etaPhiCount.first;
            else
                ++etaPhiCount.second;
        }
    }

    int                          maxHits   = -1;
    int                          mainSec   = -1;
    std::map<int, int>::iterator secIt     = sectorLayerCounts.begin();
    std::map<int, int>::iterator secIt_end = sectorLayerCounts.end();
    for (; secIt != secIt_end; ++secIt) {
        if (secIt->second > maxHits) {
            maxHits = secIt->second;
            mainSec = secIt->first;
        }
    }
    sum.mainSector = mainSec;

    std::map<MuonStationIndex::StIndex, std::map<MuonStationIndex::PhiIndex, std::pair<int, int> > >::iterator sit =
        countLayersPerStation.begin();
    std::map<MuonStationIndex::StIndex, std::map<MuonStationIndex::PhiIndex, std::pair<int, int> > >::iterator sit_end =
        countLayersPerStation.end();
    for (; sit != sit_end; ++sit) {
        HitSummary& hitSummary = sum.stationLayers[sit->first];

        std::map<MuonStationIndex::PhiIndex, std::pair<int, int> >::iterator pit     = sit->second.begin();
        std::map<MuonStationIndex::PhiIndex, std::pair<int, int> >::iterator pit_end = sit->second.end();
        for (; pit != pit_end; ++pit) {
            if (pit->second.first != 0) ++hitSummary.nphiLayers;
            if (pit->second.second != 0 && pit->first != MuonStationIndex::CSC) ++hitSummary.netaTriggerLayers;
            if (pit->second.second != 0 && pit->second.first != 0) ++hitSummary.netaPhiLayers;
        }
    }
    calculateSummaryCounts(sum);
    return sum;
}

}  // namespace Muon

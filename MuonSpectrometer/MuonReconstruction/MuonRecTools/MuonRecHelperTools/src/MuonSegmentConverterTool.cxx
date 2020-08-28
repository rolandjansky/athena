/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentConverterTool.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Muon {

MuonSegmentConverterTool::MuonSegmentConverterTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t, n, p)
{
    declareInterface<xAODMaker::IMuonSegmentConverterTool>(this);
}

StatusCode
MuonSegmentConverterTool::initialize()
{
    ATH_CHECK(m_hitSummaryTool.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
    ATH_CHECK(m_hitTimingTool.retrieve());

    return StatusCode::SUCCESS;
}

xAOD::MuonSegment*
MuonSegmentConverterTool::convert(const ElementLink< ::Trk::SegmentCollection>& segLink,
                                  xAOD::MuonSegmentContainer*                   container) const
{
    // sanity checks
    if (!segLink.isValid() || !*segLink) {
        ATH_MSG_WARNING(" Got invalid element link");
        return 0;
    }
    const MuonSegment* seg = dynamic_cast<const MuonSegment*>(*segLink);
    if (!seg) {
        ATH_MSG_WARNING(" Trk::Segment is not a MuonSegment ");
        return 0;
    }

    // create xAOD::Muon and set link
    xAOD::MuonSegment* xaodSeg = convert(*seg, container);
    if (xaodSeg) xaodSeg->setMuonSegment(segLink);
    return xaodSeg;
}


void
MuonSegmentConverterTool::addClusterTiming(const MuonSegment& seg, xAOD::MuonSegment& xaodSeg) const
{

    // loop over hits and extract clusters
    std::vector<const MuonClusterOnTrack*>                   clusters;
    std::vector<const Trk::MeasurementBase*>::const_iterator mit     = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg.containedMeasurements().end();
    for (; mit != mit_end; ++mit) {

        // get Identifier and remove MDT hits
        Identifier id = m_edmHelper->getIdentifier(**mit);
        if (!id.is_valid() || !m_idHelperSvc->isTrigger(id)) continue;

        // cast to  MuonClusterOnTrack
        const MuonClusterOnTrack* clus = dynamic_cast<const MuonClusterOnTrack*>(*mit);
        if (clus)
            clusters.push_back(clus);
        else {
            const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*mit);
            if (!crot || crot->containedROTs().empty()) continue;
            clusters.insert(clusters.end(), crot->containedROTs().begin(), crot->containedROTs().end());
        }
    }

    // call timing tool and dress xaodSeg
    IMuonHitTimingTool::TimingResult result = m_hitTimingTool->calculateTimingResult(clusters);
    if (std::fabs(result.time) > std::numeric_limits<float>::max()
        || std::fabs(result.error) > std::numeric_limits<float>::max())
    {
        // xAOD stores this as a float. To avoid FPE, we need to check here...
        if (result.valid)
            ATH_MSG_WARNING("Unphysical time returned by tool - ignoring. result.valid = "
                            + std::to_string(result.valid));
        xaodSeg.auxdata<float>("clusterTime")      = std::numeric_limits<float>::max();
        xaodSeg.auxdata<float>("clusterTimeError") = std::numeric_limits<float>::max();
        xaodSeg.auxdata<int>("clusterTimeValid")   = 0;
    } else {
        xaodSeg.auxdata<float>("clusterTime")      = result.time;
        xaodSeg.auxdata<float>("clusterTimeError") = result.error;
        xaodSeg.auxdata<int>("clusterTimeValid")   = result.valid;
    }
}


xAOD::MuonSegment*
MuonSegmentConverterTool::convert(const MuonSegment& seg, xAOD::MuonSegmentContainer* container) const
{

    // create xAOD::MuonSegment
    xAOD::MuonSegment* xaodSeg = new xAOD::MuonSegment();
    if (container)
        container->push_back(xaodSeg);
    else
        xaodSeg->makePrivateStore();

    // set position and direction
    xaodSeg->setPosition(seg.globalPosition().x(), seg.globalPosition().y(), seg.globalPosition().z());
    xaodSeg->setDirection(seg.globalDirection().x(), seg.globalDirection().y(), seg.globalDirection().z());

    // fit chi2
    const Trk::FitQuality* fq = seg.fitQuality();
    if (fq) xaodSeg->setFitQuality(fq->chiSquared(), fq->numberDoF());

    // identifier
    Identifier                        id         = m_edmHelper->chamberId(seg);
    int                               eta        = m_idHelperSvc->stationEta(id);
    int                               sector     = m_idHelperSvc->sector(id);
    MuonStationIndex::ChIndex         chIndex    = m_idHelperSvc->chamberIndex(id);
    MuonStationIndex::TechnologyIndex technology = m_idHelperSvc->technologyIndex(id);
    xaodSeg->setIdentifier(sector, chIndex, eta, technology);

    // hit counts
    IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(seg);
    xaodSeg->setNHits(hitCounts.nmdtHitsMl1 + hitCounts.nmdtHitsMl2 + hitCounts.ncscHitsEta,
                      hitCounts.nphiTrigHitLayers, hitCounts.netaTrigHitLayers);

    // MDT + cluster timing
    if (seg.hasFittedT0()) xaodSeg->setT0Error(seg.time(), seg.errorTime());
    if (!m_hitTimingTool.empty()) addClusterTiming(seg, *xaodSeg);

    return xaodSeg;
}


}  // namespace Muon

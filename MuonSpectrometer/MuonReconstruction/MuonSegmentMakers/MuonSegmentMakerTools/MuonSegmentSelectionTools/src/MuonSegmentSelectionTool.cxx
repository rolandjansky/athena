/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentSelectionTool.h"

#include <map>

#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"

namespace Muon {

MuonSegmentSelectionTool::MuonSegmentSelectionTool(const std::string& ty, const std::string& na, const IInterface* pa)
    : AthAlgTool(ty, na, pa) {
    declareInterface<IMuonSegmentSelectionTool>(this);    
}

StatusCode
MuonSegmentSelectionTool::initialize()
{
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_hitSummaryTool.retrieve());
    return StatusCode::SUCCESS;
}

int
MuonSegmentSelectionTool::quality(const MuonSegment& seg, bool ignoreHoles, bool useEta, bool usePhi) const
{
    Identifier chid = m_edmHelperSvc->chamberId(seg);
    if (!chid.is_valid()) {
        ATH_MSG_WARNING("Got invalid segment identifier");
        return -1;
    }

    // different treatment for CSC and MDT segments
    // mdt treatment
    if (m_idHelperSvc->isMdt(chid)) return mdtSegmentQuality(seg, ignoreHoles);

    // csc segments
    if (m_idHelperSvc->isCsc(chid)) return cscSegmentQuality(seg, useEta, usePhi);

    // rpc/tgc case
    if (m_idHelperSvc->isTgc(chid) || m_idHelperSvc->isRpc(chid)) return 1;

    // NSW segments
    return nswSegmentQuality(seg, chid, ignoreHoles);
}

int
MuonSegmentSelectionTool::cscSegmentQuality(const MuonSegment& seg, bool useEta, bool usePhi) const
{

    // get hit counts
    IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(seg);

    /**********************************/
    /*       cuts for quality level 0 */

    // remove CSC segments with only 3  hits
    // unless either eta or phi are broken, in that case we accept the segment
    if (hitCounts.ncscHits() < 4 && useEta && usePhi) return -1;


    /**********************************/
    /*       cuts for quality level 1 */

    // remove CSC segments with no layer with 4 hits
    if (hitCounts.ncscHits.nphiHits != 4 && hitCounts.ncscHits.netaHits != 4) return 1;


    /**********************************/
    /*      cuts for quality level 2  */

    // require hits in both projections
    if (!hitCounts.ncscHits.hasEtaAndPhi()) return 1;


    /**********************************/
    /*      cuts for quality level 3  */

    // require four hits in one of the projections and hits in the other
    if ((hitCounts.ncscHits.netaHits == 4 && hitCounts.ncscHits.nphiHits != 4)
        || (hitCounts.ncscHits.netaHits != 4 && hitCounts.ncscHits.nphiHits == 4))
        return 2;


    /**********************************/
    /*   segment has highest quality  */
    return 3;
}

int
MuonSegmentSelectionTool::nswSegmentQuality(const MuonSegment& seg, const Identifier& /*chid*/, bool /*ignoreHoles*/) const {

    
    // get hit counts
    IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(seg);
    const uint8_t nPrecHits = hitCounts.nnswHits();
    /// For the moment count the micromega hits to precision eta hits
    const uint8_t nPrecEtaHits = hitCounts.nmmEtaHits + hitCounts.nstgcHits.netaHits;
    const uint8_t nPrecPhiHits =  nPrecHits - nPrecEtaHits;

    const uint8_t nMM_Hits = hitCounts.nmmHits();
    const uint8_t nSTGC_Hits = hitCounts.nstgcHits();
    /**********************************/
    /*       cuts for quality level 0 */
    // remove NSW segments with only 3  hits
    if (nPrecHits < 4) return -1;

    /**********************************/
    /*       cuts for quality level 1 */

    // remove NSW segments where both chambers have below than 4 associated hits
    if (std::max(nMM_Hits, nSTGC_Hits) < 4) return 0;

    /**********************************/
    /*      cuts for quality level 2  */
    // require hits in both projections
    if (nSTGC_Hits && !hitCounts.nstgcHits.hasEtaAndPhi()) return 1;

    if (nMM_Hits && (!hitCounts.nmmEtaHits || !hitCounts.nmmStereoHits)) return 1;

    /**********************************/
    /*      cuts for quality level 3  */

    // require four hits in one of the projections and hits in the other
    if (nPrecHits < 12 || nPrecPhiHits < 4)
        return 2;

    /**********************************/
    /*   segment has highest quality  */
    return 3;
}

int
MuonSegmentSelectionTool::mdtSegmentQuality(const MuonSegment& seg, bool ignoreHoles) const
{


    // get hit counts
    IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(seg);

    /**********************************/
    /*       cuts for quality level 0 */

    // remove segments with less than 3 mdt hits
    if (hitCounts.nmdtHits() < 3) return -1;

    // get holes
    unsigned int nholes = !ignoreHoles ? hitCounts.nmdtHoles : 0;

    // calculate hole fraction
    double holeFraction = (double)nholes / (double)hitCounts.nmdtHits();

    // check whether we expect trigger hits in the station
    bool triggerHitRegion = hitCounts.nexpectedTrigHitLayers > 0;

    ATH_MSG_DEBUG("Segment: " << m_printer->print(seg) << std::endl
                              << hitCounts.print() << "  Hole frac " << holeFraction);

    // only look at segments with phi hits
    if (triggerHitRegion && hitCounts.nphiTrigHitLayers == 0) {

        // reject if more holes than hits
        if (holeFraction > 1.1) return -1;
    }

    // reject events with a good ADC fraction below the cut
    if (hitCounts.goodADCFraction() < m_adcFractionCut) return -1;
    if (hitCounts.nmdtHits() == 3 && hitCounts.adcMax < m_minAdcPerSegmentCut) return -1;

    /**********************************/
    /*       cuts for quality level 1 */

    if (!hitCounts.closeToChamberEdge) {

        // reject segments with phi hits and more holes as hits
        if (hitCounts.nphiTrigHitLayers != 0 && holeFraction > 1.1) return 0;

        //       // reject segments with phi hits and a hole fraction larger than 50%
        //       if( triggerHitRegion && hitCounts.nphiTrigHitLayers == 0 && holeFraction > 0.5 ) return 0;
    }

    // reject segments with more than one enclosed hole and no trigger hits
    if (hitCounts.nphiTrigHitLayers == 0 && hitCounts.nmdtEnclosedHoles > 2) return 0;


    /**********************************/
    /*      cuts for quality level 2  */

    // reject segments with phi hits and more holes as hits
    if (hitCounts.nphiTrigHitLayers != 0 && holeFraction > 1.1) return 1;

    // reject segments with phi hits and a hole fraction larger than 50%
    if (triggerHitRegion && hitCounts.nphiTrigHitLayers == 0 && holeFraction > 0.5) return 1;

    // reject segments with more than one enclosed hole
    if (hitCounts.nmdtEnclosedHoles > 2) return 0;

    // reject all segments that have no phi hits and hits in only one multi layer
    bool twoMultiLayerSegment = hitCounts.nmdtHitsMl1 > 1 && hitCounts.nmdtHitsMl2 > 1;
    if (hitCounts.nphiTrigHitLayers == 0 && !twoMultiLayerSegment) return 1;


    if (hitCounts.nphiTrigHitLayers == 0) {

        // cut on segment quality
        if (hitCounts.segmentQuality > m_cutSegmentQuality) return 1;

        // tighter cut for segments with only on  multi layer
        if ((!twoMultiLayerSegment || hitCounts.nmdtHits() < 5) && hitCounts.segmentQuality > 0.5 * m_cutSegmentQuality)
            return 1;
    }

    // cut on number of holes
    if (hitCounts.nmdtHoles > 2) {
        ATH_MSG_DEBUG("Failed hole cut " << hitCounts.nmdtHoles);
        return 1;
    }

    /**********************************/
    /*      cuts for quality level 3  */

    // cut on segment quality
    if (hitCounts.segmentQuality > m_cutSegmentQuality) {
        ATH_MSG_DEBUG("Failed quality cut" << hitCounts.segmentQuality << "   cut " << m_cutSegmentQuality);
        return 2;
    }
    // tighter cut for segments with only on  multi layer
    if ((!twoMultiLayerSegment || hitCounts.nmdtHits() < 5) && hitCounts.segmentQuality > 0.5 * m_cutSegmentQuality) {
        ATH_MSG_DEBUG("Single multi layer or too few hits: quality " << hitCounts.segmentQuality << "   cut "
                                                                     << 0.5 * m_cutSegmentQuality);
        return 2;
    }

    // reject all segments that have in only one multi layer
    if (!twoMultiLayerSegment) {
        ATH_MSG_DEBUG("Single multi layer");
        return 2;
    }

    // reject segments without missing trigger layers in regions where we expect phi hits
    if (triggerHitRegion && hitCounts.nphiTrigHitLayers < hitCounts.nexpectedTrigHitLayers) {
        ATH_MSG_DEBUG("Missing trigger hits: phi hits " << hitCounts.nphiTrigHitLayers << " expected "
                                                        << hitCounts.nexpectedTrigHitLayers);
        return 2;
    }

    /**********************************/
    /*   segment has highest quality  */
    return 3;
}

bool
MuonSegmentSelectionTool::select(const MuonSegment& seg, bool ignoreHoles, int qualityLevel, bool useEta,
                                 bool usePhi) const
{
    // check quality is better or equal to required level
    return quality(seg, ignoreHoles, useEta, usePhi) >= qualityLevel;
}

}  // namespace Muon

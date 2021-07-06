/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Csc4dSegmentMaker.h"

#include <sstream>

#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscStripFitter.h"
#include "CscSegmentMakers/ICscSegmentUtilTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkRoad/TrackRoad.h"
#include "TrkSegment/Segment.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"

using Muon::CscPrepData;
using Muon::CscPrepDataCollection;
using Muon::CscPrepDataContainer;

using MuonGM::CscReadoutElement;

using Muon::CscClusterOnTrack;
using Muon::MuonSegment;
using Muon::MuonSegmentCombination;

using Muon::MdtDriftCircleOnTrack;
using Muon::MuonClusterOnTrack;

//******************************************************************************

Csc4dSegmentMaker::Csc4dSegmentMaker(const std::string& type, const std::string& aname, const IInterface* parent) :
    AthAlgTool(type, aname, parent) {
    declareInterface<ICscSegmentFinder>(this);
}

//******************************************************************************

// Destructor.

Csc4dSegmentMaker::~Csc4dSegmentMaker() = default;

//******************************************************************************

StatusCode Csc4dSegmentMaker::initialize() {
    ATH_MSG_DEBUG("Initializing");

    // Show keys.
    ATH_MSG_DEBUG("  Max chi-square: " << m_max_chisquare);
    ATH_MSG_DEBUG("  Max r slope: " << m_max_slope_r);
    ATH_MSG_DEBUG("  Max phi slope: " << m_max_slope_phi);
    ATH_MSG_DEBUG("  Max segments/chamber: " << m_max_seg_per_chamber);
    ATH_MSG_DEBUG("  SegmentTool: " << m_segmentTool.typeAndName());

    ATH_CHECK(m_segmentTool.retrieve());
    ATH_CHECK(m_printer.retrieve());

    return StatusCode::SUCCESS;
}

//******************************************************************************

std::unique_ptr<MuonSegmentCombinationCollection> Csc4dSegmentMaker::find(const MuonSegmentCombinationCollection& segcols,
                                                                          const EventContext& ctx) const {
    // Set dump flag.

    std::unique_ptr<MuonSegmentCombinationCollection> pcols = std::make_unique<MuonSegmentCombinationCollection>();
    if (segcols.empty()) { return nullptr; }

    for (MuonSegmentCombinationCollection::const_iterator icom = segcols.begin(); icom != segcols.end(); ++icom) {
        const MuonSegmentCombination& insegs = **icom;

        int nsta = insegs.numberOfStations();
        if (nsta != 2) {  // This can be happened if segment is not made at all in case of two hit layers !!
            // NOW, 2d segment is not made....
            //      ATH_MSG_FATAL ( "  Combination has " << nsta << " stations where 2 are expected" );
            return pcols;
        }

        ATH_MSG_DEBUG("Csc4dSegmentMaker called get4dMuonSegmentCombination");

        MuonSegmentCombination* pcol = m_segmentTool->get4dMuonSegmentCombination(&insegs, ctx);
        if (pcol) {
            pcols->push_back(pcol);
            ATH_MSG_DEBUG("Found 4d CSC segment " << std::endl << m_printer->print(*pcol));
        }
    }

    ATH_MSG_DEBUG("    Created " << pcols->size() << " 4D segments.");
    return pcols;
}

// dummy ICscSegmentFinder interface
std::unique_ptr<MuonSegmentCombinationCollection> Csc4dSegmentMaker::find(const std::vector<const Muon::CscPrepDataCollection*>&,
                                                                          const EventContext&) const {
    return nullptr;
}

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

/*namespace {
  std::string station_name(int station) {
    if ( station == 1 ) return "CSS";
    if ( station == 2 ) return "CSL";
    return "UNKNOWN_STATION";
  }
  std::string measphi_name(bool measphi) {
    if ( measphi ) return "phi";
    return "r";
  }
}*/

//******************************************************************************

Csc4dSegmentMaker::Csc4dSegmentMaker(const std::string& type, const std::string& aname, const IInterface* parent)
    : AthAlgTool(type, aname, parent), m_dumped(0), m_dump(false)
{

    declareInterface<ICscSegmentFinder>(this);
    declareProperty("dump_count", m_dumpcount = -1);
    declareProperty("max_chisquare", m_max_chisquare = 25);
    declareProperty("max_slope_r", m_max_slope_r = 0.2);
    declareProperty("max_slope_phi", m_max_slope_phi = 0.2);
    declareProperty("max_seg_per_chamber", m_max_seg_per_chamber = 50);
}

//******************************************************************************

// Destructor.

Csc4dSegmentMaker::~Csc4dSegmentMaker() {}

//******************************************************************************

StatusCode
Csc4dSegmentMaker::initialize()
{
    ATH_MSG_DEBUG("Initializing");

    // Show keys.
    ATH_MSG_DEBUG("  Dump count: " << m_dumpcount);
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

std::unique_ptr<MuonSegmentCombinationCollection>
Csc4dSegmentMaker::find(const MuonSegmentCombinationCollection& segcols) const
{

    // Set dump flag.
    m_dump = m_dumped < m_dumpcount || m_dumpcount < 0;
    if (m_dump) ++m_dumped;

    std::unique_ptr<MuonSegmentCombinationCollection> pcols(new MuonSegmentCombinationCollection);
    if (segcols.empty()) {
        pcols.reset();
        return pcols;
    }

    for (MuonSegmentCombinationCollection::const_iterator icom = segcols.begin(); icom != segcols.end(); ++icom) {
        const MuonSegmentCombination& insegs = **icom;

        int nsta = insegs.numberOfStations();
        if (nsta != 2) {  // This can be happened if segment is not made at all in case of two hit layers !!
            // NOW, 2d segment is not made....
            //      ATH_MSG_FATAL ( "  Combination has " << nsta << " stations where 2 are expected" );
            return pcols;
        }

        ATH_MSG_DEBUG("Csc4dSegmentMaker called get4dMuonSegmentCombination");

        MuonSegmentCombination* pcol = m_segmentTool->get4dMuonSegmentCombination(&insegs);
        if (pcol) {
            pcols->push_back(pcol);
            ATH_MSG_DEBUG("Found 4d CSC segment " << std::endl << m_printer->print(*pcol));
        }
    }

    ATH_MSG_DEBUG("    Created " << pcols->size() << " 4D segments.");
    return pcols;
}

//******************************************************************************

StatusCode
Csc4dSegmentMaker::finalize()
{
    ATH_MSG_DEBUG("Goodbye");
    return StatusCode::SUCCESS;
}

//******************************************************************************

// dummy ICscSegmentFinder interface
std::unique_ptr<MuonSegmentCombinationCollection>
Csc4dSegmentMaker::find(const std::vector<const Muon::CscPrepDataCollection*>&) const
{
    return 0;
}

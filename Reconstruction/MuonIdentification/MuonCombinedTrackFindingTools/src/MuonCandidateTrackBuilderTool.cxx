/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCandidateTrackBuilderTool.h"

#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"

#include "MuonLayerEvent/MuonCandidate.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "MuonSegment/MuonSegment.h"

namespace Muon {

  MuonCandidateTrackBuilderTool::MuonCandidateTrackBuilderTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_muonTrackBuilder("Muon::MooTrackBuilder/MooMuonTrackBuilder"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_trackFitter("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder")
  {
    declareInterface<IMuonCandidateTrackBuilderTool>(this);

    declareProperty("MuonSegmentTrackBuilder",    m_muonTrackBuilder );
    declareProperty("MuonEDMPrinterTool",         m_printer );    
    declareProperty("MuonTrackBuilder",           m_trackFitter );    
  }

  MuonCandidateTrackBuilderTool::~MuonCandidateTrackBuilderTool() { }

  StatusCode MuonCandidateTrackBuilderTool::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCandidateTrackBuilderTool::initialize() {

    ATH_CHECK(m_muonTrackBuilder.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_trackFitter.retrieve());

    return StatusCode::SUCCESS;
  }

  const Trk::Track* MuonCandidateTrackBuilderTool::buildCombinedTrack( const Trk::Track& idTrack, const MuonCandidate& candidate ) const {

    
    ATH_MSG_DEBUG("Building track from candidate with " << candidate.layerIntersections.size() << " layers ");
    // copy and sort layerIntersections according to their distance to the IP
    std::vector<MuonLayerIntersection> layerIntersections = candidate.layerIntersections;
    std::stable_sort(layerIntersections.begin(),layerIntersections.end(),[](const MuonLayerIntersection& lay1,const MuonLayerIntersection& lay2) {
        auto getDistance = []( const MuonLayerIntersection& layerIntersection ) {
          if( layerIntersection.intersection.trackParameters.get() == nullptr ) return 1e9;
          return layerIntersection.intersection.trackParameters->position().mag();
        };
        return getDistance(lay1) < getDistance(lay2);
      });

    // loop over sorted layers and extract measurements
    std::vector<const Trk::MeasurementBase*> measurements;
    for( const auto& layerIntersection : layerIntersections ){
      measurements.insert( measurements.end(),
                           layerIntersection.segment->containedMeasurements().begin(),
                           layerIntersection.segment->containedMeasurements().end() );
    }

    ATH_MSG_DEBUG("Extracted hits from candidate: " << measurements.size() );
    Trk::Track* refittedTrack = m_trackFitter->indetExtension(idTrack,measurements);
    if( refittedTrack ){
      ATH_MSG_DEBUG("got Track: " << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack) );
      
    }
    return refittedTrack;
  }


}
 

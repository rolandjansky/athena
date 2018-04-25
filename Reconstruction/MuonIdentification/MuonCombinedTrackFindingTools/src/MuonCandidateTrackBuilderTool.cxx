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
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "Identifier/Identifier.h"

namespace Muon {

  MuonCandidateTrackBuilderTool::MuonCandidateTrackBuilderTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_muonTrackBuilder("Muon::MooTrackBuilder/MooMuonTrackBuilder"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_reOrderMeasurements(true),
    m_trackFitter("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder")
  {
    declareInterface<IMuonCandidateTrackBuilderTool>(this);

    declareProperty("MuonSegmentTrackBuilder",    m_muonTrackBuilder );
    declareProperty("MuonEDMPrinterTool",         m_printer );    
    declareProperty("ReOrderMeasurements",        m_reOrderMeasurements);    
    declareProperty("MuonTrackBuilder",           m_trackFitter );    
  }

  MuonCandidateTrackBuilderTool::~MuonCandidateTrackBuilderTool() { }

  StatusCode MuonCandidateTrackBuilderTool::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCandidateTrackBuilderTool::initialize() {

    ATH_CHECK(m_muonTrackBuilder.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
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
    int intersec = 0;
    bool isEndcap = false;
    bool isBarrel = false;
    bool isSmall  = false;
    bool isLarge  = false;
    for( const auto& layerIntersection : layerIntersections ){
      intersec++;
      ATH_MSG_VERBOSE(" layerIntersection " << intersec << " perp " <<  layerIntersection.intersection.trackParameters->position().perp() << " z " << layerIntersection.intersection.trackParameters->position().z() << " distance to IP " << layerIntersection.intersection.trackParameters->position().mag());  
      ATH_MSG_VERBOSE(" segment surface center perp " <<  layerIntersection.segment->associatedSurface().center().perp() << " z " << layerIntersection.segment->associatedSurface().center().z() << " nr of msts " << layerIntersection.segment->containedMeasurements().size() );
      ATH_MSG_VERBOSE( m_printer->print(*(layerIntersection.segment))) ;

// Fix problem with segments where measurements are not ordered wrt IP 

// first check whether it is a Barrel or Endcap segment

      std::vector<const Trk::MeasurementBase*> containedMeasurements = layerIntersection.segment->containedMeasurements();
      std::vector<const Trk::MeasurementBase*>::const_iterator mit     = containedMeasurements.begin();
      std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = containedMeasurements.end();
      for(; mit!=mit_end; ++mit) {
     // get Identifier
        Identifier id;
        const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(*mit);
        if (rio)  id=rio->identify();
        else {
          const Trk::CompetingRIOsOnTrack* crio=dynamic_cast<const Trk::CompetingRIOsOnTrack*>(*mit);
          if (crio) id=crio->rioOnTrack(0).identify();
         else continue;
        }
     // check if valid ID
       if( !id.is_valid() ) continue;
     // check if muon
        if (!m_idHelper->isMuon(id)) continue;
        
        if(m_idHelper->isEndcap(id)) { 
         isEndcap = true;
        } else {
         isBarrel = true;
        }
        if(m_idHelper->isTrigger(id)) continue; 

        if(m_idHelper->isSmallChamber(id)) { 
         isSmall = true;
        } else {
         isLarge = true;
        }
        break;
      } 
       
      if(m_reOrderMeasurements) {
	// reorder measurements using SortMeas (defined in header file)
	std::stable_sort(containedMeasurements.begin(),containedMeasurements.end(),SortMeas(&*m_edmHelper,&*m_idHelper,isEndcap));
      }
      // insert in measurements list
      measurements.insert( measurements.end(),
			   containedMeasurements.begin(),
			   containedMeasurements.end() );
    }

    // reorder in case of Small Large overlaps in Barrel or Endcap ONLY   

    bool reorderAllMeasurements = false;
    if(isSmall&&isLarge) reorderAllMeasurements = true;
    if(isEndcap&&isBarrel) reorderAllMeasurements = false;

    if(m_reOrderMeasurements&&reorderAllMeasurements) {
      // reorder measurements using SortMeas (defined in header file)
      ATH_MSG_VERBOSE(" reorder all measurements before " <<  m_printer->print(measurements)) ;
      std::stable_sort(measurements.begin(),measurements.end(),SortMeas(&*m_edmHelper,&*m_idHelper,isEndcap));
    }

    ATH_MSG_VERBOSE( m_printer->print(measurements)) ;

    ATH_MSG_DEBUG("Extracted hits from candidate: " << measurements.size() );
    Trk::Track* refittedTrack = m_trackFitter->indetExtension(idTrack,measurements);
    if( refittedTrack ){
      ATH_MSG_DEBUG("got Track: " << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack) );
    }
    return refittedTrack;
  }


}
 

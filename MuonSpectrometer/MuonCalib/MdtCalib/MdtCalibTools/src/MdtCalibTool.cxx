/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibTools/MdtCalibTool.h"

// gaudi
#include "GaudiKernel/MsgStream.h"

// calibration 
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MdtCalibITools/IMdtCalibTool.h"
#include "MdtCalibITools/IMdtSegmentSelectorTool.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MdtCalibInterfaces/IMdtSegmentSelector.h"

#include "MdtCalibEventLoop/MdtCalibEventLoop.h"

#include "MuonIdHelpers/MdtIdHelper.h"

#include <algorithm>
#include <fstream>

namespace MuonCalib {

  MdtCalibTool::MdtCalibTool(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_mdtCalibTool("MuonCalib::MdtCalibNtupleMakerTool"),
    m_segmentSelectorTool("MuonCalib::SimpleMdtSegmentSelectorTool"),
    m_regionSelector("MuonCalib::AtlasRegionSelectorTool")
  {
    declareInterface< IMuonCalibTool >(this) ;
 
    declareProperty("MdtCalibTool", m_mdtCalibTool);
    declareProperty("SelectorSwitch", m_SelectorSwitch=0);
    declareProperty("MdtSegmentSelectorTool", m_segmentSelectorTool);
    declareProperty("RegionSelector", m_regionSelector);  
  }

  StatusCode MdtCalibTool::initialize()
  {
    ATH_CHECK(AthAlgTool::initialize());
    ATH_CHECK(m_mdtCalibTool.retrieve());
    ATH_MSG_DEBUG("Retrieved" << m_mdtCalibTool);
    ATH_CHECK(m_segmentSelectorTool.retrieve());
    ATH_MSG_DEBUG("Retrieved" << m_segmentSelectorTool);
    m_segmentSelector = m_segmentSelectorTool->getImp();
    ATH_CHECK(m_regionSelector.retrieve());
    ATH_MSG_DEBUG("Retrieved" << m_regionSelector);
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  bool MdtCalibTool::handleEvent( const MuonCalibEvent* event )
  {  
    unsigned int nb_segments(0);
    std::vector<MuonCalibSegment *> selected_segment;
    bool segment_found;
    bool segment_rejected;
    ATH_MSG_DEBUG("handleEvent(events) with patSize " << event->numberOfPatterns());

    // loop over patterns
    MuonCalibEvent::MCPVecCit pat_it     = event->patternBegin();
    MuonCalibEvent::MCPVecCit pat_it_end = event->patternEnd();
    for( ;pat_it!=pat_it_end; ++pat_it ){
      // loop over segments in pattern

      ATH_MSG_DEBUG("New pattern with segments " << (*pat_it)->muonSegments());

      MuonCalibPattern::MuonSegCit seg_it     = (*pat_it)->muonSegBegin();
      MuonCalibPattern::MuonSegCit seg_it_end = (*pat_it)->muonSegEnd();
      for( ;seg_it!=seg_it_end;++seg_it ){
        MuonCalibSegment* seg = *seg_it;
	// protect against null pointer
	if (!seg) {
	  ATH_MSG_WARNING("Got Segment nullptr");
	  continue;
	}
	// Only process segments with MDT hits
	if ( seg->mdtHOT().empty() ) {
	  continue;
	}

// if no internal selection is requested, pass the segment to the algorithm //
        if (m_SelectorSwitch==0 && seg) {
		handleMuonSegment( seg );
		continue;
	}

// segment selection //

	// reject all segments which contain hits from different chambers //
        MuonFixedId id1(seg->mdtHOT()[0]->identify());
        segment_rejected = false;
        for (unsigned int k=1; k<seg->mdtHitsOnTrack(); k++) {
            MuonFixedId id2(seg->mdtHOT()[k]->identify());
            if (id1.stationName()!=id2.stationName() || id1.eta()!=id2.eta() ||
	                                                id1.phi()!=id2.phi()) {
                segment_rejected = true;
            }
        }
        if (segment_rejected) {
            continue;
        }

    // if a segment is accepted, only keep it if it has more hits than        //
    // another segment in the same chamber; if two segments in a chamber have //
    // the same number of hits, take the one with the better chi^2            //
        segment_found = false;
        for (unsigned int k=0; k<nb_segments; k++) {
            MuonFixedId id2(selected_segment[k]->mdtHOT()[0]->identify());
            if (id1.stationName()==id2.stationName() && id1.eta()==id2.eta() &&
                                                         id1.phi()==id2.phi()) {
                segment_found = true;
                if (selected_segment[k]->mdtHitsOnTrack() <
							seg->mdtHitsOnTrack()) {
                    selected_segment[k] = seg;
                    break;
                }
                if (selected_segment[k]->mdtHitsOnTrack() == 
                                                      seg->mdtHitsOnTrack()) {
                    if (selected_segment[k]->chi2() > seg->chi2()) {
                        selected_segment[k] = seg;
                    break;
                    }
                }
            }
        }
        if (!segment_found) {
            selected_segment.push_back(seg);
            nb_segments++;
        }
      } // loop over segments

    } // loop over patterns

    if (m_SelectorSwitch!=0) {
    for (unsigned int k=0; k<nb_segments; k++) {
        if (selected_segment[k]!=0) {
            handleMuonSegment( selected_segment[k] );
	}
    }
    }
    return true;
  }

  bool MdtCalibTool::analyse()
  {
    // loop over all regions
    ATH_MSG_INFO("Performing loop over regions: " << m_segmentsPerRegion.size());

    RegionEventMap::iterator it     = m_segmentsPerRegion.begin();
    RegionEventMap::iterator it_end = m_segmentsPerRegion.end();
    for( ; it!=it_end; ++it ){
      ATH_MSG_DEBUG("Performing analysis for region ");
 
      // create new calibration instance
      IMdtCalibration* calibImp = m_mdtCalibTool->getImp( it->first );

      // set rt calibration implementation
      it->second->setCalibImp( calibImp );

      // perform calibration 
      it->second->performAnalysis();

      // get results
      const IMdtCalibrationOutput* result = it->second->calibrationResults();

      // write result to db
      m_mdtCalibTool->writeToDb( result, it->first );
    }

    ATH_MSG_INFO("End of calibration started     ");
    return true;
  }

  void MdtCalibTool::handleMuonSegment( const MuonCalibSegment* seg )
  { 
    // subdivide segment into regions and get key 
    IRegionSelectorTool::RegionVec rvec = m_regionSelector->splitIntoRegions( seg );  

    ATH_MSG_VERBOSE("RegionSelector produced " << rvec.size() << " subregions for segment ");
  
    // loop over different region 
    IRegionSelectorTool::RegionVec::iterator it = rvec.begin();
    IRegionSelectorTool::RegionVec::iterator it_end = rvec.end();

    for( ; it!=it_end; ++it ){

      // decompose pair
      const MuonCalibSegment * sseg = it->first;
      IRegionSelectorTool::RegionKey key = it->second;

      if( !m_segmentSelector->select(*sseg) ){
        ATH_MSG_VERBOSE(" ssegment rejected by selector ");
	// return; this prevents the second ML from being accepted!
        continue;
      }

      // check if region should be calibrated
      if( m_regionSelector->useRegion( key ) ){
  
	ATH_MSG_VERBOSE("handleMuonSegment with region key " << key);
	
	// pointer to current event loop
	MdtCalibEventLoop* loop = getEventLoopForRegion( key );
      
	// fill current ssegment into event loop
	loop->handleSegment( sseg );
      }else{
	ATH_MSG_VERBOSE("MuonSegment not selected by RegionSelector " << key);
      }
    }
  }


  IRegionSelectorTool::RegionKey MdtCalibTool::keyFromIdentifier( const IRegionSelectorTool::id_type& id ) const
  {
    ATH_MSG_VERBOSE(" keyFromIdentifier ");
    return m_regionSelector->getRegionKey(id);
  }


  MdtCalibEventLoop* MdtCalibTool::getEventLoopForRegion( const IRegionSelectorTool::RegionKey& key )
  {  
    MdtCalibEventLoop* loop = nullptr;
    // look up key in map
    RegionEventMap::iterator sit = m_segmentsPerRegion.find(key);
      
    // if not found create new entry in map
    if( sit == m_segmentsPerRegion.end() ){
      ATH_MSG_VERBOSE("created new entry in region map for key " << key);
      
      // create new event loop
      loop = new MdtCalibEventLoop( key );
    
      // add loop to RegionMap
      m_segmentsPerRegion.insert( sit, std::make_pair( key, loop ) );
    }else{
      // get event loop from RegionMap
      loop = sit->second;
    }
    return loop;
  }


}

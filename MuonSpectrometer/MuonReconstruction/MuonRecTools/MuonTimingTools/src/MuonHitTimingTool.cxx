/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTimingTool.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"


namespace Muon {

  MuonHitTimingTool::MuonHitTimingTool(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p),
    m_hitTimingTools(this)
  {
    declareInterface<IMuonHitTimingTool>(this);
    
    for( unsigned int tech = 0;tech<MuonStationIndex::TechnologyIndexMax;++tech ){
      if( tech == static_cast<unsigned int>(MuonStationIndex::RPC) ) m_hitTimingTools.push_back(ToolHandle<IMuonHitTimingTool>("Muon::RPC_TimingTool/RPC_TimingTool"));
      else                                                           m_hitTimingTools.push_back(ToolHandle<IMuonHitTimingTool>(""));
    }
    
  }

  StatusCode MuonHitTimingTool::initialize() {

    ATH_CHECK(m_idHelperSvc.retrieve());

    // ensure that the number of tool handles corresponds to the number of technologies
    if( m_hitTimingTools.size() != MuonStationIndex::TechnologyIndex::TechnologyIndexMax ){
      ATH_MSG_ERROR(" The MuonHitTimingTool ToolHandleArray SHOULD contain exactly " << MuonStationIndex::TechnologyIndex::TechnologyIndexMax
                    << " ToolHandles (they can be empty). ");
      return StatusCode::FAILURE;
    }

    // loop over timing tool handles and check that they handle the technology they are supposed to, if not return a FAILURE
    for( unsigned int tech = 0;tech<MuonStationIndex::TechnologyIndexMax;++tech ){
      // get handle, accept empty handles
      auto& toolHandle = m_hitTimingTools[tech];
      ATH_MSG_INFO(" tech " << MuonStationIndex::technologyName((MuonStationIndex::TechnologyIndex)tech) << " " << toolHandle);
      if( toolHandle.empty() ) continue;
      ATH_CHECK(toolHandle.retrieve());
      ATH_MSG_INFO(" retrieved " << toolHandle );
      // if not empty get list of handles technolgies and check that the current tech is handled
      std::set<MuonStationIndex::TechnologyIndex> acceptedTechnologies = toolHandle->acceptedTechnologies();
      if( !acceptedTechnologies.count(static_cast<MuonStationIndex::TechnologyIndex>(tech)) ) return StatusCode::FAILURE;
      m_acceptedTechnologies.insert(acceptedTechnologies.begin(),acceptedTechnologies.end());
    }
    return StatusCode::SUCCESS;
  }

  IMuonHitTimingTool::TimingResult MuonHitTimingTool::calculateTimingResult( const std::vector<const MuonClusterOnTrack*>& hits ) const {
    
    // treat case of no hits and the case the first pointer is zero (should not happen)
    if( hits.empty() || !hits.front() )       return TimingResult();

    // for now assume that all hits are of the same technolgy
    Identifier id = hits.front()->identify();
    MuonStationIndex::TechnologyIndex tech = m_idHelperSvc->technologyIndex(id);
    if( !m_acceptedTechnologies.count(tech) ) return TimingResult();
    
    // get handle and use it if it is not empty
    const ToolHandle<IMuonHitTimingTool>& toolHandle = m_hitTimingTools[tech];
    if( toolHandle.empty() ) {
      ATH_MSG_WARNING("Unable to fill timing, timing tool missing. Tech = " << MuonStationIndex::technologyName(tech) );
      return TimingResult();
    }
    return toolHandle->calculateTimingResult(hits);
  }


}

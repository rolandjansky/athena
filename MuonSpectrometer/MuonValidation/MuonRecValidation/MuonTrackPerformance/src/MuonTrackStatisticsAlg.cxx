/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackPerformance/MuonTrackStatisticsAlg.h"
#include "MuonTrackPerformance/MuonTrackStatisticsTool.h"

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
MuonTrackStatisticsAlg::MuonTrackStatisticsAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : 
  AthAlgorithm(name, pSvcLocator),
  m_statisticsTool("MuonTrackStatisticsTool"),
  m_writeToFile (false),
  m_doTruth     (false)
{
  declareProperty("doTruth",         m_doTruth);
  declareProperty("writeToFile",     m_writeToFile);
  declareProperty("FileName",               m_fileName);
}

// INITIALIZE METHOD:
 
StatusCode MuonTrackStatisticsAlg::initialize()
{

  ATH_CHECK(m_statisticsTool.retrieve());
  ATH_CHECK(m_trackKeys.initialize());
  ATH_CHECK(m_truthKeys.initialize());
  
  for (unsigned int i=0; i<m_trackKeys.size() ; i++) m_statisticsTool->addTrackCounters(m_trackKeys[i].key());
  for (unsigned int i=0; i<m_truthKeys.size() ; i++) m_statisticsTool->addTrackCounters(m_truthKeys[i].key());
    
  return StatusCode::SUCCESS;
}
 
StatusCode MuonTrackStatisticsAlg::execute()
{
  
  ATH_MSG_DEBUG("MuonTrackStatisticsAlg in execute() ...");

  for(SG::ReadHandle<TrackCollection>& trackColl : m_trackKeys.makeHandles()){
    if(!trackColl.isPresent()){
      ATH_MSG_DEBUG("track collection "<<trackColl.key()<<" not present");
      continue;
    }
    if(!trackColl.isValid()){
      ATH_MSG_WARNING("track collection "<<trackColl.key()<<" not valid!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_statisticsTool->updateTrackCounters(trackColl.key(),trackColl.cptr()) );
  }

  for(SG::ReadHandle<DetailedTrackTruthCollection>& truthMap : m_truthKeys.makeHandles()){
    if(!truthMap.isPresent()){
      ATH_MSG_DEBUG("truth map "<<truthMap.key()<<" not present");
      continue;
    }
    if(!truthMap.isValid()){
      ATH_MSG_WARNING("truth map "<<truthMap.key()<<" not valid!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_statisticsTool->updateTruthTrackCounters(truthMap.key(),truthMap.cptr()) );
  }
  
  return StatusCode::SUCCESS;
  
}


/////////////////////////////////////////////////////////////////
// ATHENA FINALIZE:


StatusCode MuonTrackStatisticsAlg::finalize() 
{

  ATH_MSG_INFO(std::endl << m_statisticsTool->printTrackCounters());
  //write to file
  if (m_writeToFile){
    m_fileOutput.open(m_fileName.c_str(), std::ios::trunc);
    m_fileOutput <<  m_statisticsTool->printTrackCounters() << std::endl;
    m_fileOutput.close();
  }

  return StatusCode::SUCCESS;
}

void
MuonTrackStatisticsAlg::storeTruthTracks(void)
{
  
  
}

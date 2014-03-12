/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWSectorRoadFinderAlg.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCombination.h"

NSWSectorRoadFinderAlg::NSWSectorRoadFinderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator)
{  
  declareProperty("MMPrepDataContainer", m_keyMM = "MM_Measurements");
  declareProperty("sTgcPrepDataContainer", m_keysTgc = "STGC_Measurements");
  //
  declareProperty("PatternOutputLocation",m_patternLocation="NSWPatternCombinations");
}

NSWSectorRoadFinderAlg::~NSWSectorRoadFinderAlg()
{

}

StatusCode NSWSectorRoadFinderAlg::initialize()
{
  if (AthAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if( m_idHelper.retrieve().isFailure() ){
    ATH_MSG_ERROR("Could not retrieve " << m_idHelper);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS; 
}

StatusCode NSWSectorRoadFinderAlg::execute()
{

  const Muon::sTgcPrepDataContainer* stgcPrds = 0;      
  if (evtStore()->retrieve(stgcPrds,m_keysTgc).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve sTgcPrepDataContainer " << m_keysTgc << " accessing via collections ");
  }
  const Muon::MMPrepDataContainer* mmPrds = 0;      
  if (evtStore()->retrieve(mmPrds,m_keyMM).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve MMPrepDataContainer " << m_keyMM << " accessing via collections ");
  }
  if( !stgcPrds && !mmPrds )  return StatusCode::SUCCESS;
  MuonPatternCombinationCollection* patterns = new MuonPatternCombinationCollection();
  
  for( int si=1;si<=16;++si ){
    std::vector< Muon::MuonPatternChamberIntersect > chamberData;
    if( mmPrds ) addContainerToMuonPatternCombination(*mmPrds,chamberData,si);
    if( stgcPrds ) addContainerToMuonPatternCombination(*stgcPrds,chamberData,si);
    if( !chamberData.empty() ){
      patterns->push_back( new Muon::MuonPatternCombination( new Trk::Perigee(chamberData.front().intersectPosition(),
									      chamberData.front().intersectDirection(),
									      -1.,
									      chamberData.front().intersectPosition()), 
							     chamberData ) );
      ATH_MSG_VERBOSE("sector  " << si << " chambers with hits " << chamberData.size() );
    }
  }
  if( evtStore()->record(patterns,m_patternLocation).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonPatternCombinationCollection " << patterns->size() << " at " << m_patternLocation);
  }else{
    ATH_MSG_ERROR("Failed to store MuonPatternCombinationCollection at " << m_patternLocation);
  }

  return StatusCode::SUCCESS;
} // execute


StatusCode NSWSectorRoadFinderAlg::finalize()
{

  return AthAlgorithm::finalize();
}



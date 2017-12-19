/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWSectorRoadFinderAlg.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"

#include "MuonPattern/MuonPatternCombination.h"

NSWSectorRoadFinderAlg::NSWSectorRoadFinderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_idHelper("Muon::MuonIdHelperTool"),
  m_keysTgc("STGC_Measurements"),
  m_keyMM("MM_Measurements"),
  m_patternLocation("NSWPatternCombinations")
{  
  declareProperty("MMPrepDataContainer", m_keyMM);
  declareProperty("sTgcPrepDataContainer", m_keysTgc);
  //
  declareProperty("PatternOutputLocation",m_patternLocation);
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

  ATH_CHECK( m_keysTgc.initialize() );
  ATH_CHECK( m_keyMM.initialize() );
  ATH_CHECK( m_patternLocation.initialize() );
  return StatusCode::SUCCESS; 
}

StatusCode NSWSectorRoadFinderAlg::execute()
{
  SG::ReadHandle< Muon::sTgcPrepDataContainer> stgcPrds(m_keysTgc); 
  if (!stgcPrds.isValid()) {
    ATH_MSG_WARNING("Cannot retrieve sTgcPrepDataContainer " << m_keysTgc.key() << " accessing via collections ");
  }

  SG::ReadHandle< Muon::MMPrepDataContainer> mmPrds(m_keyMM);      
  if (!mmPrds.isValid()) {
    ATH_MSG_WARNING("Cannot retrieve MMPrepDataContainer " << m_keyMM.key() << " accessing via collections ");
  }
  if( !stgcPrds.isValid() && !mmPrds.isValid() )  return StatusCode::SUCCESS;
  std::unique_ptr<MuonPatternCombinationCollection> patterns(new MuonPatternCombinationCollection());
  
  for( int si=1;si<=16;++si ){
    std::vector< Muon::MuonPatternChamberIntersect > chamberData;
    if( mmPrds.isValid() ) addContainerToMuonPatternCombination(*mmPrds,chamberData,si);
    if( stgcPrds.isValid() ) addContainerToMuonPatternCombination(*stgcPrds,chamberData,si);
    if( !chamberData.empty() ){
      patterns->push_back( new Muon::MuonPatternCombination( new Trk::Perigee(chamberData.front().intersectPosition(),
									      chamberData.front().intersectDirection(),
									      -1.,
									      chamberData.front().intersectPosition()), 
							     chamberData ) );
      ATH_MSG_VERBOSE("sector  " << si << " chambers with hits " << chamberData.size() );
    }
  }
  SG::WriteHandle<MuonPatternCombinationCollection> writehandle(m_patternLocation);
  if( writehandle.record(std::move(patterns)).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonPatternCombinationCollection " << patterns->size() << " at " << m_patternLocation.key());
  }else{
    ATH_MSG_ERROR("Failed to store MuonPatternCombinationCollection at " << m_patternLocation.key());
  }

  return StatusCode::SUCCESS;
} // execute


StatusCode NSWSectorRoadFinderAlg::finalize()
{

  return AthAlgorithm::finalize();
}



/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibPatRec/MuonSegmentToCalibSegment.h"

// gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

// idhelpers
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

// muon geomodel
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"

#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"


// hack to get MdtCalibrationSvcInput to compile
namespace Trk {
  class StraightLineSurface;
}

// mdt calibration
#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MdtCalibSvc/MdtCalibrationSvcSettings.h"
#include "MdtCalibSvc/MdtCalibrationSvcInput.h"
#include "MuonCalibEvent/MdtCalibHit.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MuonCalibEventBase/CscCalibHitBase.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibNtuple/MuonSegmentNtupleBranch.h"


#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"


#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"

#include <iostream>


namespace MuonCalib {

  MuonSegmentToCalibSegment::MuonSegmentToCalibSegment(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_detMgr(NULL),
    m_mdtIdHelper(NULL),
    m_cscIdHelper(NULL),
    m_rpcIdHelper(NULL),
    m_tgcIdHelper(NULL),
    m_calibSvc("MdtCalibrationSvc", name),
    m_assocTool("Muon::MuonPatternSegmentAssociationTool/MuonPatternSegmentAssociationTool"),
    m_idToFixedIdTool("MuonCalib::IdToFixedIdTool/MuonCalib_IdToFixedIdTool")

  {

    declareProperty("ReadSegments", m_readSegments = false);
    declareProperty("UseCscSegments", m_useCscSegments = true);
    declareProperty("SegmentLocation", m_segmentLocation = "Muonboy_SegmentStore");
    declareProperty("SegmentCombiLocation", m_segmentCombiLocation = "MooreSegmentCombinations");
    declareProperty("SegmentCscLocation", m_segmentCscLocation = "Csc4dSegmentCombinations");
    declareProperty("NewImpactParameter", m_newImpactParameter = false);

    // vector of segment locations
    std::vector<std::string> segmentAlgs;
    segmentAlgs.push_back("ConvertedMBoySegments");
    segmentAlgs.push_back("MooreSegments");
    declareProperty("SegmentLocations", m_segmentLocationVector = StringArrayProperty(segmentAlgs));
    m_segment_authors.push_back(3);
    m_segment_authors.push_back(4);
    declareProperty("SegmentAuthors", m_segment_authors);
    // Location of output patterns and segments
    declareProperty("PatternLocation", m_patternLocation = "PatternsForCalibration");

    declareProperty("UpdateForT0Shift", m_updateForT0Shift = -1 );
    declareProperty("DoTOF", m_doTof = true );
    declareProperty("DoCosmicsTof", m_cosmics_tof=false);
  }

  // Initialize
  StatusCode MuonSegmentToCalibSegment::initialize(){
    //
    if(m_segmentLocationVector.value().size() < m_segment_authors.size())
    	{
	m_segment_authors.resize(m_segmentLocationVector.value().size(), -1);
	}
/*    m_log << MSG::INFO << "Initialisation started     " << endmsg;
    //
    //
    m_log << MSG::INFO << "================================" << endmsg;
    m_log << MSG::INFO << "= Proprieties are     " << endmsg;
    std::vector<std::string>::const_iterator segLocit     = m_segmentLocationVector.value().begin();  
    std::vector<std::string>::const_iterator segLocit_end = m_segmentLocationVector.value().end();      
    for ( ; segLocit != segLocit_end ; ++segLocit) {
      m_log << MSG::INFO << "= SegmentLocation       " << *segLocit << endmsg;
    }
    m_log << MSG::INFO << "= SegmentCscLocation    " << m_segmentCscLocation    << endmsg;
    m_log << MSG::INFO << "= SegmentCombiLocation          " << m_segmentCombiLocation          << endmsg;
    
    m_log << MSG::INFO << "= PatternLocation     " << m_patternLocation     << endmsg;
    m_log << MSG::INFO << "================================" << endmsg;*/

    std::string managerName="Muon";
    ATH_CHECK( detStore()->retrieve(m_detMgr) );
    ATH_CHECK( m_idToFixedIdTool.retrieve() );
    ATH_CHECK( m_assocTool.retrieve() );

    // initialize MuonIdHelpers
    if (m_detMgr) {
      m_mdtIdHelper = m_detMgr->mdtIdHelper();
      m_cscIdHelper = m_detMgr->cscIdHelper();
      m_rpcIdHelper = m_detMgr->rpcIdHelper();
      m_tgcIdHelper = m_detMgr->tgcIdHelper();
    } else {
      m_mdtIdHelper = 0;
      m_cscIdHelper = 0;
      m_rpcIdHelper = 0;
      m_tgcIdHelper = 0;
    }

    ATH_CHECK( m_calibSvc.retrieve() );

    // Get the maximum number of segments each algorithm can
    // store in the ntuple
    MuonSegmentNtupleBranch segBranch;
    m_maxStoredSegs = segBranch.MuonSegmentNtupleBranch::blockSize();
    if (m_segmentLocationVector.value().size()) {
      m_maxStoredSegs /= m_segmentLocationVector.value().size();
    }
    ATH_MSG_INFO( "Maximum number of stored segments for each algorithm = " << m_maxStoredSegs  );

    ATH_MSG_INFO( "Initialisation ended     "  );
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentToCalibSegment::execute(){

    ATH_MSG_DEBUG( " execute()     "  );
    ATH_CHECK( convertPatterns() );
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentToCalibSegment::finalize(){
    ATH_MSG_DEBUG( "Finalisation started     "  );
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentToCalibSegment::savePatterns( const MuonCalibPatternCollection* newPatterns) const
  {
    ATH_CHECK( evtStore()->record(newPatterns, m_patternLocation) );
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentToCalibSegment::convertPatterns() {

    /** 
	Routine to extract mdt calibration segments from Mboy mdt segments
    */

    MuonCalibPatternCollection* patterns = new MuonCalibPatternCollection();

    ATH_MSG_DEBUG( " convertPatterns() "  );

    if( !m_readSegments ) {

      const MuonSegmentCombinationCollection* segCombis = retrieveSegmentCombinations();
      
      if( segCombis ) { 

	MuonSegmentCombinationCollection::const_iterator sit = segCombis->begin();
	MuonSegmentCombinationCollection::const_iterator sit_end = segCombis->end();
	
	ATH_MSG_DEBUG( " Looping over segment combination " << segCombis->size()  );
	
	for( ;sit!=sit_end;++sit){
	  
	  // get association to pattern
	  const Muon::MuonPatternCombination* pat = 0;
	  Muon::IMuonPatternSegmentAssociationTool::AssociationMapRange range = m_assocTool->find(*sit);
	  if ((range.first)!=(range.second)) {
	    if( m_assocTool->count(*sit) != 1 ){
	      ATH_MSG_INFO( " This MuonSegPatAssMap for MDTs should only have one entry!! "  );
	    }	  // retrieve association map:       
	    pat = (range.first)->second;
	  }else{
	    ATH_MSG_WARNING("MDT Combination missing from the map - something is wrong! " << *sit  );
	  }
	  
	  MuonCalibPattern* calibpat = createMuonCalibPattern(pat);
	  
	  ATH_MSG_DEBUG( "New segment combination covering " << (*sit)->numberOfStations() << " station "  );
	  
	  // loop over segments in combination
	  unsigned int nstations = (*sit)->numberOfStations();
	  for(unsigned int i=0; i!=nstations; ++i){
	    
	    const Muon::MuonSegmentCombination::SegmentVec* stationSegs = (*sit)->stationSegments( i ) ;
	    
	    ATH_MSG_VERBOSE( "New station with " << stationSegs->size() << " segments "  );
	    
	    // 	  if( stationSegs->size() != 1 ) {
	    // 	    m_log << MSG::DEBUG << " -> skipping station, to many segments " << endmsg;
	    // 	    continue;
	    // 	  }
	    
	    Muon::MuonSegmentCombination::SegmentVec::const_iterator segit     = stationSegs->begin();
	    Muon::MuonSegmentCombination::SegmentVec::const_iterator segit_end = stationSegs->end();
	    
	    for( ; segit!=segit_end;++segit ){
	      const Muon::MuonSegment* seg = *segit;
	      
	      if( !seg ) {
		ATH_MSG_WARNING( " go NULL pointer for MuonSegment "  );
		continue;
	      }
	      
	      MuonCalibSegment* mdtSeg = createMuonCalibSegment( *seg );
	      mdtSeg->setAuthor(seg->author()); 
	      calibpat->addMuonSegment( mdtSeg );
	    }
	  }
	  
	  // add pattern to segment
	  patterns->push_back( calibpat );
	}
      }
    }else{
      
      // loop over MDT segment collections

      std::vector<std::string>::const_iterator segLocit     = m_segmentLocationVector.value().begin();  
      std::vector<std::string>::const_iterator segLocit_end = m_segmentLocationVector.value().end();
      
      std::vector<int>::const_iterator autIt=m_segment_authors.begin();
      
      for ( ; segLocit != segLocit_end ; ++segLocit, autIt++) {

	int nStoredSegments = 0;
	
	std::string mdtSegmentLocation = *segLocit;

	const Trk::SegmentCollection* segCol = retrieveSegments(mdtSegmentLocation);
	
	if( segCol ) {
	  
	  Trk::SegmentCollection::const_iterator sit = segCol->begin();
	  Trk::SegmentCollection::const_iterator sit_end = segCol->end();
	  
	  ATH_MSG_DEBUG( " Looping over segments " << segCol->size()  );
	  
	  for( ;sit!=sit_end;++sit){

	    if (nStoredSegments >= m_maxStoredSegs) {

	      ATH_MSG_INFO( "For " << mdtSegmentLocation 
                            << ", hit max number of segments = " << nStoredSegments  );
	      break;
	    }
	    	    
	    if( !*sit ) {
	      ATH_MSG_WARNING( " go NULL pointer for Segment "  );
	      continue;
	    }
	    
	    const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*>(*sit);
	    
	    if( !seg ) {
	      ATH_MSG_WARNING( " dynamic_cast to MuonSegment failed "  );
	      continue;
	    }
	    
	    // one pattern per segment
	    ATH_MSG_DEBUG( "WARNING, empty muoncalibpattern created"  );
	    MuonCalibPattern* pat = new MuonCalibPattern();
	    
	    MuonCalibSegment* mdtSeg = createMuonCalibSegment( *seg );
	    if(*autIt<0)
		    mdtSeg->setAuthor(seg->author()); 
	    else
		    mdtSeg->setAuthor(*autIt); 
	    
	    pat->addMuonSegment( mdtSeg );
	    
	    // add pattern to segment
	    patterns->push_back( pat );
	    nStoredSegments++;
	  }
	}
      }
    }

    // retrieve CscSegmentCombis
    if( m_useCscSegments ) {
      const MuonSegmentCombinationCollection* segCombis = retrieveCscSegmentCombinations();
      
      if( !segCombis ) {
	ATH_CHECK( savePatterns(patterns) );
	return StatusCode::SUCCESS;
      }

      //      m_segmentAuthor = segmentAuthor(m_segmentCscLocation);
      
      MuonSegmentCombinationCollection::const_iterator sit = segCombis->begin();
      MuonSegmentCombinationCollection::const_iterator sit_end = segCombis->end();
      
      ATH_MSG_DEBUG( " Looping over Csc segment combination " << segCombis->size()  );
      
      for( ;sit!=sit_end;++sit) {

	// don't create pattern when csc segment combination is empty (bug in csc segmentmaker?, 3-12-7, JS)
	if ((*sit)->numberOfStations()==1) {
	  if ((*sit)->stationSegments(0)->size()==0) continue;
	}

	// get association to pattern
	const Muon::MuonPatternCombination* pat = 0;
	Muon::IMuonPatternSegmentAssociationTool::AssociationMapRange range = m_assocTool->find(*sit);
	if ((range.first)!=(range.second)) {
	  pat = (range.first)->second;
	}else{
	  ATH_MSG_DEBUG("CSC Combination missing from the map - No combined pattern found for this CSC Segment Combination! " << *sit  );
	}
	
	MuonCalibPattern* calibpat = createMuonCalibPattern(pat);
	
	ATH_MSG_DEBUG( "New Csc segment combination covering " << (*sit)->numberOfStations() << " station "  );
	
	// loop over segments in combination
	unsigned int nstations = (*sit)->numberOfStations();
	for(unsigned int i=0; i!=nstations; ++i){
	  
	  const Muon::MuonSegmentCombination::SegmentVec* stationSegs = (*sit)->stationSegments( i ) ;
	  
	  ATH_MSG_VERBOSE( "New Csc station with " << stationSegs->size() << " segments "  );
	  
	  // 	  if( stationSegs->size() != 1 ) {
	  // 	    m_log << MSG::DEBUG << " -> skipping station, too many segments " << endmsg;
	  // 	    continue;
	  // 	  }
	  
	  Muon::MuonSegmentCombination::SegmentVec::const_iterator segit     = stationSegs->begin();
	  Muon::MuonSegmentCombination::SegmentVec::const_iterator segit_end = stationSegs->end();

	  for( ; segit!=segit_end;++segit) {
	    const Muon::MuonSegment* seg = *segit;
	    
	    if( !seg ) {
	      ATH_MSG_WARNING( " go NULL pointer for MuonSegment "  );
	      continue;
	    }
	    
	    MuonCalibSegment* CscSeg = createMuonCalibSegment( *seg );
	    CscSeg->setAuthor(seg->author()); 
	    calibpat->addMuonSegment( CscSeg );
	  }
	}
	
	// add pattern to segment
	patterns->push_back( calibpat );
      }
    

    }//if(m_useCscSegments)
    // store patterns in storegate
    ATH_CHECK( savePatterns(patterns) );
    return StatusCode::SUCCESS;
  }

  Identifier MuonSegmentToCalibSegment::getChId( const Muon::MuonSegment& seg ) const
  {
    
    const std::vector<const Trk::RIO_OnTrack*>& rots = seg.containedROTs();
    if( rots.empty() ) {
      ATH_MSG_DEBUG( " Oops, segment without hits!!! "  );
      return Identifier();
    }

    std::vector<const Trk::RIO_OnTrack*>::const_iterator rit = rots.begin();
    std::vector<const Trk::RIO_OnTrack*>::const_iterator rit_end = rots.end();
    
    for( ; rit!=rit_end;++rit ){
      
      // use pointer to rot
      const Trk::RIO_OnTrack* rot = *rit;
      
      if( m_mdtIdHelper->is_mdt( rot->identify() ) ){
	return rot->identify();
      }else if( m_cscIdHelper->is_csc( rot->identify() ) ){
	return rot->identify();
      }
    }

    // if we get here the segment did not contain any csc or mdt hits, in which case we return the identifier of the first rot
    return rots.front()->identify();
  }

  Amg::Transform3D MuonSegmentToCalibSegment::getGlobalToStation( const Identifier& id ) const
  {
    
    if( m_mdtIdHelper->is_mdt( id ) ){
      const MuonGM::MdtReadoutElement* detEl = m_detMgr->getMdtReadoutElement(id);
      if( !detEl ) {
	ATH_MSG_WARNING( "getGlobalToStation failed to retrieve detEL byebye"  );
      }else{
	return detEl->GlobalToAmdbLRSTransform();
      }
    }else if( m_cscIdHelper->is_csc( id ) ){
      const MuonGM::CscReadoutElement* detEl = m_detMgr->getCscReadoutElement(id);
      if( !detEl ) {
	ATH_MSG_WARNING( "getGlobalToStation failed to retrieve detEL byebye"  );
      }else{
	return detEl->transform().inverse();
      }
    }else if( m_tgcIdHelper->is_tgc( id ) ){
      const MuonGM::TgcReadoutElement* detEl = m_detMgr->getTgcReadoutElement(id);
      if( !detEl ) {
	ATH_MSG_WARNING( "getGlobalToStation failed to retrieve detEL byebye"  );
      }else{
	return detEl->transform().inverse();
      }
    }else if( m_rpcIdHelper->is_rpc( id ) ){
      const MuonGM::RpcReadoutElement* detEl = m_detMgr->getRpcReadoutElement(id);
      if( !detEl ) {
	ATH_MSG_WARNING( "getGlobalToStation failed to retrieve detEL byebye"  );
      }else{
	return detEl->transform().inverse();
      }
    }
    ATH_MSG_WARNING( " Oops, should not be here, returning default transform "  );
    return Amg::Transform3D();
  }

 
  const MuonSegmentCombinationCollection* MuonSegmentToCalibSegment::retrieveSegmentCombinations() const
  {
    bool contains =  evtStore()->contains<MuonSegmentCombinationCollection>(m_segmentCombiLocation);
    if (!contains) { // requested segment location is not in storeGate
      ATH_MSG_WARNING( "MuonSegmentCombinationCollection " << m_segmentCombiLocation << " is not in storeGate"  );
      return 0;
    }

    const MuonSegmentCombinationCollection* segCombis = 0;
    StatusCode sc = evtStore()->retrieve(segCombis,m_segmentCombiLocation);
    if (sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not find MuonSegmentCombinationCollection at " << m_segmentCombiLocation  );
      return 0;
    }else{
      ATH_MSG_DEBUG( "retrieved MuonSegmentCombinationCollection "  << segCombis->size()  );
    }
    return segCombis;
  }


  const Trk::SegmentCollection* MuonSegmentToCalibSegment::retrieveSegments(const std::string colName) const
  {
    bool contains =  evtStore()->contains<Trk::SegmentCollection>(colName);
    if (!contains) { // requested segment location is not in storeGate
      ATH_MSG_WARNING( "MuonSegmentCollection " << colName << " is not in storeGate"  );
      return 0;
    }

    const Trk::SegmentCollection* segCol = 0;
    StatusCode sc = evtStore()->retrieve(segCol,colName);
    if (sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not find MuonSegmentCollection at " << colName  );
      return 0;
    }else{
      ATH_MSG_DEBUG( "retrieved MuonSegmentCollection "  << segCol->size()  );
    }
    return segCol;
  }

  const MuonSegmentCombinationCollection* MuonSegmentToCalibSegment::retrieveCscSegmentCombinations() const
  {
    bool contains =  evtStore()->contains<MuonSegmentCombinationCollection>(m_segmentCscLocation);
    if (!contains) { // requested segment location is not in storeGate
      ATH_MSG_WARNING( "MuonSegmentCombinationCollection " << m_segmentCscLocation << " is not in storeGate"  );
      return 0;
    }

    const MuonSegmentCombinationCollection* segCombis = 0;
    StatusCode sc = evtStore()->retrieve(segCombis,m_segmentCscLocation);
    if (sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not find Csc MuonSegmentCombinationCollection at " << m_segmentCscLocation << " csc segmentmaker beging run? "  );
      return 0;
    }
    else
      {
	ATH_MSG_DEBUG( "retrieved Csc MuonSegmentCombinationCollection "  << segCombis->size()  );
      }
    return segCombis;
  }


  MuonCalibSegment* MuonSegmentToCalibSegment::createMuonCalibSegment( const Muon::MuonSegment& seg ) const 
  {
    // convert MuonSegment to MuonCalibSegment

    Identifier chid = getChId( seg );
    
    // global to station transformation for this chamber
    Amg::Transform3D gToStationCheck = seg.associatedSurface().transform().inverse();
    Amg::Transform3D gToStation = getGlobalToStation( chid );
    // create the local position and direction vector
    Amg::Vector3D  segPosG(seg.globalPosition());
    Amg::Vector3D segDirG(seg.globalDirection());

    // calculate local position and direction of segment
    Amg::Vector3D segPosL  = gToStation*segPosG;
    Amg::Vector3D segDirL = gToStation.linear()*segDirG;
    Amg::Vector3D segDirLCheck = gToStationCheck.linear()*segDirG;
    // 	    std::cout << "   local segment  " << segPosL << " dir " << segDirL.unit() << std::endl;
    // 	    std::cout << "   global segment " << segPosG << " dir " << segDirG.unit() << std::endl;
//    bool mdtSegment = false;
	
    double qualityFactor(1e9);
    if( seg.fitQuality() ){ 
      qualityFactor = seg.fitQuality()->chiSquared();
    }
    // get segment quality
    unsigned int segQuality = getQuality( seg );

    // pointer to new MuonSegment
    MuonCalibSegment* mdtSeg = new MuonCalibSegment( qualityFactor, segPosL, Amg::Vector3D(segDirL.unit()), gToStation.inverse(), segQuality );
    double t0Shift = 0.;
    if(seg.hasFittedT0())
    	{
	 mdtSeg->setFittedT0(seg.time());
	}

    Identifier cachedId; // Cached Identifier of previous hit (invalid for now)

    double chi2check = 0;
    double thetap = atan2(mdtSeg->direction().y(),mdtSeg->direction().z());
    double cosin = cos(thetap);
    double sinus = sin(thetap);
    double thetan = atan2(mdtSeg->direction().z(),mdtSeg->direction().y());
    double thetaCheck = atan2(segDirLCheck[2],segDirLCheck[1]);
    ATH_MSG_DEBUG( " MuonSegment TO CalibSegment segment found "  );
    if(msgLvl(MSG::DEBUG)) {
      if (fabs(thetaCheck-thetan)>0.0001) ATH_MSG_DEBUG( " ALARM angle difference " << thetaCheck-thetan  );
        ATH_MSG_DEBUG( " segPosL " << segPosL <<  " segPosG " << segPosG << " local angle " << thetan << " thetaCheck " << thetaCheck  );
        ATH_MSG_DEBUG( " segDirL " << segDirL << " segDirG " << segDirG << " phi " << segDirG.phi() <<  " segDirLCheck " << segDirLCheck  );
    }
   bool segment_with_multiple_t0s(false); 
// for debug purposes count number of mdt,csc,tgc and rpc segments
   int nm(0),nr(0),nt(0),nc(0);

// loop over hits
   const std::vector<const Trk::MeasurementBase*>& rots = seg.containedMeasurements();
   std::vector<const Trk::MeasurementBase*>::const_iterator rit = rots.begin();
   std::vector<const Trk::MeasurementBase*>::const_iterator rit_end = rots.end();
    for( ; rit!=rit_end;++rit ){
      Identifier id;
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (*rit);
      const Trk::CompetingRIOsOnTrack* rotc = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (*rit);
      bool competingRio = false;
      if (rot) {
        // loop over Rios
         const Trk::PrepRawData* prd = rot->prepRawData();
         id = prd->identify();
      } else {
     // loop over Competing Rios
         if (rotc) {
           id = rotc->rioOnTrack(0).identify();
           competingRio = true; 
          } else { continue;}
      }
      
      if( m_mdtIdHelper->is_mdt(id)) {
        if (competingRio) {
          ATH_MSG_WARNING( "  MDT hit is competing Rio !!! "  );
         continue;
        }
	// Mdt digit	  
	++nm;

	const Muon::MdtDriftCircleOnTrack* mrot = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(rot);
	if( !mrot ){
	  ATH_MSG_WARNING( "This is not a  MdtDriftCircleOnTrack!!! "  );
	  continue;
	}
//      mdtSegment = true;

	// get digit from segment
	const Muon::MdtPrepData* prd = mrot->prepRawData();
	  
	// digit identifier
	id = prd->identify();

	// get MdtDetectorElement for current digit
	const MuonGM::MdtReadoutElement* detEl = prd->detectorElement();

	// get tube geometry
        const Trk::StraightLineSurface* pStraightLineSurface 
         = dynamic_cast<const Trk::StraightLineSurface*> (&(detEl->surface(prd->identify())));
        if (!pStraightLineSurface) {
          ATH_MSG_WARNING( "This has no StraightLineSurface  !!! "  );
          continue;
        }
//        mdtSegment = true;

// Prd has no second coordinate        
	Amg::Vector3D tubePosLoc = gToStation*prd->globalPosition();

        //Get local tube direction, orient tube direction along the x local axis direction and get all DCA stuff in local coordinates    
        Amg::Vector3D tubeDirGlo = (pStraightLineSurface->transform()).rotation().col(2) ;
        Amg::Vector3D tubeDirLoc = gToStation.linear()*tubeDirGlo ;
        ATH_MSG_DEBUG( " tubeDirLoc " << tubeDirLoc  );
        tubeDirLoc = tubeDirLoc.unit() ;
        if ( tubeDirLoc.x() < 0.) tubeDirLoc = -tubeDirLoc ;

        Amg::Vector3D segPosLoc(mdtSeg->position() ) ;
        Amg::Vector3D segDirLoc(mdtSeg->direction()) ;
        segDirLoc = segDirLoc.unit();

        Amg::Vector3D TubSegLoc ( segPosLoc - tubePosLoc ) ;

        Amg::Vector3D segDirLocprojected = segDirLoc - (tubeDirLoc.dot(segDirLoc)) * tubeDirLoc ;
        segDirLocprojected = segDirLocprojected.unit();

        double ImpactParameter =  tubeDirLoc.dot(TubSegLoc.cross(segDirLocprojected)) ;
        
        double ScaleDenomi = 1. - std::pow( tubeDirLoc.dot(segDirLoc) ,2) ;
        double ScaleOnTube = ( tubeDirLoc.dot(TubSegLoc) - (  segDirLoc.dot(TubSegLoc) )*( tubeDirLoc.dot(segDirLoc) ) )/ScaleDenomi ;
        double ScaleOnSeg  = (- segDirLoc.dot(TubSegLoc) + ( tubeDirLoc.dot(TubSegLoc) )*( tubeDirLoc.dot(segDirLoc) ) )/ScaleDenomi ;

        Amg::Vector3D tubePosLocAtDCA = tubePosLoc + ScaleOnTube * tubeDirLoc ;
        Amg::Vector3D segPosLocAtDCA  = segPosLoc  + ScaleOnSeg  * segDirLoc ;

//        Amg::Vector3D tubePosAtDCA = gToStation.inverse()*tubePosLocAtDCA;
        Amg::Vector3D segPosAtDCA  = gToStation.inverse()*segPosLocAtDCA ;


	// global and local position of rot 
	Amg::Vector3D trk_pos_rot( mrot->globalPosition());
	Amg::Vector3D trk_pos_loc_rot = gToStation*trk_pos_rot;

	double rtrk = cosin*( mdtSeg->position().y()- tubePosLoc.y() ) - sinus*( mdtSeg->position().z() - tubePosLoc.z() ) ;
        
 
// Recalculate Point of closest approach (taking local x from second measurments)
	double locx = trk_pos_loc_rot.x();
	double locy =  tubePosLoc.y() + cosin*rtrk;
	double locz =  tubePosLoc.z() - sinus*rtrk;
        Amg::Vector3D trk_pos_loc(locx,locy,locz);
        Amg::Vector3D trk_pos =  gToStation.inverse()*trk_pos_loc; 
	ATH_MSG_DEBUG( "  trk_pos_loc_rot " << trk_pos_loc_rot << " tubePosLoc " << tubePosLoc << " trk_pos_loc " <<  trk_pos_loc << "  trk_pos_rot " <<  trk_pos_rot  );

	ATH_MSG_DEBUG( " standard rtrk " << rtrk << " ImpactParameter " << ImpactParameter  << " diff rtrk " << rtrk-ImpactParameter << " trk_pos " << trk_pos << " OR segPosAtDCA " << segPosAtDCA  );

        if (fabs(rtrk-ImpactParameter) > 0.001)  ATH_MSG_DEBUG( " ALARM Impact parameter difference " <<  rtrk-ImpactParameter   );

        //Alternative
        if ( seg.author() == 3 || m_newImpactParameter) {
          rtrk    = ImpactParameter    ;
          trk_pos = segPosAtDCA ;
        }
      
	ATH_MSG_DEBUG( "MDT RIO tdc " << prd->tdc() << " adc " << prd->adc() 
                       << " r_time " << rot->localParameters()[Trk::driftRadius]
                       << " r_track " << rtrk  );
	
       	tubePosLoc[Trk::locX]=trk_pos_loc.x();

        //Alternative
        if ( seg.author() == 3 ) {
          tubePosLoc = tubePosLocAtDCA ;
        }

//	const Trk::LocalPosition* lpos = prd->detectorElement()->surface(id).globalToLocal(mrot->globalPosition(), 5000 );

	double xLocTwin(-99999999.);

	// Store local twin tube coordinate 
	if( prd->localPosition()[Trk::locY] ){
	  Identifier test_prd_Id = prd->detectorElement()->identify();
	  ATH_MSG_DEBUG( " Twin Position :  prd->localPosition()[Trk::locY] = " <<  prd->localPosition()[Trk::locY] 
                         << " in station " << m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(test_prd_Id))
                         << "  multilayer = " << m_mdtIdHelper->multilayer(test_prd_Id) << "  layer = " << m_mdtIdHelper->tubeLayer(test_prd_Id)
                         << " tube = " <<  m_mdtIdHelper->tube(test_prd_Id) << "  modulo4 = " << (m_mdtIdHelper->tube(test_prd_Id)%4)  );
	  
      
	 Amg::Vector3D lposTrking(0.,0., prd->localPosition()[Trk::locY]);
         Amg::Vector3D gposAMDB = detEl->surface(id).transform()*lposTrking;
         Amg::Vector3D lposAMDB = detEl->GlobalToAmdbLRSTransform()*gposAMDB;
	 ATH_MSG_DEBUG( " CHECK lposTrking = " <<    lposTrking.z() << " lposAMDB " << lposAMDB.x()  );
          
	  xLocTwin = lposAMDB.x();
	}

	Amg::Vector3D tubePos    = gToStation.inverse()*tubePosLoc;
//      If the wire is rotated wrt segmentsurface we need this transform
//	Amg::Vector3D tubePos    = detEl->surface(id).transform()*tubePosLoc;

	// 	std::cout << " loc pos " << tubePosLoc << " gpos " << tubePos << std::endl;

	// get distance to readoud from detector manager
	double distRo_det = detEl->distanceFromRO(mrot->globalPosition(), id);
	  
	// create new MdtCalibHit
	MdtCalibHit calibHit( id, prd->tdc(), prd->adc(), tubePos, tubePosLoc, detEl);
	calibHit.setGlobalPointOfClosestApproach(trk_pos);

	calibHit.setLocalPos(tubePosLoc);
	calibHit.setLocXtwin(xLocTwin);

	MdtCalibrationSvcSettings settings;
	// Copy settings from ROT
	// Window lower & upper bounds not set yet
	const Muon::MuonDriftCircleErrorStrategy& rotErrorStrategy = mrot->errorStrategy();
	settings.doTof = rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::TofCorrection);
	settings.doProp = rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::PropCorrection);
	settings.doTemp = rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::TempCorrection);
	settings.doField = rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::MagFieldCorrection);
	settings.doWireSag = rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::WireSagTimeCorrection);
	settings.doSlew = rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::SlewCorrection);
	settings.doBkg = rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::BackgroundCorrection);
	settings.windowSetting = rotErrorStrategy.calibWindow();
	settings.initialize();
	bool apply_t0=((m_updateForT0Shift<0) ? rotErrorStrategy.creationParameter(Muon::MuonDriftCircleErrorStrategy::T0Refit) : static_cast<bool>(m_updateForT0Shift)) && seg.hasFittedT0();
	if(apply_t0 && (t0Shift==0.0))
		{
		t0Shift=seg.time();
		}
	MdtCalibrationSvcInput input;
	// if( m_doTof ) input.tof = tubePos.mag()/299.792458;
	if( m_doTof ) input.tof = calibHit.globalPointOfClosestApproach().mag()*(1./299.792458); 
	input.trackDirection = &seg.globalDirection(); 
 	
	input.pointOfClosestApproach = &calibHit.globalPointOfClosestApproach();
	bool sameChamber = false; 
	if (cachedId.is_valid()) { 
	  sameChamber = (m_mdtIdHelper->stationName(id) == m_mdtIdHelper->stationName(cachedId)) && (m_mdtIdHelper->stationEta(id) == m_mdtIdHelper->stationEta(cachedId)) && (m_mdtIdHelper->stationPhi(id) == m_mdtIdHelper->stationPhi(cachedId)); 
	} 
	if (!sameChamber) ATH_MSG_DEBUG( "Moving to a new chamber! " << cachedId << " to " << id  );
	// We're done with the cached Id for now, so immediately reassign it 
	cachedId = id; 
	
	if (t0Shift == 0 || seg.author() !=3 || sameChamber) { 
	  // There is one t0 shift for the whole segment - only one calibration is needed 
	  input.tof += t0Shift; 
	  ATH_MSG_DEBUG( "Author " << seg.author() << " added single t0 shift of " << t0Shift  );
	} else { 
	  segment_with_multiple_t0s=true;
	  // We may be in a new chamber, with a different fitted t0 
	  m_calibSvc->driftRadiusFromTime( calibHit, input, settings ); 
	  
	  // Reset the value of the t0 shift 
	  t0Shift = calibHit.driftTime() - mrot->driftTime(); 
	  input.tof += t0Shift; 
	  ATH_MSG_DEBUG( "t0 shift updated to " << t0Shift  );
	  
	  if( fabs( seg.time() - t0Shift ) > 0.01 && fabs(t0Shift) > 0.01 ){ 
	    ATH_MSG_INFO( " Inconsistent fitted t0 found: from ROT " << t0Shift << " from segment " << seg.time()  );
	  } 
	  
	}

	// Calculate drift radius from drift time using MdtCalibrationSvc
	double oldDriftTime = calibHit.driftTime(); // 0 unless we shift t0
	m_calibSvc->driftRadiusFromTime( calibHit, input, settings );
	
	double timeDif = calibHit.driftTime() - mrot->driftTime();
	
	// Store Sign of DriftRadius from Tracking (ROT) convention 
	float driftR = calibHit.driftRadius();
	float sigmaDriftR = calibHit.sigmaDriftRadius();
	if ( rot->localParameters()[Trk::driftRadius] < 0) {
	  driftR = - driftR;
	  calibHit.setDriftRadius(driftR,sigmaDriftR);
	}

	if( fabs(timeDif) >= 0.1 && !segment_with_multiple_t0s) {
	  ATH_MSG_WARNING( " Bad T0Shift " << t0Shift << "  cor " << timeDif
                           << " ROT " << mrot->driftRadius()  << " t  " << mrot->driftTime() 
                           << " calib " << calibHit.driftRadius() << " t " << calibHit.driftTime()
                           << " old " << oldDriftTime << " author " << seg.author()  );
	}
	if(fabs(mrot->driftRadius() - calibHit.driftRadius()) > 0.01 && !segment_with_multiple_t0s)
		{
                  ATH_MSG_WARNING( "Detected radius difference> 10 mu. MROT r= " <<mrot->driftRadius()<<" calib r="<<calibHit.driftRadius() );
		}
	ATH_MSG_DEBUG( "B-field correction: " << calibHit.lorentzTime()  );

	//        std::cout << " Drift radius ROT " << rot->localParameters()[Trk::driftRadius] << " Calibsvc " << driftR <<std::endl;
	// fill distance to track
	calibHit.setDistanceToTrack( rtrk, 0. );
	    
	// set distance to readout 
	calibHit.setDistanceToReadout( distRo_det );

	// convert MdtCalibHit to MdtCalibHitBase and then delete it
	// add hit to MuonSegment

        double resi = fabs(driftR) - fabs(rtrk);
        if (rtrk < 0) resi = -resi;
        double error2 = rot->localCovariance()(0,0);
        double chi2c = (resi*resi)/error2;
        chi2check+= chi2c;
        if(msgLvl(MSG::DEBUG) && seg.author() == 4 ) {
// Craig Blocker his checks
          MuonFixedId fixid = m_idToFixedIdTool->idToFixedId(id);
          std::string st = fixid.stationNumberToFixedStationString(fixid.stationName());
          int ml = fixid.mdtMultilayer();
          int la = fixid.mdtTubeLayer();
          ATH_MSG_DEBUG( " station " << st << " eta " << fixid.eta() << " phi " << fixid.phi() << " ML " << ml << " Layer " << la << " drift R " << driftR  << " MROT drift R " << mrot->driftRadius() << " drift Time " << mrot->driftTime() << " ROT error " << sqrt(error2) << " residual " << resi << " tubePosLoc " << tubePosLoc << " t0 shift " << t0Shift << " chi2c " << chi2c  );
          if (sqrt(error2) < 1.999 ) ATH_MSG_DEBUG( " ALARM TOO SMALL drift error "  );
          if (chi2c > qualityFactor)   ATH_MSG_DEBUG( " ALARM TOO LARGE chi2 single hit "  );
        }  
	MdtCalibHitBase* basehit = calibHit.hitBase(*m_idToFixedIdTool);
	basehit->setSegmentT0Applied(apply_t0);
	mdtSeg->addHitOnTrack(basehit);

	// 	std::cout << " MDT hit " << calibHit << std::endl;
	// 	std::cout << " base hit " << *basehit << std::endl;

      }else if( m_rpcIdHelper->is_rpc( id ) ){
	// rpc ROT
	++nr;

        int nRios = 1;

	if (competingRio) nRios = rotc->numberOfContainedROTs(); 
        for (int irio=0; irio < nRios; ++irio) { 
// Loop over competing Rios or Rios  
	  if (competingRio) rot = &rotc->rioOnTrack(irio);

	  if( msgLvl(MSG::DEBUG) ) {
	    if (!competingRio) ATH_MSG_DEBUG( "Found RPC Rio !"  );
	    if (competingRio) ATH_MSG_DEBUG( "Found RPC Competing Rio !"  );
	  }

          const Muon::RpcClusterOnTrack* rrot = dynamic_cast<const Muon::RpcClusterOnTrack*>(rot);
	  if( !rrot ){
	    ATH_MSG_WARNING( "This is not a  RpcClusterOnTrack!!! "  );
	    continue;
  	  }

  	  const Muon::RpcPrepData* rprd = rrot->prepRawData();
	  id = rprd->identify();
	//	m_rpcIdHelper->print(id);
	  int nStrips = rprd->rdoList().size();
	// get detector element
	  const MuonGM::RpcReadoutElement* detEl = rprd->detectorElement();

	
  	  double stripWidth =  detEl->StripWidth( m_rpcIdHelper->measuresPhi(id ));
	  double time = rprd->time();
	  double error = sqrt( rrot->localCovariance()(0,0) );
	  Amg::Vector3D rgp = rrot->globalPosition();
//          if (competingRio) {
//            error = sqrt( rotc->localCovariance()(0,0) );
//	    rgp = rotc->globalPosition();
//          }
	  Amg::Vector3D rlp = gToStation*rgp;

	// get strip lengths
	  double stripLen = detEl->StripLength(m_rpcIdHelper->measuresPhi(id));

	  double distRO;
	  if( m_rpcIdHelper->measuresPhi(id) ){
	    distRO = detEl->distanceToPhiReadout( rgp, id );
  	  }else{
	    distRO = detEl->distanceToEtaReadout( rgp, id );
	  }

	  RpcCalibHitBase* rpcCH = new RpcCalibHitBase( nStrips, stripWidth, time, error, rgp, rlp );
	
	  MuonFixedId fixid = m_idToFixedIdTool->idToFixedId(id);
	  rpcCH->setIdentifier( fixid);
  	  rpcCH->setStripLength(stripLen);
	  rpcCH->setDistanceToRO(distRO);
	
	  mdtSeg->addHitOnTrack( rpcCH );
        }
      }else if( m_tgcIdHelper->is_tgc( id ) ){
	++nt;

        int nRios = 1;

	if (competingRio) nRios = rotc->numberOfContainedROTs(); 
        for (int irio=0; irio < nRios; ++irio) { 
// Loop over competing Rios or Rios  
	  if (competingRio) rot = &rotc->rioOnTrack(irio);

	  if( msgLvl(MSG::DEBUG) ) {
	    if (!competingRio) ATH_MSG_DEBUG( "Found TGC Rio !"  );
	    if (competingRio) ATH_MSG_DEBUG( "Found TGC Competing Rio !"  );
	  }

 	  const Muon::TgcClusterOnTrack* trot = dynamic_cast<const Muon::TgcClusterOnTrack*>(rot);
	  if( !trot ){
            ATH_MSG_WARNING( "This is not a  TgcClusterOnTrack!!! "  );
	  continue;
	  }

	  const Muon::TgcPrepData* tprd = trot->prepRawData();
	  id = tprd->identify();
	  ATH_MSG_DEBUG( "TGC RIO "  );
	//	m_tgcIdHelper->print(id);

	  int nStrips = tprd->rdoList().size();
//          if (competingRio) {
//           nStrips = rotc->numberOfContainedROTs();
//          }

	  double stripWidth;
	  bool measuresPhi = (bool) m_tgcIdHelper->isStrip(tprd->identify());
	  int gasGap = m_tgcIdHelper->gasGap(tprd->identify());
	  int channel = m_tgcIdHelper->channel(tprd->identify());
	  const MuonGM::TgcReadoutElement* detEl = tprd->detectorElement();
	  if (!measuresPhi){
	    stripWidth = detEl->gangMaxZ(gasGap,channel)-detEl->gangMinZ(gasGap,channel);
  	  }else{
	    Amg::Vector3D localPos = detEl->transform(tprd->identify()).inverse()*detEl->channelPos(tprd->identify());
	    stripWidth = detEl->stripMaxX(gasGap, channel, localPos.y())-detEl->stripMinX(gasGap, channel, localPos.y());
	  }	 

	  double error = sqrt( trot->localCovariance()(0,0) );
	  Amg::Vector3D tgp = trot->globalPosition();
//        if (competingRio) {
//          error = sqrt( rotc->localCovariance()(0,0) );
//          tgp = rotc->globalPosition();
//        }

	  Amg::Vector3D tlp = gToStation*tgp;
	  TgcCalibHitBase* tgcCH = new TgcCalibHitBase( nStrips, stripWidth, error, tgp, tlp );

	  MuonFixedId fixid = m_idToFixedIdTool->idToFixedId(id);

	  tgcCH->setIdentifier( fixid);

	  mdtSeg->addHitOnTrack( tgcCH );
        }
      }else if( m_cscIdHelper->is_csc( id ) ){
	++nc;

        int nRios = 1;

	if (competingRio) nRios = rotc->numberOfContainedROTs(); 
        for (int irio=0; irio < nRios; ++irio) { 
// Loop over competing Rios or Rios  
	  if (competingRio) rot = &rotc->rioOnTrack(irio);
          ATH_MSG_DEBUG( (competingRio ?
                          "Found CSC Competing Rio !" :
                          "Found CSC Rio !" ) );
	  const Muon::CscClusterOnTrack* crot = dynamic_cast<const Muon::CscClusterOnTrack*>(rot);
 	  if( !crot ){
            ATH_MSG_WARNING( "This is not a  CscClusterOnTrack!!!"  );
 	   continue;
  	  }

  	  const Muon::CscPrepData* cprd = crot->prepRawData();
	  Identifier id = cprd->identify();
	// m_cscIdHelper->print(id);

	  int nStrips = cprd->rdoList().size();
//        if (competingRio) {
//          nStrips = rotc->numberOfContainedROTs();
//        }

	  int measuresPhi    = m_cscIdHelper->measuresPhi(id);
	  int chamberLayer   = m_cscIdHelper->chamberLayer(id);
	  double stripWidth   = cprd->detectorElement()->cathodeReadoutPitch( chamberLayer, measuresPhi );
	  int charge = cprd->charge();

	  double error = sqrt( crot->localCovariance()(0,0) );
	  Amg::Vector3D cgp = crot->globalPosition();
//          if (competingRio) {
//            error = sqrt( rotc->localCovariance()(0,0) );
//            cgp = rotc->globalPosition();
//          }

  	  Amg::Vector3D clp = gToStation *cgp;
	  CscCalibHitBase* cscCH = new CscCalibHitBase( nStrips, stripWidth, charge, error, cgp, clp );

	  MuonFixedId fixid = m_idToFixedIdTool->idToFixedId(id);

	  cscCH->setIdentifier( fixid);

	  mdtSeg->addHitOnTrack( cscCH );
          ATH_MSG_DEBUG( "mdtSeg->cscHitsOnTrack()=" << mdtSeg->cscHitsOnTrack()  );
          ATH_MSG_DEBUG( "mdtSeg->hitsOnTrack()=" << mdtSeg->hitsOnTrack()  );
	// set the global to amdb transform in case of first hit
	//if( rit == rots.begin() ) {
	  // global to AMDB transform
	//  gToStation = prd->detectorElement()->GlobalToAmdbLRSTransform();
	//}
       }

      }else{
	ATH_MSG_WARNING( "ERROR unknown RIO type " );
      }
   
    }
      ATH_MSG_VERBOSE( "Number of *&* mdt " << nm << " rpc " << nr 
                       << " tgc " << nt << " csc " << nc  );

    // add magnetic-field entries for MDT hits //

      if (msgLvl(MSG::DEBUG) && seg.author() == 4 ) {
       if (qualityFactor > 0.0001) {
         if (chi2check/qualityFactor > 1.01 || chi2check/qualityFactor < 0.99) {
           ATH_MSG_DEBUG( " ALARM wrong chi2 " << "Mdt segment chi2 " << qualityFactor << " mdt hits " << nm << " chi2check " << chi2check << " t0Shift " << t0Shift  );
         }  else {   
           ATH_MSG_DEBUG( " good chi2 " << "Mdt segment chi2 " << qualityFactor << " mdt hits " << nm << " chi2check " << chi2check << " t0Shift " << t0Shift  );
         }
       } else {
         ATH_MSG_DEBUG( " good chi2 " << "Mdt segment chi2 " << qualityFactor << " mdt hits " << nm << " chi2check " << chi2check << " t0Shift " << t0Shift  );
       }
    }

    return mdtSeg;
  } // createMuonCalibSegment

  MuonCalibPattern* MuonSegmentToCalibSegment::createMuonCalibPattern(const Muon::MuonPatternCombination* pat)const{
    ATH_MSG_VERBOSE( "createMuonCalibPattern"  );
    MuonCalibPattern* calibpat = 0;

    if( pat ) {
      
      const Trk::TrackParameters* trkparameters = pat->trackParameter(); 
      double phi = 0., theta=0., r0=0., charge=0., Pinv=0., z0=0.;
      const Trk::Perigee* perigee = 0;
      perigee = dynamic_cast<const Trk::Perigee*>(trkparameters);
      if (perigee)
	{
	  const AmgVector(5) parameters = perigee->parameters();
	  phi = parameters[Trk::phi];
	  theta = parameters[Trk::theta];
	  r0 = - perigee->position().y()*std::cos(phi) + perigee->position().x()*std::sin(phi) ;
	  charge = 1.;
	  if ( r0 < 0) charge = -1.;
	  
	  // Approximate conversion of radius of curvature to Pinv in MeV-1
	  Pinv = (10.*charge)/(perigee->momentum().mag());
	  z0 = perigee->position().z();
          ATH_MSG_DEBUG( " r0,z0 " << r0 << " " << z0 << " phi,theta " << phi << " " << theta   );
          ATH_MSG_DEBUG( " pat "  << perigee->position() << " " << perigee->momentum() );
	}
      else {
        ATH_MSG_WARNING( "Trackparameters are not set or is not a Perigee!! Pattern gets empty parameters"  );
      }
      
      //Here I have to add the nmdt, nrpc, ntgc and ncsc...
      unsigned int nmdt = 0, nrpc = 0, ntgc = 0, ncsc = 0;
      
      const std::vector< Muon::MuonPatternChamberIntersect > mpcivec = pat->chamberData();
      std::vector< Muon::MuonPatternChamberIntersect >::const_iterator pat_it = mpcivec.begin();
      for (;pat_it!=mpcivec.end();++pat_it)
	{
	  const std::vector< const Trk::PrepRawData* > prdvec = (*pat_it).prepRawDataVec();
	  std::vector< const Trk::PrepRawData* >::const_iterator prd_it = prdvec.begin();
	  for( ; prd_it!= prdvec.end() ;++prd_it ) {
	    Identifier id = (*prd_it)->identify();
	    if( m_mdtIdHelper->is_mdt(id) ) {
	      nmdt += 1000; //a mdt is always an eta-hit.
	    } else if ( m_rpcIdHelper->is_rpc(id) ) {
	      if      ( m_rpcIdHelper->measuresPhi(id) ) nrpc += 1;
	      else    nrpc += 1000 ;
	    } else if ( m_tgcIdHelper->is_tgc(id) ) {
	      if      ( m_tgcIdHelper->isStrip(id) ) ntgc +=    1 ;
	      else    ntgc += 1000 ;
	    } else if ( m_cscIdHelper->is_csc(id) ) {
	      if      ( m_cscIdHelper->measuresPhi(id) ) ncsc += 1 ;
	      else    ncsc += 1000 ;
	    } else    ATH_MSG_INFO( "PrepRawData on pat is not a muon-technom_logy"  );
	  }
	}
      calibpat = new MuonCalibPattern(0.,z0,r0,Pinv,phi,theta, nmdt, nrpc, ntgc, ncsc);
    }else{
      ATH_MSG_DEBUG( "WARNING, empty muoncalibpattern created"  );
      calibpat = new MuonCalibPattern();
    }
    return calibpat;
  }

  unsigned int MuonSegmentToCalibSegment::getQuality( const Muon::MuonSegment& seg ) const
  {
    ATH_MSG_DEBUG( " plotting quality "  );
    // try to dynamic_cast to MdtSegmentQuality in order to obtain quality

    const Muon::MuonSegmentQuality* q = dynamic_cast<const Muon::MuonSegmentQuality*>(seg.fitQuality());

    if( !q ) {
//    NO quality available for CSC   std::cout << "dynamic_cast MdtSegmentQuality failed" << std::endl;
      return 0;
    }
    ATH_MSG_DEBUG( "Got MuonSegmentQuality "
                   << " hots " << q->numberDoF()+2
                   << " number of holes " << q->channelsWithoutHit().size()  );
    
    unsigned int packedInfo = 0;
    
    packedInfo += 100*( q->channelsWithoutHit().size() < 9 ? q->channelsWithoutHit().size() : 9 );

    ATH_MSG_DEBUG( " packedInfo " << packedInfo  );
    return packedInfo;
  }

}

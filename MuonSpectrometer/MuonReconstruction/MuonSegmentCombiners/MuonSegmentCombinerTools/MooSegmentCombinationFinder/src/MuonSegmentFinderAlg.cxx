/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentFinderAlg.h"
#include "CscSegmentMakers/ICscSegmentFinder.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPatternSegmentMaker/MuonPatternCalibration.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"

#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

using HepGeom::Transform3D;
MuonSegmentFinderAlg::MuonSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  m_patternCalibration("Muon::MuonPatternCalibration/MuonPatternCalibration"),
  m_patternSegmentMaker("Muon::MuonPatternSegmentMaker/MuonPatternSegmentMaker"),
  m_segmentMaker("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
  m_clusterSegMaker("Muon::MuonClusterSegmentFinder/MuonClusterSegmentFinder"),
  m_segmentOverlapRemovalTool("Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool"),
  m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
  m_clusterSegMakerNSW("Muon::MuonClusterSegmentFinderTool/MuonClusterSegmentFinderTool"),
  m_truthSummaryTool("Muon::MuonTruthSummaryTool/MuonTruthSummaryTool"),
  m_csc2dSegmentFinder("Csc2dSegmentMaker/Csc2dSegmentMaker"),
  m_csc4dSegmentFinder("Csc4dSegmentMaker/Csc4dSegmentMaker")
{  
  //tools
  declareProperty("EDMPrinter", m_printer);
  declareProperty("IdHelper", m_idHelperTool);
  declareProperty("MuonPatternCalibration", m_patternCalibration);
  declareProperty("MuonPatternSegmentMaker", m_patternSegmentMaker);
  declareProperty("SegmentMaker",m_segmentMaker);
  declareProperty("ClusterCreator",m_clusterCreator);
  declareProperty("MuonClusterSegmentFinderTool",m_clusterSegMakerNSW);
  declareProperty("MuonTruthSummaryTool",m_truthSummaryTool);
  declareProperty("Csc2dSegmentMaker", m_csc2dSegmentFinder);
  declareProperty("Csc4dSegmentMaker", m_csc4dSegmentFinder);
  declareProperty("PrintSummary",m_printSummary = false);
  declareProperty("MuonClusterSegmentFinder",m_clusterSegMaker);
  //
  declareProperty("UseNSWMode",m_useNSWMode = false);
  declareProperty("doTGCClust",m_doTGCClust = false);
  declareProperty("doRPCClust",m_doRPCClust = false);
  declareProperty("doClusterTruth",m_doClusterTruth=false);

}

MuonSegmentFinderAlg::~MuonSegmentFinderAlg()
{

}

StatusCode MuonSegmentFinderAlg::initialize()
{
  if (AthAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(m_idHelperTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not get " << m_idHelperTool );
    return StatusCode::FAILURE;
  }   
  if(m_printer.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not get " << m_printer );
    return StatusCode::FAILURE;
  }   
  if(m_patternCalibration.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Could not get " << m_patternCalibration ); 
    return StatusCode::FAILURE; 
  }
  if(m_patternSegmentMaker.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Could not get " << m_patternSegmentMaker ); 
    return StatusCode::FAILURE; 
  }
  ATH_CHECK(m_segmentOverlapRemovalTool.retrieve());

 
  if( m_segmentMaker.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not get " << m_segmentMaker );
    return StatusCode::FAILURE;
  }
  if( m_clusterSegMaker.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not get " << m_clusterSegMaker);
    return StatusCode::FAILURE;
  }  
  if( !m_truthSummaryTool.empty() && m_truthSummaryTool.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not get " << m_truthSummaryTool );
    return StatusCode::FAILURE;
  }  
  if( m_clusterCreator.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not get " << m_clusterCreator );
    return StatusCode::FAILURE;
  }
  if( m_clusterSegMakerNSW.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not get " << m_clusterSegMakerNSW );
    return StatusCode::FAILURE;
  }

  if (!m_csc2dSegmentFinder.empty() && m_csc2dSegmentFinder.retrieve().isFailure()){
    ATH_MSG_FATAL("Could not get " << m_csc2dSegmentFinder); 
    return StatusCode::FAILURE;
  }
      
  if (!m_csc4dSegmentFinder.empty()&&m_csc4dSegmentFinder.retrieve().isFailure()){
    ATH_MSG_FATAL("Could not get " << m_csc4dSegmentFinder); 
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_segmentCollectionKey.initialize() );
  ATH_CHECK( m_cscPrdsKey.initialize() );
  ATH_CHECK( m_mdtPrdsKey.initialize(m_doTGCClust || m_doRPCClust) );
  ATH_CHECK( m_rpcPrdsKey.initialize(m_doRPCClust) );
  ATH_CHECK( m_tgcPrdsKey.initialize(m_doTGCClust) );
  ATH_CHECK( m_patternCollKey.initialize() );
  ATH_CHECK( m_tgcTruth.initialize(m_doClusterTruth) );
  ATH_CHECK( m_rpcTruth.initialize(m_doClusterTruth) );


  return StatusCode::SUCCESS; 
}

StatusCode MuonSegmentFinderAlg::execute()
{
  
  // vector to hold segments
  std::vector<const Muon::MuonSegment*> segs;

  //check for the Muon Layer Hough storegate container
  if( evtStore()->contains<MuonPatternCombinationCollection>("MuonLayerHoughCombis") ) {
    
    SG::ReadHandle<MuonPatternCombinationCollection> patternColl(m_patternCollKey);
    if(!patternColl.isValid()) {
      ATH_MSG_FATAL( "Could not to retrieve the PatternCombinations from StoreGate" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ( "Processing the pattern collections with  " << patternColl->size() << " Collections " );

    for(MuonPatternCombinationCollection::const_iterator patt=patternColl->begin(); patt!=patternColl->end(); ++patt) {
      ATH_MSG_DEBUG( "Working on pattern combination " << m_printer->print( **patt ) );
      //check the technology & call the corresponding segment finder
      createSegmentsWithMDTs( *patt, segs );
      createSegmentsFromClusters( *patt, segs );
    }//end loop on pattern combinations

  }//end SG container check
  
  //do cluster based segment finding
  std::vector<const Muon::MuonSegment*>* clustSegs(NULL);
  if (m_doTGCClust || m_doRPCClust){
    SG::ReadHandle<Muon::MdtPrepDataContainer> mdtPrds(m_mdtPrdsKey);
    const Muon::RpcPrepDataContainer* rpcPrdCont=0;
    const Muon::TgcPrepDataContainer* tgcPrdCont=0;
    const PRD_MultiTruthCollection* tgcTruthColl=0;
    const PRD_MultiTruthCollection* rpcTruthColl=0;
    if(m_doTGCClust){
      SG::ReadHandle<Muon::TgcPrepDataContainer> tgcPrds(m_tgcPrdsKey);
      tgcPrdCont=tgcPrds.cptr();
    }
    if(m_doRPCClust){
      SG::ReadHandle<Muon::RpcPrepDataContainer> rpcPrds(m_rpcPrdsKey);
      rpcPrdCont=rpcPrds.cptr();
    }
    if(m_doClusterTruth){
      SG::ReadHandle<PRD_MultiTruthCollection> tgcTruth(m_tgcTruth);
      SG::ReadHandle<PRD_MultiTruthCollection> rpcTruth(m_rpcTruth);
      tgcTruthColl=tgcTruth.cptr();
      rpcTruthColl=rpcTruth.cptr();
    }
    clustSegs = m_clusterSegMaker->getClusterSegments(mdtPrds.cptr(),rpcPrdCont,tgcPrdCont,tgcTruthColl,rpcTruthColl);
  }
  if (clustSegs) {
    segs.insert(segs.end(),clustSegs->begin(),clustSegs->end());
    delete clustSegs;
  }

  std::vector<const Muon::MuonSegment*> resolvedSegments = m_segmentOverlapRemovalTool->removeDuplicates(segs);
  
  //create a new SG container to store the segments
  std::unique_ptr<Trk::SegmentCollection> segmentCollection = std::unique_ptr<Trk::SegmentCollection>( new Trk::SegmentCollection());
  segmentCollection->reserve(resolvedSegments.size());
  for( std::vector<const Muon::MuonSegment*>::iterator sit = resolvedSegments.begin(); sit!=resolvedSegments.end();++sit ){
    segmentCollection->push_back( const_cast<Muon::MuonSegment*>(*sit) );
    if( !m_truthSummaryTool.empty() ) m_truthSummaryTool->add(**sit,3);
  }
  ATH_MSG_DEBUG(" Segments after overlap removal: " << resolvedSegments.size() << " removed " << segs.size() - resolvedSegments.size() );

  // need to delete removed segments
  if( segs.size() != resolvedSegments.size() ){
    for( auto seg : segs ){
      auto pos = std::find(resolvedSegments.begin(),resolvedSegments.end(),seg);
      if( pos == resolvedSegments.end() ) delete seg;
    }
  }

  if( !m_csc2dSegmentFinder.empty() && !m_csc4dSegmentFinder.empty() ){

    std::vector<const Muon::CscPrepDataCollection*> cscCols;

    SG::ReadHandle<Muon::CscPrepDataContainer> cscPrds(m_cscPrdsKey);

    if (cscPrds.isValid()) {
      
      Muon::CscPrepDataContainer::const_iterator it = cscPrds->begin();
      Muon::CscPrepDataContainer::const_iterator it_end = cscPrds->end();
      for( ; it!=it_end; ++it ) {
	// skip empty collections
	if( (*it)->empty() ) continue;
	cscCols.push_back( *it );
      }
      ATH_MSG_DEBUG("Retrieved CscPrepDataContainer " <<  cscCols.size());
      // reconstruct segments in the CSC eta and phi plane
      MuonSegmentCombinationCollection* csc2dSegmentCombinations = m_csc2dSegmentFinder->find( cscCols );
      // combine CSC segments in eta and phi plane if any were found
      if (csc2dSegmentCombinations) {
	MuonSegmentCombinationCollection* csc4dSegmentCombinations = m_csc4dSegmentFinder->find( *csc2dSegmentCombinations );
	delete csc2dSegmentCombinations;
	if( csc4dSegmentCombinations ){
	
	  // now copy the segments into the collection, not optimal as unneeded copy
	  MuonSegmentCombinationCollection::const_iterator cit = csc4dSegmentCombinations->begin();
	  MuonSegmentCombinationCollection::const_iterator cit_end = csc4dSegmentCombinations->end();
	  for(; cit!=cit_end;++cit ){
	    if( !*cit ) {
	      ATH_MSG_INFO(" empty MuonSegmentCombination!!! ");
	      continue;
	    }
	    const Muon::MuonSegmentCombination& combi = **cit;
	    unsigned int nstations = combi.numberOfStations();
	  
	    // loop over chambers in combi and extract segments
	    for(unsigned int i=0; i<nstations; ++i){

	      // loop over segments in station
	      const Muon::MuonSegmentCombination::SegmentVec* segments = combi.stationSegments( i ) ;
         
	      // check if not empty
	      if( !segments || segments->empty() ) continue;
	      // loop over new segments, copy them into collection
	      Muon::MuonSegmentCombination::SegmentVec::const_iterator sit = segments->begin();
	      Muon::MuonSegmentCombination::SegmentVec::const_iterator sit_end = segments->end();
	      for( ; sit!=sit_end;++sit) segmentCollection->push_back( new Muon::MuonSegment(**sit) );
	    }
	  }  
	  delete csc4dSegmentCombinations;
	}
      }
    }
  }

  if( m_printSummary || msgLvl(MSG::DEBUG) ){
    msg() << msg().level() << "Number of segments found " << resolvedSegments.size() << std::endl << m_printer->print(resolvedSegments) << endmsg;
  }
  SG::WriteHandle<Trk::SegmentCollection> handle(m_segmentCollectionKey);
  
  //Add the segments to store gate
  if(handle.record(std::move(segmentCollection)).isFailure()) {
    ATH_MSG_FATAL( "Could not add the Trk::SegmentCollection to StoreGate" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
} // execute


StatusCode MuonSegmentFinderAlg::finalize()
{
  return AthAlgorithm::finalize();
}

void MuonSegmentFinderAlg::createSegmentsFromClusters(const Muon::MuonPatternCombination* patt, std::vector<const Muon::MuonSegment*>& segments ) {
  //turn the PRD into MuonCluster
  std::map<int,std::vector<const Muon::MuonClusterOnTrack*> > clustersPerSector;
  std::vector< Muon::MuonPatternChamberIntersect >::const_iterator it = patt->chamberData().begin();
  for (; it!=patt->chamberData().end(); ++it) {
    if((*it).prepRawDataVec().empty()) continue;
    const Identifier& id = (*it).prepRawDataVec().front()->identify();
    if( !m_idHelperTool->isMM(id) && !m_idHelperTool->issTgc(id) ) continue;
    for(std::vector< const Trk::PrepRawData* >::const_iterator pit = (*it).prepRawDataVec().begin(); pit!=(*it).prepRawDataVec().end(); ++pit) {

      const Muon::MuonCluster* cl = dynamic_cast<const Muon::MuonCluster*>(*pit);
      if( !cl ) continue;
      int sector = m_idHelperTool->sector(id);
      std::vector<const Muon::MuonClusterOnTrack*>& clusters = clustersPerSector[sector];
      const Muon::MuonClusterOnTrack* clust = m_clusterCreator->createRIO_OnTrack( *cl, cl->globalPosition() );
      
      clusters.push_back(clust);
    }
  }
  
  std::map<int,std::vector<const Muon::MuonClusterOnTrack*> >::iterator sit = clustersPerSector.begin();
  std::map<int,std::vector<const Muon::MuonClusterOnTrack*> >::iterator sit_end = clustersPerSector.end();
  for( ;sit!=sit_end;++sit ){
    std::vector<const Muon::MuonClusterOnTrack*>& clusters = sit->second;
    std::vector<const Muon::MuonSegment*>* segs = m_clusterSegMakerNSW->find(clusters);

    //cleanup the memory
    for(std::vector<const Muon::MuonClusterOnTrack*>::iterator cit = clusters.begin(); cit!=clusters.end(); ++cit) {
      delete *cit;
    }
    if( segs ){
      segments.insert(segments.end(),segs->begin(),segs->end());
      delete segs;
    }    
  }
}


void MuonSegmentFinderAlg::createSegmentsWithMDTs(const Muon::MuonPatternCombination* patcomb, std::vector<const Muon::MuonSegment*>& segments ) { 

  if( m_useNSWMode ){

    //break the pattern combination into regions and calibrate the PRDs
    std::set<int> calibratedRegions;
    std::vector< Muon::MuonPatternChamberIntersect >::const_iterator it = patcomb->chamberData().begin();   
    for (; it!=patcomb->chamberData().end(); ++it) {

      if((*it).prepRawDataVec().empty()) continue;
      Identifier id = (*it).prepRawDataVec().front()->identify();
      if(m_idHelperTool->isMM(id) || m_idHelperTool->issTgc(id) ) continue;


      int regionID = m_patternCalibration->getRegionId(id);

      //check the chamber is not part of a region already calibrated
      std::pair<std::set<int>::iterator,bool> val = calibratedRegions.insert(regionID);
      if(!val.second) continue;
      ATH_MSG_VERBOSE(" Region " << regionID << " adding chamber " << m_idHelperTool->toStringChamber(id) << " size " << (*it).prepRawDataVec().size() );

      std::vector<Muon::MuonPatternChamberIntersect> chambers;
      chambers.push_back( *it );
      for(std::vector< Muon::MuonPatternChamberIntersect >::const_iterator rit=(it+1); rit!=patcomb->chamberData().end(); ++rit) {

	if((*rit).prepRawDataVec().empty()) continue;
	Identifier id2 = (*rit).prepRawDataVec().front()->identify();
	if(m_idHelperTool->isMM(id2) || m_idHelperTool->issTgc(id2) ) continue;

	int region = m_patternCalibration->getRegionId(id2);
	if(region == regionID) {
	  ATH_MSG_VERBOSE("  adding additional chamber " << m_idHelperTool->toStringChamber(id2) << " size " << (*rit).prepRawDataVec().size() );
	  chambers.push_back( *rit );		  
	}
      }

      Trk::TrackParameters* trkpars = 0;
      if( patcomb->trackParameter() ) trkpars = patcomb->trackParameter()->clone();
      Muon::MuonPatternCombination pattern( trkpars,chambers );
      ATH_MSG_VERBOSE(" created pattern for region: number of chambers " << chambers.size() );
      bool hasPhiMeasurements = m_patternCalibration->checkForPhiMeasurements( pattern );
      //calibrate the hits
      Muon::IMuonPatternCalibration::ROTsPerRegion hitsPerRegion;
      m_patternCalibration->calibrate( pattern, hitsPerRegion );      
    
      Muon::IMuonPatternCalibration::ROTsPerRegion::iterator rotit = hitsPerRegion.begin();
      for(; rotit!=hitsPerRegion.end(); ++rotit) {
	std::vector<std::vector< const Muon::MdtDriftCircleOnTrack* > >::iterator mdtit = rotit->mdts.begin();	
	for( ; mdtit!=rotit->mdts.end(); ++mdtit ){
	  if( mdtit->empty() ) continue;
	  ATH_MSG_VERBOSE("Calling segment finding for sector " << m_idHelperTool->toString(mdtit->front()->identify()) );
	  //fit the segments
	  std::vector<const Muon::MuonSegment*>* segs = m_segmentMaker->find( rotit->regionPos, rotit->regionDir, *mdtit, rotit->clusters,
									      hasPhiMeasurements, rotit->regionDir.mag() );
	  if(!segs) continue;
	  segments.insert(segments.end(),segs->begin(), segs->end());
	  delete segs;

	}//end loop on hits per region

      }//end loop on regions (rotit)
      
      //memory cleanup
      m_patternCalibration->clearRotsPerRegion( hitsPerRegion );
      //store the region ID
      calibratedRegions.insert(regionID);    

    }//end loop on chambers in the pattern
  }else{
    Muon::MuonSegmentCombination* segmentCombi = m_patternSegmentMaker->find(*patcomb);
    if( segmentCombi ){
      // loop over stations
      unsigned int nst = segmentCombi->numberOfStations();
      for( unsigned int i = 0 ;i<nst;++i ){
	if( !segmentCombi->stationSegments(i) ) continue;

	// loop over segments 
	for( auto sit : *segmentCombi->stationSegments(i) ){
	  segments.push_back( sit->clone() );
	}
      }
      delete segmentCombi;
    }
  }
}

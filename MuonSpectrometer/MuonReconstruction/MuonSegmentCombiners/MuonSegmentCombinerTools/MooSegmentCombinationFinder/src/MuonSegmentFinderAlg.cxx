/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  ATH_CHECK( m_idHelperTool.retrieve() );
  ATH_CHECK( m_printer.retrieve() );
  ATH_CHECK( m_patternCalibration.retrieve() );
  ATH_CHECK( m_patternSegmentMaker.retrieve() ); 
  ATH_CHECK( m_segmentOverlapRemovalTool.retrieve() );
  ATH_CHECK( m_segmentMaker.retrieve() );
  ATH_CHECK( m_clusterSegMaker.retrieve() );

  if( !m_truthSummaryTool.empty() )
    ATH_CHECK( m_truthSummaryTool.retrieve() );
  else
    m_truthSummaryTool.disable();
  
  ATH_CHECK( m_clusterCreator.retrieve() );
  ATH_CHECK( m_clusterSegMakerNSW.retrieve() );

  if ( !m_csc2dSegmentFinder.empty() )
    ATH_CHECK( m_csc2dSegmentFinder.retrieve() ); 
  else
    m_csc2dSegmentFinder.disable();
      
  if ( !m_csc4dSegmentFinder.empty() )
    ATH_CHECK( m_csc4dSegmentFinder.retrieve() ); 
  else
    m_csc4dSegmentFinder.disable();

  ATH_CHECK( m_segmentCollectionKey.initialize() );
  ATH_CHECK( m_cscPrdsKey.initialize() );
  ATH_CHECK( m_mdtPrdsKey.initialize(m_doTGCClust || m_doRPCClust));
  ATH_CHECK( m_rpcPrdsKey.initialize());
  ATH_CHECK( m_tgcPrdsKey.initialize());
  ATH_CHECK( m_patternCollKey.initialize() );
  ATH_CHECK( m_tgcTruth.initialize(m_doClusterTruth) );
  ATH_CHECK( m_rpcTruth.initialize(m_doClusterTruth) );

  return StatusCode::SUCCESS; 
}

StatusCode MuonSegmentFinderAlg::execute()
{
  
  // vector to hold segments
  SG::WriteHandle<Trk::SegmentCollection> handle(m_segmentCollectionKey);
  ATH_CHECK(handle.record(std::make_unique<Trk::SegmentCollection>()));

  SG::ReadHandle<Muon::TgcPrepDataContainer> tgcPrds(m_tgcPrdsKey);
  const Muon::TgcPrepDataContainer* tgcPrdCont=tgcPrds.cptr();
  SG::ReadHandle<Muon::RpcPrepDataContainer> rpcPrds(m_rpcPrdsKey);
  const Muon::RpcPrepDataContainer* rpcPrdCont=rpcPrds.cptr();

  SG::ReadHandle<MuonPatternCombinationCollection> patternColl(m_patternCollKey);
  if(!patternColl.isValid()) {
    ATH_MSG_FATAL( "Could not to retrieve the PatternCombinations from StoreGate" );
    return StatusCode::FAILURE;
  }
  if(patternColl.isPresent()){ //pretty sure this is always the case, but can't hurt to check
    ATH_MSG_DEBUG ( "Processing the pattern collections with  " << patternColl->size() << " Collections " );

    for(MuonPatternCombinationCollection::const_iterator patt=patternColl->begin(); patt!=patternColl->end(); ++patt) {
      ATH_MSG_DEBUG( "Working on pattern combination " << m_printer->print( **patt ) );
      //check the technology & call the corresponding segment finder 
      std::vector<const Muon::TgcPrepDataCollection*> tgcCols;
      std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
      tgcCols.reserve(tgcPrdCont->size());
      for(auto p : *tgcPrdCont){
	if(!p->empty()) tgcCols.push_back(p);
      }
      rpcCols.reserve(rpcPrdCont->size());
      for(auto p : *rpcPrdCont){
        if(!p->empty()) rpcCols.push_back(p);
      }
      createSegmentsWithMDTs( *patt, handle.ptr(), rpcCols, tgcCols );
      createSegmentsFromClusters( *patt, handle.ptr() );
    }//end loop on pattern combinations

  }
  else{
    ATH_MSG_DEBUG("no pattern collection, moving on ...");
  }

  //do cluster based segment finding
  if (m_doTGCClust || m_doRPCClust){
    SG::ReadHandle<Muon::MdtPrepDataContainer> mdtPrds(m_mdtPrdsKey);
    const PRD_MultiTruthCollection* tgcTruthColl=0;
    const PRD_MultiTruthCollection* rpcTruthColl=0;
    if(m_doClusterTruth){
      SG::ReadHandle<PRD_MultiTruthCollection> tgcTruth(m_tgcTruth);
      SG::ReadHandle<PRD_MultiTruthCollection> rpcTruth(m_rpcTruth);
      tgcTruthColl=tgcTruth.cptr();
      rpcTruthColl=rpcTruth.cptr();
    }
    m_clusterSegMaker->getClusterSegments(mdtPrds.cptr(),m_doRPCClust ? rpcPrdCont : 0,m_doTGCClust ? tgcPrdCont : 0,tgcTruthColl,rpcTruthColl,handle.ptr());
  }

  ATH_MSG_DEBUG("segments before overlap removal: "<<handle->size());
  m_segmentOverlapRemovalTool->removeDuplicates(handle.ptr());
  
  if(!m_truthSummaryTool.empty()){
    for(unsigned int i=0;i<handle.ptr()->size();i++){
      Trk::Segment* tseg=handle.ptr()->at(i);
      m_truthSummaryTool->add(*(dynamic_cast<const Muon::MuonSegment*>(tseg)),3);
    }
  }

  ATH_MSG_DEBUG(" Segments after overlap removal: " << handle->size());

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
      std::unique_ptr<MuonSegmentCombinationCollection> csc2dSegmentCombinations = m_csc2dSegmentFinder->find( cscCols );
      // combine CSC segments in eta and phi plane if any were found
      if (csc2dSegmentCombinations) {
	std::unique_ptr<MuonSegmentCombinationCollection> csc4dSegmentCombinations = m_csc4dSegmentFinder->find( *csc2dSegmentCombinations );
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
	      Muon::MuonSegmentCombination::SegmentVec* segments = combi.stationSegments( i ) ;

	      // check if not empty
	      if( !segments || segments->empty() ) continue;
	      // loop over new segments, copy them into collection
	      Muon::MuonSegmentCombination::SegmentVec::iterator sit = segments->begin();
	      Muon::MuonSegmentCombination::SegmentVec::iterator sit_end = segments->end();
	      for( ; sit!=sit_end;++sit) handle->push_back( (*sit).release()); //releasing so the handle can take ownership
	    }
	  }  
	}
      }
    }
  }

  ATH_MSG_DEBUG("Number of segments found " << handle->size());
  for(unsigned int i=0;i<handle.ptr()->size();i++){
    Trk::Segment* tseg=handle.ptr()->at(i);
    ATH_MSG_DEBUG(m_printer->print(*(dynamic_cast<Muon::MuonSegment*>(tseg))));
  }
  return StatusCode::SUCCESS;
} // execute


StatusCode MuonSegmentFinderAlg::finalize()
{
  return AthAlgorithm::finalize();
}

void MuonSegmentFinderAlg::createSegmentsFromClusters(const Muon::MuonPatternCombination* patt, Trk::SegmentCollection* segments ) {
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
    std::vector<Muon::MuonSegment*> segVec;
    m_clusterSegMakerNSW->find(clusters,segVec,segments);

    //cleanup the memory
    for(std::vector<const Muon::MuonClusterOnTrack*>::iterator cit = clusters.begin(); cit!=clusters.end(); ++cit) {
      delete *cit;
    }
  }
}


void MuonSegmentFinderAlg::createSegmentsWithMDTs(const Muon::MuonPatternCombination* patcomb, Trk::SegmentCollection* segs,
						  const std::vector<const Muon::RpcPrepDataCollection*> rpcCols, const std::vector<const Muon::TgcPrepDataCollection*> tgcCols) { 

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
	  m_segmentMaker->find( rotit->regionPos, rotit->regionDir, *mdtit, rotit->clusters,
				hasPhiMeasurements, segs, rotit->regionDir.mag() );

	}//end loop on hits per region

      }//end loop on regions (rotit)
      
      //memory cleanup
      m_patternCalibration->clearRotsPerRegion( hitsPerRegion );
      //store the region ID
      calibratedRegions.insert(regionID);    

    }//end loop on chambers in the pattern
  }else{
    m_patternSegmentMaker->find(*patcomb, rpcCols, tgcCols, segs);
  }
}

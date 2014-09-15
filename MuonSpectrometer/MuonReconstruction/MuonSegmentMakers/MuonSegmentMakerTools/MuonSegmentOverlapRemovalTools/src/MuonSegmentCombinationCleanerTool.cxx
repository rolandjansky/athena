/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentCombinationCleanerTool.h"
 
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>


#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

#include "MuonPattern/MuonPatternCombination.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"

#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"

#include "TrkEventPrimitives/FitQuality.h"

namespace Muon {

  MuonSegmentCombinationCleanerTool::MuonSegmentCombinationCleanerTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AlgTool(t,n,p),
    m_log(msgSvc(),name()),
    m_logLevel(0),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_idHelperTool("Muon::MuonIdHelpers/MuonIdHelperTool"),
    m_overlapRemovalTool("Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool"),
    m_assocTool("Muon::MuonPatternSegmentAssociationTool/MuonPatternSegmentAssociationTool")
  {
    declareInterface<IMuonSegmentCombinationCleanerTool>(this);

    declareProperty("SegmentOverlapRemovalTool", m_overlapRemovalTool, "tool to removal overlaps in segment combinations" );
    declareProperty("PatternAssociationTool", m_assocTool, "tool to keep track of associations between pattern and segment combinations" );
    declareProperty("MergeAllCombis", m_mergeAllCombis = false, "merge all combinations into one large combination" );

  }

  MuonSegmentCombinationCleanerTool::~MuonSegmentCombinationCleanerTool()
  {
  }

  StatusCode MuonSegmentCombinationCleanerTool::initialize()
  {
  
    StatusCode sc = AlgTool::initialize(); 
    if (sc.isFailure()) return sc;
    m_log.setLevel(outputLevel());
    m_logLevel = outputLevel();

    m_log << MSG::VERBOSE << " MuonSegmentCombinationCleanerTool::Initializing " << endreq;
  
    sc = m_printer.retrieve();
    if (sc.isSuccess()){
      m_log<<MSG::INFO << "Retrieved " << m_printer << endreq;
    }else{
      m_log<<MSG::FATAL<<"Could not get " << m_printer <<endreq; 
      return sc;
    }
    sc = m_helperTool.retrieve();
    if (sc.isSuccess()){
      m_log<<MSG::INFO << "Retrieved " << m_helperTool << endreq;
    }else{
      m_log<<MSG::FATAL<<"Could not get " << m_helperTool <<endreq; 
      return sc;
    }
    sc = m_idHelperTool.retrieve();
    if (sc.isSuccess()){
      m_log<<MSG::INFO << "Retrieved " << m_idHelperTool << endreq;
    }else{
      m_log<<MSG::FATAL<<"Could not get " << m_idHelperTool <<endreq; 
      return sc;
    }
    
    sc = m_overlapRemovalTool.retrieve();
    if (sc.isSuccess()){
      m_log<<MSG::DEBUG << "Retrieved " << m_overlapRemovalTool << endreq;
    }else{
      m_log<<MSG::FATAL<<"Could not get " << m_overlapRemovalTool <<endreq; 
      return sc;
    }

    sc = m_assocTool.retrieve();
    if (sc.isSuccess()){
      m_log<<MSG::DEBUG << "Retrieved " << m_assocTool << endreq;
    }else{
      m_log<<MSG::FATAL<<"Could not get " << m_assocTool <<endreq; 
      return sc;
    }

    m_log << MSG::VERBOSE << "End of Initializing" << endreq;  
    return StatusCode::SUCCESS; 
  }

  MuonSegmentCombinationCollection* MuonSegmentCombinationCleanerTool::clean( const MuonSegmentCombinationCollection& combiCol ){

    MuonSegmentCombinationCollection* combiCleanCol = new MuonSegmentCombinationCollection;
    
    cleanAndMergeCombis(combiCol,combiCleanCol);
    
    return combiCleanCol;
  }


  StatusCode MuonSegmentCombinationCleanerTool::finalize()
  {
    m_log << MSG::VERBOSE << "finalize()" << endreq;
    return StatusCode::SUCCESS;
  }

  void MuonSegmentCombinationCleanerTool::cleanAndMergeCombis( const MuonSegmentCombinationCollection& combiCol,
							       MuonSegmentCombinationCollection* combiCleanCol ) {

    if( m_logLevel <= MSG::DEBUG ) m_log << MSG::DEBUG  << " cleaning combis " << combiCol.size() << endreq;

    MuonSegmentCombinationCollection::const_iterator cit = combiCol.begin();
    MuonSegmentCombinationCollection::const_iterator cit_end = combiCol.end();

    std::vector<MuonSegmentCombiSummary> summaries;

    for(; cit!=cit_end;++cit ){
      const Muon::MuonSegmentCombination* combi = *cit;
      if( !combi ) {
        m_log << MSG::INFO  << " empty MuonSegmentCombination!!! " << endreq;
        continue;
      }
      
      MuonSegmentCombiSummary summary = createSummary( *combi );
      if( m_logLevel <= MSG::VERBOSE ) m_log << MSG::VERBOSE << print(summary) << endreq;

      if( summary.stations.size() < 2 ){
	
	// keep combination if Endcap middle 
	if( summary.stations.count(MuonStationIndex::EM) ){

	  if( m_logLevel <= MSG::VERBOSE ) m_log << MSG::VERBOSE  << " Keeping single station endcap middle MuonSegmentCombination " << endreq;

	}else{

	  if( m_logLevel <= MSG::DEBUG ) m_log << MSG::DEBUG  << " MuonSegmentCombination has too few station layers, rejecting  " << endreq;
	  continue;
	}
      }

      if( summary.stationsGood.size() == 0 ){
	if( m_logLevel <= MSG::DEBUG ) m_log << MSG::DEBUG  << " MuonSegmentCombination has no station layer with good segment, rejecting  " << endreq;
	continue;
      }
      
      if( !subSetOfPrevious(summary,summaries) ){
	summaries.push_back(summary);
      }
    } 

    std::vector<MuonSegmentCombiSummary>::iterator sit = summaries.begin();
    std::vector<MuonSegmentCombiSummary>::iterator sit_end = summaries.end();
    for( ;sit!=sit_end;++sit ){


      // check whether there is an original, if so pass the pointer else clone the combi as it comes from the input collection
      const MuonSegmentCombination* originalCombi = 0;
      MuonSegmentCombination* newCombi = 0;
      if( sit->combiOriginal ){

	// HACK: cast away const as DataVector expects no-const pointer
	newCombi = const_cast<MuonSegmentCombination*>(&sit->segmentCombination());
	if( !newCombi ){
	  m_log << MSG::INFO  << " const_cast to MuonSegmentCombination failed " << endreq;
	  continue;
	}

	originalCombi = sit->combiOriginal;


      }else{
	originalCombi = &sit->segmentCombination();
	newCombi = new MuonSegmentCombination(sit->segmentCombination());
      }

      MuonSegmentCombination* finalCombi = const_cast<MuonSegmentCombination*>(m_overlapRemovalTool->removeDuplicates( *newCombi ));
      if( finalCombi != newCombi ) delete newCombi;

      // lookup the patterncombi and add association
      unsigned int count = m_assocTool->count(originalCombi);
      if(count != 1 ){
	m_log << MSG::INFO  << " This list should only have one entry!! " << endreq;
      }else{
	IMuonPatternSegmentAssociationTool::AssociationMapRange range = m_assocTool->find(originalCombi);
	const Muon::MuonPatternCombination* pat = count == 1 ? (range.first)->second : 0;
	if( pat ) m_assocTool->insert(finalCombi, pat);
	else m_log << MSG::INFO  << " The pattern pointer should never be zero!!!! " << endreq;
      }

      combiCleanCol->push_back( finalCombi );
      

    }
  }

   
  bool MuonSegmentCombinationCleanerTool::subSetOfPrevious( MuonSegmentCombiSummary& summary, std::vector<MuonSegmentCombiSummary>& summaries) const {
    
    if( summaries.empty() ) return false;
    
    if( m_logLevel <= MSG::DEBUG ) m_log << MSG::DEBUG  << " comparing combi with aleady handled ones " << endreq;
    bool hasOverlap = false;

    std::vector<MuonSegmentCombiSummary>::iterator sit = summaries.begin();
    std::vector<MuonSegmentCombiSummary>::iterator sit_end = summaries.end();
    for( ;sit!=sit_end;++sit ){

      MuonSegmentCombiOverlapSummary overlap = calculateOverlap( summary, *sit );

      // simplest case: no subsets
      if( overlap.subsetFirst.empty() && overlap.subsetSecond.empty() ) {
	  
	// no overlap
	if( overlap.shared.empty() ) {
	  if( !m_mergeAllCombis ) continue;
	}
	// complete overlap, keep previous
	if( overlap.uniqueFirst.empty() && overlap.uniqueSecond.empty() ){
	  if( m_logLevel <= MSG::DEBUG )  m_log << MSG::VERBOSE <<  "    -> complete overlap, keeping first " << endreq;
	  hasOverlap = true;
	  break;
	}
      }
	
      // second simple case, no unique segments in first and no subsets in second combi
      if( overlap.uniqueFirst.empty() && overlap.subsetSecond.empty() ){
	  
	if( m_logLevel <= MSG::DEBUG )  m_log << MSG::VERBOSE <<  "    -> keeping selected combi " << endreq;
	hasOverlap = true;
	break;
      }else if( overlap.uniqueSecond.empty() && overlap.subsetFirst.empty() ){
	  
	if( m_logLevel <= MSG::DEBUG )  m_log << MSG::VERBOSE <<  "    -> replacing selected combi " << endreq;
	  
	if( sit->combiOriginal ) {
	  // copy pointer to original combi, delete combi
	  delete sit->m_combi;
	}
	*sit = summary;
	hasOverlap = true;
	break;
      }

      // large overlap
      
      unsigned int overlappingSegments1 = overlap.subsetFirst.size() + overlap.shared.size();
      unsigned int overlappingSegments2 = overlap.subsetSecond.size() + overlap.shared.size();

      unsigned int uniqueSegments1      = overlap.uniqueFirst.size();
      unsigned int uniqueSegments2      = overlap.uniqueSecond.size();
      if( overlappingSegments1 > uniqueSegments1 || overlappingSegments2 > uniqueSegments2 || m_mergeAllCombis ) {
	if( m_logLevel <= MSG::DEBUG )  m_log << MSG::VERBOSE <<  "    -> large overlap, merge candidate " << endreq;

	// set to make sure we are not adding segments twice
	std::set<const MuonSegment*> addedSegments;
	std::vector< std::vector<const MuonSegment*> > segmentsPerChamberLayer(MuonStationIndex::ChIndexMax);

	// first add shared segments, take one with best chi2
	std::vector< std::pair<const MuonSegment*,const MuonSegment*> >::iterator shit = overlap.shared.begin(); 
	std::vector< std::pair<const MuonSegment*,const MuonSegment*> >::iterator shit_end = overlap.shared.end(); 
	for( ;shit!=shit_end;++shit ){
	  
	  // select segment with best chi2
	  const Trk::FitQuality* fq1 =shit->first->fitQuality();
	  const Trk::FitQuality* fq2 =shit->second->fitQuality();
	  double chi2First = fq1 ? fq1->chiSquared() : 1e10;
	  double chi2Second = fq2 ? fq2->chiSquared() : 1e10;
	  const MuonSegment* bestSegment =  chi2First < chi2Second ? shit->first : shit->second;
	  
	  // check whether already added
	  if( addedSegments.count(bestSegment) ) continue;
	  addedSegments.insert(bestSegment);
	  Identifier chId = m_helperTool->chamberId( *bestSegment );
	  MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chId);
	  segmentsPerChamberLayer[chIndex].push_back(bestSegment);
	}
	
	std::vector<const MuonSegment*>::iterator uit = overlap.uniqueFirst.begin();
	std::vector<const MuonSegment*>::iterator uit_end = overlap.uniqueFirst.end();
	for( ;uit!=uit_end;++uit ){
	  // check whether already added
	  const MuonSegment* bestSegment = *uit;
	  
	  // check whether already added
	  if( addedSegments.count(bestSegment) ) continue;
	  addedSegments.insert(bestSegment);
	  Identifier chId = m_helperTool->chamberId( *bestSegment );
	  MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chId);
	  segmentsPerChamberLayer[chIndex].push_back(bestSegment);
	}

	uit = overlap.uniqueSecond.begin();
	uit_end = overlap.uniqueSecond.end();
	for( ;uit!=uit_end;++uit ){
	  // check whether already added
	  const MuonSegment* bestSegment = *uit;
	  
	  // check whether already added
	  if( addedSegments.count(bestSegment) ) continue;
	  addedSegments.insert(bestSegment);
	  Identifier chId = m_helperTool->chamberId( *bestSegment );
	  MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chId);
	  segmentsPerChamberLayer[chIndex].push_back(bestSegment);
	}
    
	shit = overlap.subsetFirst.begin(); 
	shit_end = overlap.subsetFirst.end(); 
	for( ;shit!=shit_end;++shit ){
	  // check whether already added
	  const MuonSegment* bestSegment = shit->second;
	  
	  // check whether already added
	  if( addedSegments.count(bestSegment) ) continue;
	  addedSegments.insert(bestSegment);
	  Identifier chId = m_helperTool->chamberId( *bestSegment );
	  MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chId);
	  segmentsPerChamberLayer[chIndex].push_back(bestSegment);
	}

	shit = overlap.subsetSecond.begin(); 
	shit_end = overlap.subsetSecond.end(); 
	for( ;shit!=shit_end;++shit ){
	  // check whether already added
	  const MuonSegment* bestSegment = shit->first;
	  
	  // check whether already added
	  if( addedSegments.count(bestSegment) ) continue;
	  addedSegments.insert(bestSegment);
	  Identifier chId = m_helperTool->chamberId( *bestSegment );
	  MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chId);
	  segmentsPerChamberLayer[chIndex].push_back(bestSegment);
	}

	// now create new combi
	MuonSegmentCombination* newCombi = new MuonSegmentCombination();
	
	// loop over layers and add segments
	std::vector< std::vector<const MuonSegment*> >::iterator chit = segmentsPerChamberLayer.begin();
	std::vector< std::vector<const MuonSegment*> >::iterator chit_end = segmentsPerChamberLayer.end();
	for( ;chit!=chit_end;++chit ){
	  
	  // skip empty ones
	  if( chit->empty() ) continue;

	  std::vector< const MuonSegment* >* segVec = new std::vector< const MuonSegment* >();
	  segVec->reserve( chit->size() );
	  std::vector<const MuonSegment*>::iterator segit = chit->begin();
	  std::vector<const MuonSegment*>::iterator segit_end = chit->end();
	  for( ;segit!=segit_end;++segit ) segVec->push_back( (*segit)->clone() );
	  newCombi->addSegments( segVec );
	}

	// create new summary
	MuonSegmentCombiSummary newSummary = createSummary( *newCombi );
	// hack to keep link with original combi
	if( summary.combiOriginal ) {
	  // copy pointer to original combi, delete combi
	  newSummary.combiOriginal = summary.combiOriginal;
	  delete summary.m_combi;
	}else{
	  // set pointer to original
	  newSummary.combiOriginal = summary.m_combi;
	}
	if( sit->combiOriginal ) {
	  // copy pointer to original combi, delete combi
	  delete sit->m_combi;
	}

	if( m_logLevel <= MSG::DEBUG )  m_log << MSG::VERBOSE <<  "    -> merged combis " << endreq;
	*sit = newSummary;
	hasOverlap = true;
	break;
      }
    }


    return hasOverlap;
  }
    

  MuonSegmentCombiOverlapSummary MuonSegmentCombinationCleanerTool::calculateOverlap( MuonSegmentCombiSummary& summary1, 
										      MuonSegmentCombiSummary& summary2 ) const {
    
    if( m_logLevel <= MSG::DEBUG ) {
      m_log << MSG::DEBUG  << " calculating overlap, size first " << summary1.nsegments << " size second " << summary2.nsegments << endreq;
    }
    
    MuonSegmentCombiOverlapSummary summary;
    
    // loop over all chamber layers and compare per layer
    for( unsigned int i=0;i<MuonStationIndex::ChIndexMax; ++i ){
      
      // get segment in chamber layer
      MuonSegmentCombiSummary::SegVec& chamberVec1 = summary1.chamberSegments(i);
      MuonSegmentCombiSummary::SegVec& chamberVec2 = summary2.chamberSegments(i);
      
      // if both no segments do nothing
      if( chamberVec1.empty() && chamberVec2.empty() ) continue;
      
      // if both not empty compare the two, exception for CSCs
      if( !chamberVec1.empty() &&  !chamberVec2.empty() ) {
	if( m_logLevel <= MSG::DEBUG ) m_log << MSG::DEBUG  << " resolving chambe layer " << MuonStationIndex::chName((MuonStationIndex::ChIndex)i) << endreq;

	resolveLayerOverlap(chamberVec1,chamberVec2,summary);

      }else{
	if( !chamberVec1.empty() ) summary.uniqueFirst.insert(summary.uniqueFirst.begin(),chamberVec1.begin(),chamberVec1.end());
	if( !chamberVec2.empty() ) summary.uniqueSecond.insert(summary.uniqueSecond.begin(),chamberVec2.begin(),chamberVec2.end());
      }      
    }
    if( m_logLevel <= MSG::DEBUG )  m_log << MSG::VERBOSE << print(summary) << endreq;
    return summary;
  }

  void MuonSegmentCombinationCleanerTool::resolveLayerOverlap( const std::vector<const MuonSegment*>& chamberVec1,
							       const std::vector<const MuonSegment*>& chamberVec2,
							       MuonSegmentCombiOverlapSummary& summary) const {
    CompareMuonSegmentKeys compareKeys;

    std::vector<int> uniqueFirst(chamberVec1.size(),1);
    std::vector<int> uniqueSecond(chamberVec2.size(),1);

    unsigned int index1 = 0;
    std::vector<const MuonSegment*>::const_iterator sit1 = chamberVec1.begin();
    std::vector<const MuonSegment*>::const_iterator sit1_end = chamberVec1.end();
    for( ;sit1!=sit1_end;++sit1,++index1 ){
      // identifier(s) of MDT chambers on segment
      std::set<Identifier> chIds1 = m_helperTool->chamberIds( **sit1 );

      unsigned int index2 = 0;
      std::vector<const MuonSegment*>::const_iterator sit2 = chamberVec2.begin();
      std::vector<const MuonSegment*>::const_iterator sit2_end = chamberVec2.end();
      for( ;sit2!=sit2_end;++sit2,++index2 ){

	if( !uniqueSecond[index2] ) continue;

	// identifier(s) of MDT chambers on segment
	std::set<Identifier> chIds2 = m_helperTool->chamberIds( **sit2 );
	
	// check whether chamber identifiers overlap 
	bool hasOverlap = false;
	std::set<Identifier>::iterator idit = chIds2.begin();
	std::set<Identifier>::iterator idit_end = chIds2.end();
	for( ;idit!=idit_end;++idit ){
	  if( chIds1.count(*idit) ){ 
	    hasOverlap = true;
	    break;
	  }
	}
	// if no chamber identifier overlap continue with next segment
	if( !hasOverlap ) {
	  continue;
	}
	// compare the hits on the two segments

	MuonSegmentKey key1(**sit1);
	MuonSegmentKey key2(**sit2);
	CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(key1,key2);
	if( m_logLevel <= MSG::DEBUG )  m_log << MSG::VERBOSE << "  overlap Result: " << compareKeys.print(overlapResult) << endreq;

	if( overlapResult == CompareMuonSegmentKeys::Identical ) {
	  summary.shared.push_back( std::make_pair(*sit1,*sit2) );
	  uniqueFirst[index1] = 0;
	  uniqueSecond[index2] = 0;
	}else if( overlapResult == CompareMuonSegmentKeys::SuperSet ){
	  summary.subsetSecond.push_back( std::make_pair(*sit1,*sit2) );
	  uniqueFirst[index1] = 0;
	  uniqueSecond[index2] = 0;
	}else if( overlapResult == CompareMuonSegmentKeys::SubSet ){
	  summary.subsetFirst.push_back( std::make_pair(*sit1,*sit2) );
	  uniqueFirst[index1] = 0;
	  uniqueSecond[index2] = 0;
	}else if( overlapResult == CompareMuonSegmentKeys::PartialOverlap ){
	  // keep partial overlaps for now....
	  
	  // in case of full overlap in the MDTs
	  if( compareKeys.intersectionSize == compareKeys.segment1Size && compareKeys.intersectionSize == compareKeys.segment2Size ) {
	    
	    // mark as same if more than 75% overlap in trigger hits
	    double intersectFrac1 = compareKeys.intersectionSizeTrigger == 0 ? 0. : compareKeys.intersectionSizeTrigger/(double)compareKeys.segment1SizeTrigger;
	    double intersectFrac2 = compareKeys.intersectionSizeTrigger == 0 ? 0. : compareKeys.intersectionSizeTrigger/(double)compareKeys.segment2SizeTrigger;
	    double openingAngle = (*sit1)->globalDirection().dot((*sit2)->globalDirection());
	    if( m_logLevel <= MSG::VERBOSE )  m_log << MSG::VERBOSE << "  Partial overlap: (" << intersectFrac1 << "," << intersectFrac2 
						    << "), opening angle " << openingAngle << endreq;
	    if( intersectFrac1 > 0.75 && intersectFrac2 > 0.75 && openingAngle > 0.99 ){
	      summary.shared.push_back( std::make_pair(*sit1,*sit2) );
	      uniqueFirst[index1] = 0;
	      uniqueSecond[index2] = 0;	    
	    }
	  }

	}else if( overlapResult == CompareMuonSegmentKeys::NoOverlap ){

	}else if( overlapResult == CompareMuonSegmentKeys::Unknown ){
	  m_log << MSG::WARNING  << " Got invalid return argument comparing segments: " << compareKeys.print(overlapResult) << endreq;
	}
      }
    }

    // add unique segments
    index1 = 0;
    sit1 = chamberVec1.begin();
    sit1_end = chamberVec1.end();
    for( ;sit1!=sit1_end;++sit1,++index1 ){
      if( uniqueFirst[index1] ) summary.uniqueFirst.push_back(*sit1);
    }

    unsigned int index2 = 0;
    std::vector<const MuonSegment*>::const_iterator sit2 = chamberVec2.begin();
    std::vector<const MuonSegment*>::const_iterator sit2_end = chamberVec2.end();
    for( ;sit2!=sit2_end;++sit2,++index2 ){
      if( uniqueSecond[index2] ) summary.uniqueSecond.push_back(*sit2);
    }

  }


  MuonSegmentCombiSummary MuonSegmentCombinationCleanerTool::createSummary( const MuonSegmentCombination& combi ) const {

    // create summary object
    MuonSegmentCombiSummary summary(combi);

    unsigned int nstations = combi.numberOfStations();

    // loop over segments, add them to the chamber layers
    // loop over chambers in combi and extract segments
    for(unsigned int i=0; i<nstations; ++i){

      // loop over segments in station
      const MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments( i ) ;
         
      // check if not empty
      if( !stationSegs || stationSegs->empty() ) continue;
  
      // get chamber identifier, chamber index and station index
      Identifier chid = m_helperTool->chamberId( *stationSegs->front() );
      MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chid);
      MuonStationIndex::StIndex stIndex = MuonStationIndex::toStationIndex( chIndex );
      summary.stations.insert(stIndex);
      // reserve space for the new segments
      MuonSegmentCombiSummary::SegVec& chamberVec = summary.chamberSegments(chIndex);
      chamberVec.reserve( chamberVec.size() + stationSegs->size() );
      
      summary.nsegments += stationSegs->size();

      MuonSegmentCombination::SegmentVec::const_iterator ipsg=stationSegs->begin();    
      MuonSegmentCombination::SegmentVec::const_iterator ipsg_end=stationSegs->end();
      for (;ipsg!=ipsg_end;++ipsg){

	const MuonSegment* seg = dynamic_cast<const MuonSegment*>(*ipsg);

	if( !seg ){
	  m_log << MSG::WARNING << " MuonSegmentCombination contains a segment that is not a MuonSegment!! " << endreq;
	  continue;
	}
	
	if( seg->containedROTs().size() > 3 ) {
	  summary.stationsGood.insert(stIndex);
	}
	
	chamberVec.push_back(seg);
      }
    }
    
    return summary;
  }

  MuonSegmentCombination* MuonSegmentCombinationCleanerTool::removeDuplicateSegments( MuonSegmentCombination& combi ) const {
    
    CompareMuonSegmentKeys compareKeys;

    // store pointers to segments that should be kept
    std::set<const MuonSegment*> segmentsToBeKept;
    unsigned int nsegments = 0; // total number of segments 

    unsigned int nstations = combi.numberOfStations();

    // loop over segments, add them to the chamber layers
    // loop over chambers in combi and extract segments
    for(unsigned int i=0; i<nstations; ++i){

      // loop over segments in station
      const MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments( i ) ;
         
      // check if not empty
      if( !stationSegs || stationSegs->empty() ) continue;

      std::vector< std::pair<MuonSegmentKey,const MuonSegment*> > keys;
      keys.reserve(stationSegs->size());

      MuonSegmentCombination::SegmentVec::const_iterator ipsg=stationSegs->begin();    
      MuonSegmentCombination::SegmentVec::const_iterator ipsg_end=stationSegs->end();
      for (;ipsg!=ipsg_end;++ipsg){

	++nsegments;

	const MuonSegment* seg = dynamic_cast<const MuonSegment*>(*ipsg);

	if( !seg ){
	  m_log << MSG::WARNING << " MuonSegmentCombination contains a segment that is not a MuonSegment!! " << endreq;
	  continue;
	}

	
	if( m_logLevel <= MSG::VERBOSE )  m_log << MSG::VERBOSE << "  segment " << m_printer->print(*seg) << std::endl
						<< m_printer->print( seg->containedMeasurements() ) << endreq;

	// create key
	MuonSegmentKey key(*seg);

	// loop over already added keys
	bool addSegment = true;
	std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator kit = keys.begin();
	std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator kit_end = keys.end();
	for( ;kit!=kit_end;++kit ) {

	  CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(key,kit->first);
	  if( m_logLevel <= MSG::VERBOSE )  m_log << MSG::VERBOSE << "  overlap Result: " << compareKeys.print(overlapResult) << endreq;

	  if( overlapResult == CompareMuonSegmentKeys::Identical || 
	      overlapResult == CompareMuonSegmentKeys::SubSet ){
	    addSegment = false;
	    break;
	  }else if( overlapResult == CompareMuonSegmentKeys::SuperSet ) {
	    *kit = std::make_pair(key,seg); 
	    addSegment = false;
	    break;
	  }else if( overlapResult == CompareMuonSegmentKeys::Unknown ){
	    m_log << MSG::WARNING  << " Got invalid return argument comparing segments: " << compareKeys.print(overlapResult) << endreq;
	  }
	}
	
	if( addSegment ) keys.push_back( std::make_pair(key,seg) ); 
      }
      std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator kit = keys.begin();
      std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator kit_end = keys.end();
      for( ;kit!=kit_end;++kit ) segmentsToBeKept.insert( kit->second );
    }

    // check whether all segments were kept
    if( nsegments == segmentsToBeKept.size() ){
      if( m_logLevel <= MSG::DEBUG )  m_log << MSG::DEBUG << "  no segments removed " << endreq;
      return &combi;
    }

    MuonSegmentCombination* newCombi = new MuonSegmentCombination();

    // create a new combination removing segments that overlap
    for(unsigned int i=0; i<nstations; ++i){

      // loop over segments in station
      const MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments( i ) ;
         
      // check if not empty
      if( !stationSegs || stationSegs->empty() ) continue;

      std::vector< const MuonSegment* >* segVec = new std::vector< const MuonSegment* >();
      segVec->reserve( stationSegs->size() );


      MuonSegmentCombination::SegmentVec::const_iterator ipsg=stationSegs->begin();    
      MuonSegmentCombination::SegmentVec::const_iterator ipsg_end=stationSegs->end();
      for (;ipsg!=ipsg_end;++ipsg){

	const MuonSegment* seg = dynamic_cast<const MuonSegment*>(*ipsg);

	if( !segmentsToBeKept.count(seg) ){
	  if( m_logLevel <= MSG::VERBOSE ) m_log << MSG::VERBOSE << " dropping segment  " << m_printer->print(*seg) << endreq;
	  continue;
	}else if( m_logLevel <= MSG::VERBOSE ) m_log << MSG::VERBOSE << " adding   segment  " << m_printer->print(*seg) << endreq;
	segVec->push_back( seg->clone() );
      }	
      newCombi->addSegments( segVec );
    }
    
    return newCombi;
  }


  std::string MuonSegmentCombinationCleanerTool::print( MuonSegmentCombiSummary& summary ) const {
    
    std::ostringstream sout;

    // loop over chamber layers
    int index = 0;
    MuonSegmentCombiSummary::ChSegVec& segmentsPerLayer = summary.segmentsPerLayer();

    sout << "SegmentCombi " << std::endl;

    MuonSegmentCombiSummary::ChSegIt chit = segmentsPerLayer.begin();
    MuonSegmentCombiSummary::ChSegIt chit_end = segmentsPerLayer.end();
    for( ;chit!=chit_end;++chit ){
      
      // skip empty layers
      if( chit->empty() ) continue;
      
      sout << " Chamber Layer: " << MuonStationIndex::chName((MuonStationIndex::ChIndex)index) << " with " << chit->size() << " segments " << std::endl
	   << m_printer->print( *chit ) << std::endl;
      
      ++index;
    }
    
    return sout.str();
  }

  std::string MuonSegmentCombinationCleanerTool::print( MuonSegmentCombiOverlapSummary& summary ) const {
    
    std::ostringstream sout;

    sout << "SegmentCombi overlap " << std::endl
	 << "  shared:       " << summary.shared.size() << std::endl
	 << "  uniqueFirst:  " << summary.uniqueFirst.size() << std::endl
	 << "  uniqueSecond: " << summary.uniqueSecond.size() << std::endl
	 << "  subsetFirst:  " << summary.subsetFirst.size() << std::endl
	 << "  subsetSecond: " << summary.subsetSecond.size() << std::endl;
    return sout.str();
  }


}

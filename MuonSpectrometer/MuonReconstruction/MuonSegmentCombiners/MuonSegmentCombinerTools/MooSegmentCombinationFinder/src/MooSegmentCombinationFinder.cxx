/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MooSegmentCombinationFinder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MooSegmentCombinationFinder.h"

#include "CscSegmentMakers/ICscSegmentFinder.h"
#include "MuonRecToolInterfaces/IMuonHoughPatternFinderTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonPatternSegmentMaker.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonSegmentCombinationCleanerTool.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonCurvedSegmentCombiner.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "MuonChamberT0s/ChamberT0s.h"
#include "MuonSegmentMakerUtils/SortSegmentsByNumberOfHits.h"

#include "TrkEventPrimitives/FitQuality.h" 

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"


#include "GaudiKernel/IAuditor.h"
#include "StoreGate/StoreGateSvc.h"

  //================ Constructor =================================================

Muon::MooSegmentCombinationFinder::MooSegmentCombinationFinder(const std::string& t,
                       const std::string& n,
                       const IInterface*  p )
    :
  AthAlgTool(t,n,p),
    m_auditorExecute(false),
    m_edmPrinter("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_csc2dSegmentFinder("Csc2dSegmentMaker/Csc2dSegmentMaker"),
    m_csc4dSegmentFinder("Csc4dSegmentMaker/Csc4dSegmentMaker"),
    m_houghPatternFinder("Muon::MuonHoughPatternFinderTool/MuonHoughPatternFinderTool"),
    m_patternSegmentMaker("Muon::MuonPatternSegmentMaker/MuonPatternSegmentMaker"),
    m_curvedSegmentCombiner("Muon::MuonCurvedSegmentCombiner/MuonCurvedSegmentCombiner"),
    m_segmentCombinationCleaner("Muon::MuonSegmentCombinationCleanerTool/MuonSegmentCombinationCleanerTool"),
    m_overlapRemovalTool("Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool"),
    m_segmentSelector("Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"),
    m_csc2dLocation("Csc2dSegmentCombinations"),
    m_csc4dLocation("Csc4dSegmentCombinations"),
    m_mdtSegmentCombinationLocation("MdtSegmentCombinations"),
    m_curvedCombinationLocation("CurvedSegmentCombinations"),
    m_nevents(0),
    m_ncsc2SegmentCombinations(0),
    m_ncsc4SegmentCombinations(0),
    m_npatternCombinations(0),
    m_nmdtSegmentCombinations(0),
    m_ncombinedSegmentCombinations(0),
    m_ncleanedSegmentCombinations(0),
    m_nsegments(0),
    m_nsegmentsStraight(0),
    m_nsegmentsCurved(0),
    m_nremovedSegments(0),
    m_nremovedBadSegments(0)

  {
    declareInterface<IMooSegmentCombinationFinder>(this);

    declareProperty("DoSummary", m_doSummary = false);

    declareProperty("Csc2dSegmentMaker",         m_csc2dSegmentFinder);
    declareProperty("Csc4dSegmentMaker",         m_csc4dSegmentFinder);
    declareProperty("HoughPatternFinder",        m_houghPatternFinder);
    declareProperty("MdtSegmentMaker",           m_patternSegmentMaker);
    declareProperty("SegmentCombiner",           m_curvedSegmentCombiner);
    declareProperty("SegmentCombinationCleaner", m_segmentCombinationCleaner);
    declareProperty("SegmentOverlapRemovalTool", m_overlapRemovalTool, "tool to removal overlaps in segment combinations" );

    declareProperty("DoCscSegments", m_doCscSegments = true );
    declareProperty("DoMdtSegments", m_doMdtSegments = true );
    declareProperty("DoSegmentCombinations", m_doSegmentCombinations = true );
    declareProperty("DoSegmentCombinationCleaning", m_doSegmentCombinationCleaning = true );

    declareProperty("Csc2dSegmentCombinationLocation",  m_csc2dLocation  );
    declareProperty("Csc4dSegmentCombinationLocation",  m_csc4dLocation  );
    declareProperty("MdtSegmentCombinationLocation",    m_mdtSegmentCombinationLocation);
    declareProperty("CurvedSegmentCombinationLocation", m_curvedCombinationLocation);
    declareProperty("CloneSegments",                    m_cloneSegments = false );
  }

  //================ Destructor =================================================

Muon::MooSegmentCombinationFinder::~MooSegmentCombinationFinder()
{}


  //================ Initialisation =================================================

StatusCode 
Muon::MooSegmentCombinationFinder::initialize()
{

    if (AthAlgTool::initialize().isFailure()) return StatusCode::FAILURE;
    m_auditorExecute = dynamic_cast<const BooleanProperty&>(getProperty("AuditTools")).value();
    
    if(m_edmPrinter.retrieve().isFailure()){
      ATH_MSG_FATAL("Could not get " << m_edmPrinter); 
      return StatusCode::FAILURE;
    }

    if (m_helperTool.retrieve().isFailure()){
      ATH_MSG_FATAL("Could not get " << m_helperTool); 
      return StatusCode::FAILURE;
    }

    if (m_idHelperTool.retrieve().isFailure()){
      ATH_MSG_FATAL("Could not get " << m_idHelperTool); 
      return StatusCode::FAILURE;
    }

    if( m_doCscSegments ){
      if (m_csc2dSegmentFinder.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not get " << m_csc2dSegmentFinder); 
        return StatusCode::FAILURE;
      }
      
      if (m_csc4dSegmentFinder.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not get " << m_csc4dSegmentFinder); 
        return StatusCode::FAILURE;
      }
    }

    if( m_doMdtSegments ){

      if (m_houghPatternFinder.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not get " << m_houghPatternFinder); 
        return StatusCode::FAILURE;
      }
      
      if (m_patternSegmentMaker.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not get " << m_patternSegmentMaker); 
        return StatusCode::FAILURE;
      }
      
      
      if (m_overlapRemovalTool.retrieve().isFailure()){
	ATH_MSG_FATAL("Could not get " << m_overlapRemovalTool); 
	return StatusCode::FAILURE;
      }
    }
    else{
      m_houghPatternFinder.disable();
      m_patternSegmentMaker.disable();
      m_overlapRemovalTool.disable();
    }

    
    if (m_segmentSelector.retrieve().isFailure()){
      ATH_MSG_FATAL("Could not get " << m_segmentSelector); 
      return StatusCode::FAILURE;
    }

    if( m_doSegmentCombinations ){

      if (m_curvedSegmentCombiner.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not get " << m_curvedSegmentCombiner); 
        return StatusCode::FAILURE;
      }
      
      if ( m_doSegmentCombinationCleaning ) {
	if (m_segmentCombinationCleaner.retrieve().isFailure()){
	  ATH_MSG_FATAL("Could not get " << m_segmentCombinationCleaner); 
	  return StatusCode::FAILURE;
	}
      }
      else{
	m_segmentCombinationCleaner.disable();
      }
    }
    else{
      m_curvedSegmentCombiner.disable();
      m_segmentCombinationCleaner.disable();
    }

    ATH_CHECK( m_csc2dLocation.initialize() );
    ATH_CHECK( m_csc4dLocation.initialize() );
    ATH_CHECK( m_mdtSegmentCombinationLocation.initialize() );
    ATH_CHECK( m_curvedCombinationLocation.initialize() );

    return StatusCode::SUCCESS;
}

  //================ Finalisation =================================================

StatusCode 
Muon::MooSegmentCombinationFinder::finalize()
{

    // print statistics
    double nevents = m_nevents!=0 ? m_nevents : 1.;
    
    ATH_MSG_INFO(" Segment finding Summary: total number of events " << m_nevents << std::endl
		 << " CSC segment combinations: 2D   " << std::setw(12) << m_ncsc2SegmentCombinations << " per event " << m_ncsc2SegmentCombinations/nevents << std::endl
		 << " CSC segment combinations: 4D   " << std::setw(12) << m_ncsc4SegmentCombinations << " per event " << m_ncsc4SegmentCombinations/nevents << std::endl
		 << " Pattern combinations:          " << std::setw(12) << m_npatternCombinations << " per event " << m_npatternCombinations/nevents << std::endl
		 << " MDT segment combinations:      " << std::setw(12) << m_nmdtSegmentCombinations << " per event " << m_nmdtSegmentCombinations/nevents << std::endl
		 << " Combined segment combinations: " << std::setw(12) << m_ncombinedSegmentCombinations << " per event " << m_ncombinedSegmentCombinations/nevents << std::endl
		 << " Cleaned segment combinations:  " << std::setw(12) << m_ncleanedSegmentCombinations << " per event " << m_ncleanedSegmentCombinations/nevents << std::endl
		 << " Segments:                      " << std::setw(12) << m_nsegments << " per event " << m_nsegments/nevents << std::endl
		 << "   straight:                    " << std::setw(12) << m_nsegmentsStraight << " per event " << m_nsegmentsStraight/nevents << std::endl
		 << "   curved:                      " << std::setw(12) << m_nsegmentsCurved << " per event " << m_nsegmentsCurved/nevents << std::endl
		 << " Discarded overlap segments:    " << std::setw(12) << m_nremovedSegments << " per event " << m_nremovedSegments/nevents << std::endl
		 << " Discarded bad segments:        " << std::setw(12) << m_nremovedBadSegments << " per event " << m_nremovedBadSegments/nevents);

    StatusCode sc = AthAlgTool::finalize();
    return sc;
}

void Muon::MooSegmentCombinationFinder::auditorBefore( IAlgTool* /*tool*/ ) {
  if ( m_auditorExecute ) {
    //auditorSvc()->before(IAuditor::Execute, tool);
  }
}

void Muon::MooSegmentCombinationFinder::auditorAfter( IAlgTool* /*tool*/, bool /*isOK*/ ) {
  if ( m_auditorExecute ) {
//     StatusCode status(isOK);
    //auditorSvc()->after( IAuditor::Execute, tool, status );
//     status.ignore();
  }
}


Muon::IMooSegmentCombinationFinder::Output* 
Muon::MooSegmentCombinationFinder::findSegments( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
						 const std::vector<const CscPrepDataCollection*>& cscCols,  
						 const std::vector<const TgcPrepDataCollection*>& tgcCols,  
						 const std::vector<const RpcPrepDataCollection*>& rpcCols ) 
{
    // Super tool has various different stages.
    // 1. 2d Csc segment making, which produces MuonSegmentCombinations that are passed to the 4D segment maker, and the MuonCurvedSegmentCombiner
    // 2. 4d Csc segment making, which produces MuonSegmentCombinations that are passed to the MuonHoughPatternFinder, and the MuonCurvedSegmentCombiner
    // 3. MuonHoughPatternFinder, which produces MuonPatternCombinations, and Segment-Pattern Associations (which are passed to the MuonCurvedSegmentCombiner)
    // 4. MuonPatternSegmentMaker, which takes in the remaining PRD types, and outputs MuonSegmentCombinations and seg-pattern associations.
    // 5. MuonCurvedSegmentCombiner, which produces a MuonSegmentCombination
    // 6. MuonSegmentCombinationCleaner
    
    // 1. 2s Csc Segment Making

    MuonSegmentCombinationCollection* finalSegmentCombinations = 0;
    Trk::SegmentCollection*           finalSegmentCollection = 0;

    MuonSegmentCombinationCollection* csc2dSegmentCombinations = 0;
    MuonSegmentCombinationCollection* csc4dSegmentCombinations = 0;
    if( m_doCscSegments ){
      // reconstruct segments in the CSC eta and phi plane
      auditorBefore( m_csc2dSegmentFinder );
      csc2dSegmentCombinations = m_csc2dSegmentFinder->find( cscCols );
      auditorAfter( m_csc2dSegmentFinder, csc2dSegmentCombinations );
      printSummary( "CSC 2D segment finding", csc2dSegmentCombinations );
      
      // combine CSC segments in eta and phi plane if any were found
      if (csc2dSegmentCombinations) {
	auditorBefore( m_csc4dSegmentFinder );
	csc4dSegmentCombinations = m_csc4dSegmentFinder->find( *csc2dSegmentCombinations );
	auditorAfter( m_csc4dSegmentFinder, csc4dSegmentCombinations );
	printSummary( "CSC 4D segment finding", csc4dSegmentCombinations );
      } else {
	csc4dSegmentCombinations = 0;
      }

      if( csc4dSegmentCombinations ){
	if( !finalSegmentCollection ) finalSegmentCollection = new Trk::SegmentCollection();
	extractSegmentCollection( *csc4dSegmentCombinations, *finalSegmentCollection );
      }

      finalSegmentCombinations = csc4dSegmentCombinations;

    }

    const MuonPatternCombinationCollection* patternCombinations = 0;
    MuonSegmentCombinationCollection* mdtSegmentCombinations = 0;
    if( m_doMdtSegments ){
      // search for global patterns 
      auditorBefore( m_houghPatternFinder );
      patternCombinations = m_houghPatternFinder->find( mdtCols, cscCols, tgcCols, rpcCols, csc4dSegmentCombinations );
      auditorAfter( m_houghPatternFinder, patternCombinations );
      printSummary( "Pattern finding", patternCombinations );

      // search for MDT segments 
      if (patternCombinations) {
	auditorBefore( m_patternSegmentMaker );
	mdtSegmentCombinations = m_patternSegmentMaker->find( *patternCombinations, &m_segmentPatternMap );
	auditorAfter( m_patternSegmentMaker, mdtSegmentCombinations );
	if( msgLvl(MSG::DEBUG) ) printSummary( "MDT segment finding", mdtSegmentCombinations );
      } else {
	mdtSegmentCombinations = 0;
      }

      finalSegmentCombinations = mdtSegmentCombinations;
      
      if( mdtSegmentCombinations ){
	if( !finalSegmentCollection ) finalSegmentCollection = new Trk::SegmentCollection();
	extractSegmentCollection( *mdtSegmentCombinations, *finalSegmentCollection );
      }
      printSummary( "MDT segment finding", finalSegmentCollection );
    }


    MuonSegmentCombinationCollection* curvedSegmentCombinations = 0;
    MuonSegmentCombinationCollection* cleanedSegmentCombinations = 0;
    if( m_doSegmentCombinations ){

      // create dummy collections if CSCs are missing
      if( !csc2dSegmentCombinations ) csc2dSegmentCombinations = new MuonSegmentCombinationCollection();
      if( !csc4dSegmentCombinations ) csc4dSegmentCombinations = new MuonSegmentCombinationCollection();

      // combine MDT and CSC segments
      if ( mdtSegmentCombinations ) {
	auditorBefore( m_curvedSegmentCombiner );
	curvedSegmentCombinations = m_curvedSegmentCombiner->combineSegments(*mdtSegmentCombinations, 
									     *csc4dSegmentCombinations, 
									     *csc2dSegmentCombinations,
									     &m_segmentPatternMap);
	auditorAfter( m_curvedSegmentCombiner, curvedSegmentCombinations );
	if( msgLvl(MSG::DEBUG) ) printSummary( "Segment combining", curvedSegmentCombinations );

      } else {
	curvedSegmentCombinations = 0;
      }
     
      finalSegmentCombinations = curvedSegmentCombinations;
    }

    if( m_doSegmentCombinationCleaning ){
      // clean segment combinations
      if ( finalSegmentCombinations ) {
	auditorBefore( m_segmentCombinationCleaner );
	cleanedSegmentCombinations = m_segmentCombinationCleaner->clean(*finalSegmentCombinations, &m_segmentPatternMap);
	auditorAfter( m_segmentCombinationCleaner, cleanedSegmentCombinations );
	printSummary( "Segment combination cleaning", cleanedSegmentCombinations );

      } else {
	cleanedSegmentCombinations = 0;
      }
      finalSegmentCombinations = cleanedSegmentCombinations;
    }

    if( m_doSummary || msgLvl(MSG::DEBUG) ) msg() << endmsg;

    // increase counters, assume that the tools is only called once per event
    ++m_nevents;
    if( csc2dSegmentCombinations )   m_ncsc2SegmentCombinations += csc2dSegmentCombinations->size();
    if( csc4dSegmentCombinations )   m_ncsc4SegmentCombinations += csc4dSegmentCombinations->size();
    if( patternCombinations )        m_npatternCombinations += patternCombinations->size();
    if( mdtSegmentCombinations )     m_nmdtSegmentCombinations  += mdtSegmentCombinations->size();
    if( curvedSegmentCombinations )  m_ncombinedSegmentCombinations += curvedSegmentCombinations->size();
    if( finalSegmentCollection ){
      m_nsegments += finalSegmentCollection->size();
      Trk::SegmentCollection::const_iterator sit = finalSegmentCollection->begin();
      Trk::SegmentCollection::const_iterator sit_end = finalSegmentCollection->end();
      for( ;sit!=sit_end;++sit ){
	if( (*sit)->localParameters().contains(Trk::qOverP) ) ++m_nsegmentsCurved;
	else ++m_nsegmentsStraight;
      }
    }
    if( m_doSegmentCombinationCleaning && finalSegmentCombinations ) m_ncleanedSegmentCombinations += finalSegmentCombinations->size();

    // clean up intermediate steps
    if( csc2dSegmentCombinations ) 
      postProcess( csc2dSegmentCombinations,  m_csc2dLocation );

    if( csc4dSegmentCombinations && finalSegmentCombinations != csc4dSegmentCombinations ) 
      postProcess( csc4dSegmentCombinations,  m_csc4dLocation );

    if( mdtSegmentCombinations && finalSegmentCombinations != mdtSegmentCombinations ) 
      postProcess( mdtSegmentCombinations,    m_mdtSegmentCombinationLocation );

    if( curvedSegmentCombinations && finalSegmentCombinations != curvedSegmentCombinations )
      postProcess( curvedSegmentCombinations, m_curvedCombinationLocation );
    
    Muon::IMooSegmentCombinationFinder::Output* output = new Muon::IMooSegmentCombinationFinder::Output();
    output->patternCombinations = patternCombinations;
    output->segmentCollection   = finalSegmentCollection;
    output->segmentCombinations = finalSegmentCombinations;
    return output;
  }

void 
Muon::MooSegmentCombinationFinder::postProcess(  MuonSegmentCombinationCollection* col, SG::WriteHandleKey<MuonSegmentCombinationCollection> &colLocation ) {

  if( !col ) col = new MuonSegmentCombinationCollection();
  
  SG::WriteHandle<MuonSegmentCombinationCollection> handle(colLocation);
  if( handle.record(std::unique_ptr<MuonSegmentCombinationCollection> (col)).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonSegmentCombinationCollection " << col->size() 
			   << " at " << colLocation.key());
  }else{
    ATH_MSG_ERROR("Failed to store MuonSegmentCombinationCollection at " << colLocation.key());
  }
}


void 
Muon::MooSegmentCombinationFinder::printStage( std::string stageTag) const {
  ATH_MSG_INFO("Reco stage: " << stageTag);
}

void 
Muon::MooSegmentCombinationFinder::printSummary( std::string stageTag, const MuonSegmentCombinationCollection* col ) const {
  if( m_doSummary || msgLvl(MSG::DEBUG) ){
    printStage( stageTag );
    if( !col ){
      ATH_MSG_INFO("No segment combinations found ");
    }else{
      msg(MSG::INFO) << "Found " << col->size() << " segment combinations " << std::endl
		     << m_edmPrinter->print( *col );
    }
  }
}

void 
Muon::MooSegmentCombinationFinder::printSummary( std::string stageTag, const Trk::SegmentCollection* col ) const {
  if( m_doSummary || msgLvl(MSG::DEBUG) ){
    printStage( stageTag );
    if( !col ){
      ATH_MSG_INFO("No segments found ");
    }else{
      msg(MSG::INFO) << "Found " << col->size() << " segments found " << std::endl;
      Trk::SegmentCollection::const_iterator sit = col->begin();
      Trk::SegmentCollection::const_iterator sit_end = col->end();
      for( ;sit!=sit_end;++sit ){
	const MuonSegment* seg = dynamic_cast<const MuonSegment*>(*sit);
	if( seg ){
	  msg() << m_edmPrinter->print( *seg ) ;
	  if( sit+1 != sit_end ) msg() << std::endl;
	}
      }	
    }
  }
}

void 
Muon::MooSegmentCombinationFinder::printSummary( std::string stageTag, const MuonPatternCombinationCollection* col ) const 
{
  if( m_doSummary || msgLvl(MSG::DEBUG) ){
    printStage( stageTag );
    if( !col ){
      ATH_MSG_INFO("No pattern combinations found ");
    }else{
      ATH_MSG_INFO("Found " << col->size() << " pattern combinations found " << std::endl
	    << m_edmPrinter->print( *col ));
    }
  }
}

void
Muon::MooSegmentCombinationFinder::extractSegmentCollection( const MuonSegmentCombinationCollection& combiCol, Trk::SegmentCollection& segmentCol  ) const {


  // store single segments per chamber layer
  typedef std::vector<const Muon::MuonSegment*> SegVec;
  //typedef SegVec::iterator SegVecIt;
  typedef std::map<Muon::MuonStationIndex::ChIndex, SegVec > RSMap;
  typedef RSMap::iterator RSMapIt;
  RSMap segMap;

  unsigned int nremovedSegments(0);
  unsigned int nremovedBadSegments(0);
  unsigned int naccepted(0);

  MuonSegmentCombinationCollection::const_iterator cit = combiCol.begin();
  MuonSegmentCombinationCollection::const_iterator cit_end = combiCol.end();
  for(; cit!=cit_end;++cit ){
    if( !*cit ) {
      ATH_MSG_DEBUG(" empty MuonSegmentCombination!!! ");
      continue;
    }
    const Muon::MuonSegmentCombination& combi = **cit;
    unsigned int nstations = combi.numberOfStations();

    // segment quality level
    bool ignoreHoles = false;
    int quality = 1;

    //chamber status for csc
    bool useEta=combi.useStripsInSegment(1);
    bool usePhi=combi.useStripsInSegment(0);

    // loop over chambers in combi and extract segments
    for(unsigned int i=0; i<nstations; ++i){

      // loop over segments in station
      const Muon::MuonSegmentCombination::SegmentVec* segments = combi.stationSegments( i ) ;
         
      // check if not empty
      if( !segments || segments->empty() ) continue;

      // get chamber identifier, chamber index and station index
      Identifier chid = m_helperTool->chamberId( *segments->front() );
      Muon::MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chid);

      // add segments to region segment map, remove ambigueties (missing at the moment)
      RSMapIt rsit = segMap.find( chIndex );
      if( rsit == segMap.end() ){
	SegVec segs;
	// loop over new segments, copy them into collection
	Muon::MuonSegmentCombination::SegmentVec::const_iterator sit = segments->begin();
	Muon::MuonSegmentCombination::SegmentVec::const_iterator sit_end = segments->end();
	for( ; sit!=sit_end;++sit){
	  
	  // remove bad segments
	  if( !m_segmentSelector->select( **sit, ignoreHoles, quality, useEta, usePhi ) ){
	    if( msgLvl(MSG::VERBOSE) ) {
	      int q = m_segmentSelector->quality(**sit,ignoreHoles,useEta,usePhi);
	      msg(MSG::VERBOSE) << " bad segment " << m_edmPrinter->print(**sit) << " quality " << q << endmsg;
	    }
	    ++nremovedBadSegments;
	    continue;
	  }
	  segs.push_back( *sit );
	}
	segMap.insert( std::make_pair( chIndex, segs ) );
      }else{
	SegVec& segs = rsit->second;
	// loop over new segments, copy them into collection
	Muon::MuonSegmentCombination::SegmentVec::const_iterator sit = segments->begin();
	Muon::MuonSegmentCombination::SegmentVec::const_iterator sit_end = segments->end();
	for( ; sit!=sit_end;++sit){
	  
	  // remove bad segments
	  if( !m_segmentSelector->select( **sit, ignoreHoles, quality,useEta,usePhi ) ){
	    if( msgLvl(MSG::VERBOSE) ) {
	      int q = m_segmentSelector->quality(**sit,ignoreHoles,useEta,usePhi);
	      msg(MSG::VERBOSE) << " bad segment " << m_edmPrinter->print(**sit) << " quality " << q << endmsg;
	    }
	    ++nremovedBadSegments;
	    continue;
	  }
	  segs.push_back( *sit );
	}
      }
    }
  }

  // optionally output 
  RSMapIt rsit = segMap.begin();
  RSMapIt rsit_end = segMap.end();
  for( ;rsit!=rsit_end;++rsit){
    SegVec& segs = rsit->second;


    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Working on new chamber layer with  " << segs.size() 
					     << " segments" << std::endl;


    // sort segments according to they number of hits
    std::stable_sort( segs.begin(), segs.end(), SortSegmentsByNumberOfHits() );

    // remove duplicates
    SegVec goodSegments = m_overlapRemovalTool->removeDuplicates(segs);
    
    // insert remaining segments into segment collection, cast away const as Trk::SegmentCollection contains none const pointers
    segmentCol.reserve( segmentCol.size()+goodSegments.size());
    IMuonSegmentOverlapRemovalTool::SegCit sit = goodSegments.begin();
    IMuonSegmentOverlapRemovalTool::SegCit sit_end = goodSegments.end();
    for( ;sit!=sit_end;++sit ) {
      segmentCol.push_back( m_cloneSegments ? (*sit)->clone() : new Muon::MuonSegment(**sit) );
    }

    nremovedSegments += segs.size() - goodSegments.size();
    naccepted += goodSegments.size();

  }

  ATH_MSG_DEBUG(" Accepted " << naccepted 
		<< " segments and removed: overlap " << nremovedSegments << " bad " << nremovedBadSegments);
  m_nremovedSegments += nremovedSegments;
  m_nremovedBadSegments += nremovedBadSegments;
}


std::pair<int,int> Muon::MooSegmentCombinationFinder::hitsInMultilayer( const Muon::MuonSegment& segment ) const {
  int nMl1(0);
  int nMl2(0);
  const std::vector< const Trk::MeasurementBase* >& measurements = segment.containedMeasurements();
  std::vector< const Trk::MeasurementBase* >::const_iterator it = measurements.begin(), itEnd = measurements.end();
  for(;it!=itEnd;++it){
    const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(*it);
    if( mdt ){
      int ml = m_idHelperTool->mdtIdHelper().multilayer(mdt->identify());
      if( ml == 1 ) ++nMl1;
      if( ml == 2 ) ++nMl2;
    }
  }
  return std::make_pair(nMl1,nMl2);
}

bool Muon::MooSegmentCombinationFinder::goodSegment( const Muon::MuonSegment& segment ) const {

  std::pair<int,int> hitsInMl = hitsInMultilayer(segment);
  
  if( hitsInMl.first > 1 && hitsInMl.second > 1 ) return true;

  const Muon::MuonSegmentQuality* quality = dynamic_cast<const Muon::MuonSegmentQuality*>(segment.fitQuality());
  int missedHits(0);
  if( quality ) {
    missedHits = quality->numberOfHoles();
  }
  if( missedHits <= 3 ) return true;
  
  return false;
}

 
bool Muon::MooSegmentCombinationFinder::firstIsBest( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const {

  std::pair<int,int> hitsInMl_1 = hitsInMultilayer(seg1);
  std::pair<int,int> hitsInMl_2 = hitsInMultilayer(seg2);

  unsigned int nmdtHits1 = hitsInMl_1.first + hitsInMl_1.second;
  unsigned int nmdtHits2 = hitsInMl_2.first + hitsInMl_2.second;
  if( nmdtHits1 > nmdtHits2 ) return true;
  if( nmdtHits1 < nmdtHits2 ) return false;
	
  // prefer segments with fit quality (always expected)
  const Trk::FitQuality* fq1 = seg1.fitQuality();
  const Trk::FitQuality* fq2 = seg2.fitQuality();
  if( !fq1 && fq2 ) return false;
  if( fq1  && !fq2 ) return true;
  if( !fq1 && !fq2 ) return false; 
 
  // select candidate with smallest chi2
  double chi2Ndof1 = fq1->chiSquared()/fq1->numberDoF();
  double chi2Ndof2 = fq2->chiSquared()/fq2->numberDoF();
  return chi2Ndof1 < chi2Ndof2;
}

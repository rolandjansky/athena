/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
// MuonSegmentTagTool
//  AlgTool performing MS segment tagging of ID tracks.
//  A SegmentTag is added to the InDetCandidate object.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "MuonSegmentTagTool.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "MuonSegment/MuonSegment.h"

#include "MSSurfaces.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "AthLinks/ElementLink.h"

#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"

#include "TrkCaloExtension/CaloExtension.h" 

#include <iomanip>
#include <vector>

#include <stdio.h>

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonSegmentTagTool::MuonSegmentTagTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_CutTheta(999.), 
	m_CutPhi(999.), 
	m_HitInfoFor2ndCoord(0.),
	m_doSegmentsFilter(true), 
	m_doTable(false), 
	m_doBidirectional(false), 
	m_doPtDependentPullCut(true),

	m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
	p_MuTagMatchingTool               ( "MuTagMatchingTool/MuTagMatchingTool"                          ) ,
	p_MuTagAmbiguitySolverTool        ( "MuTagAmbiguitySolverTool/MuTagAmbiguitySolverTool"             ) ,
        m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
	m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
	m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
	m_segmentSelector("Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"),
	m_hitSummaryTool("Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool"),
	m_surfaces(0),
	m_ntotTracks(0),
	m_nangleMatch(0),
	m_npmatch(0),
	m_natMSEntrance(0),
	m_naccepted(0)
  {
    declareInterface<IMuonSegmentTagTool>(this);
    declareProperty("CutTheta"               , m_CutTheta               = 3.                             ) ;
    declareProperty("CutPhi"                 , m_CutPhi                 = 3.                             ) ;
  
    //Option for determination of segment 2nd coordinate measurement
    declareProperty("HitInfoFor2ndCoord"     , m_HitInfoFor2ndCoord     = 1                              ) ; 
  
    declareProperty("DoSegmentsFilter"                , m_doSegmentsFilter = true      ) ;
    declareProperty("MuTagMatchingTool"               , p_MuTagMatchingTool            ) ;
    declareProperty("MuTagAmbiguitySolverTool"        , p_MuTagAmbiguitySolverTool     ) ;
    declareProperty("ParticleCaloExtensionTool",m_caloExtensionTool );    

    declareProperty("DoOverviewTable"                 , m_doTable = false              ) ;
    declareProperty("DoBidirectionalExtrapolation"    , m_doBidirectional = false      ) ;
    declareProperty("DoPtDependentPullCut"            , m_doPtDependentPullCut = false ) ;
    declareProperty("Printer",m_printer );
    declareProperty( "RemoveLowPLowFieldRegion"       , m_removeLowPLowFieldRegion = false); // remove tracks with momenta below 6 GeV in eta 1.4-17 region (low p and low field)
    declareProperty("UseIDTrackSegmentPreSelect"      , m_useSegmentPreselection = true );
    declareProperty("SegmentQualityCut"               , m_segmentQualityCut = 1 );
    declareProperty("nmdtHits"                        , m_nmdtHits = 4 );
    declareProperty("nmdtHoles"                       , m_nmdtHoles = 3 );
    declareProperty("nmdtHitsML"                      , m_nmdtHitsML = 2 );
    declareProperty("TriggerHitCut"                   , m_triggerHitCut = true );
    declareProperty("MakeMuons"                       , m_makeMuons = false );
    declareProperty("IgnoreSiAssociatedCandidates"    , m_ignoreSiAssocated = true );
    m_extrapolated.resize(15,0);
    m_goodExtrapolated.resize(15,0);

  }

  MuonSegmentTagTool::~MuonSegmentTagTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonSegmentTagTool::initialize() {

    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(p_MuTagMatchingTool.retrieve());
    ATH_CHECK(p_MuTagAmbiguitySolverTool.retrieve());
    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
    ATH_CHECK(m_segmentSelector.retrieve());
    if( !m_caloExtensionTool.empty() )       ATH_CHECK(m_caloExtensionTool.retrieve());

    ATH_MSG_INFO( "Initializing MuonSegmentTagTool - package version " << PACKAGE_VERSION );

    ATH_MSG_DEBUG( "Initialisation started                     "                            );
    ATH_MSG_DEBUG( "================================           "                            );
    ATH_MSG_DEBUG( "=Proprieties are                           "                            );
    ATH_MSG_DEBUG( "= CutTheta                                 " << m_CutTheta              );
    ATH_MSG_DEBUG( "= CutPhi                                   " << m_CutPhi                );
    ATH_MSG_DEBUG( "= HitInfoFor2ndCoord                       " << m_HitInfoFor2ndCoord    ); 
    ATH_MSG_DEBUG( "================================           "                            );

    if(0!=m_HitInfoFor2ndCoord && 1!=m_HitInfoFor2ndCoord)
      {
	ATH_MSG_FATAL( "HitInfoFor2ndCoord=" << m_HitInfoFor2ndCoord 
		       << " : Unacceptable value. MuTagIMO will not run." );
	return StatusCode::FAILURE;
      }

    m_surfaces = new MSSurfaces();

    m_doTable= true;


    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentTagTool::finalize() {
    delete m_surfaces;
    ATH_MSG_INFO( "Total number of considered ID tracks         " << m_ntotTracks);
    ATH_MSG_INFO( "Total number of ID tracks with angular match " << m_nangleMatch);
    ATH_MSG_INFO( "Total number of preselected ID tracks        " << m_npmatch);
    ATH_MSG_INFO( "Total number of ID tracks at MS entry        " << m_natMSEntrance);
    ATH_MSG_INFO( "Total number of accepted ID tracks           " << m_naccepted);
    for( unsigned int i=0;i<12;++i ){
      double ratio = m_extrapolated[i] == 0 ? 0 : m_goodExtrapolated[i]/(double)m_extrapolated[i];
      ATH_MSG_INFO( "Layer " << i << " extrap " << m_extrapolated[i] << " good " << m_goodExtrapolated[i] 
		    << " ratio " << ratio );
    
    }

    return StatusCode::SUCCESS;
  }

  void MuonSegmentTagTool::tag( const InDetCandidateCollection& inDetCandidates, const xAOD::MuonSegmentContainer& xaodSegments ) const {

    // loop over segments are extract MuonSegments + create links between segments and xAOD segments
    std::map< const Muon::MuonSegment*, ElementLink<xAOD::MuonSegmentContainer> > segmentToxAODSegmentMap;
    std::vector< const Muon::MuonSegment* > segments;
    segments.reserve(xaodSegments.size());
    unsigned int index = 0;
    for( auto it = xaodSegments.begin();it!=xaodSegments.end();++it,++index ){
      if( !(*it)->muonSegment().isValid() ) continue;
      const Muon::MuonSegment* mseg = dynamic_cast<const Muon::MuonSegment*>(*(*it)->muonSegment());
      ElementLink<xAOD::MuonSegmentContainer> link(xaodSegments,index);
      link.toPersistent();
      if( mseg ) {
        segments.push_back(mseg);
        segmentToxAODSegmentMap[mseg] = link;
      }
    }
    tag(inDetCandidates, segments, &segmentToxAODSegmentMap);
  }
  //todo: fix segmentToxAODSegmentMap
  void MuonSegmentTagTool::tag( const InDetCandidateCollection& inDetCandidates, const std::vector<const Muon::MuonSegment*>& segments, SegmentMap* segmentToxAODSegmentMap ) const {

    std::vector<const Muon::MuonSegment*>  FilteredSegmentCollection;
    if (m_doSegmentsFilter) {

      for (std::vector<const Muon::MuonSegment*>::const_iterator itSeg = segments.begin(); 
	   itSeg!=segments.end(); ++itSeg ){
	int quality = m_segmentSelector->quality(**itSeg);
	ATH_MSG_DEBUG( " Segment quality " << quality); 
	if( quality < m_segmentQualityCut ) continue;
	Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(**itSeg);
	if(hitCounts.ncscHitsPhi+hitCounts.ncscHitsEta>0)  ATH_MSG_DEBUG(" CSC segment "); 

        if(hitCounts.ncscHitsPhi+hitCounts.ncscHitsEta==0) { 
  	  if( hitCounts.nmdtHoles >= m_nmdtHoles) continue;
  	  if( hitCounts.nmdtHitsMl1 + hitCounts.nmdtHitsMl2 < m_nmdtHits) continue; //precision hits
	  if( hitCounts.nmdtHitsMl1 >0 && hitCounts.nmdtHitsMl1 < m_nmdtHitsML) continue;
	  if( hitCounts.nmdtHitsMl2 >0 && hitCounts.nmdtHitsMl2 < m_nmdtHitsML) continue;
	// If Trigger hits are expected one should at least find one
	  ATH_MSG_DEBUG( " expected TrigHits " << hitCounts.nexpectedTrigHitLayers << " TrigHits found " << hitCounts.nphiTrigHitLayers+hitCounts.netaTrigHitLayers ); 
	  if( m_triggerHitCut && hitCounts.nexpectedTrigHitLayers> 1 && hitCounts.nphiTrigHitLayers == 0 && hitCounts.netaTrigHitLayers == 0 ) continue;
        } 
	if( m_segmentQualityCut > 0 ){
	  Identifier chId = m_edmHelper->chamberId(**itSeg);
	  Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(chId);
	  if( !m_triggerHitCut && stIndex == Muon::MuonStationIndex::EM ){
	    // don't apply the TGC requirement for the first station as it sometimes has not trigger hits due to TGC acceptance
	    int stationEta = m_idHelper->stationEta(chId);
	    if( abs(stationEta) != 1 ){
	      // remove EM segments without trigger hits
	      if( hitCounts.nphiTrigHitLayers == 0 && hitCounts.netaTrigHitLayers == 0 ) continue;
	    }
	  }  
	  if( stIndex == Muon::MuonStationIndex::EI ){
	    // remove CSC segment with hits in only one projection
	    if( hitCounts.nmdtHits() == 0 && 
		( (hitCounts.ncscHitsPhi > 0 && hitCounts.ncscHitsEta == 0) ||
		  (hitCounts.ncscHitsPhi == 0 && hitCounts.ncscHitsEta > 0) ) ) continue;
	  }
	}
	if(hitCounts.ncscHitsPhi+hitCounts.ncscHitsEta>0)  ATH_MSG_DEBUG(" CSC segment passed"); 
	FilteredSegmentCollection.push_back(*itSeg);
      }
    }
    else {
      ATH_MSG_VERBOSE( "No segment filtering required. " );
      for (std::vector<const Muon::MuonSegment*>::const_iterator itSeg = segments.begin(); itSeg!=segments.end(); ++itSeg)
	FilteredSegmentCollection.push_back((*itSeg));
    }
    ATH_MSG_DEBUG( "Filtered segments... in: " << segments.size() << ", out: " << FilteredSegmentCollection.size() );
 
    if ( (FilteredSegmentCollection.size() == 0) || (inDetCandidates.size() == 0) ) return;
  
    bool matchedSegment(false);
    unsigned int trackCount(0);
 
    ATH_MSG_DEBUG( "performing tag of " << inDetCandidates.size() << " tracks with " 
		   << FilteredSegmentCollection.size() << " segments. " );
    // Checking which surfaces have segments, to avoid useless extrapolations
    std::vector< bool > hasSeg(12, false);
    unsigned int segmentCount(0);
    if( m_doBidirectional ){
      for (std::vector<const Muon::MuonSegment*>::iterator itSeg = FilteredSegmentCollection.begin(); 
	   itSeg!=FilteredSegmentCollection.end(); ++itSeg, ++segmentCount)
	{

	  Identifier chId = m_edmHelper->chamberId(**itSeg);
	  Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(chId);
	  if( stIndex == Muon::MuonStationIndex::BI || 
	      stIndex == Muon::MuonStationIndex::BE )      hasSeg[0] = true;
	  else if( stIndex == Muon::MuonStationIndex::BM ) hasSeg[1] = true;
	  else if( stIndex == Muon::MuonStationIndex::BO ) hasSeg[2] = true;
	  else{
	    int stationEta = m_idHelper->stationEta(chId);
	    if( stationEta > 0 ){
	      if(  stIndex == Muon::MuonStationIndex::EI ) hasSeg[4] = true;
	      if(  stIndex == Muon::MuonStationIndex::EM ) hasSeg[5] = true;
	      if(  stIndex == Muon::MuonStationIndex::EO ) hasSeg[6] = true;
	      if(  stIndex == Muon::MuonStationIndex::EE ) hasSeg[7] = true;
	    }else{
	      if(  stIndex == Muon::MuonStationIndex::EI ) hasSeg[8] = true;
	      if(  stIndex == Muon::MuonStationIndex::EM ) hasSeg[9] = true;
	      if(  stIndex == Muon::MuonStationIndex::EO ) hasSeg[10] = true;
	      if(  stIndex == Muon::MuonStationIndex::EE ) hasSeg[11] = true;
	    }
	  }
	}
      ATH_MSG_DEBUG("Surfaces with segments: " << hasSeg);
    }


    std::vector<  MuonCombined::MuonSegmentInfo > segmentsInfoSelected; 
    for( auto idTP : inDetCandidates ){
      
      // ensure that the id trackparticle has a track
      if( ! idTP->indetTrackParticle().track() ) continue;

      // Ignore if this is a siAssociated disk (and property set).
      if ( m_ignoreSiAssocated && idTP->isSiliconAssociated() ) continue;

      ++trackCount;
       
      matchedSegment = false ;
      const Trk::Track* track = idTP->indetTrackParticle().track();

      if( !track ) continue;
      if( !track->perigeeParameters() ) continue;

      ATH_MSG_DEBUG( "Treating track " << trackCount );
      ATH_MSG_VERBOSE( "========================== dumping the full track =========================" );
      ATH_MSG_VERBOSE( *track );

      const Trk::TrackSummary* summary =  track->trackSummary() ; // Summary retrieved from TrackParticle (not from Track: it might be absent there)
      bool trkEtaInfo(false);
      if( !summary ) {
	ATH_MSG_WARNING( "Track has no trackSummary ! No ID eta information retrieved !" );
      } else {
	unsigned int nrIDetaHits = summary->get( Trk::numberOfSCTHits ) +  summary->get( Trk::numberOfPixelHits ) ;
	if( nrIDetaHits >=5 ) trkEtaInfo = true;
	if( !trkEtaInfo ) ATH_MSG_DEBUG( "Track has no ID eta information! (" << nrIDetaHits << " etaHits)" );
      }
      ++m_ntotTracks;
      bool hasMatch = m_doBidirectional;
      if( !m_doBidirectional ){


	hasSeg.clear();
	hasSeg.resize(12, false);
	segmentCount = 0;
	bool hasAngleMatch = false;
	double phiID = track->perigeeParameters()->momentum().phi();
	double thetaID = track->perigeeParameters()->momentum().theta();
	double etaID = track->perigeeParameters()->momentum().eta();
	double pID = track->perigeeParameters()->momentum().mag();
	double qID = track->perigeeParameters()->charge();
	for (std::vector<const Muon::MuonSegment*>::iterator itSeg = FilteredSegmentCollection.begin(); 
	     itSeg!=FilteredSegmentCollection.end(); ++itSeg, ++segmentCount) {


	  const Amg::Vector3D& pos = (*itSeg)->globalPosition();
	  double phiSeg = atan2(pos.y(),pos.x());
	  double thetaSeg = atan2(pos.perp(),pos.z());
	  double dotprodPhi = cos(phiID)*cos(phiSeg) + sin(phiID)*sin(phiSeg);
	  double dPhi = qID*acos(dotprodPhi) - qID/pID;
	  if(phiSeg<phiID) dPhi = - dPhi;
	  double dTheta = thetaSeg - thetaID;
	
	  if( fabs(dPhi) < 0.6 && qID*dTheta<0.2&&qID*dTheta>-0.6 ){
	    hasAngleMatch = true;
	    if( pID > 20000*(qID*etaID-2) ) {
	      Identifier chId = m_edmHelper->chamberId(**itSeg);
              if( !m_idHelper->isCsc(chId) ) hasMatch = true;
	      Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(chId);
	      if( stIndex == Muon::MuonStationIndex::BI || 
		  stIndex == Muon::MuonStationIndex::BE )      hasSeg[0] = true;
	      else if( stIndex == Muon::MuonStationIndex::BM ) hasSeg[1] = true;
	      else if( stIndex == Muon::MuonStationIndex::BO ) hasSeg[2] = true;
	      else{
		int stationEta = m_idHelper->stationEta(chId);
		if( stationEta > 0 ){
		  if(  stIndex == Muon::MuonStationIndex::EI ) hasSeg[4] = true;
		  if(  stIndex == Muon::MuonStationIndex::EM ) hasSeg[5] = true;
		  if(  stIndex == Muon::MuonStationIndex::EO ) hasSeg[6] = true;
		  if(  stIndex == Muon::MuonStationIndex::EE ) hasSeg[7] = true;
		}else{
		  if(  stIndex == Muon::MuonStationIndex::EI ) hasSeg[8] = true;
		  if(  stIndex == Muon::MuonStationIndex::EM ) hasSeg[9] = true;
		  if(  stIndex == Muon::MuonStationIndex::EO ) hasSeg[10] = true;
		  if(  stIndex == Muon::MuonStationIndex::EE ) hasSeg[11] = true;
		}
	      }
	    }
	  }
	}
	if( hasAngleMatch ) ++m_nangleMatch;
	if( m_useSegmentPreselection && !hasMatch ) continue;
	if( hasMatch ) ++m_npmatch;
      }


      //    std::vector< const MuTaggedSegment* > muTagSegVec;

      std::vector< MuonCombined::MuonSegmentInfo > segmentsInfo;
      int multiply(1);
      if( m_doBidirectional ) multiply=2; 
      std::vector< std::string > didExtrapolate( 12*multiply , "o" );
      std::vector< std::string > segStation( FilteredSegmentCollection.size(), "XXX" );
      std::vector< std::vector<std::string> > trkToSegment( 12*multiply, segStation ) ; 

      unsigned int extrapolation_counter(0);
      int numberOfExtrapolations = 1;
      if( m_doBidirectional ) numberOfExtrapolations = 2;
      const Trk::TrackParameters* trackAtMSEntrance[2] = {0,0};

      for( int i_extrapolations = 0; i_extrapolations < numberOfExtrapolations ; ++i_extrapolations ){
	ATH_MSG_DEBUG( "Extrapolation " << i_extrapolations );
      

	Trk::PropDirection direction(Trk::alongMomentum);
	if( i_extrapolations == 1 ) direction = Trk::oppositeMomentum;
        
        // in case of along momentum extrapolation, use pre-existing extrapolation if available 
        const Trk::CaloExtension* extension = 0;
        if( !m_caloExtensionTool.empty() )  m_caloExtensionTool->caloExtension( idTP->indetTrackParticle(), extension );
        if( direction == Trk::alongMomentum ){
          if( extension && extension->muonEntryLayerIntersection() ){
            const Trk::TrackParameters& pars = *extension->muonEntryLayerIntersection();
            trackAtMSEntrance[i_extrapolations] = pars.clone();
            ATH_MSG_DEBUG("Got MS entry pos: r " << pars.position().perp() 
                          << " z " << pars.position().z()
                          << " momentum " << pars.momentum().perp() << " cov " << pars.covariance() );
          }
        }else{
          trackAtMSEntrance[i_extrapolations] = p_MuTagMatchingTool->ExtrapolateTrktoMSEntrance( track, direction ); 
        }

	if( !trackAtMSEntrance[i_extrapolations] ) {
	  ATH_MSG_DEBUG( "there is no track at MS entrance... not tagging for direction " << i_extrapolations );
	  continue;
	}
	++m_natMSEntrance;
	    
	const Trk::TrackParameters* atSurface = 0;
	const Trk::TrackParameters* nextSurface = 0;
	std::vector< bool > hasSurf(12, false);
     

	for( unsigned int surface_counter = 0; surface_counter<12 ; ++surface_counter, ++extrapolation_counter ){
	  ATH_MSG_DEBUG( "Surface " << surface_counter );
	  if (surface_counter == 3) continue;
	  if (!hasSeg[surface_counter]) continue;

	  if( m_doTable ) didExtrapolate[extrapolation_counter] = "X" ;

	  std::vector<std::string> segVsSurf( FilteredSegmentCollection.size() , "xxx" );
	  //        int ndof(0);
	  const Trk::Surface* surface = m_surfaces->getSurface( surface_counter ) ;
	
	  switch (surface_counter) {

	  case 0: // BI + BEE
 	    atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	    if (atSurface) hasSurf[surface_counter] = true;
	    break;
 	  
	  case 1: // BM
 	    if ((hasSurf[0]) && (atSurface)) {
	      nextSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, atSurface, direction );
	      if (nextSurface) hasSurf[surface_counter] = true;
	      { delete atSurface; atSurface = 0; }
	      atSurface = nextSurface;
	    } else {
	      { delete atSurface; atSurface = 0; }
	      atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	      if (atSurface) hasSurf[surface_counter] = true;
	      if ((atSurface)&&(hasSeg[0])) ATH_MSG_DEBUG("Extrap to BI failed but to BM successful: why?");
	    }
	    break;
 	  
	  case 2: // BO
 	    if ((hasSurf[0]||hasSurf[1]) && (atSurface)) {
	      { delete nextSurface; nextSurface = 0; }
	      nextSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, atSurface, direction );
	      if (nextSurface) hasSurf[surface_counter] = true;
	      { delete atSurface; atSurface = 0; }
	      atSurface = nextSurface;
	    } else {
	      { delete atSurface; atSurface = 0; }
	      atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	      if (atSurface) hasSurf[surface_counter] = true;
	      if ((atSurface)&&(hasSeg[0]||hasSeg[1])) ATH_MSG_DEBUG("Extrap to BI/BM failed but to BO successful: why?");
	    }
	    break;

	    //	  case 3: // BE -> merged with BI
 	  
	  case 4: // EIA
	    { delete atSurface; atSurface = 0; }
 	    atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	    if (atSurface) hasSurf[surface_counter] = true;
	    break;
 	  
	  case 5: // EMA
 	    if ((hasSurf[4]) && (atSurface)) {
	      { delete nextSurface; nextSurface = 0; }
	      nextSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, atSurface, direction );
	      if (nextSurface) hasSurf[surface_counter] = true;
	      { delete atSurface; atSurface = 0; }
	      atSurface = nextSurface;
	    } else {
	      { delete atSurface; atSurface = 0; }
	      atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	      if (atSurface) hasSurf[surface_counter] = true;
	      if ((atSurface)&&(hasSeg[4])) ATH_MSG_DEBUG("Extrap to EIA failed but to EMA successful: why?");
	    }
	    break;
 	  
	  case 6: // EOA
 	    if ((hasSurf[4]||hasSurf[5]) && (atSurface)) {
	      { delete nextSurface; nextSurface = 0; }
	      nextSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, atSurface, direction );
	      if (nextSurface) hasSurf[surface_counter] = true;
	      { delete atSurface; atSurface = 0; }
	      atSurface = nextSurface;
	    } else {
	      { delete atSurface; atSurface = 0; }
	      atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	      if (atSurface) hasSurf[surface_counter] = true;
	      if ((atSurface)&&(hasSeg[4]||hasSeg[5])) ATH_MSG_DEBUG("Extrap to EMA failed but to EOA successful: why?");
	    }
	    break;
 	  
	  case 7: // EEA
	    { delete atSurface; atSurface = 0; }
 	    atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	    break;
 	  
	  case 8: // EIC
	    { delete atSurface; atSurface = 0; }
 	    atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	    if (atSurface) hasSurf[surface_counter] = true;
	    break;

 	  case 9: // EMC
 	    if ((hasSurf[8]) && (atSurface)) {
	      { delete nextSurface; nextSurface = 0; }	
	      nextSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, atSurface, direction );
	      if (nextSurface) hasSurf[surface_counter] = true;
	      { delete atSurface; atSurface = 0; }
	      atSurface = nextSurface;
	    } else {
	      { delete atSurface; atSurface = 0; }
	      atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	      if (atSurface) hasSurf[surface_counter] = true;
	      if ((atSurface)&&(hasSeg[8])) ATH_MSG_DEBUG("Extrap to EIC failed but to EMC successful: why?");
	    }
	    break;

 	  case 10: // EOC
 	    if ((hasSurf[8]||hasSurf[9]) && (atSurface)) {
	      { delete nextSurface; nextSurface = 0; }	
	      nextSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, atSurface, direction );
	      if (nextSurface) hasSurf[surface_counter] = true;
	      { delete atSurface; atSurface = 0; }
	      atSurface = nextSurface;
	    } else {
	      { delete atSurface; atSurface = 0; }
	      atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	      if (atSurface) hasSurf[surface_counter] = true;
	      if ((atSurface)&&(hasSeg[8]||hasSeg[9])) ATH_MSG_DEBUG("Extrap to EMC failed but to EOC successful: why?");
	    }
	    break;

 	  case 11: // EEC
	    { delete atSurface; atSurface = 0; }
 	    atSurface = p_MuTagMatchingTool->ExtrapolateTrktoMSSurface( surface, trackAtMSEntrance[i_extrapolations], direction );
	    break;

	  }

	  ++m_extrapolated[surface_counter];

	  if( !atSurface ){
	    if( m_doTable ) trkToSegment[extrapolation_counter] = segVsSurf ;
	    continue;
	  }
	  if( m_doTable ) didExtrapolate[extrapolation_counter] = "V" ;
	  ++m_goodExtrapolated[surface_counter];
	
	  //	p_MuTagMatchingTool->testExtrapolation( surface, track );

	  unsigned int segmentCount(0);

	  for (std::vector<const Muon::MuonSegment*>::iterator itSeg = FilteredSegmentCollection.begin(); 
	       itSeg!=FilteredSegmentCollection.end(); ++itSeg, ++segmentCount)
	    {
	      bool sameSeg(false);
	      for( unsigned int iseg = 0; iseg<  segmentsInfo.size() ; ++iseg){
                if( *itSeg ==  segmentsInfo[iseg].segment ) sameSeg=true;
              }
	      if(sameSeg){
		continue;
	      }
	    
	      if(m_doTable) {
		//		std::string threeLetters( p_OwnEDMHelper->SegmentAssociatedStationName( **itSeg ) );
		//		segStation[segmentCount] = threeLetters + p_OwnEDMHelper->EtaStationString( **itSeg) ;
		segStation[segmentCount] = "   ";
	      }

	      ////// Per Abstract Layer, loop over all segments, and find out wether the segment is close to the 
	      ////// extrapolated track to the abstract layer.
	      bool isMatched(false);
	      if( !m_doTable ){
		isMatched = p_MuTagMatchingTool->match( atSurface, 
							*itSeg, 
							m_surfaces->stationType(surface_counter) );
	      } else {
		if( !p_MuTagMatchingTool->surfaceMatch( atSurface,
							*itSeg, 
							m_surfaces->stationType(surface_counter) ) ) {
		  segVsSurf[segmentCount] = "surface" ;
		  continue;
		}
	      
		ATH_MSG_VERBOSE( "treating track " << trackCount << " (extrapolation = " << direction << ") and Segment " 
				 << segStation[segmentCount] << " (segment " << segmentCount << ")" ); 

		if( !p_MuTagMatchingTool->phiMatch( atSurface,
						    *itSeg, 
						    m_surfaces->stationType(surface_counter) ) ) {    
		  segVsSurf[segmentCount] = "RghPhi" ;
		  continue ;
		}
		if( trkEtaInfo ){
		  if( (m_surfaces->stationType(surface_counter)).find('B') != std::string::npos ) {
		    if( !p_MuTagMatchingTool->thetaMatch( atSurface, *itSeg ) ){
		      segVsSurf[segmentCount] = "RghTheta" ;
		      continue;
		    }
		  } else { 
		    if( !p_MuTagMatchingTool->rMatch( atSurface, *itSeg ) ){
		      segVsSurf[segmentCount] = "RghR" ;
		      continue;
		    }
		  }
		  isMatched = true;
		} else { //trk has no eta info
		  ATH_MSG_DEBUG( "bypassed roughEta/roughR cut since ID has no eta info!" );
		  isMatched = true;
		}
	      } //!m_dotable
	    
	      if( !isMatched ){ 
		if( m_doTable ){
		  ATH_MSG_DEBUG("isMatched is " << isMatched << " at this point, incoherent!");
		  if( !trkEtaInfo ) segVsSurf[segmentCount] = "posPhi" ;
		  else segVsSurf[segmentCount] = "pos" ;
		}
		continue;
	      } 
	      ////// after a rough match (on station name, second coordinate and finally precision coordinate)
	      ////// extrapolate the track to the plane-surface associated to the matching segment.
	      const Trk::AtaPlane* atSegSurface = p_MuTagMatchingTool->ExtrapolateTrktoSegmentSurface( *itSeg, trackAtMSEntrance[i_extrapolations], direction );
	      if( !atSegSurface ) continue;

	      MuonCombined::MuonSegmentInfo info = p_MuTagMatchingTool->muTagSegmentInfo(track,*itSeg,atSegSurface) ;
	      if(segmentToxAODSegmentMap)
		info.link = (*segmentToxAODSegmentMap)[*itSeg];
		//              ATH_MSG_DEBUG( " MuTagSegmentInfo  matchPosition  pullY " <<  info.pullY << " pullX " << info.pullX << " resY " <<  info.resY << " resX " << info.resX << " exErrorY " <<  info.exErrorY << " ex errorX " << info.exErrorX << " seg errorY " << info.segErrorY << " seg errorX " << info.segErrorX );
	      //              ATH_MSG_DEBUG( " MuTagSegmentInfo matchDirection pullYZ " <<  info.pullYZ << " pullXZ " << info.pullXZ << " resYZ " <<  info.dangleYZ << " resXZ " << info.dangleXZ << " exErrorYZ " <<  info.exErrorYZ << " ex errorXZ " << info.exErrorXZ << " seg errorYZ " << info.segErrorYZ << " seg errorXZ " << info.segErrorXZ );
	      //              ATH_MSG_DEBUG( " MuTagSegmentInfo matchDistance hasPhi " << info.hasPhi << " resX " << info.resX << " dangleXZ " << info.dangleXZ << " maximumResidualAlongTube " << info.maximumResidualAlongTube << " resY " << info.resY << " dangleYZ " << info.dangleYZ );    
	      //               ATH_MSG_DEBUG( " MuTagSegmentInfo matchCombined Pull hasPhi " << info.hasPhi << " minimumPullPhi " << info.minimumPullPhi << " pullChamber " << info.pullChamber << " pullCY " << info.pullCY );


	      isMatched =  p_MuTagMatchingTool->matchSegmentPosition( &info,trkEtaInfo);

	      if( !isMatched ){  
		if( m_doTable ){
		  if( !trkEtaInfo ) segVsSurf[segmentCount] = "posPhi" ;
		  else segVsSurf[segmentCount] = "pos" ;
		}
		delete atSegSurface;
		continue;	
	      } 
	      isMatched =  p_MuTagMatchingTool->matchSegmentDirection( &info,trkEtaInfo);

	      if( !isMatched ){ 
		if( m_doTable ){
		  if( !trkEtaInfo ) segVsSurf[segmentCount] = "dirPhi" ;
		  else segVsSurf[segmentCount] = "dir" ;
		}
		delete atSegSurface;
		continue;	
	      }

	      isMatched =  p_MuTagMatchingTool->matchDistance( &info);

	      if( !isMatched ){ 
		if( m_doTable ){
		  segVsSurf[segmentCount] = "dist" ;
		}
		delete atSegSurface;
		continue;	
	      }
	  
	      isMatched =  p_MuTagMatchingTool->matchCombinedPull( &info);
	      //           if(!isMatchedNew) {
	      //                ATH_MSG_DEBUG( " Segment rejected by combined pull cut " );
	      //           }

	      if( !isMatched ){ 
		if( m_doTable ){
		  segVsSurf[segmentCount] = "cpull" ;
		}
		delete atSegSurface;
		continue;	
	      }
	  
	      if( m_doPtDependentPullCut ){
		isMatched =  p_MuTagMatchingTool->matchPtDependentPull(&info, track);
		if( !isMatched ){ 
		  if( m_doTable ){
		    segVsSurf[segmentCount] = "ptpull" ;
		  }
		  delete atSegSurface;
		  continue;	
		}
	      }
	      // 
	      // Remove low p and low Field region defined by p < 6 and eta between 1.4-1.7
	      //	   
	      const Trk::Perigee* aMeasPer = track->perigeeParameters();
	      if( aMeasPer && m_removeLowPLowFieldRegion){
		double eta = - log(tan(aMeasPer->parameters()[Trk::theta]/2.));
		double p =  fabs( 1.0/(aMeasPer->parameters()[Trk::qOverP]) ) ;
		if(fabs(eta)>1.4 && fabs(eta)<1.7 && p < 6000.) isMatched = false;
		if( !isMatched ){ 
		  if( m_doTable ){
		    segVsSurf[segmentCount] = "lowField" ;
		  }
		  delete atSegSurface;
		  continue;	
		}
	      }
 
	    
	      if( m_doTable ) segVsSurf[segmentCount] = "TAG" ;
	      matchedSegment = true;
	      ATH_MSG_DEBUG( "Tagged the track with Segment " << segmentCount );

 	      segmentsInfo.push_back( info );
	      delete atSegSurface; //MuTaggedSegment creates its own MeasuredAtaPlane exTrk!! This one should be deleted.
	
	    } //end loop over segments
	
	  if( m_doTable ) trkToSegment[extrapolation_counter] = segVsSurf ;
	  if ((surface_counter == 2)||(surface_counter == 6)||(surface_counter == 7)||(surface_counter == 10)||(surface_counter == 11)) { delete atSurface; atSurface = 0; }
	  if ( atSurface )    { delete atSurface; atSurface = 0; }          // these 2 lines fix some major mem-leaks 
	  if ( nextSurface )  { delete nextSurface; nextSurface = 0; } 
	} //end loop over MS surfaces
         
        delete trackAtMSEntrance[i_extrapolations]; 
      } //end loop over nr Directions of extrapolation.
  
      if( m_doTable ){
	const Trk::Perigee* Perigee = track->perigeeParameters();
	//    const Trk::Perigee*  idperigee = dynamic_cast< const Trk::Perigee* >(perigee);
	ATH_MSG_DEBUG( "Dump Table for TP " << std::setw(5) << trackCount );
	ATH_MSG_DEBUG( "storing track to ntuple : " << Perigee->parameters()[Trk::d0] << ", " <<Perigee->parameters()[Trk::z0] 
		       << Perigee->parameters()[Trk::phi] << ", " <<Perigee->parameters()[Trk::theta]  << ", " <<Perigee->parameters()[Trk::qOverP] );

	printTable( didExtrapolate, segStation, trkToSegment );
      }

      if( !matchedSegment ) {
	ATH_MSG_DEBUG( "ID " << std::setw(3) << trackCount << " could not be matched with any segment on any abstract surface" );
	continue;   
      }
      
      // The loop over SM surfaces yielded matches for the extrapolated track.
      // Per match, a MuTagObject is created and added to the MuTagObjectContainer
      // A Track which has at least one match with the extrapolated track may be called a muon
      //----------------------------------------------------------------
      //----------------------------------------------------------------
      if(  segmentsInfo.size() > 0 ){
      
	++m_naccepted;

	if( !m_doBidirectional && !hasMatch ){
	  ATH_MSG_WARNING("MuTagIMO Muon rejected by matching cut ");
	}

	ATH_MSG_DEBUG( "number of accepted tracks "  << m_naccepted << " segmentsInfo size " << segmentsInfo.size() );       

	std::vector<  MuonCombined::MuonSegmentInfo > segmentsInfoSolved = p_MuTagAmbiguitySolverTool->selectBestMuTaggedSegments( segmentsInfo );
        for( auto x : segmentsInfoSolved ) segmentsInfoSelected.push_back(x);

	ATH_MSG_DEBUG(  "segmentsInfoSelected size " << segmentsInfoSelected.size() );

      }
  
    } //end loop over tracks  
 
    ATH_MSG_DEBUG(  "segmentsInfoSelected size after track loop " << segmentsInfoSelected.size() );
    std::vector< MuonCombined::MuonSegmentInfo > segmentsInfoFinal = p_MuTagAmbiguitySolverTool->solveAmbiguities( segmentsInfoSelected );
    ATH_MSG_DEBUG(  "segmentsInfoFinal size " << segmentsInfoFinal.size() );

    //    for( auto x : segmentsInfoSelected ) delete &x;
   
    for( unsigned int ns1 = 0;  ns1 < segmentsInfoFinal.size(); ns1++){
    
      if( ns1==0) ATH_MSG_DEBUG( m_printer->print(*segmentsInfoFinal[ns1].track) );
      else if( segmentsInfoFinal[ns1].track !=  segmentsInfoFinal[ns1-1].track)  ATH_MSG_DEBUG( m_printer->print(*segmentsInfoFinal[ns1].track) );
      ATH_MSG_DEBUG( m_printer->print(*segmentsInfoFinal[ns1].segment) );	

    }

    const InDetCandidate*     tagCandidate = 0;
    for( auto idTP : inDetCandidates ){
 
      if( ! idTP->indetTrackParticle().track() ) continue;
      matchedSegment = false ;
      const Trk::Track* track = idTP->indetTrackParticle().track();
      std::vector< MuonCombined::MuonSegmentInfo > segmentsInfoTag;
      segmentsInfoTag.reserve(segmentsInfoFinal.size());
      bool match = false;
      for( unsigned int ns1 = 0;  ns1 < segmentsInfoFinal.size(); ns1++){
        if(segmentsInfoFinal[ns1].track==track) {
          if(segmentsInfoFinal[ns1].nsegments>0) {
            segmentsInfoTag.push_back(segmentsInfoFinal[ns1]);
            tagCandidate = idTP;
            match = true;
          }
        }
      }
      if(match) {     
        ATH_MSG_DEBUG( "make Segment Tag object for " << m_printer->print(*track) << " nr segments " << segmentsInfoTag.size() );          
        SegmentTag* tag = new SegmentTag(segmentsInfoTag);
        // FIXME const-cast changes object passed in as const
        const_cast<InDetCandidate*>(tagCandidate)->addTag(*tag);
      } 
    }   




  }

  void MuonSegmentTagTool::printTable( std::vector< std::string > didEx ,  
				       std::vector< std::string > segStation , 
				       std::vector< std::vector<std::string> > segToSurf  ) const {
  
    ATH_MSG_DEBUG( std::setw(6) << "" << "EX? (o: no extrap, X: extrap failed, V: extrap OK)" );

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Surface" << std::setw(2) << "" ;
    for( unsigned int segment_counter = 0; segment_counter < segStation.size() ; ++segment_counter ){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << segStation[segment_counter] ;
    } 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;

    unsigned int extrapolation_counter(0);
    std::string signstr("");
    if (m_doBidirectional) signstr = "+";

    for( unsigned int counter = 0; counter<12 ; ++counter ){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(5) <<  m_surfaces->stationType(counter) << signstr << std::setw(3) << didEx[extrapolation_counter];
      for( unsigned int segment_counter = 0; segment_counter < segStation.size() ; ++segment_counter ){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << (segToSurf[extrapolation_counter])[segment_counter];
      } //end loop segments
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;
      ++extrapolation_counter;
    } //end loop surfaces

    if(m_doBidirectional ){
      signstr = "-";
      for( unsigned int counter = 0; counter<12 ; ++counter ){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(5) <<  m_surfaces->stationType(counter) << signstr << std::setw(3) << didEx[extrapolation_counter]; 
	for( unsigned int segment_counter = 0; segment_counter < segStation.size() ; ++segment_counter ){
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << (segToSurf[extrapolation_counter])[segment_counter];
	} //end loop segments
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;
	++extrapolation_counter;
      } //end loop surfaces
    }
  
    return ;
  }

}	// end of namespace

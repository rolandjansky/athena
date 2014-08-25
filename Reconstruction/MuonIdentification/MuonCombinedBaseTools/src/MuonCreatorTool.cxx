/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCreatorTool
//  AlgTool performing MS hit reallocation for a likely spectrometer-indet 
//  match which has given combined fit problems.
//  Extrapolates indet track to MS.
//  Returns a combined track with full track fit.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"

#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCreatorTool.h"
#include "SortInDetCandidates.h"
#include "MuonCombinedEvent/StacoTag.h"
#include "MuonCombinedEvent/CombinedFitTag.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "MuonCombinedEvent/MuGirlTag.h"
#include "MuonCombinedEvent/MuGirlLowBetaTag.h"
#include "MuonCombinedEvent/CaloTag.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkExInterfaces/IPropagator.h"

#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "MuGirlTagTool.h"

#include "MuonCombinedToolInterfaces/IMuonMeanMDTdADCFiller.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonCreatorTool::MuonCreatorTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_makeMSPreExtrapLink(false),
	m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
	m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
	m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
	m_muonPrinter("Rec::MuonPrintingTool/MuonPrintingTool"),
	m_particleCreator("Trk::TrackParticleCreatorTool/MuonCombinedTrackParticleCreator"),
	m_ambiguityProcessor("Trk::TrackSelectionProcessorTool/MuonSimpleAmbiProcessorTool"),
	m_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
	m_muonDressingTool("MuonCombined::MuonDressingTool/MuonDressingTool"),
	m_trackIsolationTool("xAOD::TrackIsolationTool"),
	m_caloIsolationTool("xAOD::CaloIsolationTool"),
        m_momentumBalanceTool("Rec::MuonMomentumBalanceSignificanceTool/MuonMomentumBalanceSignificanceTool"),
        m_scatteringAngleTool("Rec::MuonScatteringAngleSignificanceTool/MuonScatteringAngleSignificanceTool"),
        m_selectorTool("CP::MuonSelectionTool/MuonSelectionTool"),
	m_muonSegmentConverterTool("Muon::MuonSegmentConverterTool/MuonSegmentConverterTool"),
	m_meanMDTdADCTool("Rec::MuonMeanMDTdADCFillerTool/MuonMeanMDTdADCFillerTool")	
  {
    declareInterface<IMuonCreatorTool>(this);
    declareProperty("MakeTrackAtMSLink",m_makeMSPreExtrapLink=false);
    declareProperty("MuonIdHelperTool",m_idHelper );
    declareProperty("Printer",m_printer );
    declareProperty("Helper",m_edmHelper );
    declareProperty("MuonPrinter",m_muonPrinter );
    declareProperty("TrackParticleCreator",m_particleCreator );
    declareProperty("AmbiguityProcessor",m_ambiguityProcessor );
    declareProperty("Propagator",m_propagator );
    declareProperty("MuonDressingTool",m_muonDressingTool );
    declareProperty("TrackIsolationTool",m_trackIsolationTool );
    declareProperty("CaloIsolationTool",m_caloIsolationTool );
    declareProperty("MomentumBalanceTool",m_momentumBalanceTool);
    declareProperty("ScatteringAngleTool",m_scatteringAngleTool);
    declareProperty("MeanMDTdADCTool",m_meanMDTdADCTool);	
    declareProperty("BuildStauContainer",m_buildStauContainer=false);
  }

  MuonCreatorTool::~MuonCreatorTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonCreatorTool::initialize() {

    ATH_MSG_DEBUG("initialize: m_buildStauContainer "<<m_buildStauContainer);

    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_muonPrinter.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
    ATH_CHECK(m_particleCreator.retrieve());
    ATH_CHECK(m_ambiguityProcessor.retrieve());
    ATH_CHECK(m_propagator.retrieve());
    ATH_CHECK(m_muonDressingTool.retrieve());
    ATH_CHECK(m_muonSegmentConverterTool.retrieve());
    if(!m_trackIsolationTool.empty()) ATH_CHECK(m_trackIsolationTool.retrieve());
    if(!m_caloIsolationTool.empty()) ATH_CHECK(m_caloIsolationTool.retrieve());
    if(!m_momentumBalanceTool.empty()) ATH_CHECK(m_momentumBalanceTool.retrieve());
    if(!m_scatteringAngleTool.empty()) ATH_CHECK(m_scatteringAngleTool.retrieve());   
    if(!m_selectorTool.empty()) ATH_CHECK(m_selectorTool.retrieve());
    if(!m_meanMDTdADCTool.empty()) ATH_CHECK(m_meanMDTdADCTool.retrieve());
	
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCreatorTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCreatorTool::create( const MuonCandidateCollection* muonCandidates,  const InDetCandidateCollection* inDetCandidates,
				xAOD::MuonContainer& container, 
				xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
				xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection,
				xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection ) const {

    unsigned int numIdCan = inDetCandidates ? inDetCandidates->size() : 0;
    unsigned int numMuCan = muonCandidates  ? muonCandidates->size() : 0;
    ATH_MSG_DEBUG("Creating xAOD::Muons from: " << numIdCan << " indet candidates and " << numMuCan << " muon candidates ");
    
    // Create containers for resolved candidates (always of type VIEW_ELEMENTS)
    InDetCandidateCollection resolvedInDetCandidates( SG::VIEW_ELEMENTS );
    MuonCandidateCollection resolvedMuonCandidates( SG::VIEW_ELEMENTS );
    
    // Resolve Overlap
    if (!m_buildStauContainer) resolveOverlaps(inDetCandidates, muonCandidates, resolvedInDetCandidates, resolvedMuonCandidates);    
    else selectStaus(inDetCandidates, resolvedInDetCandidates);    

    if( inDetCandidates ) ATH_MSG_DEBUG("InDetCandidates : overlap removal " << inDetCandidates->size() << " in, " <<resolvedInDetCandidates.size() <<" out");
    if( muonCandidates  ) ATH_MSG_DEBUG("MuonCandidates  : overlap removal " << muonCandidates->size() << " in, " <<resolvedMuonCandidates.size() <<" out");

    // Create a container for resolved candidates (always of type VIEW_ELEMENTS)
    for( auto can : resolvedInDetCandidates ) {
      ATH_MSG_DEBUG("New InDetCandidate");
      xAOD::Muon* muon = create(*can,container,combinedTrackParticleContainer,combinedTrackCollection,
				extrapolatedTrackParticleContainer,extrapolatedTrackCollection,
				xaodSegments, muonSegmentCollection);
      if( !muon ) ATH_MSG_DEBUG("no muon found");
      else        ATH_MSG_DEBUG("muon found");
      ATH_MSG_DEBUG("Creation of Muon from InDetCandidates done");
    }
    for( auto can : resolvedMuonCandidates )  {
      ATH_MSG_DEBUG("New MuonCandidate");
      create(*can,container,extrapolatedTrackParticleContainer,extrapolatedTrackCollection);
      ATH_MSG_DEBUG("Creation of Muon from MuonCandidates done");
    }
    
    ATH_MSG_DEBUG("Printing muon container:");
    ATH_MSG_DEBUG(m_muonPrinter->print(container));
    ATH_MSG_DEBUG("Done");
  }

  xAOD::Muon* MuonCreatorTool::create( const MuonCandidate& candidate, xAOD::MuonContainer& container,
				       xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ) const {

    // skip all muons without extrapolated track
    if( !candidate.extrapolatedTrack() ) return 0;

    // Create the xAOD object:
    xAOD::Muon* muon = new xAOD::Muon();
    container.push_back( muon );

    muon->setAuthor(xAOD::Muon::MuidSA);
    muon->setMuonType(xAOD::Muon::MuonStandAlone);
    muon->addAllAuthor(xAOD::Muon::MuidSA);

    // create candidate from SA muon only
    addMuonCandidate(candidate,*muon,extrapolatedTrackParticleContainer,extrapolatedTrackCollection);
    
    if( !dressMuon(*muon) ){
      ATH_MSG_WARNING("Failed to dress muon");
      delete muon; 
      return 0;
    }
    return muon;
  }

  xAOD::Muon* MuonCreatorTool::create( const InDetCandidate& candidate, xAOD::MuonContainer& container, 
				       xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
				       xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection,
				       xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection
 ) const {

    // no tags, no muon
    if( candidate.combinedDataTags().empty() ) return 0;

    // Create the xAOD object:
    xAOD::Muon* muon = new xAOD::Muon();
    container.push_back( muon );
    std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
    muon->setMuonSegmentLinks(segments) ;
  
    std::vector<const TagBase*> tags = candidate.combinedDataTags();
    // now we need to sort these to get the best muon
    
    // set the link to the ID track particle
    if( candidate.indetTrackParticleLink().isValid() ) {
      muon->setTrackParticleLink(xAOD::Muon::InnerDetectorTrackParticle, candidate.indetTrackParticleLink() );
      ATH_MSG_DEBUG("Adding InDet Track: pt " << (*candidate.indetTrackParticleLink())->pt() 
		    << " eta " << (*candidate.indetTrackParticleLink())->eta() << " phi " << (*candidate.indetTrackParticleLink())->phi() );
    }
    ATH_MSG_DEBUG("creating Muon with " << tags.size() << " tags ");
    // loop over the tags
    bool first = true;
    for( auto tag : tags ){

      ATH_MSG_DEBUG("Handling tag: type " << tag->type() );

      // staus
      if( m_buildStauContainer ){
	const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);
	if( muGirlLowBetaTag ) {
	  ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag ");
	  
	  muon->setAuthor(tag->author());
	  muon->setMuonType(tag->type());
	  
	  if (tag->type() == xAOD::Muon::Combined ){
	    ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag combined");
	    
	    addMuGirlLowBeta(*muon,*muGirlLowBetaTag,
			     combinedTrackParticleContainer,combinedTrackCollection,
			     extrapolatedTrackParticleContainer, extrapolatedTrackCollection,
			     xaodSegments, muonSegmentCollection);
	    continue;   
	  }
	}
      }else{
	
	//Don't want staus in muon branch
	const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);
	if (muGirlLowBetaTag) continue; 
	
        // set author info 
        if( first ) {

	  ATH_MSG_DEBUG("MuonCreatorTool first muon: author="<<tag->author()<<"  type="<<tag->type());
      	  muon->setAuthor(tag->author());
      	  muon->setMuonType(tag->type());
	  first = false;
        }

        muon->addAllAuthor(tag->author());

        // this is not too elegant, maybe rethink implementation
        if( tag->type() == xAOD::Muon::Combined ){
	  // work out type of tag
	  const CombinedFitTag* cbFitTag = dynamic_cast<const CombinedFitTag*>(tag);
	  if( cbFitTag ) {
	    addCombinedFit(*muon,*cbFitTag,
	  		 combinedTrackParticleContainer,combinedTrackCollection,
	  		 extrapolatedTrackParticleContainer, extrapolatedTrackCollection);
	    continue;
	  }
	  const StacoTag* stacoTag = dynamic_cast<const StacoTag*>(tag);
	  if( stacoTag ) {
	    addStatisticalCombination(*muon,candidate,*stacoTag,combinedTrackParticleContainer,
	  			    extrapolatedTrackParticleContainer, extrapolatedTrackCollection);
	    continue;
	  }
	  const MuGirlTag* muGirlTag = dynamic_cast<const MuGirlTag*>(tag);
	  if( muGirlTag ) {
	    addMuGirl(*muon,*muGirlTag,
  		      combinedTrackParticleContainer,combinedTrackCollection,
  		      extrapolatedTrackParticleContainer, extrapolatedTrackCollection,
		      xaodSegments, muonSegmentCollection);
  	    continue;
  	  }
	  ATH_MSG_WARNING("Unknown combined tag ");
	  continue;
        }else if( tag->type() == xAOD::Muon::SegmentTagged ){
	  const SegmentTag* segTag = dynamic_cast<const SegmentTag*>(tag);
	  if( segTag ) {
	    addSegmentTag(*muon,*segTag);
	    continue;
	  }
	 
	  const MuGirlTag* muGirlTag = dynamic_cast<const MuGirlTag*>(tag);
	  if( muGirlTag ) {
	    addMuGirl(*muon,*muGirlTag,
	  	      combinedTrackParticleContainer,combinedTrackCollection,
		      extrapolatedTrackParticleContainer, extrapolatedTrackCollection,
		      xaodSegments, muonSegmentCollection);
	    continue;
	  }
	  ATH_MSG_WARNING("Unknown MuGirl tag ");
	  continue;
        }else if( tag->type() == xAOD::Muon::CaloTagged ){
	  const CaloTag* caloTag = dynamic_cast<const CaloTag*>(tag);
	  if( caloTag ) {
	    addCaloTag(*muon,*caloTag);
	    continue;
	  }
	  ATH_MSG_WARNING("Unknown calo tag type ");
	  continue;
        }
        ATH_MSG_WARNING("Unknown tag type ");
      }
    }// m_buildStauContainer

    if( !dressMuon(*muon) ){
      ATH_MSG_WARNING("Failed to dress muon");
      delete muon; 
      return 0;
    }
    ATH_MSG_DEBUG("Done creating muon");

    return muon;
  }

  void MuonCreatorTool::addStatisticalCombination( xAOD::Muon& muon, const InDetCandidate& candidate, const StacoTag& tag,
						   xAOD::TrackParticleContainer* combinedTrackParticleContainer,
						   xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ) const {
    
    ATH_MSG_DEBUG("Adding Staco Muon  " << tag.author() << " type " << tag.type());
    
    if( !muon.combinedTrackParticleLink().isValid() ){
      // create primary track particle

      // get summary
      const Trk::Track* idTrack = candidate.indetTrackParticle().track();
      const Trk::Track* msTrack = tag.muonCandidate().extrapolatedTrack() ? tag.muonCandidate().extrapolatedTrack() : &tag.muonCandidate().muonSpectrometerTrack();

      const Trk::TrackSummary* idSummary = idTrack ? idTrack->trackSummary() : 0;
      const Trk::TrackSummary* msSummary = msTrack->trackSummary();

      Trk::TrackSummary summary;
      if( idSummary ) summary += *idSummary;
      if( msSummary ) summary += *msSummary;

      Trk::FitQuality fq(tag.matchChi2(),5);
      Trk::TrackInfo info(msTrack->info());
	// todo update patrec bit set adding ID values
      
      xAOD::TrackParticle* tp = m_particleCreator->createParticle(&tag.combinedParameters(),&fq,&info,&summary,
                                                                  std::vector<const Trk::TrackParameters*>(),
                                                                  std::vector<xAOD::ParameterPosition>(),
                                                                  xAOD::muon, combinedTrackParticleContainer);
      if( !tp ){
        ATH_MSG_WARNING("Failed to create track particle");
      }else{
        // if the combined track particle is part of a container set the link
        if( combinedTrackParticleContainer ){
          ElementLink<xAOD::TrackParticleContainer> link(*combinedTrackParticleContainer,combinedTrackParticleContainer->size()-1);
          if( link.isValid() ) {
            //link.toPersistent();
            ATH_MSG_DEBUG("Adding statistical combination: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
            muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
          }
          // for the purpose of the truth matching, set the track link to point to the ID track
          tp->setTrackLink(candidate.indetTrackParticle().trackLink());
        }else{
          // if the track particle is not added to a container, delete it 
          delete tp;
        }
      }
    }
    // add muon candidate
    addMuonCandidate(tag.muonCandidate(),muon,extrapolatedTrackParticleContainer,extrapolatedTrackCollection);
    
    ATH_MSG_DEBUG("Done adding Staco Muon  " << tag.author() << " type " << tag.type());
  }

  void MuonCreatorTool::addCombinedFit( xAOD::Muon& muon, const CombinedFitTag& tag, 
					xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
					xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ) const {
    ATH_MSG_DEBUG("Adding Combined fit Muon  " << tag.author() << " type " << tag.type());
    if( !muon.combinedTrackParticleLink().isValid() ){
      // if the combined track particle is part of a container set the link
      if( combinedTrackParticleContainer ){

        // create element link from the track, const_cast for now until we sort out the constness of the tags
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<CombinedFitTag&>(tag).releaseCombinedTrack(), 
        *combinedTrackParticleContainer,
        combinedTrackCollection  );
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding combined fit: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of Combined TrackParticle Link failed");
      }

    }
    // add muon candidate
    addMuonCandidate(tag.muonCandidate(),muon,extrapolatedTrackParticleContainer,extrapolatedTrackCollection);

    ATH_MSG_DEBUG("Done adding Combined Fit Muon  " << tag.author() << " type " << tag.type());

  }

  void MuonCreatorTool::addMuGirlLowBeta( xAOD::Muon& muon, const MuGirlLowBetaTag & tag, 
                                   xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
                                   xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection,
                                   xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection) const {
    ATH_MSG_DEBUG("Adding MuGirlLowBeta Muon  " << tag.author() << " type " << tag.type());
    
    if( !muon.combinedTrackParticleLink().isValid() && tag.getCombinedTrack()){  
      
      // if the combined track particle is part of a container set the link
      if( combinedTrackParticleContainer ){
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuGirlLowBetaTag&>(tag).releaseCombinedTrack(), 
                                                                                         *combinedTrackParticleContainer,
                                                                                         combinedTrackCollection  );                    
        
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirlLowBeta: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of MuGirlLowBeta TrackParticle Link failed");
      } 
    }    
    /*
      if( extrapolatedTrackParticleContainer &&  tag.updatedExtrapolatedTrack()){
	// create element link 
	ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuGirlLowBetaTag&>(tag).releaseUpdatedExtrapolatedTrack(),
											 *extrapolatedTrackParticleContainer,
											 extrapolatedTrackCollection  );                
      
	if( link.isValid() ) {
          //link.toPersistent();
	ATH_MSG_DEBUG("Adding MuGirlLowBeta: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
	muon.setTrackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle, link );
	}
	else ATH_MSG_WARNING("Creating of MuGirlLowBeta TrackParticle Link failed");
      }     
    }
    */
    
    if (muonSegmentCollection){
      ATH_MSG_DEBUG( "Adding MuGirlLowBeta muonSegmentColection");
      
      std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
      for( const auto& segs : tag.segments() ){ 
	
        ElementLink<xAOD::MuonSegmentContainer> link = createMuonSegmentElementLink(*segs,
                                                                                    *xaodSegments,
                                                                                    muonSegmentCollection);             
        
        if( link.isValid() ) {
          //link.toPersistent();
          segments.push_back(link);
          ATH_MSG_DEBUG("Adding MuGirlLowBeta: xaod::MuonSegment px " << (*link)->px() << " py " << (*link)->py() << " pz " << (*link)->pz() ); 
        }
        else ATH_MSG_WARNING("Creating of MuGirlLowBeta segment Link failed");         	
      }  
      const_cast<MuGirlLowBetaTag&>(tag).releaseSegments();  
    }  
  }
  
  void MuonCreatorTool::addMuGirl( xAOD::Muon& muon, const MuGirlTag& tag, 
                                   xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
                                   xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection,
                                   xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection) const {
    ATH_MSG_DEBUG("Adding MuGirl Muon  " << tag.author() << " type " << tag.type());

    if( !muon.combinedTrackParticleLink().isValid() && tag.getCombinedTrack()){
      // if the combined track particle is part of a container set the link
      if( combinedTrackParticleContainer ){
        
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuGirlTag&>(tag).releaseCombinedTrack(), 
                                                                                         *combinedTrackParticleContainer,
                                                                                         combinedTrackCollection  );                    
        
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
      }

      if( extrapolatedTrackParticleContainer &&  tag.updatedExtrapolatedTrack()){
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuGirlTag&>(tag).releaseUpdatedExtrapolatedTrack(),
                                                                                         *extrapolatedTrackParticleContainer,
                                                                                         extrapolatedTrackCollection  );                
                
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle, link );
        }
        else ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
      }     
    }

    if (muonSegmentCollection){
      ATH_MSG_DEBUG( "Adding MuGirl muonSegmentColection");

      std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
      for( const auto& segs : tag.segments() ){ 
	
        ElementLink<xAOD::MuonSegmentContainer> link = createMuonSegmentElementLink(*segs,
                                                                                    *xaodSegments,
                                                                                    muonSegmentCollection);             
        
        if( link.isValid() ) {
          //link.toPersistent();
          segments.push_back(link);
          ATH_MSG_DEBUG("Adding MuGirl: xaod::MuonSegment px " << (*link)->px() << " py " << (*link)->py() << " pz " << (*link)->pz() ); 
        }
        else ATH_MSG_WARNING("Creating of MuGirl segment Link failed");         
	
      }
      
    }  
    const_cast<MuGirlTag&>(tag).releaseSegments();
    
    ATH_MSG_DEBUG("Done Adding MuGirl Muon  " << tag.author() << " type " << tag.type());    


  }

  void MuonCreatorTool::addSegmentTag( xAOD::Muon& muon, const SegmentTag& tag ) const {
    ATH_MSG_DEBUG("Adding Segment Tag Muon  " << tag.author() << " type " << tag.type());
    if( !muon.muonSegmentLinks().empty() ) return;
    
    std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
    for( const auto& info : tag.segmentsInfo() ){
      if( info.link.isValid() ){
        segments.push_back(info.link);
      }
    }
    muon.setMuonSegmentLinks(segments) ;
  }

  void MuonCreatorTool::addCaloTag( xAOD::Muon& , const CaloTag& tag ) const {
    ATH_MSG_DEBUG("Adding Calo Muon  " << tag.author() << " type " << tag.type());

  }

  
  ElementLink<xAOD::TrackParticleContainer> MuonCreatorTool::createTrackParticleElementLink( const Trk::Track& track, 
											     xAOD::TrackParticleContainer& trackParticleContainer, 
											     TrackCollection* trackCollection  ) const {
    const xAOD::TrackParticle* tp = 0;
    if( trackCollection ) {
      // if a track collection is provided add the track to the collection and create the TP using the element link
      trackCollection->push_back(const_cast<Trk::Track*>(&track));
      ElementLink<TrackCollection> link( *trackCollection,trackCollection->size()-1);
      if( link.isValid() ) {
        //link.toPersistent();
        tp = m_particleCreator->createParticle( link, &trackParticleContainer, 0, xAOD::muon );
      } else ATH_MSG_WARNING("Track Collection link invalid, creating TrackParticle from track");
    }
    if( !tp ){
      // create extrapolated track particle without a link to the track
      tp = m_particleCreator->createParticle( track, &trackParticleContainer, 0, xAOD::muon );
    }
    if( !trackCollection ) delete &track;
    if( tp ){
      ElementLink<xAOD::TrackParticleContainer> link( trackParticleContainer,trackParticleContainer.size()-1);
      //link.toPersistent();
      return link;
    }
    return ElementLink<xAOD::TrackParticleContainer>();
  }

  ElementLink<xAOD::MuonSegmentContainer> MuonCreatorTool::createMuonSegmentElementLink( const Muon::MuonSegment & muonSegment, 
                                                                                         xAOD::MuonSegmentContainer& xaodSegments,
                                                                                         Trk::SegmentCollection* muonSegmentCollection
                                                                                         ) const {
    const xAOD::MuonSegment* ms = 0;
    if( muonSegmentCollection ) {

      // if a muon segment collection is provided create the MS using the element link
      muonSegmentCollection->push_back(const_cast<Muon::MuonSegment*>(&muonSegment));

      ElementLink<Trk::SegmentCollection> link( *muonSegmentCollection,muonSegmentCollection->size()-1);
      if( link.isValid() ) {
        link.toPersistent();
        ms = m_muonSegmentConverterTool->convert(link, &xaodSegments);
      } else ATH_MSG_WARNING("Segment Collection link invalid");
    }
    
    if( !ms ){
      // create muon segments without a link to the track
      ms = m_muonSegmentConverterTool->convert( muonSegment, &xaodSegments);
    }
    if( !muonSegmentCollection ) delete &muonSegment;
    if( ms ){
      ElementLink<xAOD::MuonSegmentContainer> link( xaodSegments,xaodSegments.size()-1);
      link.toPersistent();
      return link;
    }
    return ElementLink<xAOD::MuonSegmentContainer>();
  }

  void MuonCreatorTool::addMuonCandidate( const MuonCandidate& candidate, xAOD::Muon& muon,
					  xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, 
					  TrackCollection* extrapolatedTrackCollection) const {

    // only set once 
    if( muon.muonSpectrometerTrackParticleLink().isValid() ) return;

    if(m_makeMSPreExtrapLink) {
      // copy over element link to the MS track before extrapolation
      muon.auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink") = candidate.muonSpectrometerTrackLink();
      ATH_MSG_DEBUG("Added aux data element link, status = " << muon.auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink").isValid());
    }

    // we both the container and the extrapolated muon track particle to add the link
    if( !extrapolatedTrackParticleContainer || !candidate.extrapolatedTrack() ) return; 
    
    // create element link from the track, const_cast for now until we sort out the constness of the MuonCanidates
    ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuonCandidate&>(candidate).releaseExtrapolatedTrack(),
										     *extrapolatedTrackParticleContainer,
										     extrapolatedTrackCollection );
    if( link.isValid() ) {
      ATH_MSG_DEBUG("Adding standalone fit: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
      //link.toPersistent();
      muon.setTrackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle, link );

    }
  }

  void MuonCreatorTool::selectStaus( const InDetCandidateCollection* inDetCandidates,InDetCandidateCollection& resolvedInDetCandidates) const {
    //staus need to be kept so can go into separate xAOD staucontainer, if desired
    std::vector< InDetCandidate* > staus;

    if( inDetCandidates ) {

      for( auto candidate : *inDetCandidates ) {
	const InDetCandidate *can = candidate;
	std::vector<const TagBase*> tags = can->combinedDataTags();
	for( auto tag : tags ){
	  const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);
	  if( muGirlLowBetaTag) {
	    staus.push_back(candidate);	
	    ATH_MSG_DEBUG("save ID candidate stau ");
	  }
	}
      }
    }
    
    //add stau candidates
    if (!staus.empty()){
      resolvedInDetCandidates.insert( resolvedInDetCandidates.end(), staus.begin(), staus.end() );
      ATH_MSG_DEBUG("insert stau into resolvedInDetCandidates");
    }

    for(auto candidate : resolvedInDetCandidates) {
      // retrieve the primary tag
      const TagBase* primaryTag = candidate->combinedDataTags().at(0);
      if(!primaryTag) {
	ATH_MSG_WARNING("Unable to retrieve tag data for InDetCandidate");
	continue;
      }
    }
    // print-out
    if( msgLvl(MSG::DEBUG) && inDetCandidates ){
      ATH_MSG_DEBUG("ID candidates:  " << inDetCandidates->size() << "  after stau selection " << resolvedInDetCandidates.size() );
      for( auto candidate : resolvedInDetCandidates ){
        msg(MSG::DEBUG) << "ID candidate staus:  " << candidate->toString() << endreq;
      }      
    }

  }

  void MuonCreatorTool::resolveOverlaps( const InDetCandidateCollection* inDetCandidates, const MuonCandidateCollection* muonCandidates, 
					 InDetCandidateCollection& resolvedInDetCandidates, MuonCandidateCollection& resolvedMuonCandidates) const {
    
    TrackCollection* resolvedTracks=0;
    TrackCollection* muonTracksToDelete = new TrackCollection;
    std::set<const Trk::Track*> alreadyIncluded;
    
    // the muons only found by the calo tagger should always be kept so we can filter them out from the start
    std::vector< InDetCandidate* > caloMuons;

    // Each InDetCandidate corresponds to a different ID track.
    // Resolve overlap among InDetCandidates for cases where different 
    // ID tracks are tagged by the same MS info (track or segment)
    if( inDetCandidates ) {

      // first loop over ID candidates and select all candidates that have a tag
      for( auto candidate : *inDetCandidates ){
        if( !candidate->combinedDataTags().empty() ) {
          // sort the tags based on quality
          std::vector<const TagBase*>& tags = const_cast<std::vector<const TagBase*>&>(candidate->combinedDataTags());
          std::stable_sort(tags.begin(),tags.end(),SortTagBasePtr());
          if( tags.size() == 1 && tags.front()->type() == xAOD::Muon::CaloTagged ) caloMuons.push_back(candidate);
          else                                                                     resolvedInDetCandidates.push_back(candidate);
	  
          if( msgLvl(MSG::DEBUG) && tags.size() > 1 ) {
            msg(MSG::DEBUG) << " InDetCandidate ";
            for( auto tag : tags ) msg(MSG::DEBUG) << " - " << tag->toString() << " " << typeRank(tag->type()) << " " << authorRank(tag->author());
            msg(MSG::DEBUG)<< endreq;
          }
        }
      }
    
      // now sort the selected ID candidates
      std::stable_sort(resolvedInDetCandidates.begin(),resolvedInDetCandidates.end(),SortInDetCandidates());
      if( msgLvl(MSG::DEBUG) ){
        ATH_MSG_DEBUG("ID candidates:  " << inDetCandidates->size() << "  after removal " << resolvedInDetCandidates.size() );
        for( auto candidate : resolvedInDetCandidates ){
          msg(MSG::DEBUG) << "ID candidate:  " << candidate->toString() << endreq;
        }      
      }
      
      // create a track collection for MS info, to be overlap removed
      // a dummy track for segment tagged candidates to be used in the overlap check
      TrackCollection* muonTracks = new TrackCollection(SG::VIEW_ELEMENTS);
      std::map<const Trk::Track*, InDetCandidate*> trackInDetCandLinks;

      for(auto candidate : resolvedInDetCandidates) {
	// retrieve the primary tag
	const TagBase* primaryTag = candidate->combinedDataTags().at(0);
	if(!primaryTag) {
	  ATH_MSG_WARNING("Unable to retrieve tag data for InDetCandidate");
	  continue;
	}
	// check if a track is available
	if(primaryTag->primaryTrack()) {
          if( alreadyIncluded.count(primaryTag->primaryTrack()) ){
            ATH_MSG_WARNING("Duplicate canidate " << candidate->toString() );
            continue;
          }
          alreadyIncluded.insert(primaryTag->primaryTrack());
	  trackInDetCandLinks[ primaryTag->primaryTrack() ] = candidate;
	  muonTracks->push_back( const_cast<Trk::Track*>(primaryTag->primaryTrack()) );
	}
	// if not, make a dummy track out of segments
	else{
          std::vector< const Muon::MuonSegment*> segments = primaryTag->associatedSegments();
          if( !segments.empty() && candidate->indetTrackParticle().trackLink().isValid() ){
            Trk::Track* dummyTrack = createDummyTrack(primaryTag->associatedSegments(), *(candidate->indetTrackParticle().track()));
            trackInDetCandLinks[ dummyTrack ] = candidate;
            muonTracks->push_back( dummyTrack );
            muonTracksToDelete->push_back( dummyTrack );
          }
        }
      }
      
      // Resolve ambiguity between muon tracks
      resolvedTracks = m_ambiguityProcessor->process(muonTracks); 
      delete muonTracks;
      resolvedInDetCandidates.clear();
      
      // link back to InDet candidates and fill the resolved container  
      for(auto track : *resolvedTracks) {
      
        auto trackCandLink = trackInDetCandLinks.find( track );
        if( trackCandLink==trackInDetCandLinks.end() ) {
          ATH_MSG_WARNING("Unable to find internal link between MS track and ID candidate!");
          continue;	
        }
	
        resolvedInDetCandidates.push_back( trackCandLink->second );
      }
      // add muons only found by calo tagger
      resolvedInDetCandidates.insert( resolvedInDetCandidates.end(), caloMuons.begin(), caloMuons.end() );
    }

    // print-out
    if( msgLvl(MSG::DEBUG) && inDetCandidates ){
      ATH_MSG_DEBUG("ID candidates:  " << inDetCandidates->size() << "  after ambiguity solving " << resolvedInDetCandidates.size() );
      for( auto candidate : resolvedInDetCandidates ){
        msg(MSG::DEBUG) << "ID candidate:  " << candidate->toString() << endreq;
      }      
    }
    
    // MuonCandidateCollection contains all muon tracks (SA extrapolated or not)
    // Resolve overlap with InDetCandidate collection
    if( muonCandidates ) {

      if( msgLvl(MSG::DEBUG) ){
        ATH_MSG_DEBUG("Muon candidates:  " << muonCandidates->size() );
        for( auto candidate : *muonCandidates ){
          msg(MSG::DEBUG) << "Muon candidate:  " << candidate->toString() << endreq;
        }      
      }

      
      if(!resolvedTracks) {
        resolvedTracks = new TrackCollection(SG::VIEW_ELEMENTS);
      }

      // add MS tracks to resolvedTrack collection and store a link between tracks and muon candidates
      std::map<const Trk::Track*, MuonCandidate*> trackMuonCandLinks;
      for(auto candidate : *muonCandidates) {
        const Trk::Track* track = candidate->extrapolatedTrack() ? candidate->extrapolatedTrack() : &candidate->muonSpectrometerTrack();
        if( alreadyIncluded.count(track) ){
          ATH_MSG_WARNING("Duplicate track " << m_printer->print(*track));
          continue;
        }
        alreadyIncluded.insert(track);
        resolvedTracks->push_back(const_cast<Trk::Track*>(track));
        trackMuonCandLinks[ track ] = candidate;
      }
      
      // solve ambiguity
      TrackCollection* resolvedAllTracks = m_ambiguityProcessor->process(resolvedTracks); 
      
      // loop over resolved tracks and fill resolved muon candidates
      for(auto track : *resolvedAllTracks) {
        auto trackCandLink = trackMuonCandLinks.find( track );
        if( trackCandLink != trackMuonCandLinks.end() ) 
          resolvedMuonCandidates.push_back( trackCandLink->second );
      }    
      if(resolvedTracks) delete resolvedTracks;
      if(resolvedAllTracks) delete resolvedAllTracks;

      // print-out
      if( msgLvl(MSG::DEBUG) ){
        ATH_MSG_DEBUG("Muon candidates:  " << muonCandidates->size() << "  after ambiguity solving " << resolvedMuonCandidates.size() );
        for( auto candidate : resolvedMuonCandidates ){
          msg(MSG::DEBUG) << "Muon candidate:  " << candidate->toString() << endreq;
        }      
      }
    }
    delete muonTracksToDelete;
  }

 
  Trk::Track* MuonCreatorTool::createDummyTrack( std::vector<const Muon::MuonSegment*> segments, const Trk::Track& indetTrack ) const {

    ATH_MSG_VERBOSE("Creating dummy tracks from segments...");

    DataVector<const Trk::TrackStateOnSurface>*  trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    for(auto seg : segments) {
      // create pars for muon and loop over hits
      double momentum = 1e8;
      double charge   = 0.;
      const Trk::TrackParameters* pars = m_edmHelper->createTrackParameters( *seg, momentum, charge );
      std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg->containedMeasurements().begin();
      std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg->containedMeasurements().end();
      for( ;mit!=mit_end;++mit ){
        const Trk::MeasurementBase& meas = **mit;  
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
        const Trk::TrackParameters* exPars = m_propagator->propagateParameters(*pars,meas.associatedSurface(),
        Trk::anyDirection, false, Trk::MagneticFieldProperties(Trk::NoField));
        if(!exPars){
          ATH_MSG_VERBOSE("Could not propagate Track to segment surface");
        }
        const Trk::TrackStateOnSurface* trackState = new Trk::TrackStateOnSurface( meas.clone(), exPars, 0, 0, typePattern );
        trackStateOnSurfaces->push_back( trackState ); 
      } // end segment loop
      delete pars;
    }
    
    Trk::TrackInfo info(Trk::TrackInfo::Unknown,Trk::muon);
    Trk::TrackInfo::TrackPatternRecoInfo author = Trk::TrackInfo::MuTag;
    info.setPatternRecognitionInfo(author);
    Trk::Track* newtrack = new Trk::Track(info, trackStateOnSurfaces, (indetTrack.fitQuality())->clone());

    return newtrack;
  }

  void MuonCreatorTool::fillIsolation( xAOD::Muon& muon ) const {
    
    const xAOD::TrackParticle* tp = 0;
    if(muon.inDetTrackParticleLink().isValid()) tp = *muon.inDetTrackParticleLink();
    if( !tp ) tp = muon.primaryTrackParticle();
    if( !tp ) {
      ATH_MSG_WARNING(" No TrackParticle found for muon " );
      return;
    }
    if( !m_trackIsolationTool.empty() ){
      std::vector<xAOD::Iso::IsolationType> ptcones = { xAOD::Iso::ptcone40,xAOD::Iso::ptcone30,xAOD::Iso::ptcone20,xAOD::Iso::ptcone10 };
      xAOD::TrackIsolation trackIsolation;
      if( !m_trackIsolationTool->trackIsolation( trackIsolation, *tp,ptcones ) ) {
        ATH_MSG_WARNING(" Calculation of TrackIsolation failed");
      }else{
        ATH_MSG_VERBOSE("Got track isolation " << trackIsolation.ptcones[0]); 
        for( unsigned int i=0;i<ptcones.size();++i ) muon.setIsolation(trackIsolation.ptcones[i], ptcones[i]);
      }
    }
    
    if( !m_caloIsolationTool.empty() ){
      xAOD::CaloIsolation caloIsolation;
      std::vector<xAOD::Iso::IsolationType> etcones = { xAOD::Iso::etcone40,xAOD::Iso::etcone30,xAOD::Iso::etcone20,xAOD::Iso::etcone10 };
      if( !m_caloIsolationTool->caloIsolation( caloIsolation, *tp, etcones, xAOD::ICaloIsolationTool::Ecore  ) ) {
        ATH_MSG_WARNING(" Calculation of CaloIsolation failed");
      }else{
        ATH_MSG_VERBOSE("Got calo isolation " << caloIsolation.etcones[0]); 
        for( unsigned int i=0;i<etcones.size();++i ) muon.setIsolation(caloIsolation.etcones[i], etcones[i]);
      }
    }
  }

  bool MuonCreatorTool::dressMuon(  xAOD::Muon& muon ) const {

    if( muon.primaryTrackParticleLink().isValid() ){
      // update parameters with primaty track particle
      setP4(muon,*muon.primaryTrackParticle());
    }else{
      ATH_MSG_WARNING("No primary track particle set, deleting muon");
      return false;
    }
    
    // add hit summary
    m_muonDressingTool->addMuonHitSummary(muon);
    
    // add isolation
    fillIsolation(muon);

    // calculate scattering significance and momentum balance significance
    if( !m_scatteringAngleTool.empty() ){
      Rec::ScatteringAngleSignificance scatSign = m_scatteringAngleTool->scatteringAngleSignificance(muon);
      float curvatureSignificance = scatSign.curvatureSignificance();
      muon.setParameter(curvatureSignificance,xAOD::Muon::scatteringCurvatureSignificance);
      float neighbourSignificance = scatSign.curvatureSignificance();
      muon.setParameter(neighbourSignificance,xAOD::Muon::scatteringNeighbourSignificance);
      ATH_MSG_VERBOSE("Got curvatureSignificance " << curvatureSignificance << "  and neighbourSignificance " << neighbourSignificance );
    }

    if( !m_momentumBalanceTool.empty() ){
      float momentumBalanceSignificance = m_momentumBalanceTool->momentumBalanceSignificance(muon);
      muon.setParameter(momentumBalanceSignificance,xAOD::Muon::momentumBalanceSignificance);
      ATH_MSG_VERBOSE("Got momentumBalanceSignificance " << momentumBalanceSignificance);
    }
 
    if( !m_meanMDTdADCTool.empty() ){
      float meanDeltaADC = float(m_meanMDTdADCTool->meanMDTdADCFiller(muon));
      muon.setParameter(meanDeltaADC,xAOD::Muon::meanDeltaADCCountsMDT);
      ATH_MSG_VERBOSE("Got meanDeltaADCCountsMDT " << meanDeltaADC);
    }
 
    if( !m_selectorTool.empty() ){
      // set id cuts
      m_selectorTool->setPassesIDCuts(muon);
      ATH_MSG_VERBOSE("Setting passesIDCuts " << muon.passesIDCuts() );
      
      // set quality
      m_selectorTool->setQuality(muon);
      ATH_MSG_VERBOSE("Setting Quality " << muon.quality() );
      
    }

    return true;
  }
	
  

}	// end of namespace

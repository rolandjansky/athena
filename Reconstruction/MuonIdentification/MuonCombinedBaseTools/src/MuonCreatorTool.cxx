/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
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
#include "TrkParameters/TrackParameters.h"
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
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "MuGirlTagTool.h"
#include "MuGirlInterfaces/CandidateSummary.h"

#include "MuonCombinedToolInterfaces/IMuonMeanMDTdADCFiller.h"
#include "RecoToolInterfaces/IParticleCaloClusterAssociationTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"

#include "muonEvent/CaloEnergy.h"
#include "FourMomUtils/P4Helpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "TrackToCalo/CaloCellCollector.h"
#include "CaloEvent/CaloCellContainer.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonCreatorTool::MuonCreatorTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_makeMSPreExtrapLink(false),
	m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
	m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
	m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
	m_muonPrinter("Rec::MuonPrintingTool/MuonPrintingTool"),
        m_caloExtTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
        m_caloClusterAssociationTool("Rec::ParticleCaloClusterAssociationTool/ParticleCaloClusterAssociationTool"),
        m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool"),
	m_particleCreator("Trk::TrackParticleCreatorTool/MuonCombinedTrackParticleCreator"),
	m_ambiguityProcessor("Trk::TrackSelectionProcessorTool/MuonSimpleAmbiProcessorTool"),
	m_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
	m_muonDressingTool("MuonCombined::MuonDressingTool/MuonDressingTool"),
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
    declareProperty("ParticleCaloExtensionTool", m_caloExtTool);      
    declareProperty("ParticleCaloClusterAssociationTool",m_caloClusterAssociationTool);
    declareProperty("Helper",m_edmHelper );
    declareProperty("MuonPrinter",m_muonPrinter );
    declareProperty("TrackParticleCreator",m_particleCreator );
    declareProperty("AmbiguityProcessor",m_ambiguityProcessor );
    declareProperty("Propagator",m_propagator );
    declareProperty("MuonDressingTool",m_muonDressingTool );
    declareProperty("MomentumBalanceTool",m_momentumBalanceTool);
    declareProperty("ScatteringAngleTool",m_scatteringAngleTool);
    declareProperty("MeanMDTdADCTool",m_meanMDTdADCTool);	
    declareProperty("BuildStauContainer",m_buildStauContainer=false);
    declareProperty("FillEnergyLossFromTrack",m_fillEnergyLossFromTrack=true);
    declareProperty("FillExtraELossInfo", m_fillExtraELossInfo=true);
    declareProperty("SaveCaloExtensionPosition", m_saveCaloExtensionPosition=true);
    declareProperty("PrintSummary", m_printSummary=false);
    //Default data source for the calocells
    declareProperty("CaloCellContainer", m_cellContainerName="AllCalo");
  }

  MuonCreatorTool::~MuonCreatorTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonCreatorTool::initialize() {

    if( m_buildStauContainer ) ATH_MSG_DEBUG(" building Stau container ");

    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_muonPrinter.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
    ATH_CHECK(m_particleCreator.retrieve());
    ATH_CHECK(m_ambiguityProcessor.retrieve());
    ATH_CHECK(m_propagator.retrieve());
    ATH_CHECK(m_muonDressingTool.retrieve());
    ATH_CHECK(m_muonSegmentConverterTool.retrieve());
    if(!m_momentumBalanceTool.empty()) ATH_CHECK(m_momentumBalanceTool.retrieve());
    if(!m_scatteringAngleTool.empty()) ATH_CHECK(m_scatteringAngleTool.retrieve());   
    if(!m_selectorTool.empty()) ATH_CHECK(m_selectorTool.retrieve());
    if(!m_meanMDTdADCTool.empty()) ATH_CHECK(m_meanMDTdADCTool.retrieve());
    if(m_saveCaloExtensionPosition) ATH_CHECK(m_caloExtTool.retrieve());
    if(!m_caloClusterAssociationTool.empty()) ATH_CHECK(m_caloClusterAssociationTool.retrieve());
    if(!m_caloCellAssociationTool.empty())    ATH_CHECK(m_caloCellAssociationTool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCreatorTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCreatorTool::create( const MuonCandidateCollection* muonCandidates,  const InDetCandidateCollection* inDetCandidates, OutputData& outputData ) const {

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
      xAOD::Muon* muon = create(*can,outputData);
      if( !muon ) ATH_MSG_DEBUG("no muon found");
      else        ATH_MSG_DEBUG("muon found");
      ATH_MSG_DEBUG("Creation of Muon from InDetCandidates done");
    }
    for( auto can : resolvedMuonCandidates )  {
      ATH_MSG_DEBUG("New MuonCandidate");
      create(*can,outputData);
      ATH_MSG_DEBUG("Creation of Muon from MuonCandidates done");
    }

    if( msgLvl(MSG::DEBUG) || m_printSummary ){    
      ATH_MSG_INFO("Printing muon container:");
      ATH_MSG_INFO(m_muonPrinter->print(*outputData.muonContainer));
      ATH_MSG_INFO("Done");
    }
    if( msgLvl(MSG::VERBOSE) && outputData.clusterContainer ){    
      ATH_MSG_VERBOSE("Associated clusters : " << outputData.clusterContainer->size());
    }

  }

  xAOD::Muon* MuonCreatorTool::create( const MuonCandidate& candidate, OutputData& outputData ) const {

    // skip all muons without extrapolated track
    if( !candidate.extrapolatedTrack() ) return 0;

    // Create the xAOD object:
    xAOD::Muon* muon = new xAOD::Muon();
    outputData.muonContainer->push_back( muon );

    muon->setAuthor(xAOD::Muon::MuidSA);
    muon->setMuonType(xAOD::Muon::MuonStandAlone);
    muon->addAllAuthor(xAOD::Muon::MuidSA);

    // create candidate from SA muon only
    addMuonCandidate(candidate,*muon,outputData);
    
    if( !dressMuon(*muon) ){
      ATH_MSG_WARNING("Failed to dress muon");
      delete muon; 
      return 0;
    }

    return muon;
  }


  xAOD::Muon* MuonCreatorTool::create( const InDetCandidate& candidate, OutputData& outputData ) const {

    // no tags, no muon
    if( candidate.combinedDataTags().empty() ) return 0;

    // Create the xAOD object:
    xAOD::Muon* muon = new xAOD::Muon();
    outputData.muonContainer->push_back( muon );
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
        const MuGirlLowBetaTag* muGirlLowBetaConstTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);

        if( muGirlLowBetaConstTag ) {
          ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag ");

          //want to dress tag, so non constant.
          MuGirlLowBetaTag* muGirlLowBetaTag = const_cast<MuGirlLowBetaTag*>(muGirlLowBetaConstTag);
	  
          muon->setAuthor(tag->author());
          muon->setMuonType(tag->type());
	  
          if (tag->type() == xAOD::Muon::Combined ){
            ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag combined");
	    
	    // Create the xAOD object:
	    xAOD::SlowMuon* slowMuon = 0;
            if( outputData.slowMuonContainer ) {
              slowMuon = new xAOD::SlowMuon();
              outputData.slowMuonContainer->push_back( slowMuon );
            }
            addMuGirlLowBeta(*muon,*muGirlLowBetaTag,slowMuon, outputData );


	    ATH_MSG_DEBUG("slowMuon muonContainer size "<<outputData.muonContainer->size());
	    ElementLink<xAOD::MuonContainer> muonLink(*outputData.muonContainer,outputData.muonContainer->size()-1);
	    if( slowMuon && muonLink.isValid() ) {

	      ATH_MSG_DEBUG("slowMuon muonLink valid");
	      slowMuon->setMuonLink(muonLink);
	    }
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
        
        m_haveAddedCaloInformation = false;
        // this is not too elegant, maybe rethink implementation
        if( tag->type() == xAOD::Muon::Combined ){
          // work out type of tag
          const CombinedFitTag* cbFitTag = dynamic_cast<const CombinedFitTag*>(tag);
          if( cbFitTag ) {
            addCombinedFit(*muon,*cbFitTag, outputData );
            continue;
          }
          const StacoTag* stacoTag = dynamic_cast<const StacoTag*>(tag);
          if( stacoTag ) {
            addStatisticalCombination(*muon,candidate,*stacoTag, outputData );
            continue;
          }
          const MuGirlTag* muGirlTag = dynamic_cast<const MuGirlTag*>(tag);
          if( muGirlTag ) {
            addMuGirl(*muon,*muGirlTag, outputData );
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
            addMuGirl(*muon,*muGirlTag, outputData );
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
    
    // check if there is a cluster container, if yes collect the cells around the muon
    if( outputData.clusterContainer ) collectCells(*muon,*outputData.clusterContainer);

    ATH_MSG_DEBUG("Done creating muon");

    return muon;
  }

  void MuonCreatorTool::addStatisticalCombination( xAOD::Muon& muon, const InDetCandidate& candidate, const StacoTag& tag, OutputData& outputData ) const {
    
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
                                                                  xAOD::muon, outputData.combinedTrackParticleContainer);
      if( !tp ){
        ATH_MSG_WARNING("Failed to create track particle");
      }else{
        // if the combined track particle is part of a container set the link
        if( outputData.combinedTrackParticleContainer ){
          ElementLink<xAOD::TrackParticleContainer> link(*outputData.combinedTrackParticleContainer,outputData.combinedTrackParticleContainer->size()-1);
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
    addMuonCandidate(tag.muonCandidate(),muon,outputData);
    
    ATH_MSG_DEBUG("Done adding Staco Muon  " << tag.author() << " type " << tag.type());
  }

  void MuonCreatorTool::addCombinedFit( xAOD::Muon& muon, const CombinedFitTag& tag, OutputData& outputData ) const {

    ATH_MSG_DEBUG("Adding Combined fit Muon  " << tag.author() << " type " << tag.type());
    if( !muon.combinedTrackParticleLink().isValid() ){
      // if the combined track particle is part of a container set the link
      if( outputData.combinedTrackParticleContainer ){

        // create element link from the track, const_cast for now until we sort out the constness of the tags
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<CombinedFitTag&>(tag).releaseCombinedTrack(), 
                                                                                         *outputData.combinedTrackParticleContainer,
                                                                                         outputData.combinedTrackCollection  );
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding combined fit: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of Combined TrackParticle Link failed");
      }

    }
    // add muon candidate
    addMuonCandidate(tag.muonCandidate(),muon,outputData);

    ATH_MSG_DEBUG("Done adding Combined Fit Muon  " << tag.author() << " type " << tag.type());

  }

  void MuonCreatorTool::addMuGirlLowBeta( xAOD::Muon& muon, MuGirlLowBetaTag& tag, xAOD::SlowMuon* slowMuon, OutputData& outputData ) const {

    ATH_MSG_DEBUG("Adding MuGirlLowBeta Muon  " << tag.author() << " type " << tag.type());
    
    //get stauExtras and write to slowMuon
    const MuGirlNS::StauExtras* stauExtras = tag.getStauExtras();
    if( slowMuon && stauExtras ){

      ATH_MSG_DEBUG("StauSummary beta "<<stauExtras->betaAll);
      slowMuon->setBeta(stauExtras->betaAll);
      slowMuon->setBetaT(stauExtras->betaAllt);
      slowMuon->setAnn(stauExtras->ann);
      slowMuon->setNRpcHits(stauExtras->numRpcHitsInSeg);
      slowMuon->setNTileCells(stauExtras->numCaloCells);
      slowMuon->setRpcInfo(stauExtras->rpcBetaAvg,
                           stauExtras->rpcBetaRms,
                           stauExtras->rpcBetaChi2,
                           stauExtras->rpcBetaDof);
      slowMuon->setMdtInfo(stauExtras->mdtBetaAvg,
                           stauExtras->mdtBetaRms,
                           stauExtras->mdtBetaChi2,
                           stauExtras->mdtBetaDof);
      slowMuon->setCaloInfo(stauExtras->caloBetaAvg,
                            stauExtras->caloBetaRms,
                            stauExtras->caloBetaChi2,
                            stauExtras->caloBetaDof);
      std::vector<uint8_t>    eTechVec;
      std::vector<unsigned int> idVec;
      std::vector<float> mToFVec;
      std::vector<float> xVec;
      std::vector<float> yVec;
      std::vector<float> zVec;
      std::vector<float> eVec;
      std::vector<float> errorVec;
      std::vector<float> shiftVec;
      std::vector<float> propagationTimeVec;

      for( const auto& hit : stauExtras->hits ){
        eTechVec.push_back( hit.eTech );
        idVec.push_back(hit.id.get_compact());
        mToFVec.push_back(hit.mToF);
        xVec.push_back(hit.x);
        yVec.push_back(hit.y);
        zVec.push_back(hit.z);
        eVec.push_back(hit.e);
        errorVec.push_back(hit.error);
        shiftVec.push_back(hit.shift);
        propagationTimeVec.push_back(hit.propagationTime);
      }
      slowMuon->auxdata< std::vector<uint8_t> >("hitTechnology") = eTechVec;
      slowMuon->auxdata< std::vector<unsigned int> >("hitIdentifier") = idVec;
      slowMuon->auxdata< std::vector<float> >("hitTOF") = mToFVec;
      slowMuon->auxdata< std::vector<float> >("hitPositionX") = xVec;
      slowMuon->auxdata< std::vector<float> >("hitPositionY") = yVec;
      slowMuon->auxdata< std::vector<float> >("hitPositionZ") = zVec;
      slowMuon->auxdata< std::vector<float> >("hitEnergy") = eVec;
      slowMuon->auxdata< std::vector<float> >("hitError") = errorVec;
      slowMuon->auxdata< std::vector<float> >("hitShift") = shiftVec;
      slowMuon->auxdata< std::vector<float> >("hitPropagationTime") = propagationTimeVec;
    }

    if( !muon.combinedTrackParticleLink().isValid() && tag.getCombinedTrack()){  
      
      // if the combined track particle is part of a container set the link
      if( outputData.combinedTrackParticleContainer ){
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuGirlLowBetaTag&>(tag).releaseCombinedTrack(), 
                                                                                         *outputData.combinedTrackParticleContainer,
                                                                                         outputData.combinedTrackCollection  );                    
        
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
    
    if (outputData.muonSegmentCollection){
      ATH_MSG_DEBUG( "Adding MuGirlLowBeta muonSegmentColection");
      
      std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
      for( const auto& segs : tag.segments() ){ 
	
        ElementLink<xAOD::MuonSegmentContainer> link = createMuonSegmentElementLink(*segs,
                                                                                    *outputData.xaodSegmentContainer,
                                                                                    outputData.muonSegmentCollection);             
        
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
  
  void MuonCreatorTool::addMuGirl( xAOD::Muon& muon, const MuGirlTag& tag, OutputData& outputData ) const {

    ATH_MSG_DEBUG("Adding MuGirl Muon  " << tag.author() << " type " << tag.type());

    if( !muon.combinedTrackParticleLink().isValid() && tag.getCombinedTrack()){
      // if the combined track particle is part of a container set the link
      if( outputData.combinedTrackParticleContainer ){
        
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuGirlTag&>(tag).releaseCombinedTrack(), 
                                                                                         *outputData.combinedTrackParticleContainer,
                                                                                         outputData.combinedTrackCollection  );                    
        
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
      }

      if( outputData.extrapolatedTrackParticleContainer &&  tag.updatedExtrapolatedTrack()){
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuGirlTag&>(tag).releaseUpdatedExtrapolatedTrack(),
                                                                                         *outputData.extrapolatedTrackParticleContainer,
                                                                                         outputData.extrapolatedTrackCollection  );                
                
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle, link );
        }
        else ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
      }     
    }

    if (outputData.xaodSegmentContainer){
      ATH_MSG_DEBUG( "Adding MuGirl muonSegmentColection");

      std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
      for( const auto& segs : tag.segments() ){ 
	
        ElementLink<xAOD::MuonSegmentContainer> link = createMuonSegmentElementLink(*segs,
                                                                                    *outputData.xaodSegmentContainer,
                                                                                    outputData.muonSegmentCollection);             
        
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

  void MuonCreatorTool::addCaloTag( xAOD::Muon& mu, const CaloTag& tag ) const {
    ATH_MSG_DEBUG("Adding Calo Muon  " << tag.author() << " type " << tag.type());
    
    if (!m_haveAddedCaloInformation){
      mu.setParameter(static_cast<float>( tag.fsrCandidateEnergy() ), xAOD::Muon::FSR_CandidateEnergy);
      //std::cout<<"EJWM Adding Calo muon with elt="<<tag.energyLossType()<<std::endl;
      mu.setEnergyLossType(static_cast<xAOD::Muon::EnergyLossType>( tag.energyLossType() ) );
      mu.setParameter(static_cast<float>( tag.caloLRLikelihood() ), xAOD::Muon::CaloLRLikelihood);
      mu.setParameter(static_cast<int>( tag.caloMuonIdTag() ), xAOD::Muon::CaloMuonIDTag); 
      m_haveAddedCaloInformation = true;
    }
    
    if ( m_fillExtraELossInfo) {
      // Here we can make sure that we store the extra calotag information - just always add it since this is then unambigious for debugging
      mu.auxdata< float >("CT_ET_Core")   = tag.etCore();
      mu.auxdata< float >("CT_EL_Type")   = tag.energyLossType();
      mu.auxdata< float >("CT_ET_LRLikelihood")   = tag.caloLRLikelihood();
      mu.auxdata< float >("CT_ET_FSRCandidateEnergy")   = tag.fsrCandidateEnergy();
    }
    // FIXME - calo deposits
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
                                                                                         xAOD::MuonSegmentContainer& xaodSegmentContainer,
                                                                                         Trk::SegmentCollection* muonSegmentCollection
                                                                                         ) const {
    const xAOD::MuonSegment* ms = 0;
    if( muonSegmentCollection ) {

      // if a muon segment collection is provided create the MS using the element link
      muonSegmentCollection->push_back(const_cast<Muon::MuonSegment*>(&muonSegment));

      ElementLink<Trk::SegmentCollection> link( *muonSegmentCollection,muonSegmentCollection->size()-1);
      if( link.isValid() ) {
        link.toPersistent();
        ms = m_muonSegmentConverterTool->convert(link, &xaodSegmentContainer);
      } else ATH_MSG_WARNING("Segment Collection link invalid");
    }
    
    if( !ms ){
      // create muon segments without a link to the track
      ms = m_muonSegmentConverterTool->convert( muonSegment, &xaodSegmentContainer);
    }
    if( !muonSegmentCollection ) delete &muonSegment;
    if( ms ){
      ElementLink<xAOD::MuonSegmentContainer> link( xaodSegmentContainer,xaodSegmentContainer.size()-1);
      link.toPersistent();
      return link;
    }
    return ElementLink<xAOD::MuonSegmentContainer>();
  }

  void MuonCreatorTool::addMuonCandidate( const MuonCandidate& candidate, xAOD::Muon& muon, OutputData& outputData ) const {

    // only set once 
    if( muon.muonSpectrometerTrackParticleLink().isValid() ) return;

    if(m_makeMSPreExtrapLink) {
      // copy over element link to the MS track before extrapolation
      muon.auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink") = candidate.muonSpectrometerTrackLink();
      ATH_MSG_DEBUG("Added aux data element link, status = " << muon.auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink").isValid());
    }

    // we both the container and the extrapolated muon track particle to add the link
    if( !outputData.extrapolatedTrackParticleContainer || !candidate.extrapolatedTrack() ) return; 
    
    // create element link from the track, const_cast for now until we sort out the constness of the MuonCanidates
    ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( *const_cast<MuonCandidate&>(candidate).releaseExtrapolatedTrack(),
										     *outputData.extrapolatedTrackParticleContainer,
										     outputData.extrapolatedTrackCollection );
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
	  
          if( msgLvl(MSG::DEBUG) && tags.size() >= 1 ) {
            msg(MSG::DEBUG) << " InDetCandidate ";
            for( auto tag : tags ) msg(MSG::DEBUG) << " - " << tag->toString() << " " << typeRank(tag->type()) << " " << authorRank(tag->author());
            msg(MSG::DEBUG)<< endreq;
          }
        }
      }
    
      // now sort the selected ID candidates
      std::stable_sort(resolvedInDetCandidates.begin(),resolvedInDetCandidates.end(),SortInDetCandidates());
      if( msgLvl(MSG::DEBUG) ){
        ATH_MSG_DEBUG("ID candidates:  " << inDetCandidates->size() << "  after selection " << resolvedInDetCandidates.size() );
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

  bool MuonCreatorTool::dressMuon(  xAOD::Muon& muon ) const {

    if( muon.primaryTrackParticleLink().isValid() ){
      // update parameters with primaty track particle
      setP4(muon,*muon.primaryTrackParticle());
      float qOverP = muon.primaryTrackParticle()->qOverP();
      muon.setCharge(qOverP/std::fabs(qOverP));
    }else{
      ATH_MSG_WARNING("No primary track particle set, deleting muon");
      return false;
    }
    
    // add hit summary
    m_muonDressingTool->addMuonHitSummary(muon);
    
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
    
    float energyloss=0.0;
    unsigned int numEnergyLossPerTrack=0;
    bool problem=false;
    if (m_fillEnergyLossFromTrack){
      const Trk::Track* trk =0;
      if (muon.combinedTrackParticleLink ().isValid()){
         trk = (*(muon.combinedTrackParticleLink() ) )->track();
      }
      if (!trk && muon.muonSpectrometerTrackParticleLink().isValid()) {
        trk = (*(muon.muonSpectrometerTrackParticleLink() ) )->track();
      }
      if (trk) {
        for (  auto tsos : trk->trackStateOnSurfaces ()->stdcont() ){
          const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack () );
          if ( meot ){
            const Trk::EnergyLoss* el = meot->energyLoss();
            // tsos->type(Trk::TrackStateOnSurface::CaloDeposit)
            if (false && el) {
              numEnergyLossPerTrack++;
              energyloss+=el->deltaE(); // FIXME - should we be summing these?
              if (el->deltaE()<0) {
                ATH_MSG_WARNING("Found negative energy loss (i.e. energy gain!) for this object: "<<(*el));
                problem=true;
              }
            }
            
            const CaloEnergy* caloEnergy = dynamic_cast<const CaloEnergy*>(el);
            if (caloEnergy){
              numEnergyLossPerTrack++;

              muon.setParameter(static_cast<float>( caloEnergy->deltaE() ),                xAOD::Muon::EnergyLoss);
              muon.setParameter(static_cast<float>( caloEnergy->deltaEParam() ),           xAOD::Muon::ParamEnergyLoss);
              muon.setParameter(static_cast<float>( caloEnergy->deltaEMeas() ),            xAOD::Muon::MeasEnergyLoss);
	      muon.setParameter(static_cast<float>( caloEnergy->sigmaDeltaE() ),           xAOD::Muon::EnergyLossSigma);
	      muon.setParameter(static_cast<float>( caloEnergy->sigmaDeltaEMeas() ),       xAOD::Muon::MeasEnergyLossSigma);
	      muon.setParameter(static_cast<float>( caloEnergy->sigmaPlusDeltaEParam() ),  xAOD::Muon::ParamEnergyLossSigmaPlus);
	      muon.setParameter(static_cast<float>( caloEnergy->sigmaMinusDeltaEParam() ), xAOD::Muon::ParamEnergyLossSigmaMinus);

	      muon.setEnergyLossType(static_cast<xAOD::Muon::EnergyLossType>( caloEnergy->energyLossType() ) );
              muon.setParameter(static_cast<float>( caloEnergy->fsrCandidateEnergy() ), xAOD::Muon::FSR_CandidateEnergy);
              if ( m_fillExtraELossInfo) {
                muon.auxdata< float >("ET_Core")   = caloEnergy->etCore();
              }
              m_haveAddedCaloInformation=true;
            }
          }
        }
        if (numEnergyLossPerTrack>1) {
          ATH_MSG_VERBOSE("More than one e loss per track... ");
          problem=true;
        }
        if (m_fillExtraELossInfo) muon.auxdata< unsigned int >("numEnergyLossPerTrack")   = numEnergyLossPerTrack;
        if (problem) ATH_MSG_VERBOSE("Dumping problematic track: "<<(*trk));
        // if (!m_haveAddedCaloInformation) muon.setParameter(energyloss, xAOD::Muon::EnergyLoss);
        // ATH_MSG_VERBOSE("Setting energy loss to :"<<energyloss );
      } else {
        ATH_MSG_VERBOSE("Couldn't find matching track which might have energy loss." );
      }
    }

    //// add calo eta, phi decoration
    if(m_saveCaloExtensionPosition) addCaloExtensionPositions( muon );
    
    //// add closest cluster
    if( !m_caloClusterAssociationTool.empty() ) setClosestCluster(muon);

    return true;
  }
  
  void MuonCreatorTool::setClosestCluster( xAOD::Muon& muon ) const {
    if( !muon.primaryTrackParticle() ) return;

    // get cluster association
    const Rec::ParticleClusterAssociation* association = 0;
    if( !m_caloClusterAssociationTool->particleClusterAssociation( *muon.primaryTrackParticle(), association, 0.1 ) ||
        !association || 
        association->caloExtension().caloLayerIntersections().empty() ||
        association->data().empty() ){
      return;
    }

    ATH_MSG_DEBUG("Got associated clusters " << association->data().size() );
    const Trk::CaloExtension& caloExtension =  association->caloExtension();
    float eta = caloExtension.caloLayerIntersections().front()->position().eta();
    float phi = caloExtension.caloLayerIntersections().front()->position().phi();
    float dr2Min = 1e9;
    unsigned int closestIndex = 0;
    for( unsigned int i=0;i<association->data().size();++i ){
      auto el = association->data()[i];
      if( !el.isValid() ){
        ATH_MSG_WARNING("Found invalid element to cluster!");
        continue;
      }
      float dPhi = P4Helpers::deltaPhi( (*el)->phi(), phi);
      float dEta = (*el)->eta()-eta;
      float dr2  = dPhi*dPhi+ dEta*dEta;
      ATH_MSG_DEBUG("  dr " << sqrt(dr2) << " index " << i << " min dr " << sqrt(dr2Min) );
      if( dr2 < dr2Min ){
        dr2Min = dr2;
        closestIndex = i;
      }
    }
    ATH_MSG_DEBUG("Selected closest cluster: dr " << sqrt(dr2Min) << " index " << closestIndex );
    muon.setClusterLink(association->data()[closestIndex]);
  }

  bool MuonCreatorTool::addCaloExtensionPositions(  xAOD::Muon& muon ) const {	
    const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
    if(!tp){
      ATH_MSG_WARNING("Can not get primary track.");
      return false;
    }

    const Trk::CaloExtension* caloExtension = 0;
    if(!m_caloExtTool->caloExtension(*tp,caloExtension)){
      ATH_MSG_WARNING("Can not get caloExtension.");
      return false;
    };

    const std::vector<const Trk::CurvilinearParameters*>& intersections = caloExtension->caloLayerIntersections();
    const unsigned int nIntersections = intersections.size();
    std::vector< int > caloExtIdentifier(nIntersections, -999);
    std::vector< float > caloExtEta(nIntersections, -999);
    std::vector< float > caloExtPhi(nIntersections, -999);
    if (nIntersections>0) {
      Amg::Vector3D avePoint(0,0,0);
      for (unsigned int i = 0; i < intersections.size(); ++i){
        const Amg::Vector3D& point = intersections[i]->position();
        caloExtIdentifier[i] = (int)parsIdHelper.caloSample(intersections[i]->cIdentifier());
        caloExtEta[i] = point.eta();
        caloExtPhi[i] = point.phi();
        ATH_MSG_DEBUG("Intersection: " << i << " ID: " << parsIdHelper.caloSample(intersections[i]->cIdentifier())
		      <<  " eta-phi (" << point.eta() << ", " << point.phi() << ")");
      }

      muon.auxdata< std::vector< int > >("caloExt_id") = caloExtIdentifier;
      muon.auxdata< std::vector< float > >("caloExt_eta") = caloExtEta;
      muon.auxdata< std::vector< float > >("caloExt_phi") = caloExtPhi;
    }else{
      ATH_MSG_WARNING("caloExtension intersections.size() = 0. Nothing done.");
    }

    return true;
  }

  void MuonCreatorTool::collectCells( xAOD::Muon& muon, xAOD::CaloClusterContainer& clusterContainer ) const {

    const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
    if(!tp){
      ATH_MSG_WARNING("Can not get primary track.");
      return;
    }

    // get ParticleCellAssociation
    ATH_MSG_DEBUG(" Selected track: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() );

    const Trk::CaloExtension* caloExtension = 0;
    if(!m_caloExtTool->caloExtension(*tp,caloExtension)){
      ATH_MSG_WARNING("Can not get caloExtension.");
      return;
    };

    const CaloCellContainer* container = 0;
    //retrieve the cell container
    if( evtStore()->retrieve(container, m_cellContainerName).isFailure() || !container ) {
      ATH_MSG_WARNING( "Unable to retrieve the cell container  " << m_cellContainerName << " container ptr " << container );
      return;
    }

    xAOD::CaloCluster* cluster = m_cellCollector.collectCells( *caloExtension, *container, clusterContainer );
    if( !cluster ){
      ATH_MSG_WARNING("Failed to create cluster from ParticleCellAssociation");
      return;
    }else{
      ATH_MSG_DEBUG(" New cluster: eta " << cluster->eta() << " phi " << cluster->phi() << " cells " << cluster->size() );
    }
	
    // create element links
    ElementLink< xAOD::CaloClusterContainer >   clusterLink(clusterContainer,clusterContainer.size()-1);
    muon.auxdata< ElementLink< xAOD::CaloClusterContainer > >("crossedCellClusterLink") = clusterLink;
    
  }


}	// end of namespace

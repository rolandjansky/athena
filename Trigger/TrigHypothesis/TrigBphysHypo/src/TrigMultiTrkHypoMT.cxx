// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigMultiTrkHypoMT.cxx
 **
 **   Description: Multi-track hypothesis algorithm
 **
 **   Author: H. Russell
 **
 **************************************************************************/

#include "TrigMultiTrkHypoMT.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <math.h>

#include "xAODTrigger/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

#include "AthLinks/ElementLink.h"
#include "AthViews/ViewHelper.h"
#include "CxxUtils/phihelper.h"
#include "TrigConfHLTData/HLTUtils.h"


class ISvcLocator;
using namespace TrigCompositeUtils;
TrigMultiTrkHypoMT::TrigMultiTrkHypoMT(const std::string & name, ISvcLocator* pSvcLocator):
    ::HypoBase(name, pSvcLocator)
{}

TrigMultiTrkHypoMT::~TrigMultiTrkHypoMT()
{ }

StatusCode TrigMultiTrkHypoMT::initialize()
{
    ATH_MSG_DEBUG( "Initializing " << name() << "...");
    if(static_cast<int>(m_trkMasses.size()) != m_nTrk){
        ATH_MSG_ERROR("Requested " << m_nTrk << " tracks per vertex, but only provided "
             << m_trkMasses.size() << " track masses!");
        return StatusCode::FAILURE;
    } else if(msgLvl(MSG::DEBUG)){
    	msg() << MSG::DEBUG <<  "requiring " << m_nTrk << " tracks with mass: ";
    	for(float mass :  m_trkMasses) msg() << MSG::INFO << mass <<", ";
        msg() << MSG::DEBUG << endmsg;  
    }
    if(static_cast<int>(m_ptTrkMin.size()) != m_nTrk){
        ATH_MSG_ERROR("Requested " << m_nTrk << " tracks per vertex, but only provided "
            << m_ptTrkMin.size() << " track pTs!");        
        return StatusCode::FAILURE;
    } else if(msgLvl(MSG::DEBUG)){
    	msg() << MSG::DEBUG <<  "requiring " << m_nTrk << " tracks with pT: ";
    	for(float pt :  m_ptTrkMin) msg() << MSG::INFO << pt <<", ";
        msg() << MSG::DEBUG << endmsg;  
    }
    if(m_nTrkQ  < 0 ) ATH_MSG_DEBUG("no charge requirement applied");
    else ATH_MSG_DEBUG("pair must have charge : " << m_nTrkQ);
    ATH_MSG_DEBUG("dR for track overlap removal: " << m_mindR);
    
  /*       Not printing or checking consistency of these properties yet! need to add!
  Gaudi::Property<std::vector<float>> m_nTrkMassMin { this, "nTrkMassMin", {0},""}; // both min+max need to be defined
  Gaudi::Property<std::vector<float>> m_nTrkMassMax { this, "nTrkMassMax", {-1},""};
  */
    if(m_particleType == 0){
        ATH_CHECK( m_trackParticleContainerKey.initialize() );
        renounce(m_trackParticleContainerKey);
        ATH_CHECK( m_muonContainerKey.initialize(false) );//Not using muons
    } else if (m_particleType == 1){
        ATH_CHECK( m_muonContainerKey.initialize() );
        renounce(m_muonContainerKey);
        ATH_CHECK( m_trackParticleContainerKey.initialize(false) ); //Not using tracks
    } else{
        ATH_MSG_ERROR("Particle type > 1 requested, we are not configured for that yet!");
    }
    ATH_CHECK( m_bphysHelperTool.retrieve() );

 	if ( not m_monTool.name().empty() ) {
    	ATH_CHECK( m_monTool.retrieve() );
  	}

    ATH_CHECK(m_hypoTools.retrieve());

    ATH_CHECK(m_bphysObjContKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode TrigMultiTrkHypoMT::finalize()
{
  TrigConf::HLTUtils::hashes2file();
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------------
StatusCode TrigMultiTrkHypoMT::execute( const EventContext& context) const
{

    auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
    ATH_MSG_DEBUG("decision input key: " << decisionInput().key());
    if( not previousDecisionsHandle.isValid() ) {//implicit
        ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

    // new output decisions	
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
    DecisionContainer* decisions = outputHandle.ptr();

    std::vector<TrigMultiTrkHypoToolMT::TrigMultiTrkInfo> hypoToolInput;
    
    
    ATH_MSG_VERBOSE( "Executing " <<name());

    //Monitored variables 
    std::vector<float> ini_nTrkMass(0);
    std::vector<float> ini_nTrkFitMass(0);
    std::vector<float> ini_nTrkChi2(0);
    std::vector<float> ini_pairMass(0);
  
    // Monitoring histogram
    auto mon_NTrkMass  		   = Monitored::Collection("nTrkMass", ini_nTrkMass);
    auto mon_NTrkFitMass	   = Monitored::Collection("nTrkFitMass", ini_nTrkFitMass);
    auto mon_NTrkChi2          = Monitored::Collection("nTrkChi2", ini_nTrkChi2);

    auto mon_NTrk          	     = Monitored::Scalar<int>("nTrk", 0);
    auto mon_accepted_highptNTrk = Monitored::Scalar<int>("accepted_highptNTrk", 0);
    auto mon_NPair               = Monitored::Scalar<int>("nPair", 0);
    auto mon_acceptedNPair       = Monitored::Scalar<int>("accepted_nPair", 0);

    auto monitorIt = Monitored::Group( m_monTool, mon_NTrk, 
  			mon_accepted_highptNTrk, mon_NTrkMass, mon_NTrkFitMass, mon_NTrkChi2,
  			mon_NPair, mon_acceptedNPair );

    auto bphysColl = SG::makeHandle( m_bphysObjContKey, context );

    ATH_CHECK( bphysColl.record (std::make_unique<xAOD::TrigBphysContainer>(),
                                 std::make_unique<xAOD::TrigBphysAuxContainer>()) );

    //need to combine all the muon event views - since there should be one view / RoI    

	//map which track comes from which decision object - works for good tracks too, because
	//those are done by pointer in the same way!
	std::map< ElementLink<xAOD::TrackParticleContainer>, const Decision*> track_decision_map;
	
    std::vector< ElementLink<xAOD::TrackParticleContainer> > good_tracks;
    std::vector< ElementLink<xAOD::TrackParticleContainer> > all_tracks;

    for (const Decision* previousDecision: *previousDecisionsHandle) {
		  const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
  		ATH_CHECK( viewEL.isValid() );
  		
  		//could implement something templated for tracks/muon tracks/electron tracks
  		//auto tracks = get_correct_tracks();//templated in some .icc file, hide away all
  		//the different types there. But make sure what we return is something  common and
  		//all that is necessary.
  		
  		//tracks are SG::ReadHandle<xAOD::TrackParticleContainer>	
  		if(m_particleType == 0){
  		auto tracks = ViewHelper::makeHandle( *viewEL, m_trackParticleContainerKey, context );
  		ATH_CHECK( tracks.isValid() );
    	 //so each time the loop starts, tracks is reset to contain tracks from a different view
    	 ATH_MSG_DEBUG( "Made handle " << m_trackParticleContainerKey << " size: "
                  << tracks->size() );  
 
         if(tracks->size() == 0) continue;

         for(auto track : *tracks){
        
            const ElementLink<xAOD::TrackParticleContainer> track_link = 
                     ElementLink<xAOD::TrackParticleContainer>(*tracks, track->index(), context);
            ATH_CHECK(track_link.isValid());

        	all_tracks.push_back(track_link);
        	track_decision_map[track_link] = previousDecision;
         }
         } else if(m_particleType == 1){
             auto muons = ViewHelper::makeHandle( *viewEL, m_muonContainerKey, context );
  		     ATH_CHECK( muons.isValid() );
    	     //so each time the loop starts, tracks is reset to contain tracks from a different view
    	     ATH_MSG_DEBUG( "Made handle " << m_muonContainerKey << " size: "
                  << muons->size() );  
 
             if(muons->size() == 0) continue;
                   
             for(auto muon : *muons){
                //check if this is a combined muon
                const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
                if (!tr) {
                    ATH_MSG_DEBUG("No CombinedTrackParticle found.");
                    continue;
                } else {
                    const ElementLink<xAOD::TrackParticleContainer> track_link = muon->inDetTrackParticleLink();
                    ATH_CHECK(track_link.isValid());

            	    all_tracks.push_back(track_link);
                    track_decision_map[track_link] = previousDecision;
                }
             } 
         }           
    }
    mon_NTrk = all_tracks.size();
    bool passNTrack_and_pTcuts = passNTracks(m_nTrk, m_ptTrkMin, all_tracks, good_tracks, m_mindR);
    if (!passNTrack_and_pTcuts){
      ATH_MSG_DEBUG("failed nTrack cut");
      return StatusCode::SUCCESS;
    }  
    mon_accepted_highptNTrk = good_tracks.size();

    ATH_MSG_DEBUG("Passed NTrack and track pT cuts: " << mon_accepted_highptNTrk << " tracks sent to vertexing");
    
    const auto nTracks = good_tracks.size();
    std::unique_ptr<Trk::IVKalState> state = m_bphysHelperTool->makeVKalState();
    for (unsigned int it0 = 0; it0 != nTracks; ++it0) {
    
      //dereference element link to get the track
      auto tp0 = *good_tracks.at(it0);
      
      for (unsigned int it1 = it0+1; it1 != nTracks; ++it1) {
          mon_NPair++;
          auto tp1 = *good_tracks.at(it1);

          double totalMass = m_bphysHelperTool->invariantMass( tp0, tp1, m_trkMasses[0], m_trkMasses[1]);    
          ini_nTrkMass.push_back(totalMass*0.001); 
             
          ATH_MSG_DEBUG("track 1: " << tp0->pt()<< " / " << tp0->eta()<< " /  "
                        << tp0->phi()<< " /  "  << tp0->charge()); 
          ATH_MSG_DEBUG("track 2: " << tp1->pt()<< " / " << tp1->eta()<< " /  "
                        << tp1->phi()<< " /  " << tp1->charge()); 

          ATH_MSG_DEBUG("Track pair mass: " <<  totalMass);
          
          //allow for multiple acceptable mass windows!
          bool passMassCut = false;
          for(unsigned int im =0; im < m_nTrkMassMin.size(); im++ ){
              if(totalMass >= m_nTrkMassMin[im] && totalMass <= m_nTrkMassMax[im]){
                ATH_MSG_DEBUG("Passes nTrack mass cut!");
                passMassCut = true;
              }
          }
          if(!passMassCut) continue; //don't bother with things way outside the mass window!

          mon_acceptedNPair++;
          ATH_MSG_DEBUG("before trigBphys initialized");

          xAOD::TrigBphys* trigBphys = new xAOD::TrigBphys();
          
          trigBphys->makePrivateStore(); //need this so the aux variables are accessible with initialize
          //could just add to container, but don't want to bother storing vertices with gigantic chi2
          
          trigBphys->initialise(0, 0., 0. ,0., xAOD::TrigBphys::MULTIMU, totalMass, xAOD::TrigBphys::L2);  

          std::vector<ElementLink<xAOD::TrackParticleContainer> > thisIterationTracks = {good_tracks.at(it0), good_tracks.at(it1)};
          if (m_bphysHelperTool->vertexFit(trigBphys,thisIterationTracks,m_trkMasses,*state).isFailure()) {
              ATH_MSG_DEBUG("Problems with vertex fit in TrigMultiTrkHypoMT");
              delete trigBphys; trigBphys = nullptr;
              continue;
          }
          ATH_MSG_DEBUG("Vertex chi2 = " << trigBphys->fitchi2() );

          if(trigBphys->fitchi2() > 150.){
              ATH_MSG_DEBUG("Vertex chi2 is too high, not saving it! (chi2 = " << trigBphys->fitchi2() << ")" );
              delete trigBphys; trigBphys = nullptr;
              continue;
          }
          ATH_MSG_DEBUG("Vertex fitmass = " << trigBphys->fitmass() );
          
          ini_nTrkFitMass.push_back(trigBphys->fitmass()*0.001);
          ini_nTrkChi2.push_back(trigBphys->fitchi2());

          bphysColl->push_back(trigBphys);

          //need to add some duplicate BPhys object removal here? isUnique(trigBphys)?
          //what to do about combinatorics???      
        }
    }
    good_tracks.clear();
    
    for (const xAOD::TrigBphys* bphysObj : * bphysColl){

        ATH_MSG_DEBUG("Found xAOD::TrigBphys: " << bphysObj->mass());
        // Create a new output Decision object, d, backed by the 'decisions' container.
        // Links previousDecision as the parent of d.
        Decision* newDecision = newDecisionIn( decisions );

        // Obtain an ElementLink to objA and set mandatory feature link
        const ElementLink<xAOD::TrigBphysContainer> bphyEL 
              = ElementLink<xAOD::TrigBphysContainer>( *bphysColl, bphysObj->index() );
        newDecision->setObjectLink<xAOD::TrigBphysContainer>( "feature", bphyEL );
        DecisionIDContainer previousDecisionIDs0;
        DecisionIDContainer previousDecisionIDs1;
		//remove the auto!
        const auto tplinks = bphysObj->trackParticleLinks();
        int itrk = -1;
		for ( const auto tplink : tplinks){
			itrk++;
			const Decision* previousDecision = track_decision_map[tplink];
			
			// Extract the set of previousDecisionIDs from the previousDecision.
        	// If you needed to re-map the previous decision(s), then call decisionIDs once
        	// for each previous decision
        	// if the same previous decision is called twice, that's fine - internally takes care of that
        	linkToPrevious( newDecision, previousDecision, context );
        	if(itrk == 0 ){
        		decisionIDs(previousDecision, previousDecisionIDs0);
			}else{
        		decisionIDs(previousDecision, previousDecisionIDs1);

			}
        }
	    // Collect all the required information for the tool together in a handy struct 
       hypoToolInput.push_back( TrigMultiTrkHypoToolMT::TrigMultiTrkInfo{newDecision, bphysObj, 
       		previousDecisionIDs0, previousDecisionIDs1} );
       		
    }

    ATH_MSG_DEBUG("Found "<< hypoToolInput.size()<<" inputs to tools");
	
    StatusCode sc = StatusCode::SUCCESS;
    for ( auto& tool: m_hypoTools ) {
      ATH_MSG_DEBUG("Go to " << tool );
      //could decide on individual triggers 
      ATH_CHECK( tool->decide(hypoToolInput) );
    }
    
    ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
    ATH_MSG_DEBUG("StatusCode TrigMultiTrkHypoMT::execute_r success");
    return StatusCode::SUCCESS;
}

bool TrigMultiTrkHypoMT::passNTracks(int nObjMin,
                 const std::vector<float> & ptObjMin,
  				 const std::vector<ElementLink<xAOD::TrackParticleContainer> > & inputTrkVec,
                 std::vector<ElementLink<xAOD::TrackParticleContainer> > & outputTrkVec,
                 float mindR) const
{

  if( nObjMin <= 0 ) return true; //why on earth would this ever happen...
  //don't bother running if we don't even have enough input tracks
  if(static_cast<int>(inputTrkVec.size()) < nObjMin) return false;
  
  outputTrkVec.clear();
 
  float mindR2 = mindR*mindR;
  
  float minpT = *min_element(std::begin(ptObjMin), std::end(ptObjMin)); //*min because min_element returns an iterator, not the value
  for( const auto & inTrk : inputTrkVec){ 
  
     // don't keep if it doesn't pass the min pT cut
     if( (*inTrk)->pt() < minpT ) continue;
     
     // check for overlap
     bool already_found = false;
  
     for(const auto& outTrk : outputTrkVec ){
       //obviously not the same track if OS
       if( (*outTrk)->charge() *  (*inTrk)->charge() < 0 ) continue; 
       
       double deta = (*inTrk)->eta() - (*outTrk)->eta();
       double dphi = CxxUtils::wrapToPi((*inTrk)->phi() - (*outTrk)->phi());
       double deltaR2 = deta*deta + dphi*dphi;
       if( deltaR2 <= mindR2) already_found = true;
     } 
     if( !already_found ) {
       outputTrkVec.push_back(inTrk);            
       ATH_MSG_DEBUG("Found track pt/eta/phi/q " << (*inTrk)->pt()<< " / "
                        << (*inTrk)->eta()<< " /  "
                        << (*inTrk)->phi()<< " /  "
                        << (*inTrk)->charge()); 
     }
   }// end loop over input muons
  
   //=== check if it is enough muons
  if( (int)outputTrkVec.size() < nObjMin ) {
    ATH_MSG_DEBUG("Rejecting: " <<" #Ntracks= " <<  outputTrkVec.size() 
                        << " while need "<< nObjMin);
   return false;
 }
 
 std::sort( std::begin(outputTrkVec), std::end(outputTrkVec), sortTracks); 

 //find number of tracks to check the pT of
 unsigned int Ncheck = std::min( nObjMin, int(ptObjMin.size()) );
 bool failMuonPt = false;
 for ( unsigned int ipt=0; ipt < Ncheck; ++ipt) {
   float pt = (*outputTrkVec[ipt])->pt();
   // check if value in GeV or MeV, if it was >350 GeV and multiplied by 1000, it does not matter
   if( pt < 350. && pt>0.01 ) pt *= 1000.;
   if(  pt < ptObjMin[ipt] ) failMuonPt = true;      
 }
 if( failMuonPt ){
   ATH_MSG_DEBUG ("Fail track pt cut" );
   return false;
 }
 // here would be good to limit number of objects to the minimum
 
 return true;
}

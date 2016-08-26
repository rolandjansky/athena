/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonPVBiases.cxx
// AUTHORS: Ambrosius  Vermeulen, Pierfrancesco Butti
// **********************************************************************

#include <sstream>
#include <math.h>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"


//#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "xAODEventInfo/EventInfo.h"
//#include "EventInfo/EventInfo.h"

//#include "AthenaMonitoring/AthenaMonManager.h"
#include "IDAlignMonPVBiases.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
#include "TrackSelectionTool.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"


// *********************************************************************
// Public Methods
// *********************************************************************

IDAlignMonPVBiases::IDAlignMonPVBiases( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
  m_events(0),
  m_tracksName("ExtendedTracks"),
  m_triggerChainName("NoTriggerSelection"),
  m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator"), 
  m_vertices(0),
  m_TreeFolder("/PVbiases/PVbiases"),
  m_Tree(0),
  m_TreeName("PVbiases")
{
  m_trackSelection = ToolHandle< InDetAlignMon::TrackSelectionTool >("InDetAlignMon::TrackSelectionTool");

  declareProperty("tracksName"           , m_tracksName);
  declareProperty("CheckRate"            , m_checkrate=1000);
  declareProperty("triggerChainName"     , m_triggerChainName);
  declareProperty("trackSelection"       , m_trackSelection);
  declareProperty("VxPrimContainerName"  , m_VxPrimContainerName);
  
}


IDAlignMonPVBiases::~IDAlignMonPVBiases() { }


StatusCode IDAlignMonPVBiases::initialize()
{
  StatusCode sc;
  m_events=0;  
   
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess())
    return StatusCode::SUCCESS;

  if ( m_trackSelection.retrieve().isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve tool " << m_trackSelection << endreq;
    return StatusCode::SUCCESS;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "BROOS Retrieved tool " << m_trackSelection << endreq;
  }

  //create tree and branches
  if(m_Tree == 0) {

    m_Tree = new TTree(m_TreeName.c_str(), "Tree");

    m_Tree->Branch("run_number"      ,  &m_runNumber,  "runNumber/I");
    m_Tree->Branch("event_number"    ,  &m_evtNumber,  "eventNumber/I");
    m_Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_Tree->Branch("charge",  &m_charge,  "Charge/D");
    m_Tree->Branch("pt",  &m_pt,  "pt/D");
    m_Tree->Branch("eta", &m_eta, "eta/D");
    m_Tree->Branch("phi", &m_phi, "phi/D");
    m_Tree->Branch("z0",  &m_z0,  "z0/D");
    m_Tree->Branch("d0",  &m_d0,  "d0/D");
    m_Tree->Branch("z0_err",  &m_z0_err,  "z0_err/D");
    m_Tree->Branch("d0_err",  &m_d0_err,  "d0_err/D");
    m_Tree->Branch("vertex_x",  &m_vertex_x,  "vertex_x/D");
    m_Tree->Branch("vertex_y",  &m_vertex_y,  "vertex_y/D");
    m_Tree->Branch("vertex_z",  &m_vertex_z,  "vertex_z/D");
  }

  //register the tree
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
    //m_validationMode = false;
  }

  if ((tHistSvc->regTree(m_TreeFolder, m_Tree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_Tree; m_Tree = 0;
    //m_validationMode = false;
  }

  return StatusCode::SUCCESS;
}

StatusCode IDAlignMonPVBiases::bookHistograms()
{

return StatusCode::SUCCESS;
}

StatusCode IDAlignMonPVBiases::procHistograms()
{

  return StatusCode::SUCCESS;
}

StatusCode IDAlignMonPVBiases::fillHistograms()
{
  m_events++;

 /******************************************************************
 ** Event Information
 *******************************************************************/
	   bool isMC = false;
  //get eventinfo
  ATH_MSG_DEBUG("Retrieving event info.");
  const xAOD::EventInfo * eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure())
    ATH_MSG_ERROR("Could not retrieve event info.");
  else
  {
    m_runNumber = eventInfo->runNumber();
    m_evtNumber = eventInfo->eventNumber();
    m_lumi_block = eventInfo->lumiBlock();
    ATH_MSG_DEBUG(" Execute() starting on --> Run: " << m_runNumber << "  event: " << m_evtNumber);
  }


	   /// Beam spot error
	   bsXerror = eventInfo->beamPosSigmaX();
	   bsYerror = eventInfo->beamPosSigmaY();
	   bsXYerror = eventInfo->beamPosSigmaXY();
	   bsZerror = eventInfo->beamPosSigmaZ();
	   //Info("execute()", "bsXerror: %f, bsYerror: %f, bserror: %f, correlation:%f", bsXerror, bsYerror, bserror, eventInfo->beamPosSigmaXY() );
	   
           //COMMENTED OUT BY BROOS
           //h2beamSpotXY->Fill( eventInfo->beamPosX(), eventInfo->beamPosY());
	   //h2beamSpotErrorXY->Fill( bsXerror, bsYerror);
	   //h1beamSpotZ->Fill( eventInfo->beamPosZ() );
	
	   /// check if the event is data or MC	  
	   //if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
	     // isMC = true;
	  // }
	
	   /// Scale Factor
/*	   if(isMC){
	      /// K_PV from Sarah Boutle, 18/5/2015
	      kPV_d0 = 0.95; // +- 0.03
	      kPV_z0 = 0.98; // +- 0.02
	      kPV_z0st = 0.98; // +- 0.02  TEMPORARY!!!
	   }else{
	      kPV_d0 = 1.06; // +- 0.03
	      kPV_z0 = 1.01; // +- 0.02
	      kPV_z0st = 1.01; // +- 0.02  TEMPORARY!!!
	   }
*/	
	   /// Event weight
	   /// Overall weight is sigma*efficiency/Nevent, where sigma and filter
	   /// efficiency are gotten from AMI
//	   wt = 1.;
	   //if(isMC){
	     // const std::vector< float > weights = eventInfo->mcEventWeights();
	     // if( weights.size() > 0 ) wt = weights[0];
	   //}
	
	   //COMMENTED OUT BY BROOS
	   /*
	   /// if data check if event passes GRL & remove detector imperfection
	   if(!isMC){ // it's data!
	      //if(m_debug) std::cout << " GRL " << std::endl;
	      if(!m_grl->passRunLB(*eventInfo)){
	         return EL::StatusCode::SUCCESS; // go to next event
	      }
	      //if(m_debug) std::cout << " Detector imperfection " << std::endl;
	      if(   (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error )
	         || (eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error )
	         || (eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18) )  )
	      {
	         return EL::StatusCode::SUCCESS; // go to the next event
	      } // end if event flags check
	   } // end if not MC
	   
	
	   /// examine the HLT_xe80* chains, see if they passed/failed and their total prescale
	   if(m_doTrigger){
	      if(m_debug) std::cout << " Trigger " << std::endl;
	      bool triggerIsPassed = false;
	      auto chainGroup = m_trigDecisionTool->getChainGroup( m_triggermenu.c_str() );
	      std::map<std::string,int> triggerCounts;
	      for(auto &trig : chainGroup->getListOfTriggers()) {
	         auto cg = m_trigDecisionTool->getChainGroup(trig);
	         std::string thisTrig = trig;
	         triggerIsPassed = cg->isPassed();
	         //Info( "execute()", "%30s chain passed(1)/failed(0): %d total chain prescale (L1*HLT): %.1f", thisTrig.c_str(), cg->isPassed(), cg->getPrescale() );
	      } // end for loop (c++11 style) over chain group matching "HLT_xe80*"
	      if(!triggerIsPassed){
	         return EL::StatusCode::SUCCESS; // go to the next event
	      }
	      if(m_debug) std::cout << " Finish Trigger " << std::endl;
	   }
	  

	   //// Fill the pile up
	   h1ActPileUp->Fill(eventInfo->actualInteractionsPerCrossing(), wt);
	   h1AvePileUp->Fill(eventInfo->averageInteractionsPerCrossing(), wt);
	   h1DiffPileUp->Fill(eventInfo->actualInteractionsPerCrossing() -
	                      eventInfo->averageInteractionsPerCrossing(), wt);
           */           
	

	   /******************************************************************
	    * Vertex Loop
	    ******************************************************************/
	   /*
	   if(m_debug) std::cout << "Vertex loop" << std::endl;
	   const xAOD::VertexContainer* vertices = 0;	 
           if( ! m_event->retrieve( vertices, "PrimaryVertices").isSuccess() ){
	      Error("execute()", "Failed to retrieve event info collection. Exiting." );
	      return EL::StatusCode::FAILURE;
	   }
	   xAOD::VertexContainer::const_iterator vertex_itr = vertices->begin();
	   xAOD::VertexContainer::const_iterator vertex_itrE = vertices->end();
	   unsigned int *nvertices = new unsigned int(0);
	   bool *hasMT10traks = new bool(false);
	   for (; vertex_itr!=vertex_itrE; ++vertex_itr) {
	           if((*vertex_itr)->vertexType() != 1) continue; // require primary vertex
	      int nTracksPerVtx = (*vertex_itr)->nTrackParticles();
	      if(nTracksPerVtx >= 10) (*hasMT10traks)=true;
	      if(nTracksPerVtx > 4) (*nvertices)++;
	      h1PrimVtxX->Fill( (*vertex_itr)->x() );
	   }
	   /// additional cut for primary vertex to reduce the effect of multile interactions
	   //if( hasMT10traks && (*nvertices) > 1 ){
	   //   return EL::StatusCode::SUCCESS;
	   //}
	   m_eventCounterAfterVtxCut++;
	
	   delete nvertices;
	   delete hasMT10traks;
	   */


	   /******************************************************************
	    * Track Loop
	    ******************************************************************/
	   //const xAOD::TrackParticleContainer* tracks = 0;
	   //if( ! evtStore()->retrieve( tracks, "InDetTrackParticles").isSuccess() ){
	     //msg(MSG::WARNING) << "Failed to retrieve event info collection. Exiting." << endreq;
	      //return StatusCode::FAILURE;
	   //}
	   const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >("InDetTrackParticles");

	   xAOD::TrackParticleContainer::const_iterator track_itr = tracks->begin();
	   xAOD::TrackParticleContainer::const_iterator track_itrE = tracks->end();
	
	   /// Track Loop
	   for (; track_itr!=track_itrE; ++track_itr) {
           
/*           uint8_t nhitsOfPixel = 0;
           uint8_t ndeadOfPixel = 0;
           uint8_t nhitsOfSCT = 0;
           uint8_t ndeadOfSCT = 0;
           uint8_t nSharedHitsOfPixel = 0;
           uint8_t nSharedHitsOfSCT = 0;
           uint8_t nholesOfPixel = 0;
           uint8_t nholesOfSCT = 0;
 
           if( !(*track_itr)->summaryValue(nhitsOfPixel, xAOD::numberOfPixelHits)) continue;
           if( !(*track_itr)->summaryValue(ndeadOfPixel, xAOD::numberOfPixelDeadSensors)) continue;
           if( !(*track_itr)->summaryValue(nhitsOfSCT, xAOD::numberOfSCTHits)) continue;
           if( !(*track_itr)->summaryValue(ndeadOfSCT, xAOD::numberOfSCTDeadSensors)) continue;
           if( !(*track_itr)->summaryValue(nSharedHitsOfPixel, xAOD::numberOfPixelSharedHits)) continue;
           if( !(*track_itr)->summaryValue(nSharedHitsOfSCT, xAOD::numberOfSCTSharedHits)) continue;
           if( !(*track_itr)->summaryValue(nholesOfPixel, xAOD::numberOfPixelHoles)) continue;
           if( !(*track_itr)->summaryValue(nholesOfSCT, xAOD::numberOfSCTHoles)) continue;
         
                  // To avoid crash for 20.7
	      if( (*track_itr)->pt() < 300. || isinf((*track_itr)->pt()) || isnan((*track_itr)->pt()) || (*track_itr)->pt() > 1000000. ) continue;
           

   if(   !(fabs( (*track_itr)->eta()  ) < 2.5) &&
         !((nhitsOfPixel + ndeadOfPixel + nhitsOfSCT + ndeadOfSCT) >= 7) &&
         !((nSharedHitsOfPixel + (0.5*nSharedHitsOfSCT)) <= 1) && 
         !((nholesOfPixel + nholesOfSCT) <= 2) &&
         !(nholesOfPixel <= 1)) { 
              continue;
      }	  
	      //bool trkSelection[nCuts] = {false};
	        int dummy = nholesOfPixel;
                msg(MSG::INFO) << "Number of pixel hits:" << dummy << endreq;
*/

		      //ElementLink< xAOD::VertexContainer> vtxLink = trktovxtool->getUniqueMatchVertexLink( (**track_itr), (*vertices) );
	      //if(!vtxLink.isValid() ) continue;
	      //priVtx = (*vtxLink);
	
           //if(priVtx->vertexType() != 1) continue;


		
 	
	      /// retrieve truth particle and vertex
	      /*const xAOD::TruthParticle * truthTrk = 0;
	      if(isMC){
	         typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
	         static const char* NAME = "truthParticleLink";
	         if(!  (*track_itr)->isAvailable< Link_t > ( NAME ) ){
	           // Info("execute()", "truthParticleLink is not available" );
	         }
	         const Link_t& link = (*track_itr)->auxdata< Link_t > (NAME);
	         if(!link.isValid() ) continue;
	         truthTrk = (*link);
	         /// Cut of truthMatchProbability
	         h1TruthMatchProb->Fill( (*track_itr)->auxdata<float>("truthMatchProbability") );
	         if( (*track_itr)->auxdata<float>("truthMatchProbability") < 0.8 ) continue;
	      }*/
	
	           /// retrieve track information
	           ///if( !(*track_itr)->summaryValue(nhitsOfPixel, xAOD::numberOfBLayerHits)) continue;
	           //if( !(*track_itr)->summaryValue(nhitsOfPixel, xAOD::numberOfInnermostPixelLayerHits)) continue;
	           //if( !(*track_itr)->summaryValue(nhitsOfPixel, xAOD::numberOfPixelHits)) continue;
	           //if( !(*track_itr)->summaryValue(nhitsOfSCT, xAOD::numberOfSCTHits)) continue;
	
	
	           /// Definition of parameters
	      //if(m_debug) std::cout << "[TrackParticle loop] retrieve the parameters" << std::endl;
	           //retrieveTrackPars( (*track_itr) /*, priVtx */ );
	           //if(isMC){
	         //retrieveTruthPars( truthTrk /*, priVtx*/ );
	     // }
	  //    m_d0 = (*track_itr)->d0();
	    //  m_z0 = (*track_itr)->z0();
	      //m_z0SinTheta = z0 * TMath::Sin( theta );
	      //bserror = xAODAnalysis::d0UncertaintyBeamSpot2(phi, bsXerror, bsYerror, bsXYerror);
	      //bserror = TMath::Hypot(bsXerror*TMath::Sin(phi), bsYerror*TMath::Cos(phi));
	
	      //std::cout << (*track_itr)->pt() << std::endl;
	
	
	      ////// additional cut
	      ///if( ntracks < 10 ) continue;
	      ///if( (*track_itr)->auxdata<float>("truthMatchProbability") < 0.8 ) continue;
	      ///if( truthDr > 0.1) continue;
	      ///if( fabs(truthPt - pt) > 5. ) continue;
	      ///if( fabs(truthD0) > 1.0 ) continue;
	    /*  if(!doTruthOnly){
	         if( fabs(d0Unbias) > 100.0 ||
	             fabs(z0Unbias) > 100.0 ||
	             fabs(z0stUnbias) > 100.0 ) continue;
	      }*/
	
	           ////// Track Selection
	      ///if(m_debug) std::cout << "[TrackParticle loop] Track Selection" << std::endl;
	           //////TrackQualityCut( trkSelection );
	
	           ////// Track Selection Loop
	           //for(int isel=0; isel<nCuts; isel++){
	
	              //if(trkSelection[isel] == false) continue;
	         //if( isel > 0 && !selTool[isel-1]->accept((*track_itr)) ) continue;
	         //m_trackCounter[isel]++;
	
	         /// Track Weight for MC sample
	         //float trkwt = 1.;
	         /*if(isMC && m_doReweight){
	            trkwt = readZVTXweight( isel );
	            trkwt = trkwt * readPTETAweight( isel );
	            //Info("execute()", "zwt = %f, ptetawt = %f, total = %f", readZVTXweight( isel ), readPTETAweight( isel ), trkwt);
	         }else{
	            trkwt = 1.;
	         }*/
	
	        /* if(!doTruthOnly){
	            unfoldD0Unbias = d0Unbias * UnfoldFactor(kPV_d0, 1.0, pvd0sigmaUnbias, d0sigmaUnbias);
	            unfoldZ0Unbias = z0Unbias * UnfoldFactor(kPV_z0, 1.0, pvz0sigmaUnbias, z0sigmaUnbias);
	            unfoldZ0stUnbias = z0stUnbias * UnfoldFactor(kPV_z0st, 1.0, pvz0stsigmaUnbias, z0stsigmaUnbias);
	            unfoldD0UnbiasError = TMath::Hypot(d0sigmaUnbias, pvd0sigmaUnbias) * UnfoldFactor(kPV_d0, 1.0, pvd0sigmaUnbias, d0sigmaUnbias);
	            unfoldZ0UnbiasError = TMath::Hypot(z0sigmaUnbias, pvz0sigmaUnbias) * UnfoldFactor(kPV_z0, 1.0, pvz0sigmaUnbias, z0sigmaUnbias);
	         }
	
	         /// Fill histograms
	         if(m_debug) std::cout << "[TrackParticle loop] Filling parameters" << std::endl;
	              fillTrackPars( isel, trkwt );
	              if(isMC){
	            fillTruthPars( isel, trkwt );
	         }
	
	         /// Categorization by pT and theta
	         if(m_debug) std::cout << "[TrackParticle loop] Filling parameters by categorization" << std::endl;
	         fillCategorizedMatrices( isel, trkwt );
	          */
    

	// lumi block check REMOVE THIS
	//if(m_lumi_block != 345) continue;
    
       	// require having vertex
	if(!(*track_itr)->vertex()) continue;
        // require associated with primary vertex
   	if((*track_itr)->vertex()->vertexType() != 1) continue;
	// require at least 10 tracks associated
        if((*track_itr)->vertex()->nTrackParticles() < 10) continue;

        const Trk::ImpactParametersAndSigma* myIPandSigma(NULL);
        myIPandSigma = m_trackToVertexIPEstimator->estimate(*track_itr, (*track_itr)->vertex(),true);

        // require d0_pv to be smaller than 4
        if(myIPandSigma->IPd0 > 4.0) continue;
        if(myIPandSigma->IPd0 < -4.0) continue;

        // store in ntuple 
	m_d0=myIPandSigma->IPd0;
        m_z0=myIPandSigma->IPz0;
        m_z0_err = myIPandSigma->sigmaz0;
    	m_d0_err= myIPandSigma->sigmad0;

        m_charge = (*track_itr)->charge();
 	m_pt = (*track_itr)->pt();
        m_eta = (*track_itr)->eta();
        m_phi = (*track_itr)->phi();
        m_vertex_x = (*track_itr)->vertex()->x();
        m_vertex_y = (*track_itr)->vertex()->y();
        m_vertex_z = (*track_itr)->vertex()->z();
  
	m_Tree->Fill();


	      } // End of track selection loop

 return StatusCode::SUCCESS;
}

StatusCode IDAlignMonPVBiases::finalize()
{
  return StatusCode::SUCCESS; 
}

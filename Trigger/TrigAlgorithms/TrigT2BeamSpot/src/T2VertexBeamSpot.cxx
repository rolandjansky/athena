/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//============================================================
// $Id: T2VertexBeamSpot.cxx 793164 2017-01-20 03:59:26Z ssnyder $
//
// T2VertexBeamSpot.cxx, (c) ATLAS Detector software
// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
//
// Online beam spot measurement and monitoring using
// L2 recontructed primary vertices.
//
// Authors : David W. Miller, Rainer Bartoldus,   
//           Su Dong
//============================================================
#include "TrigT2BeamSpot/T2VertexBeamSpot.h"
#include "T2TrackClusterer.h"
#include "T2Timer.h"
// General ATHENA/Trigger stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigNavigation/TriggerElement.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include <typeinfo>
#include <cmath>

using std::string;

using namespace PESA;
using Gaudi::Units::mm;
using Gaudi::Units::GeV;

//Loads all TEs into one vector
HLT::TEVec getAllTEs(const std::vector<HLT::TEVec>& tes_in){

   std::vector<std::vector<HLT::TriggerElement*> >::const_iterator iTE = tes_in.begin();
   std::vector<std::vector<HLT::TriggerElement*> >::const_iterator iTE_end = tes_in.end();
   HLT::TEVec allTEs= *iTE;

   for (;iTE!=iTE_end; ++iTE) {
      HLT::TEVec::const_iterator inner_itEnd = (*iTE).end();
      HLT::TEVec::const_iterator inner_it = (*iTE).begin();

      for (;inner_it != inner_itEnd ; ++inner_it) {
         allTEs.push_back(*inner_it);
      }
   }  
   return allTEs;
}

// Constructor
T2VertexBeamSpot::T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator )
  : HLT::AllTEAlgo(name, pSvcLocator){ 
   
   declareProperty( "TrackCollection",  m_trackCollectionKey = "TrigFastTrackFinder_Tracks"  );
   declareProperty( "VertexCollection",  m_outputVertexCollectionKey =  "myVertices"  );

   declareProperty("vertexCollName",     m_vertexCollName      = "TrigBeamSpotVertex");



   declareProperty("activateTE",          m_activateTE          = false               );
   declareProperty("activateAllTE",       m_activateAllTE       = false               );
   declareProperty("attachVertices",      m_attachVertices      = false               );
   declareProperty("attachSplitVertices", m_attachSplitVertices = false               ); 
   declareProperty("activateSI",     m_activateSI     = false);
}


T2VertexBeamSpot::~T2VertexBeamSpot(){}

//----------------
// Initialization
HLT::ErrorCode T2VertexBeamSpot::hltInitialize() {
   ATH_MSG_DEBUG( "Initialising BeamSpot algorithm");

   if( m_beamSpotTool.retrieve().isFailure() ){
      ATH_MSG_ERROR("Can't retrieve the beamspot tool!");
      return HLT::BAD_JOB_SETUP;
   }
   else ATH_MSG_DEBUG( "BeamSpotTool retrieved");

   if( m_eventInfoKey.initialize().isFailure() ){
      ATH_MSG_ERROR("Can't retrieve event info Key from SG");
      return HLT::BAD_JOB_SETUP;
   }
   else ATH_MSG_DEBUG( "Event info key initialized");

   if( m_monTool.retrieve().isFailure() ){
      ATH_MSG_ERROR("Can't retrieve the beamspot tool!");
      return HLT::BAD_JOB_SETUP;
   }
   else ATH_MSG_DEBUG( "Monitoring tool retrieved");
   return HLT::OK;
}



HLT::ErrorCode T2VertexBeamSpot::hltExecute( std::vector< HLT::TEVec >& tes_in,
                              unsigned int type_out ){
  ATH_MSG_DEBUG( "In T2VertexBeamSpot::hltExecute");  
  
  // Start the overall timer
  auto tTotal = Monitored::Timer("TIME_TotalTime");

  // Initialize vertex collections
  TrigVertexCollection myVertexCollection;
  DataVector< TrigVertexCollection > mySplitVertexCollections;

  // Be optimistic
  HLT::ErrorCode errorCode = HLT::OK;
  unsigned int nPassVtx = 0;
  const EventContext& ctx = Algorithm::getContext();
  // Process event - break to do cleanup before returning
  do 
    {

      // Check for input Trigger elements
      if ( tes_in.empty() ){
         ATH_MSG_DEBUG( " No trigger elements" );
         break;
      }
      else ATH_MSG_DEBUG( "Number of input TEs = " << tes_in.size() );
  
      //-----------------
      // Track selection

      ConstDataVector<TrackCollection> mySelectedTrackCollection;
      mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );


      unsigned nAllTracks = 0;
      unsigned nSelectedTracks = 0;
      unsigned nHighPtTracks = 0;

      //Loop over all Trigger Elements
      {
         //Monitor total all/passed/highPt track numbers
         auto nTotalTracks = Monitored::Scalar<unsigned>("nTotalTracks");
         auto nTotalPassedTracks = Monitored::Scalar<unsigned>("nTotalPassedTracks");
         auto nTotalHighPTTracks = Monitored::Scalar<unsigned>("nTotalHighPTTracks");

         //Monitor how long does it take to loop over all trigger elements
         auto tTEs = Monitored::Timer("TIME_LoopAllTEs");
         for ( HLT::TEVec iTE : tes_in){

            //Skip if no TE
            if ( iTE.empty() ) continue;

            //Monitor  all/passed/highPt track numbers per vector of Trigger Elements
            auto nPerVecTETracks = Monitored::Scalar<unsigned>("nPerVecTETracks");
            auto nPerVecTEPassedTracks = Monitored::Scalar<unsigned>("nPerVecTEPassedTracks");
            auto nPerVecTEHighPTTracks = Monitored::Scalar<unsigned>("nPerVecTEHighPTTracks");

            //Loop over Trigger Elements
            for ( HLT::TriggerElement* TE : iTE){
               //Monitor how long does it take to loop over each trigger element
               auto tTE = Monitored::Timer("TIME_EachTE");

               //Monitor  all/passed/highPt track numbers per one Trigger Element
               auto nPerTETracks = Monitored::Scalar<unsigned>("nPerTETracks");
               auto nPerTEPassedTracks = Monitored::Scalar<unsigned>("nPerTEPassedTracks");
               auto nPerTEHighPTTracks = Monitored::Scalar<unsigned>("nPerTEHighPTTracks");

               std::vector<const TrackCollection*> vectorOfTrackCollections;
               //Retrieve trackCollection
               HLT::ErrorCode status = getFeatures( TE, vectorOfTrackCollections );

               if ( status != HLT::OK ) ATH_MSG_DEBUG( " Failed to get InDetTrackCollections " );
               else ATH_MSG_DEBUG( " Got " << vectorOfTrackCollections.size() << " InDetTrackCollections" );

               for( const TrackCollection* tracks : vectorOfTrackCollections){
                  //Select tracks
                  std::vector<unsigned> nTracks (3, 0); //returning all tracks[0]/passed tracks[1]/ hipt tracks[2] counts
                  m_beamSpotTool->selectTracks( tracks, mySelectedTrackCollection, nTracks );

                  //Increment counters per TE
                  nPerTETracks       += nTracks[0];
                  nPerTEPassedTracks += nTracks[1];
                  nPerTEHighPTTracks += nTracks[2];
               }

               //Increment counters per vector of TE
               nPerVecTETracks       +=nPerTETracks;
               nPerVecTEPassedTracks += nPerTEPassedTracks;
               nPerVecTEHighPTTracks += nPerTEHighPTTracks;

            //Monitor  number of tracks per Trigger element
               auto monitor = Monitored::Group(m_monTool, nPerTETracks, nPerTEPassedTracks, nPerTEHighPTTracks, tTE );
            } //End loop over TEs

            //Increment total track counters
            nTotalTracks       +=nPerVecTETracks;
            nTotalPassedTracks += nPerVecTEPassedTracks;
            nTotalHighPTTracks += nPerVecTEHighPTTracks;

            //Monitor  number of tracks per Vector of Trigger elements
            auto monitor = Monitored::Group(m_monTool, nPerVecTETracks, nPerVecTEPassedTracks, nPerVecTEHighPTTracks, tTEs );
         } // End loop over vector of TEs

         //Store counts for all/highPt/selected tracks
         nAllTracks = nTotalTracks;
         nHighPtTracks = nTotalHighPTTracks;
         nSelectedTracks = nTotalPassedTracks;

         //Monitor total number of tracks
         auto monitor = Monitored::Group(m_monTool, nTotalTracks, nTotalPassedTracks, nTotalHighPTTracks );
      } //End part of track selection


      ATH_MSG_DEBUG( "Number of all Tracks: "<< nAllTracks <<" Selected Tracks: " << nSelectedTracks << " highPt Tracks: " << nHighPtTracks );

      if ( !m_beamSpotTool->isHighPTTrack( nHighPtTracks  ) ) {
         ATH_MSG_DEBUG( " No seed tracks for vertex");
         break;
      }



      // Check for the total number of available tracks
      if( m_beamSpotTool->notEnoughTracks( nSelectedTracks ) ){
         ATH_MSG_DEBUG( " Not enough total passed tracks to vertex");
         break;
      }

      //-----------------------
      // Vertex reconstruction
      // Cluster tracks in z around seed track and reconstruct vertices
      nPassVtx = m_beamSpotTool->reconstructVertices( mySelectedTrackCollection, myVertexCollection, mySplitVertexCollections, ctx );

    } while (false);

  //------------------------------------
  // Store output (if any) in the event
  //------------------------------------

  // Add output TEs and attach vertex collections as features if requested
  // JKirk 7/7/16 Need to attach to input TEs
      if( tes_in.size() != 1 ){
         ATH_MSG_DEBUG( "Number of input TE vectors expected to be 1, is  " << tes_in.size());
         return errorCode;
      }
   auto timeToCreateOutput = Monitored::Timer("TIME_toCreateOutput");

   //Saves all TEs under one vector
   HLT::TEVec allTEs =  getAllTEs(tes_in);
   ATH_MSG_DEBUG( "n of all TEs: " << allTEs.size() );

   //Save all events, or only those events which pass the Npv cuts (if activated)!
   if ( m_activateAllTE ) {
      ATH_MSG_DEBUG( "Activate all TEs" );
      // Create an output TE seeded by the inputs
      HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
      outputTE->setActiveState(true);
   }


   if ( ! m_activateAllTE && m_activateTE && nPassVtx > 0 ) {
      ATH_MSG_DEBUG( "Activate TE with "<< nPassVtx << " vertices" );
      // FIXME: Why do we need one TE per vertex?
      for ( unsigned i=0; i < nPassVtx; ++i ) {
         // Create an output TE seeded by the inputs
         HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
         outputTE->setActiveState(true);
      }
   }

   // Save all events, or only those events which pass the Npv cuts (if activated)
   if ( ! m_activateAllTE && m_activateSI && m_beamSpotTool->m_passNpvTrigCuts){
      ATH_MSG_DEBUG( "Activate SI with passed trig vtx cuts" );

      // Create an output TE seeded by the inputs
      HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
      outputTE->setActiveState(true);
   }


   if ( m_attachVertices ){
      //Attach vertex collection feature  to output trigger element
      attachFeatureVertex(myVertexCollection, allTEs, type_out );
      //Attach split vertex collection feature  to output trigger element
      if ( m_attachSplitVertices ) attachFeatureSplitVertex(mySplitVertexCollections, allTEs, type_out );
   }



  // Return cause you're done!
  return errorCode;
}



HLT::ErrorCode T2VertexBeamSpot::attachFeatureVertex( TrigVertexCollection &myVertexCollection,  HLT::TEVec &allTEs, unsigned int type_out ){
   ATH_MSG_DEBUG( "Saving primary vertex collection");

   // Save primary vertex collection
   std::unique_ptr<TrigVertexCollection> newColl = std::make_unique< TrigVertexCollection>();

   newColl->clear( SG::VIEW_ELEMENTS );
   newColl->swap( myVertexCollection ); // swaps ownership
   // Strip list of tracks from the vertex so we don't persist them.

   HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
   const HLT::ErrorCode status = attachFeature( outputTE, newColl.release() , m_beamSpotTool->m_vertexCollName );
   if ( status != HLT::OK ) ATH_MSG_DEBUG( "Failed to attach primary vertex collection to output trigger element");
   return status;
}


HLT::ErrorCode T2VertexBeamSpot::attachFeatureSplitVertex(DataVector< TrigVertexCollection > &mySplitVertexCollections,  HLT::TEVec &allTEs, unsigned int type_out ){
   ATH_MSG_DEBUG( "Saving split vertex collections");
   // For each primary vertex we find, we save a group of
   // (typically two) split vertices in a separate collection
   // Thus we have:
   //
   //    VertexCollection    vector<VertexCollection>
   //    (primary vertex)    (split vertex collection)      (split vertex 1)  (split vertex 2) ...
   //  --------------------------------------------------------------------------------------------
   //    |      PV1           SplitVertexCollection_1  --->      SP1_1             SP1_2       ... 
   //    |      PV2           SplitVertexCollection_2  --->      SP2_1             SP2_2            
   //    v      PV3           SplitVertexCollection_3  --->      SP3_1             SP3_2            
   //           ...
   //

   HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);

   // FIXME: This is what we would like to be able to do:
   // Save vector of split vertex collections
   //          TrigVertexCollectionVector* newCollVec = new TrigVertexCollectionVector;
   //          newCollVec->clear( SG::VIEW_ELEMENTS );
   //          newCollVec->swap( mySplitVertexCollections ); // swaps ownership
   //          const HLT::ErrorCode status = attachFeature( outputTE, newCollVec, m_vertexCollName );
   //          if ( status != HLT::OK )
   //            {
   //              msg() << MSG::WARNING << "Write of Beamspot feature into outputTE failed" );
   //            }
   // FIXME: The above needs a new (container) object to be registered.
   // FIXME: For the time being we store the individual collections one by one.
   // Save split vertex collections
   for ( unsigned i=0; i < mySplitVertexCollections.size(); ++i ){
      std::ostringstream collName;
      collName << m_beamSpotTool->m_vertexCollName << "_" << i;
      TrigVertexCollection* newColl;
      // Wrestle back ownership of this collection from DataVector
      mySplitVertexCollections.swapElement( i, 0, newColl );
      // Strip list of tracks from the vertex so we don't persist them

      const HLT::ErrorCode status = attachFeature( outputTE, newColl, collName.str() );
      if ( status != HLT::OK ) {
         ATH_MSG_DEBUG( "Failed to attach split vertex collection " << i
               << " of " << mySplitVertexCollections.size() << " to output trigger element" );
         //Should we crash here??
         //return HLT::Error;
      }

   }//end loop over splitVertexCollection
   return HLT::OK;
}


HLT::ErrorCode T2VertexBeamSpot::hltFinalize() {
   ATH_MSG_DEBUG( "T2VertexBeamSpot finalize:");
   return HLT::OK;
}




// ===============================
//        Run 3 configuration


StatusCode T2VertexBeamSpot::initialize() {
   ATH_MSG_INFO( "Initialising BeamSpot algorithm");

   if( m_beamSpotTool.retrieve().isFailure() ){
      ATH_MSG_ERROR("Can't retrieve the beamspot tool!");
      return StatusCode::FAILURE;
   }

   ATH_CHECK( m_trackCollectionKey.initialize() );


   ATH_CHECK( m_outputVertexCollectionKey.initialize() );


   ATH_CHECK( m_eventInfoKey.initialize() );

   ATH_CHECK( m_monTool.retrieve());

   ATH_MSG_INFO( "Initialising of T2VertexBeamSpot algorithm SUCCESFUL");
   return StatusCode::SUCCESS;
}


StatusCode T2VertexBeamSpot::execute(){
   // Start the overall timer
   auto tTotal = Monitored::Timer("TIME_TotalTime");

   ATH_MSG_DEBUG( "Beam spot algorithm execute method" );

   const EventContext& ctx = Algorithm::getContext();

   //// Initialize vertex collections
   SG::WriteHandle<TrigVertexCollection> myVertexCollection(m_outputVertexCollectionKey, ctx);
   myVertexCollection = std::make_unique<TrigVertexCollection>();

   //Need to convert to the write handles
   DataVector< TrigVertexCollection > mySplitVertexCollections;

   ConstDataVector<TrackCollection> mySelectedTrackCollection;
   mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );


   //Counters to keep track of number of all/selected/highPt tracks
   unsigned nAllTracks = 0;
   unsigned nSelectedTracks = 0;
   unsigned nHighPtTracks = 0;

   ATH_MSG_DEBUG( "Selecting tracks" );
   {
      //Monitor how long does it take to loop over all collections
      auto tSelectingTracks = Monitored::Timer("TIME_SelectingTracks");

      //Monitor total all/passed/highPt track numbers
      auto nTotalTracks = Monitored::Scalar<unsigned>("nTotalTracks");
      auto nTotalPassedTracks = Monitored::Scalar<unsigned>("nTotalPassedTracks");
      auto nTotalHighPTTracks = Monitored::Scalar<unsigned>("nTotalHighPTTracks");
      //Loop over track collections and select tracks
      SG::ReadHandle<TrackCollection> trackCollection (m_trackCollectionKey, ctx);
      ATH_CHECK(trackCollection.isValid());

      //Dereference tracks
      const TrackCollection* tracks = trackCollection.cptr();

      //Select tracks
      std::vector<unsigned> trackCounter(3,0);//returning all tracks[0]/passed tracks[1]/ hipt tracks[2] counts
      m_beamSpotTool->selectTracks( tracks, mySelectedTrackCollection, trackCounter );

      //FIXME: make a counter class
      nTotalTracks       += trackCounter[0];   
      nTotalPassedTracks += trackCounter[1]; 
      nTotalHighPTTracks += trackCounter[2];

      //Store counts for all/highPt/selected tracks
      nAllTracks = nTotalTracks;
      nHighPtTracks = nTotalHighPTTracks;
      nSelectedTracks = nTotalPassedTracks;

      //Monitor total number of tracks
      auto monitor = Monitored::Group(m_monTool, tSelectingTracks, nTotalTracks, nTotalPassedTracks, nTotalHighPTTracks );
      ATH_MSG_DEBUG( "Number of all Tracks: "<< nAllTracks <<" Selected Tracks: " << nSelectedTracks << " highPt Tracks: " << nHighPtTracks );
   }


   // Check for seed tracks (= high-pT tracks)
   if ( ! m_beamSpotTool->isHighPTTrack(nHighPtTracks) ) {
      ATH_MSG_DEBUG( " No seed tracks for vertex");
      return StatusCode::SUCCESS;
   }

   // Check for the total number of available tracks
   if (  ( m_beamSpotTool->notEnoughTracks(nSelectedTracks)) ) {
      ATH_MSG_DEBUG( "Not enough total passed tracks to vertex");
      return StatusCode::SUCCESS;
   }

   ATH_MSG_DEBUG( "Reconstruct vertices" );
   //Reconstruct vertices if passed track selection
   {
      //Monitor how long does it take to loop over all collections
      auto tReconstructVertices = Monitored::Timer("TIME_ReconstructVertices");
      m_beamSpotTool->reconstructVertices( mySelectedTrackCollection, *myVertexCollection, mySplitVertexCollections, ctx );
      //Monitor total number of tracks
      auto monitor = Monitored::Group(m_monTool, tReconstructVertices);
   }

   ATH_MSG_DEBUG( "Number of track collection containers: " << mySelectedTrackCollection.size() );

   //What should go as an output? SelectedTrackCollection and Vertices?
   //Atm just try add vertex
   //TODO: adding split vertices as well, will need an array

   return StatusCode::SUCCESS;
} 


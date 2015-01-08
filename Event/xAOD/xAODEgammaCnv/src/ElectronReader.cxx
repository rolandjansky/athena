/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronReader.cxx 635870 2014-12-14 16:43:35Z christos $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

// Local include(s):
#include "ElectronReader.h"

namespace xAODReader {

   ElectronReader::ElectronReader( const std::string& name, ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {
      declareProperty( "ContainerName", m_containerName = "Electrons" );
      declareProperty( "ContainerFrwdName", m_containerFrwdName = "ForwardElectrons" );

   }

   StatusCode ElectronReader::initialize() {

      ATH_MSG_DEBUG( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "  ContainerName = \"" << m_containerName << "\"" );
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode ElectronReader::execute() {

      // Retrieve the container:
      const xAOD::ElectronContainer* electrons =
	evtStore()->retrieve< const xAOD::ElectronContainer >( m_containerName );
      if( ! electrons ) {
	REPORT_MESSAGE( MSG::ERROR )
	  << "Couldn't retrieve electron container with key: "
	  << m_containerName;
	return StatusCode::FAILURE;
      }

      // Check that the auxiliary store association was made successfully:
      if( ! electrons->hasStore() ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "No auxiliary store got associated to the electron container "
            << "with key: " << m_containerName;
         return StatusCode::FAILURE;
      }


      // Retrieve the container:
      const xAOD::ElectronContainer* electronsFrwd =
	evtStore()->retrieve< const xAOD::ElectronContainer >( m_containerFrwdName );
      if( ! electronsFrwd ) {
	REPORT_MESSAGE( MSG::ERROR )
	  << "Couldn't retrieve electron container with key: "
	  << m_containerName;
	return StatusCode::FAILURE;
      }

      // Check that the auxiliary store association was made successfully:
      if( ! electronsFrwd->hasStore() ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "No auxiliary store got associated to the electron container "
            << "with key: " << m_containerName;
         return StatusCode::FAILURE;
      }
      // Retrieve the Truth container:
      const xAOD::TruthParticleContainer* electronTruth =
	evtStore()->retrieve< const xAOD::TruthParticleContainer >( "egammaTruthParticles" );
      if( ! electronTruth) {
	REPORT_MESSAGE( MSG::ERROR )
	  << "Couldn't retrieve electron Truth container with key: "
	  << "egammaTruthParticles";
	return StatusCode::FAILURE;
      }

      // Check that the auxiliary store association was made successfully:
      if( ! electronTruth->hasStore() ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "No auxiliary store got associated to the electron Truth container "
            << "with key: " << "egammaTruthParticles";
         return StatusCode::FAILURE;
      }


      // Print some basic info:
      ATH_MSG_DEBUG( "Truth : ########################################"  );
      ATH_MSG_DEBUG( "Number of Truth electrons: " << electronTruth->size() );
      auto tr_itr = electronTruth->begin();
      auto tr_end = electronTruth->end();
      for( int i = 0; tr_itr != tr_end; ++tr_itr, ++i ) {
	ATH_MSG_DEBUG( "Truth : Investigating truth electron #" << i );
	ATH_MSG_DEBUG( "Truth : truth electron eta  " <<( *tr_itr )->eta()  );
	ATH_MSG_DEBUG( "Truth : truth electron pdgID " <<( *tr_itr )->pdgId() );

      }
      ///// Reconstruction
      // Print some basic info:
      ATH_MSG_DEBUG( "Number of electrons: " << electrons->size() );

      // Now print some detailed info about each electron:
      auto el_itr = electrons->begin();
      auto el_end = electrons->end();
      for( int i = 0; el_itr != el_end; ++el_itr, ++i ) {
	ATH_MSG_DEBUG( "########################################"  );
	ATH_MSG_DEBUG( "Investigating electron #" << i );
	ATH_MSG_DEBUG( " Author = " << ( *el_itr )->author()); 
	ATH_MSG_DEBUG( "4-Momentum"  );
	ATH_MSG_DEBUG(    " Pt = "
			 << ( *el_itr )->pt() 
			 << ", eta = "
			 << ( *el_itr )->eta() 
			 << ", phi = "
			 << ( *el_itr )->phi() 
			 << ", m = "
			  << ( *el_itr )->m() 
			 << ", e = "
			 << ( *el_itr )->e() 
			 << ", P from tlv = "
			 << ( *el_itr )->p4().P() 
			 );


	   ATH_MSG_DEBUG( "Clusters"  );         
	   ATH_MSG_DEBUG( "Number of Cluster = " <<( *el_itr )->nCaloClusters());
	   const xAOD::CaloCluster* cluster= (*el_itr)->caloCluster();
	   if (cluster){
	     ATH_MSG_DEBUG( "Cluster 4-Momentum"  );
	     ATH_MSG_DEBUG(    " Energy "
			      << cluster->e() 
			      << ", raw energy = "
			      << cluster->rawE() 
			      << ", eta = "
			      << cluster->eta() 
			      << ", phi = "
			      << cluster->phi() 
			      );
	     
	   }

	   ATH_MSG_DEBUG( "TrackParticles"  );         
	   ATH_MSG_DEBUG( "Number of TrackParticle = " <<( *el_itr )->nTrackParticles());
	   const xAOD::TrackParticle* trackParticle= (*el_itr)->trackParticle();
	   if (trackParticle){
	     ATH_MSG_DEBUG( "TrackParticle 4-Momentum"  );
	     ATH_MSG_DEBUG(    " pt "
			      << trackParticle->pt() 
			      << ", eta = "
			      << trackParticle->eta() 
			      << ", phi = "
			      << trackParticle->phi() 
			      );
	     ATH_MSG_DEBUG( "Track Fitter " << trackParticle->trackFitter() );


	   }

	   ATH_MSG_DEBUG( " Charge = " << ( *el_itr )->charge()); 

	   //Pass selection
	   ATH_MSG_DEBUG( "Pass selection variables" );

	   bool val_med=0;	
	   if(( *el_itr )->passSelection(val_med, "Medium")) {
	     ATH_MSG_DEBUG( "Medium  selection exists");
	     ATH_MSG_DEBUG( "Medium value : " << static_cast<bool> (val_med));
	   }
	   bool val_loose=0;	
	   if(( *el_itr )->passSelection(val_loose, "Loose")) {
	     ATH_MSG_DEBUG( "Loose  selection exists");
	     ATH_MSG_DEBUG( "Loose value : " << static_cast<bool> (val_loose));
	   }
	   bool val_tight=0;	
	   if(( *el_itr )->passSelection(val_tight, "Tight")) {
	     ATH_MSG_DEBUG( "Tight selection exists");
	     ATH_MSG_DEBUG( "Tight value : " << static_cast<bool> (val_tight));
	   }

	   //Shower Shapes
	   float val_float=0;
	   ATH_MSG_DEBUG( "Shower shape parameter"  );         
	   if(( *el_itr )->showerShapeValue(val_float,xAOD::EgammaParameters::e237) ){
	     ATH_MSG_DEBUG(    "e237 = " <<val_float);
	   }
	   if(( *el_itr )->showerShapeValue(val_float,xAOD::EgammaParameters::e277) ){
	     ATH_MSG_DEBUG(    "e277 = " <<val_float);
	   }
	   if(( *el_itr )->showerShapeValue(val_float,xAOD::EgammaParameters::Reta) ){
	     ATH_MSG_DEBUG(    "Reta= " <<val_float);
	   }
	   if(( *el_itr )->showerShapeValue(val_float,xAOD::EgammaParameters::wtots1) ){
	     ATH_MSG_DEBUG(    "wtots1 = " <<val_float);
	   }
	   if(( *el_itr )->showerShapeValue(val_float,xAOD::EgammaParameters::weta1) ){
	     ATH_MSG_DEBUG(    "weta1 = " <<val_float);
	   }
	   if(( *el_itr )->showerShapeValue(val_float,xAOD::EgammaParameters::weta2) ){
	     ATH_MSG_DEBUG(    "weta2 = " <<val_float);
	   }


	   if(( *el_itr )->isolationValue(val_float,xAOD::Iso::ptcone20) ){
	     ATH_MSG_DEBUG(    "ptcone20 = " <<val_float);
	   }

	   if(( *el_itr )->isolationValue(val_float,xAOD::Iso::ptcone30) ){
	     ATH_MSG_DEBUG(    "ptcone30 = " <<val_float);
	   }

	   if(( *el_itr )->isolationValue(val_float,xAOD::Iso::ptcone40) ){
	     ATH_MSG_DEBUG(    "ptcone40 = " <<val_float);
	   }

	   if(( *el_itr )->isolationValue(val_float,xAOD::Iso::topoetcone20) ){
	     ATH_MSG_DEBUG(    "topoetcone20  = " <<val_float);
	   }

	   if(( *el_itr )->isolationValue(val_float,xAOD::Iso::topoetcone30) ){
	     ATH_MSG_DEBUG(    "topoetcone30 = " <<val_float);
	   }

	   if(( *el_itr )->isolationValue(val_float,xAOD::Iso::topoetcone40) ){
	     ATH_MSG_DEBUG(    "topoetcone40 = " <<val_float);
	   }

	   //Cluster-Track Match
	   ATH_MSG_DEBUG( "Track Match parameter"  );         
	   if(( *el_itr )->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhi2) ){
	     ATH_MSG_DEBUG(    "deltaPhi2 = " <<val_float);
	   }
	   if(( *el_itr )->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhiRescaled2) ){
	     ATH_MSG_DEBUG(    "deltaPhiRescaled = " <<val_float);
	   }
	   if(( *el_itr )->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhiFromLastMeasurement) ){
	       ATH_MSG_DEBUG(    "deltaPhiFromLastMeasurement = " <<val_float);
	   }
	   if(( *el_itr )->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaEta2) ){
	       ATH_MSG_DEBUG(    "deltaEta2 = " <<val_float);
	   }
	   	   
	   ATH_MSG_DEBUG( "TrackParticle Summary"  );
	   
	   uint8_t val_uint8=0;
	     if(( *el_itr )->trackParticleSummaryValue(val_uint8,xAOD::numberOfInnermostPixelLayerHits) ){
	       ATH_MSG_DEBUG(    "numberOfInnermostPixelLayerHits = " <<static_cast<unsigned int> (val_uint8));
	     }
	     
	     if(( *el_itr )->trackParticleSummaryValue(val_uint8,xAOD::numberOfPixelHits) ){
	       ATH_MSG_DEBUG(    "numberOfPixelHits = " <<static_cast<unsigned int> (val_uint8));
	     }
	     
	     ATH_MSG_DEBUG( "Electron Covariance Matrix" );
	     ATH_MSG_DEBUG( ( *el_itr )->covMatrix()   );

	     const xAOD::TrackParticle* origTrack = xAOD::EgammaHelpers::getOriginalTrackParticle(*el_itr);
	     if(origTrack){
	       ATH_MSG_DEBUG( "Original TrackParticle");
	       ATH_MSG_DEBUG( "TrackParticle 4-Momentum"  );
	       ATH_MSG_DEBUG(    " pt "
				<< origTrack->pt() 
				<< ", eta = "
				<< origTrack->eta() 
				<< ", phi = "
				<< origTrack->phi() 
				);

	       ATH_MSG_DEBUG( "Track Fitter "<< origTrack->trackFitter() );
	     }

	     //Truth info
	     ATH_MSG_DEBUG( "Reco Truth :# " <<i );
	     //Truth info
	     if ((*el_itr)->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") ) {
	       ATH_MSG_DEBUG( "truthParticleLink index " << (*el_itr)->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink").index());
	       if ((*el_itr)->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink").isValid()){
		 const xAOD::TruthParticle *truthParticle = *((*el_itr)->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink"));
		 ATH_MSG_DEBUG( "Truth : Reco truth eta " << truthParticle->eta());
		 ATH_MSG_DEBUG( "Truth : Reco truth pdgID " << truthParticle->pdgId());
	       }
	     }

	     if ((*el_itr)->isAvailable<int>("truthType") ) {
	       ATH_MSG_DEBUG( "Truth : Reco Truth Type " << (*el_itr)->auxdata<int>("truthType"));
	     }
	     if ((*el_itr)->isAvailable<int>("truthOrigin") ) {
	       ATH_MSG_DEBUG( "Truth : Reco Truth Origin " << (*el_itr)->auxdata<int>("truthOrigin"));
	       
	     }
	    
      }//Loop over Electrons
      
      // Print some basic info:
      ATH_MSG_DEBUG( "Number of electronsFrwd: " << electronsFrwd->size() );
      
      // Now print some detailed info about each electron:
      auto elFrwd_itr = electronsFrwd->begin();
      auto elFrwd_end = electronsFrwd->end();

      for( int i = 0; elFrwd_itr != elFrwd_end; ++elFrwd_itr, ++i ) {

	ATH_MSG_DEBUG( "########################################"  );
	ATH_MSG_DEBUG( "Investigating Frwd electron #" << i );
	ATH_MSG_DEBUG( " Author = " << ( *elFrwd_itr )->author()); 
	ATH_MSG_DEBUG( "4-Momentum"  );
	ATH_MSG_DEBUG(    " Pt = "
			 << ( *elFrwd_itr )->pt() 
			 << ", eta = "
			 << ( *elFrwd_itr )->eta() 
			 << ", phi = "
			 << ( *elFrwd_itr )->phi() 
			 << ", m = "
			  << ( *elFrwd_itr )->m() 
			 << ", e = "
			 << ( *elFrwd_itr )->e() 
			 << ", P from tlv = "
			 << ( *elFrwd_itr )->p4().P() 
			 );
	
	ATH_MSG_DEBUG( "Clusters"  );         
	ATH_MSG_DEBUG( "Number of Cluster = " <<( *elFrwd_itr )->nCaloClusters());
	const xAOD::CaloCluster* cluster= (*elFrwd_itr)->caloCluster();
	if (cluster){
	  ATH_MSG_DEBUG( "Cluster 4-Momentum"  );
	  ATH_MSG_DEBUG(    " Energy "
			   << cluster->e() 
			   << ", eta = "
			   << cluster->eta() 
			   << ", phi = "
			      << cluster->phi() 
			   );
	     
	  ATH_MSG_DEBUG( "Electron Covariance Matrix" );
	  ATH_MSG_DEBUG( ( *elFrwd_itr )->covMatrix()   );

	   bool val_loose=0;	
	   if(( *elFrwd_itr )->passSelection(val_loose, "Loose")) {
	     ATH_MSG_DEBUG( "Loose  selection exists");
	     ATH_MSG_DEBUG( "Loose value : " << static_cast<bool> (val_loose));
	   }
	   bool val_tight=0;	
	   if(( *elFrwd_itr )->passSelection(val_tight, "Tight")) {
	     ATH_MSG_DEBUG( "Tight selection exists");
	     ATH_MSG_DEBUG( "Tight value : " << static_cast<bool> (val_tight));
	   }

	   //Truth info
	   ATH_MSG_DEBUG( "Reco Frwd Truth : #" << i );
	   //Truth info
	   if ((*elFrwd_itr)->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") ) {
	     ATH_MSG_DEBUG( "truthParticleLink index " << (*elFrwd_itr)->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink").index());
	     if ((*elFrwd_itr)->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink").isValid()){
	     const xAOD::TruthParticle *truthParticle = *((*elFrwd_itr)->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink"));
	     ATH_MSG_DEBUG( "Truth : Reco Frwd truth pdgID " << truthParticle->pdgId());
	     }
	   }
	  
	   if ((*elFrwd_itr)->isAvailable<int>("truthType") ) {
	     ATH_MSG_DEBUG( "Truth : Reco Frwd Truth Type " << (*elFrwd_itr)->auxdata<int>("truthType"));
	   }
	   if ((*elFrwd_itr)->isAvailable<int>("truthOrigin") ) {
	     ATH_MSG_DEBUG( "Truth : Reco Frwd Truth Origin " << (*elFrwd_itr)->auxdata<int>("truthOrigin"));
	     
	   }

	}
      }//Forward electrons
      

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODReader

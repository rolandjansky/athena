/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "xAODCore/ShallowCopy.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "InDetTrackSmearingToolTester.h"
#include "InDetTrackSystematicsTools/InDetTrackSystematics.h"
#include <TH1.h>


namespace InDet {
   InDetTrackSmearingToolTester::InDetTrackSmearingToolTester( const std::string& name, ISvcLocator* svcLoc )
      : AthHistogramAlgorithm( name, svcLoc ),
        m_smearTool( "InDet::InDetTrackSystematicsTools/InDetTrackSmearingTool", this ){
          declareProperty( "TrackIP", Track_IP = "InDetTrackParticles" );
	  declareProperty( "SystematicEffects", m_systematicsNames );
          declareProperty( "InDetTrackSmearingTool", m_smearTool );

        }

   StatusCode InDetTrackSmearingToolTester::initialize() {
     
      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "InDetTrackSmearingTool   = " << m_smearTool );

      // Retrieve the tools:
      ATH_CHECK( m_smearTool.retrieve() );

      for (const auto& name : m_systematicsNames) {
	for (const auto& systpair : InDet::TrackSystematicMap) {
	  if (name == systpair.second.name()) {
	    m_systActive.insert(systpair.second);
	  }
	}
      }
      auto systCode = m_smearTool->applySystematicVariation( m_systActive );
      if (systCode != CP::SystematicCode::Ok) {
	ATH_MSG_ERROR( "Failure to apply systematic variation." );
	return StatusCode::FAILURE;
      }

      ATH_CHECK( book( TH1F("d0_B", "original d0", 100, -5.0, 5.0) ) );     
      ATH_CHECK( book( TH1F("z0_B", "original z0", 100, -200.0, 200.0) ) );     
      ATH_CHECK( book( TH1F("d0sm", "d0 after smearing", 100, -5.0, 5.0) ) );     
      ATH_CHECK( book( TH1F("z0sm", "z0 after smearing", 100, -200.0, 200.0) ) );     
      ATH_CHECK( book( TH1F("subtraction_d0", "subtraction_d0", 100, -0.10, 0.10) ) );     
      ATH_CHECK( book( TH1F("subtraction_z0", "subtraction_z0", 100,-0.50, 0.50) ) );     

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode InDetTrackSmearingToolTester::execute() {

      // Create a shallow container copy and then apply the smearingtool to impact parameters:      
      const xAOD::TrackParticleContainer *IDParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( IDParticles , Track_IP ) );
      std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > IDParticles_shallowCopy = xAOD::shallowCopyContainer( *IDParticles );
      xAOD::TrackParticleContainer::iterator itr = (IDParticles_shallowCopy.first)->begin();
      xAOD::TrackParticleContainer::iterator end = (IDParticles_shallowCopy.first)->end();
      for( xAOD::TrackParticle* track : *IDParticles_shallowCopy.first ) {
          double d0_1=0.,d0_2=0.,z0_1=0.,z0_2=0.;
          d0_1=track->d0();
          z0_1=track->z0();
          hist("d0_B")->Fill( d0_1 );
          hist("z0_B")->Fill( z0_1 );
          if (m_smearTool->applyCorrection(*track) == CP::CorrectionCode::Error) {
	    ATH_MSG_ERROR( "Could not apply correction." );
	  }
          d0_2=track->d0();
          z0_2=track->z0();
          hist("d0sm")->Fill( d0_2 );
          hist("z0sm")->Fill( z0_2 ); 
          hist("subtraction_d0")->Fill( d0_2 - d0_1 );
          hist("subtraction_z0")->Fill( z0_2 - z0_1 );
      }
      delete IDParticles_shallowCopy.first;
      delete IDParticles_shallowCopy.second;  

      // Return gracefully:
      return StatusCode::SUCCESS;

   } // End of the execute()

} // namespace InDet        

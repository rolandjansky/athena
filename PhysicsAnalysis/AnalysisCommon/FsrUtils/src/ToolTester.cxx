/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"

// Local include(s):
#include "ToolTester.h"
#include "TLorentzVector.h"

namespace FSR {

   ToolTester::ToolTester( const std::string& name, ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_fsrTool( "FSR::FsrPhotonTool/FsrPhotonTool", this) {
        //m_ntsvc("THistSvc/THistSvc", name),
        //m_hist (0){

      declareProperty( "SGMuonKey",     m_sgMuonKey     = "Muons" );
      declareProperty( "SGPhotonKey",   m_sgPhotonKey   = "Photons" );
      declareProperty( "SGElectronKey", m_sgElectronKey = "Electrons" );
      declareProperty( "FsrPhotonTool", m_fsrTool );
   }

   StatusCode ToolTester::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_INFO( "SGMuonKey = "     << m_sgMuonKey );
      ATH_MSG_INFO( "SGPhotonKey = "   << m_sgPhotonKey );
      ATH_MSG_INFO( "SGElectronKey = " << m_sgElectronKey );
      ATH_MSG_INFO( "FsrPhotonTool = " << m_fsrTool );

      // Retrieve the tools:
      ATH_CHECK( m_fsrTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode ToolTester::execute() {

      // Retrieve the muons:
      const xAOD::MuonContainer* muons = 0;
      ATH_CHECK( evtStore()->retrieve( muons, m_sgMuonKey ) );

      ATH_MSG_INFO( "Number of muons: " << muons->size() );

      // Retrieve photons and electrons
      const xAOD::PhotonContainer* phs = 0;
      ATH_CHECK( evtStore()->retrieve( phs, m_sgPhotonKey ) );
      ATH_MSG_DEBUG("retrieved photons");
      const xAOD::ElectronContainer* els = 0;
      ATH_CHECK( evtStore()->retrieve( els, m_sgElectronKey ) );
      ATH_MSG_DEBUG("retrieved electrons");

      std::vector<const xAOD::Muon*> selectedMuons;
      double tmp_energy = -999.;
      double fsr_energy = 0.;
      TLorentzVector fsr_tlv;
      // Loop over them:
      // 
      xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
      xAOD::MuonContainer::const_iterator mu_end = muons->end();
      for( ; mu_itr != mu_end; ++mu_itr ) {

          
          if ( m_fsrTool->getFsrPhoton(*mu_itr, candidate, phs, els) ==  CP::CorrectionCode::Ok){
	  ATH_MSG_INFO( " FSR candidate found !!!!!!!! ");
	  ATH_MSG_INFO( " container = " << candidate.container
			//const xAOD::IParticle* particle;
			<< " deltaR = " << candidate.deltaR
			<< " Et = " << candidate.Et
			<<" f1 = " << candidate.f1
			<<" eta = " << candidate.eta
			<<" phi = " << candidate.phi
			<<" phIso = "<< candidate.phIso
			<<" fsrtype = " << candidate.type
			);
    	 }
	
	if (candidate.container == "photon" ) {
	  if(const xAOD::Photon* photon = dynamic_cast<const xAOD::Photon*>(candidate.particle)){
	    fsr_energy = photon->e();
	  }else{
	    ATH_MSG_WARNING( "Could not cast to Photon " );
	  }
	} else if (candidate.container == "electron" ) {
	  if(const xAOD::Electron* electron = dynamic_cast<const xAOD::Electron*>(candidate.particle)){
	     fsr_energy = electron->e();
	  }else{
	    ATH_MSG_WARNING( "Could not cast to Electron " );
	  }
	} else{
	  ATH_MSG_INFO( " FSR candidate particle is unknown " );
	}
	if ( fsr_energy > tmp_energy ) {
	  tmp_energy = fsr_energy;   
	  fsr_tlv.SetPtEtaPhiE(candidate.Et, candidate.eta, candidate.phi, fsr_energy);
	}
		
      }
   
      // Return gracefully:
      return StatusCode::SUCCESS;
   }


} // namespace FSR

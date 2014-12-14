/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMuon/MuonContainer.h"

// note: stolen from CPToolTests example!
// test algorithm using the MuonEfficiencyScaleFactors class

// Local include(s):
#include "MuonEfficiencyCorrections_TestAlg.h"
#include <cmath>
namespace CP {

MuonEfficiencyCorrections_TestAlg::MuonEfficiencyCorrections_TestAlg( const std::string& name, ISvcLocator* svcLoc )
: AthAlgorithm( name, svcLoc ),
  m_sf_Tool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors", this ){

    declareProperty( "SGKey", m_sgKey = "Muons" );
    // prepare the handle
    declareProperty( "ScaleFactorTool", m_sf_Tool );
    CP::SystematicCode::enableFailure();

}

StatusCode MuonEfficiencyCorrections_TestAlg::initialize() {

    ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
    ATH_MSG_DEBUG( "SGKey = " << m_sgKey );
    ATH_MSG_DEBUG( "ScaleFactorTool  = " << m_sf_Tool );

    ATH_CHECK( m_sf_Tool.retrieve() );

    return StatusCode::SUCCESS;
}

StatusCode MuonEfficiencyCorrections_TestAlg::execute() {

    // Retrieve the muons:
    const xAOD::MuonContainer* muons = 0;
    ATH_CHECK( evtStore()->retrieve( muons, m_sgKey ) );
    ATH_MSG_INFO( "Number of muons: " << muons->size() );

    // Loop over them:
    xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
    xAOD::MuonContainer::const_iterator mu_end = muons->end();

    CP::SystematicSet statup;
    statup.insert (CP::SystematicVariation ("MUONSFSTAT", 1));
    // make sure the tool is not affected by other unsupported systematics in the same set
    statup.insert (CP::SystematicVariation ("TÖRÖÖÖÖ", 1));

    for( ; mu_itr != mu_end; ++mu_itr ) {

        if (fabs((**mu_itr).eta()) > 2.7) continue;
        if ((**mu_itr).pt() < 10000) continue;

        // Print some info about the selected muon:
        ATH_MSG_INFO( "  Selected muon: eta = " << (**mu_itr).eta()
                << ", phi = " << (**mu_itr).phi()
                << ", pt = " << (**mu_itr).pt() );

        // obtain a scale factor directly from the muon
        float sf = -1;
        if (!m_sf_Tool->applySystematicVariation(statup)){
            ATH_MSG_WARNING( "Unable to switch to stat up sys!" );
            continue;
        }
        if(  m_sf_Tool->getEfficiencyScaleFactor((**mu_itr),sf) == CP::CorrectionCode::Error ) {
            ATH_MSG_WARNING( "Couldn't run efficiencies on muon with stat up variation!" );
            continue;
        }
        ATH_MSG_INFO( "       Scale Factor (stat up) via getEfficiencyScaleFactor = "
                << sf);
        if (!m_sf_Tool->applySystematicVariation(CP::SystematicSet())){
            ATH_MSG_WARNING( "Unable to switch to nominal sys!" );
            continue;
        }
        if( m_sf_Tool->getEfficiencyScaleFactor((**mu_itr),sf) == CP::CorrectionCode::Error ) {
            ATH_MSG_WARNING( "Couldn't run efficiencies on muon!" );
            continue;
        }
        ATH_MSG_INFO( "       Scale Factor (central value) via getEfficiencyScaleFactor = "
                << sf);
        // decorate the muon with SF info (alternative to the above)
        if(  m_sf_Tool->applyEfficiencyScaleFactor(**mu_itr) == CP::CorrectionCode::Error || m_sf_Tool->applyRecoEfficiency(**mu_itr) == CP::CorrectionCode::Error) {
            ATH_MSG_WARNING( "Couldn't run efficiencies on muon!" );
            continue;
        }
        // read the efficiency we decorated the muon with
        ATH_MSG_INFO( "       efficiency from decorated Muon = "
                << ((**mu_itr).isAvailable<float> ("Efficiency") ? (**mu_itr).auxdata< float >( "Efficiency" ) : -999. ));
        ATH_MSG_INFO( "       Scale Factor from decorated Muon = "
                << (**mu_itr).auxdata< float >( "EfficiencyScaleFactor" ) );
        // to estimate the stat uncertainty in a 'correct' way, roll replicas
        // and assign the variation of the final observable across the replicas as a systematic
        if (m_sf_Tool->applyEfficiencyScaleFactorReplicas((**mu_itr),20) == CP::CorrectionCode::Error){
            ATH_MSG_WARNING( "Couldn't run efficiency replica generation on muon!" );
            continue;
        }
        // here we load one of the replicas
        ATH_MSG_INFO( "       Scale Factor Replica #2 = "
                << (**mu_itr).auxdata< std::vector<float> >( "EfficiencyScaleFactorReplicas" )[1] );

    }

    // Return gracefully:
    return StatusCode::SUCCESS;
}

} // namespace CP

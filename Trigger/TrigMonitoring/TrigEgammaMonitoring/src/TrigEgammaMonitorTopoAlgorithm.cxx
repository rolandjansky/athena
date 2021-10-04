/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitorTopoAlgorithm.h"
#include "TrigDecisionTool/FeatureRequestDescriptor.h"
#include "xAODBase/IParticleContainer.h"

using namespace Trig;


TrigEgammaMonitorTopoAlgorithm::TrigEgammaMonitorTopoAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  TrigEgammaMonitorBaseAlgorithm( name, pSvcLocator )
{}

//!=========================================================================

TrigEgammaMonitorTopoAlgorithm::~TrigEgammaMonitorTopoAlgorithm()
{}

//!=========================================================================

StatusCode TrigEgammaMonitorTopoAlgorithm::initialize() 
{
  ATH_MSG_INFO("TrigEgammaMonitorTopoAlgorithm::initialize()...");
  
  ATH_CHECK(TrigEgammaMonitorBaseAlgorithm::initialize());

  ATH_CHECK(m_offElectronKey.initialize());
  ATH_CHECK(m_offPhotonKey.initialize());

  return StatusCode::SUCCESS;
}

//!=========================================================================

StatusCode TrigEgammaMonitorTopoAlgorithm::fillHistograms( const EventContext& ctx ) const  
{

    ATH_MSG_DEBUG("Executing TrigEgammaMonitorTopoAlgorithm");

    if(isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
        return StatusCode::SUCCESS; 
    }
    
    // Open the offline electron container
    SG::ReadHandle<xAOD::ElectronContainer> offElectrons(m_offElectronKey, ctx);
    SG::ReadHandle<xAOD::PhotonContainer> offPhotons(m_offPhotonKey, ctx);

    if(!offElectrons.isValid())
    {
      ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
      return StatusCode::SUCCESS;
    }

    if(!offPhotons.isValid())
    {
      ATH_MSG_WARNING("Failed to retrieve offline Photons ");
      return StatusCode::SUCCESS;
    }
    

    for ( auto &d : m_trigListConfig)
    {
      std::string trigger_num = d.at("trigger_num");
      std::string trigger_den = d.at("trigger_den");

      auto monGroup_online  = getGroup( trigger_num + "_Efficiency_HLT" );
      auto monGroup_offline = getGroup( trigger_num + "_Efficiency_Offline" );

      std::vector<float> mass_vec, mass_off_vec, match_mass_vec, match_mass_off_vec;
      std::vector<float> dphi_vec, dphi_off_vec, match_dphi_vec, match_dphi_off_vec;

      auto mass_col             = Monitored::Collection( "mass"         , mass_vec            );
      auto mass_off_col         = Monitored::Collection( "mass"         , mass_off_vec        );
      auto match_mass_col       = Monitored::Collection( "match_mass"   , match_mass_vec      );
      auto match_mass_off_col   = Monitored::Collection( "match_mass"   , match_mass_off_vec  );
      auto dphi_col             = Monitored::Collection( "dphi"         , dphi_vec            );
      auto dphi_off_col         = Monitored::Collection( "dphi"         , dphi_off_vec        );
      auto match_dphi_col       = Monitored::Collection( "match_dphi"   , match_dphi_vec      );
      auto match_dphi_off_col   = Monitored::Collection( "match_dphi"   , match_dphi_off_vec  );
      

      std::vector<Legs> legs_den_vec, legs_num_vec;

      make_legs( trigger_num , d.at("leg0_key"), d.at("leg1_key"), legs_num_vec);
      make_legs( trigger_den , d.at("leg0_key"), d.at("leg1_key"), legs_den_vec);

      
      // Fill denominator histograms
      for (auto &legs : legs_den_vec){
        // Fill online mass
        if( legs.leg0 && legs.leg1 ){
          mass_vec.push_back( (legs.leg0->p4() + legs.leg1->p4()).M() );
          dphi_vec.push_back( legs.leg0->p4().DeltaPhi(legs.leg1->p4()) );

          const xAOD::IParticle *leg0_off, *leg1_off =nullptr;
          if( match( *offElectrons, legs.leg0, leg0_off) &&
              match( *offElectrons, legs.leg1, leg1_off))
          {
            if (! (leg0_off == leg1_off)){ // should not be the same electron
              mass_off_vec.push_back(  (leg0_off->p4() + leg1_off->p4()).M() );
              dphi_off_vec.push_back( leg0_off->p4().DeltaPhi(leg1_off->p4()) );
            }
          }
        }
      } // Loop over denominator legs

      
       // Fill denominator histograms
      for (auto &legs : legs_num_vec){

        // Fill online mass
        if( legs.leg0 && legs.leg1 ){
          match_mass_vec.push_back( (legs.leg0->p4() + legs.leg1->p4()).M() );
          match_dphi_vec.push_back( legs.leg0->p4().DeltaPhi(legs.leg1->p4()) );

          const xAOD::IParticle *leg0_off, *leg1_off =nullptr;
          if( match( *offElectrons, legs.leg0, leg0_off) &&
              match( *offElectrons, legs.leg1, leg1_off))
          {
            if (! (leg0_off == leg1_off)){ // should not be the same electron
              match_mass_off_vec.push_back(  (leg0_off->p4() + leg1_off->p4()).M() );
              match_dphi_off_vec.push_back( leg0_off->p4().DeltaPhi(leg1_off->p4()) );
            }
          }
        }

      }// Loop over numerator legs

      fill(monGroup_online, mass_col, match_mass_col, dphi_col, match_dphi_col);
      fill(monGroup_offline, mass_off_col, match_mass_off_col, dphi_off_col, match_dphi_off_col);

    }// Loop over all triggers

    return StatusCode::SUCCESS;
}

//!=========================================================================

bool TrigEgammaMonitorTopoAlgorithm::match( const xAOD::IParticleContainer &container,
                                             const xAOD::IParticle *part_on , 
                                             const xAOD::IParticle *&part_off) const
{
  part_off=nullptr;
  float min_deltaR = 999;
  for (auto part : container){
    float dR = part_on->p4().DeltaR(part->p4());
    if ( dR < min_deltaR ){
      part_off = part;
      min_deltaR = dR;
    }
  }
  return ( (min_deltaR < m_dR) && part_off);
}

//!=========================================================================

void TrigEgammaMonitorTopoAlgorithm::make_legs( std::string trigger, 
                                                 std::string key_leg0,
                                                 std::string key_leg1,
                                                 std::vector<Legs> &legs_vec ) const
{
  // Configure TDT to get each leg. only electrons that fired this triggers (e.g: e26_lhtight_e15_etcut_(Zee))
  // are retrieved (passed by HLT and has at least one Electron online object)
  Trig::FeatureRequestDescriptor frd_leg0;
  frd_leg0.reset();
  frd_leg0.setChainGroup(trigger);
  frd_leg0.setCondition(TrigDefs::Physics); // Only fired trigger
  frd_leg0.setRequireSGKey( TrigEgammaMonitorBaseAlgorithm::match()->key(key_leg0));
  frd_leg0.setRestrictRequestToLeg(0);
  
  Trig::FeatureRequestDescriptor frd_leg1;
  frd_leg1.reset();
  frd_leg1.setChainGroup(trigger);
  frd_leg1.setCondition(TrigDefs::Physics); // Only fired trigger
  frd_leg1.setRequireSGKey( TrigEgammaMonitorBaseAlgorithm::match()->key(key_leg1));
  frd_leg1.setRestrictRequestToLeg(1);

  // Get all combinations given by the L1
  auto vec_leg0=tdt()->features<xAOD::IParticleContainer>(frd_leg0);
  auto vec_leg1=tdt()->features<xAOD::IParticleContainer>(frd_leg1);
  
  ATH_MSG_DEBUG("We have " << vec_leg0.size() << " combinations for leg0 from " << trigger);
  ATH_MSG_DEBUG("We have " << vec_leg1.size() << " combinations for leg1 from " << trigger);

  for ( auto & leg0_feat : vec_leg0){

    for ( auto & leg1_feat : vec_leg1){
      // Check if all links are valid
      if ( !leg0_feat.isValid() || !leg1_feat.isValid() )
        continue;
      //auto leg0 = static_cast<xAOD::Egamma>(*leg0_feat.link);
      //auto leg1 = static_cast<xAOD::Egamma>(*leg1_feat.link);
      auto leg0 = (*leg0_feat.link);
      auto leg1 = (*leg1_feat.link);

      if (leg0 == leg1) continue;

      legs_vec.push_back( Legs{leg0 , leg1} );
    } // Loop over leg 1
  } // Loop over leg 0
}


/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h"

NSubjettinessRatiosTool::NSubjettinessRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("AlphaList", m_rawAlphaVals = {});
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode NSubjettinessRatiosTool::initialize() {

  /// Call base class initialize to fix up m_prefix
  ATH_CHECK(JetSubStructureMomentToolsBase::initialize());

  /// Add alpha = 1.0 by default
  m_moments.emplace( 1.0, moments_t(1.0, m_prefix) );

  /// Clean up input list of alpha values
  for( float alpha : m_rawAlphaVals ) {

    /// Round to the nearest 0.1
    float alphaFix = round( alpha * 10.0 ) / 10.0;
    if( std::abs(alpha-alphaFix) > 1.0e-5 ) ATH_MSG_DEBUG( "alpha = " << alpha << " has been rounded to " << alphaFix );

    /// Skip negative values of alpha
    if( alphaFix < 0.0 ) {
      ATH_MSG_WARNING( "alpha must be positive. Skipping alpha = " << alpha );
      continue;
    }

    /// Store value. std::map::emplace prevents duplicate entries
    m_moments.emplace( alphaFix, moments_t(alphaFix, m_prefix) );

  }

  for( auto const& moment : m_moments ) {
    ATH_MSG_DEBUG( "Including alpha = " << moment.first );
  }

  return StatusCode::SUCCESS;

}

int NSubjettinessRatiosTool::modifyJet(xAOD::Jet &jet) const {

  for( auto const& moment : m_moments ) {

    std::string suffix = moment.second.suffix;

    if( !moment.second.acc_Tau1->isAvailable(jet) ||
        !moment.second.acc_Tau2->isAvailable(jet) ||
        !moment.second.acc_Tau3->isAvailable(jet) ||
        !moment.second.acc_Tau4->isAvailable(jet) ||
        !moment.second.acc_Tau1_wta->isAvailable(jet) ||
        !moment.second.acc_Tau2_wta->isAvailable(jet) ||
        !moment.second.acc_Tau3_wta->isAvailable(jet) ||
        !moment.second.acc_Tau4_wta->isAvailable(jet)) {

      ATH_MSG_ERROR( "Not all " << m_prefix << " Tau decorations with " << suffix << " are available. Exiting." );
      return 1;

    }

    if( m_doDichroic ) {

      if( !moment.second.acc_Tau2_ungroomed->isAvailable(jet) ||
          !moment.second.acc_Tau3_ungroomed->isAvailable(jet) ||
          !moment.second.acc_Tau4_ungroomed->isAvailable(jet) ||
          !moment.second.acc_Tau2_wta_ungroomed->isAvailable(jet) ||
          !moment.second.acc_Tau3_wta_ungroomed->isAvailable(jet) ||
          !moment.second.acc_Tau4_wta_ungroomed->isAvailable(jet)) {

        ATH_MSG_ERROR( "Not all ungroomed " << m_prefix << " Tau decorations with " << suffix << " are available. Exiting." );
        return 1;

      }

    }

    /// Regular NSubjettiness
    float Tau1 = (*moment.second.acc_Tau1)(jet);
    float Tau2 = (*moment.second.acc_Tau2)(jet);
    float Tau3 = (*moment.second.acc_Tau3)(jet);
    float Tau4 = (*moment.second.acc_Tau4)(jet);

    float Tau2_ungroomed = -999.0;
    float Tau3_ungroomed = -999.0;
    float Tau4_ungroomed = -999.0;

    if( m_doDichroic ) {
      Tau2_ungroomed = (*moment.second.acc_Tau2_ungroomed)(jet);
      Tau3_ungroomed = (*moment.second.acc_Tau3_ungroomed)(jet);
      Tau4_ungroomed = (*moment.second.acc_Tau4_ungroomed)(jet);
    }

    float Tau21 = -999.0;
    float Tau32 = -999.0;
    float Tau42 = -999.0;

    float Tau21_dichroic = -999.0;
    float Tau32_dichroic = -999.0;
    float Tau42_dichroic = -999.0;

    /// Prevent div-0 and check against default value (-999) of the decoration
    if( Tau1 > 1e-8 ) {

      Tau21 = Tau2 / Tau1;

      if( Tau2_ungroomed > 1e-8 ) {
        Tau21_dichroic = Tau2_ungroomed/Tau1;
      }

    }

    /// Prevent div-0 and check against default value (-999) of the decoration
    if( Tau2 > 1e-8 ) {

      Tau32 = Tau3 / Tau2;
      Tau42 = Tau4 / Tau2;

      if(Tau3_ungroomed > 1e-8) {
        Tau32_dichroic = Tau3_ungroomed / Tau2;
      }

      if(Tau4_ungroomed > 1e-8) {
        Tau42_dichroic = Tau4_ungroomed / Tau2;
      }

    }

    /// WTA NSubjettiness
    float Tau1_wta = (*moment.second.acc_Tau1_wta)(jet);
    float Tau2_wta = (*moment.second.acc_Tau2_wta)(jet);
    float Tau3_wta = (*moment.second.acc_Tau3_wta)(jet);
    float Tau4_wta = (*moment.second.acc_Tau4_wta)(jet);

    float Tau2_wta_ungroomed = -999.0;
    float Tau3_wta_ungroomed = -999.0;
    float Tau4_wta_ungroomed = -999.0;

    if( m_doDichroic ) {
      Tau2_wta_ungroomed = (*moment.second.acc_Tau2_wta_ungroomed)(jet);
      Tau3_wta_ungroomed = (*moment.second.acc_Tau3_wta_ungroomed)(jet);
      Tau4_wta_ungroomed = (*moment.second.acc_Tau4_wta_ungroomed)(jet);
    }

    float Tau21_wta = -999.0;
    float Tau32_wta = -999.0;
    float Tau42_wta = -999.0;

    float Tau21_wta_dichroic = -999.0;
    float Tau32_wta_dichroic = -999.0;
    float Tau42_wta_dichroic = -999.0;

    /// Prevent div-0 and check against default value (-999) of the decoration
    if( Tau1_wta > 1e-8 ) {

      Tau21_wta = Tau2_wta / Tau1_wta;

      if( Tau2_wta_ungroomed > 1e-8 ) {
        Tau21_wta_dichroic = Tau2_wta_ungroomed / Tau1_wta;
      }

    }

    /// Prevent div-0 and check against default value (-999) of the decoration
    if( Tau2_wta > 1e-8 ) {

      Tau32_wta = Tau3_wta / Tau2_wta;
      Tau42_wta = Tau4_wta / Tau2_wta;

      if( Tau3_wta_ungroomed > 1e-8 ) {
        Tau32_wta_dichroic = Tau3_wta_ungroomed / Tau2_wta;
      }

      if(Tau4_wta_ungroomed > 1e-8) {
        Tau42_wta_dichroic = Tau4_wta_ungroomed / Tau2_wta;
      }

    }

    (*moment.second.dec_Tau21)(jet) = Tau21;
    (*moment.second.dec_Tau32)(jet) = Tau32;
    (*moment.second.dec_Tau42)(jet) = Tau42;

    (*moment.second.dec_Tau21_dichroic)(jet) = Tau21_dichroic;
    (*moment.second.dec_Tau32_dichroic)(jet) = Tau32_dichroic;
    (*moment.second.dec_Tau42_dichroic)(jet) = Tau42_dichroic;

    (*moment.second.dec_Tau21_wta)(jet) = Tau21_wta;
    (*moment.second.dec_Tau32_wta)(jet) = Tau32_wta;
    (*moment.second.dec_Tau42_wta)(jet) = Tau42_wta;

    (*moment.second.dec_Tau21_wta_dichroic)(jet) = Tau21_wta_dichroic;
    (*moment.second.dec_Tau32_wta_dichroic)(jet) = Tau32_wta_dichroic;
    (*moment.second.dec_Tau42_wta_dichroic)(jet) = Tau42_wta_dichroic;

  }

  return 0;

}

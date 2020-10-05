/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h"

EnergyCorrelatorTool::EnergyCorrelatorTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Beta",       m_Beta = 1.0);
  declareProperty("BetaList",   m_rawBetaVals = {});
  declareProperty("DoC3",       m_doC3 = false);
  declareProperty("DoC4",       m_doC4 = false);
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode EnergyCorrelatorTool::initialize() {

  /// Call base class initialize to fix up m_prefix
  ATH_CHECK( JetSubStructureMomentToolsBase::initialize() );

  /// Add beta = 1.0 by default
  m_moments.emplace( 1.0, moments_t(1.0, m_prefix) );

  /// Add beta = m_Beta by default to keep backwards compatibility
  if( std::abs(m_Beta-1.0) > 1.0e-5 ) {

    /// Give warning about deprecation
    ATH_MSG_WARNING( "The Beta property is deprecated, please use the BetaList property to provide a list of values");

    /// Use m_Beta to not break analysis code
    m_moments.emplace( m_Beta, moments_t(m_Beta, m_prefix) );

  }

  /// Clean up input list of beta values
  for( float beta : m_rawBetaVals ) {

    /// Round to the nearest 0.1
    float betaFix = round( beta * 10.0 ) / 10.0;
    if( std::abs(beta-betaFix) > 1.0e-5 ) ATH_MSG_DEBUG( "beta = " << beta << " has been rounded to " << betaFix );

    /// Skip negative values of beta
    if( betaFix < 0.0 ) {
      ATH_MSG_WARNING( "beta must be positive. Skipping beta = " << beta );
      continue;
    }

    /// Store value. std::map::emplace prevents duplicate entries
    m_moments.emplace( betaFix, moments_t(betaFix, m_prefix) );

  }

  /// Print out list of beta values to debug stream
  for( auto const& moment : m_moments ) {
    ATH_MSG_DEBUG( "Including beta = " << moment.first );
  }

  /// If DoC4 is set to true, set DoC3 to true by default since it won't
  /// add any additional computational overhead
  if( m_doC4 ) m_doC3 = true;

  return StatusCode::SUCCESS;

}

int EnergyCorrelatorTool::modifyJet(xAOD::Jet &injet) const {

  fastjet::PseudoJet jet;
  fastjet::PseudoJet jet_ungroomed;

  /// Bool to decide whether calculation should be performed
  bool calculate = SetupDecoration(jet,injet);

  /// Bool to decide if ungroomed jet moments should be calculated
  bool calculate_ungroomed = false;

  if( m_doDichroic ) {

    /// Get parent jet
    ElementLink<xAOD::JetContainer> parentLink = injet.auxdata<ElementLink<xAOD::JetContainer> >("Parent");

    /// Return error is parent element link is broken
    if( !parentLink.isValid() ) {
      ATH_MSG_ERROR( "Parent element link is not valid. Aborting" );
      return 1;
    }

    const xAOD::Jet* parentJet = *(parentLink);
    calculate_ungroomed = SetupDecoration(jet_ungroomed,*parentJet);

  }

  for( auto const& moment : m_moments ) {

    float beta = moment.first;

    float ECF1_value = -999;
    float ECF2_value = -999;
    float ECF3_value = -999;
    float ECF4_value = -999;
    float ECF5_value = -999;

    float ECF1_ungroomed_value = -999;
    float ECF2_ungroomed_value = -999;
    float ECF3_ungroomed_value = -999;

    if( calculate ) {

      JetSubStructureUtils::EnergyCorrelator ECF1(1, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      JetSubStructureUtils::EnergyCorrelator ECF2(2, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      JetSubStructureUtils::EnergyCorrelator ECF3(3, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);

      ECF1_value = ECF1.result(jet);
      ECF2_value = ECF2.result(jet);
      ECF3_value = ECF3.result(jet);

      if( m_doC3 ) {
        JetSubStructureUtils::EnergyCorrelator ECF4(4, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
        ECF4_value = ECF4.result(jet);
      }

      if( m_doC4 ) {
        JetSubStructureUtils::EnergyCorrelator ECF5(5, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
        ECF5_value = ECF5.result(jet);
      }

      if( calculate_ungroomed ) {
        ECF1_ungroomed_value = ECF1.result(jet_ungroomed);
        ECF2_ungroomed_value = ECF2.result(jet_ungroomed);
        ECF3_ungroomed_value = ECF3.result(jet_ungroomed);
      }

    }

    (*moment.second.dec_ECF1)(injet) = ECF1_value;
    (*moment.second.dec_ECF2)(injet) = ECF2_value;
    (*moment.second.dec_ECF3)(injet) = ECF3_value;
    (*moment.second.dec_ECF4)(injet) = ECF4_value;
    (*moment.second.dec_ECF5)(injet) = ECF5_value;

    (*moment.second.dec_ECF1_ungroomed)(injet) = ECF1_ungroomed_value;
    (*moment.second.dec_ECF2_ungroomed)(injet) = ECF2_ungroomed_value;
    (*moment.second.dec_ECF3_ungroomed)(injet) = ECF3_ungroomed_value;

  }

  return 0;

}

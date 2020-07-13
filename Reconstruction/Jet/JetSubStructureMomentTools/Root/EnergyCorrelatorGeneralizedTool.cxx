/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h"
#include "JetSubStructureUtils/EnergyCorrelator.h"

EnergyCorrelatorGeneralizedTool::EnergyCorrelatorGeneralizedTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Beta",       m_Beta = 1.0);
  declareProperty("BetaList",   m_rawBetaVals = {});
  declareProperty("DoM3",       m_doM3 = false);
  declareProperty("DoN3",       m_doN3 = false);
  declareProperty("DoLSeries",  m_doLSeries = false);
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode EnergyCorrelatorGeneralizedTool::initialize() {

  /// Call base class initialize to fix up m_prefix
  ATH_CHECK( JetSubStructureMomentToolsBase::initialize() );

  /// Add beta = 1.0 by default
  m_moments.emplace( 1.0, moments_t(1.0, m_prefix) );

  /// Add beta = m_Beta by default to keep backwards compatibility
  if( std::abs(m_Beta-1.0) > 1.0e-5 ) {

    /// Give warning about deprecation
    ATH_MSG_WARNING( "The Beta property is deprecated, please use the BetaList property to provide a list of values" );

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

  /// Initialize decorators for L-series
  m_dec_ECFG_2_1_2 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"ECFG_2_1_2");
  m_dec_ECFG_3_1_1 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"ECFG_3_1_1");
  m_dec_ECFG_3_2_1 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"ECFG_3_2_1");
  m_dec_ECFG_3_2_2 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"ECFG_3_2_2");
  m_dec_ECFG_3_3_1 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"ECFG_3_3_1");
  m_dec_ECFG_4_2_2 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"ECFG_4_2_2");
  m_dec_ECFG_4_4_1 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"ECFG_4_4_1");

  return StatusCode::SUCCESS;

}

int EnergyCorrelatorGeneralizedTool::modifyJet(xAOD::Jet &injet) const {

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

  /// Loop over all of the moments
  for( auto const& moment : m_moments ) {

    float beta = moment.first;

    /// These are used for M2 and N2
    float ECFG_2_1_value = -999.0;
    float ECFG_3_2_value = -999.0;
    
    /// These are used for dichroic M2 and N2
    float ECFG_2_1_ungroomed_value = -999.0;
    float ECFG_3_1_ungroomed_value = -999.0;
    float ECFG_3_2_ungroomed_value = -999.0;

    /// These are used for M3 and N3
    float ECFG_3_1_value = -999.0;
    float ECFG_4_1_value = -999.0;
    float ECFG_4_2_value = -999.0;

    if( calculate ) {

      /// These are used for N2 and M2
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2(2, 3, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1(1, 2, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);

      ECFG_2_1_value = ECFG_2_1.result(jet);
      ECFG_3_2_value = ECFG_3_2.result(jet);

      /// These are used for dichroic N2 and M2
      if( calculate_ungroomed ) {
        ECFG_2_1_ungroomed_value = ECFG_2_1.result(jet_ungroomed);
        ECFG_3_2_ungroomed_value = ECFG_3_2.result(jet_ungroomed);
      }

      /// These are used for M3 and N3
      if( m_doM3 || m_doN3 ) {

        JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1(1, 3, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);

        ECFG_3_1_value = ECFG_3_1.result(jet);

        if( calculate_ungroomed ) {
          ECFG_3_1_ungroomed_value = ECFG_3_1.result(jet_ungroomed);
        }

        /// This is used for M3
        if( m_doM3 ) {
          JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_1(1, 4, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
          ECFG_4_1_value = ECFG_4_1.result(jet);
        }

        /// This is used for N3
        if( m_doN3 ) {
          JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2(2, 4, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
          ECFG_4_2_value = ECFG_4_2.result(jet);
        }

      }

    }

    (*moment.second.dec_ECFG_2_1)(injet) = ECFG_2_1_value;
    (*moment.second.dec_ECFG_3_1)(injet) = ECFG_3_1_value;
    (*moment.second.dec_ECFG_3_2)(injet) = ECFG_3_2_value;
    (*moment.second.dec_ECFG_4_1)(injet) = ECFG_4_1_value;
    (*moment.second.dec_ECFG_4_2)(injet) = ECFG_4_2_value;

    (*moment.second.dec_ECFG_2_1_ungroomed)(injet) = ECFG_2_1_ungroomed_value;
    (*moment.second.dec_ECFG_3_1_ungroomed)(injet) = ECFG_3_1_ungroomed_value;
    (*moment.second.dec_ECFG_3_2_ungroomed)(injet) = ECFG_3_2_ungroomed_value;

  }

  /// ECFGs for L-series ratios that are for t/H discrimination
  float ECFG_2_1_2_value = -999;
  float ECFG_3_1_1_value = -999;
  float ECFG_3_2_1_value = -999;
  float ECFG_3_2_2_value = -999;
  float ECFG_3_3_1_value = -999;
  float ECFG_4_2_2_value = -999;
  float ECFG_4_4_1_value = -999;

  /// N.B. ECFG_angles_n_beta !!

  if( calculate && m_doLSeries ) {

    /**
     * ------------------------------------------------------
     * Some of the ECFGs for the L-series ratios may already have been calculated
     * depending on which beta values are included. Checks are put in place for
     * each one and if it has already been calculated the value is simply copied.
     * This is meant to prevent duplicating CPU intensive calculations that have
     * already been performed.
     * ------------------------------------------------------
     */

    /// 212
    if( m_moments.count(2.0) ) {
      ECFG_2_1_2_value = (*m_moments.at(2.0).dec_ECFG_2_1)(injet);
    }
    else {
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1_2(1, 2, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_2_1_2_value = ECFG_2_1_2.result(jet);
    }

    /// 311
    if( m_doN3 ) {
      ECFG_3_1_1_value = (*m_moments.at(1.0).dec_ECFG_3_1)(injet);
    }
    else {
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1_1(1, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_1_1_value = ECFG_3_1_1.result(jet);
    }

    /// 321
    ECFG_3_2_1_value = (*m_moments.at(1.0).dec_ECFG_3_2)(injet);

    /// 322
    if( m_moments.count(2.0) ) {
      ECFG_3_2_2_value = (*m_moments.at(2.0).dec_ECFG_3_2)(injet);
    }
    else {
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2_2(2, 3, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_2_2_value = ECFG_3_2_2.result(jet);
    }

    /// 331
    JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_3_1(3, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
    ECFG_3_3_1_value = ECFG_3_3_1.result(jet);

    /// 422
    if( m_doN3 && m_moments.count(2.0) ) {
      ECFG_4_2_2_value = (*m_moments.at(2.0).dec_ECFG_4_2)(injet);
    }
    else {
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2_2(2, 4, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_4_2_2_value = ECFG_4_2_2.result(jet);
    }

    /// 441
    JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_4_1(4, 4, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
    ECFG_4_4_1_value = ECFG_4_4_1.result(jet);
  
  }

  (*m_dec_ECFG_2_1_2)(injet) = ECFG_2_1_2_value;
  (*m_dec_ECFG_3_1_1)(injet) = ECFG_3_1_1_value;
  (*m_dec_ECFG_3_2_1)(injet) = ECFG_3_2_1_value;
  (*m_dec_ECFG_3_2_2)(injet) = ECFG_3_2_2_value;
  (*m_dec_ECFG_3_3_1)(injet) = ECFG_3_3_1_value;
  (*m_dec_ECFG_4_2_2)(injet) = ECFG_4_2_2_value;
  (*m_dec_ECFG_4_4_1)(injet) = ECFG_4_4_1_value;

  return 0;

}

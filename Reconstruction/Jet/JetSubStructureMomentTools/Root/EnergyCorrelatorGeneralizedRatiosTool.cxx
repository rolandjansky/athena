/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h" 

EnergyCorrelatorGeneralizedRatiosTool::EnergyCorrelatorGeneralizedRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("BetaList",   m_rawBetaVals = {});
  declareProperty("DoM3",       m_doM3 = false);
  declareProperty("DoN3",       m_doN3 = false);
  declareProperty("DoLSeries",  m_doLSeries = false);
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode EnergyCorrelatorGeneralizedRatiosTool::initialize() {

  /// Call base class initialize to fix up m_prefix
  ATH_CHECK( JetSubStructureMomentToolsBase::initialize() );

  /// Add beta = 1.0 by default
  m_moments.emplace( 1.0, moments_t(1.0, m_prefix) );

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

  /// Initialize accessors for L-series
  m_acc_ECFG_2_1_2 = std::make_unique< SG::AuxElement::Accessor<float> >(m_prefix+"ECFG_2_1_2");
  m_acc_ECFG_3_1_1 = std::make_unique< SG::AuxElement::Accessor<float> >(m_prefix+"ECFG_3_1_1");
  m_acc_ECFG_3_2_1 = std::make_unique< SG::AuxElement::Accessor<float> >(m_prefix+"ECFG_3_2_1");
  m_acc_ECFG_3_2_2 = std::make_unique< SG::AuxElement::Accessor<float> >(m_prefix+"ECFG_3_2_2");
  m_acc_ECFG_3_3_1 = std::make_unique< SG::AuxElement::Accessor<float> >(m_prefix+"ECFG_3_3_1");
  m_acc_ECFG_4_2_2 = std::make_unique< SG::AuxElement::Accessor<float> >(m_prefix+"ECFG_4_2_2");
  m_acc_ECFG_4_4_1 = std::make_unique< SG::AuxElement::Accessor<float> >(m_prefix+"ECFG_4_4_1");

  /// Initialize decorators for L-series
  m_dec_L1 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"L1");
  m_dec_L2 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"L2");
  m_dec_L3 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"L3");
  m_dec_L4 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"L4");
  m_dec_L5 = std::make_unique< SG::AuxElement::Decorator<float> >(m_prefix+"L5");

  return StatusCode::SUCCESS;

}

int EnergyCorrelatorGeneralizedRatiosTool::modifyJet(xAOD::Jet &jet) const {

  for( auto const& moment : m_moments ) {

    std::string suffix = moment.second.suffix;

    /// Check to make sure all of the necessary ECFG decorations are available
    if( !moment.second.acc_ECFG_2_1->isAvailable(jet) ) {
      ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_2_1" << suffix << "' is not available. Exiting." );
      return 1;
    }

    if( !moment.second.acc_ECFG_3_2->isAvailable(jet) ) {
      ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_3_2" << suffix << "' is not available. Exiting." );
      return 1;
    }

    if( m_doM3 || m_doN3 ) {
      if( !moment.second.acc_ECFG_3_1->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_3_1" << suffix << "' is not available. Exiting." );
        return 1;
      }
    }

    if( m_doM3 ) {
      if( !moment.second.acc_ECFG_4_1->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_4_1" << suffix << "' is not available. Exiting." );
        return 1;
      }
    }

    if( m_doN3 ) {
      if( !moment.second.acc_ECFG_4_2->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_4_2" << suffix << "' is not available. Exiting." );
        return 1;
      }
    }

    if( m_doDichroic ) {
      if( !moment.second.acc_ECFG_2_1_ungroomed->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_2_1_ungroomed" << suffix << "' is not available. Exiting." );
        return 1;
      }

      if( !moment.second.acc_ECFG_3_1_ungroomed->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_3_1_ungroomed" << suffix << "' is not available. Exiting." );
        return 1;
      }

      if( !moment.second.acc_ECFG_3_2_ungroomed->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECFG_3_2_ungroomed" << suffix << "' is not available. Exiting." );
        return 1;
      }
    }

    float ecfg_2_1 = (*moment.second.acc_ECFG_2_1)(jet);
    float ecfg_3_1 = (*moment.second.acc_ECFG_3_1)(jet);
    float ecfg_3_2 = (*moment.second.acc_ECFG_3_2)(jet);
    float ecfg_4_1 = (*moment.second.acc_ECFG_4_2)(jet);
    float ecfg_4_2 = (*moment.second.acc_ECFG_4_2)(jet);
    
    float ecfg_2_1_ungroomed = -999.0;
    float ecfg_3_1_ungroomed = -999.0;
    float ecfg_3_2_ungroomed = -999.0;

    if( m_doDichroic ) {
      ecfg_2_1_ungroomed = (*moment.second.acc_ECFG_2_1_ungroomed)(jet);
      ecfg_3_1_ungroomed = (*moment.second.acc_ECFG_3_1_ungroomed)(jet);
      ecfg_3_2_ungroomed = (*moment.second.acc_ECFG_3_2_ungroomed)(jet);
    }

    float M2 = -999.0;
    float M3 = -999.0;

    float N2 = -999.0;
    float N3 = -999.0;
    
    float M2_dichroic = -999.0;
    float N2_dichroic = -999.0;

    /// M2
    if( ecfg_2_1 > 1e-8 ) /// Prevent div-0
    {
     
      M2 = ecfg_3_1 / ecfg_2_1;

      if( ecfg_3_1_ungroomed > 1e-8 )
      {
        M2_dichroic = ecfg_3_1_ungroomed / ecfg_2_1;
      }
    
    }
    
    /// M3
    if( m_doM3 && ecfg_3_1 > 1e-8 ) /// Prevent div-0
    {
      M3 = ecfg_4_1 / ecfg_3_1;
    }

    /// N2
    if( ecfg_2_1 > 1e-8 ) /// Prevent div-0
    {
      
      N2 = ecfg_3_2 / pow( ecfg_2_1, 2.0 );

      if( ecfg_3_2_ungroomed > 1e-8 && ecfg_2_1_ungroomed > 1e-8 )
      {
        N2_dichroic = ecfg_3_2_ungroomed / ( ecfg_2_1_ungroomed * ecfg_2_1 );
      }

    }

    /// N3
    if( m_doN3 && ecfg_3_1 > 1e-8 ) /// Prevent div-0
    {
      N3 = ecfg_4_2 / pow( ecfg_3_1, 2.0 );
    }

    (*moment.second.dec_M2)(jet) = M2;
    (*moment.second.dec_M3)(jet) = M3;
    
    (*moment.second.dec_N2)(jet) = N2;
    (*moment.second.dec_N3)(jet) = N3;
    
    (*moment.second.dec_M2_dichroic)(jet) = M2_dichroic;
    (*moment.second.dec_N2_dichroic)(jet) = N2_dichroic;

  }

  /**
   * ------------------------------------------------------------------
   * L-series ECFG ratios (experimental for ttH t/H discrimination)
   *
   * The exponents E are determined in order to make the ratios dimensionless
   *
   * E = (a*n) / (b*m)
   * for an ECFG_X_Y_Z, a=Y, n=Z
   *
   * e.g. for L1
   * ecfg_3_3_1 / ecfg_2_1_2
   * E = (3*1) / (1*2) = 3./2.
   * ------------------------------------------------------------------
   */

  float L1 = -999.0;
  float L2 = -999.0;
  float L3 = -999.0;
  float L4 = -999.0;
  float L5 = -999.0;

  if( m_doLSeries ) {

    if( !m_acc_ECFG_2_1_2->isAvailable(jet) ||
        !m_acc_ECFG_3_1_1->isAvailable(jet) ||
        !m_acc_ECFG_3_2_1->isAvailable(jet) ||
        !m_acc_ECFG_3_2_2->isAvailable(jet) ||
        !m_acc_ECFG_3_3_1->isAvailable(jet) ||
        !m_acc_ECFG_4_2_2->isAvailable(jet) ||
        !m_acc_ECFG_4_4_1->isAvailable(jet) ) {
      ATH_MSG_WARNING("L series energy correlation functions with prefix '" << m_prefix << "' are not all available. Exiting..");
      return 1;
    }

    float ecfg_2_1_2 = (*m_acc_ECFG_2_1_2)(jet);
    float ecfg_3_1_1 = (*m_acc_ECFG_3_1_1)(jet);
    float ecfg_3_2_1 = (*m_acc_ECFG_3_2_1)(jet);
    float ecfg_3_2_2 = (*m_acc_ECFG_3_2_2)(jet);
    float ecfg_3_3_1 = (*m_acc_ECFG_3_3_1)(jet);
    float ecfg_4_2_2 = (*m_acc_ECFG_4_2_2)(jet);
    float ecfg_4_4_1 = (*m_acc_ECFG_4_4_1)(jet);

    if( ecfg_2_1_2 > 1e-8 ) /// Prevent div-0
    {
      L1 = ecfg_3_2_1 / pow( ecfg_2_1_2, 1.0 );
      L2 = ecfg_3_3_1 / pow( ecfg_2_1_2, (3.0/2.0) );
    }

    if( ecfg_3_3_1 > 1e-8 ) /// Prevent div-0
    {
      L3 = ecfg_3_1_1 / pow( ecfg_3_3_1, (1.0/3.0) );
      L4 = ecfg_3_2_2 / pow( ecfg_3_3_1, (4.0/3.0) );
    }

    if( ecfg_4_4_1 > 1e-8 ) /// Prevent div-0
    {
      L5 = ecfg_4_2_2 / pow( ecfg_4_4_1, 1.0 );
    }


  }

  (*m_dec_L1)(jet) = L1;
  (*m_dec_L2)(jet) = L2;
  (*m_dec_L3)(jet) = L3;
  (*m_dec_L4)(jet) = L4;
  (*m_dec_L5)(jet) = L5;

  return 0;

}

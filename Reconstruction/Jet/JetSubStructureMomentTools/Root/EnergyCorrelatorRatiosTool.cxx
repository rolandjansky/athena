/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h" 

EnergyCorrelatorRatiosTool::EnergyCorrelatorRatiosTool(std::string name) :
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("BetaList", m_rawBetaVals = {});
  declareProperty("DoC3",  m_doC3 = false);
  declareProperty("DoC4",  m_doC4 = false);
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode EnergyCorrelatorRatiosTool::initialize() {

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

  /// If DoC4 is set to true, set DoC3 to true by default since it won't
  /// add any additional computational overhead
  if( m_doC4 ) m_doC3 = true;

  return StatusCode::SUCCESS;

}

int EnergyCorrelatorRatiosTool::modifyJet(xAOD::Jet &jet) const {
  
  for( auto const& moment : m_moments ) {
 
    std::string suffix = moment.second.suffix;

    /// Check to make sure all of the necessary ECF decorations are available
    if( !moment.second.acc_ECF1->isAvailable(jet) ) {
      ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF1" << suffix << " is not available. Exiting." );
      return 1;
    }

    if( !moment.second.acc_ECF2->isAvailable(jet) ) {
      ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF2" << suffix << " is not available. Exiting." );
      return 1;
    }

    if( !moment.second.acc_ECF3->isAvailable(jet) ) {
      ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF3" << suffix << " is not available. Exiting." );
      return 1;
    }

    if( m_doC3 ) {
      if( !moment.second.acc_ECF4->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF4" << suffix << " is not available. Exiting." );
        return 1;
      }
    }

    if( m_doC4 ) {
      if( !moment.second.acc_ECF5->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF5" << suffix << " is not available. Exiting." );
        return 1;
      }
    }

    if(m_doDichroic) {
      if( !moment.second.acc_ECF1_ungroomed->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF1_ungroomed" << suffix << " is not available. Exiting." );
        return 1;
      }

      if( !moment.second.acc_ECF2_ungroomed->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF2_ungroomed" << suffix << " is not available. Exiting." );
        return 1;
      }

      if( !moment.second.acc_ECF3_ungroomed->isAvailable(jet) ) {
        ATH_MSG_WARNING( "Energy correlation function " << m_prefix << "ECF3_ungroomed" << suffix << " is not available. Exiting." );
        return 1;
      }
    }

    float ecf1 = (*moment.second.acc_ECF1)(jet);
    float ecf2 = (*moment.second.acc_ECF2)(jet);
    float ecf3 = (*moment.second.acc_ECF3)(jet);

    float ecf4 = -999.0;
    if( m_doC3 ) {
      ecf4 = (*moment.second.acc_ECF4)(jet);
    }

    float ecf5 = -999.0;
    if( m_doC4 ) {
      ecf5 = (*moment.second.acc_ECF5)(jet);
    }

    float ecf1_ungroomed = -999.0;
    float ecf2_ungroomed = -999.0;
    float ecf3_ungroomed = -999.0;

    if( m_doDichroic ) {
      ecf1_ungroomed = (*moment.second.acc_ECF1_ungroomed)(jet);
      ecf2_ungroomed = (*moment.second.acc_ECF2_ungroomed)(jet);
      ecf3_ungroomed = (*moment.second.acc_ECF3_ungroomed)(jet);
    }

    float C1 = -999.0;
    float C2 = -999.0;
    float C3 = -999.0;
    float C4 = -999.0;
    
    float D2 = -999.0;
    
    float D2_dichroic = -999.0;

    /// C1
    if( ecf1 > 1e-8 ) /// Prevent div-0
    {
      C1 = ecf2 / pow( ecf1, 2.0 );
    }

    /// C2
    if( ecf2 > 1e-8 ) /// Prevent div-0
    {
      C2 = ecf3 * ecf1 / pow( ecf2, 2.0 );
    }

    /// C3
    if( m_doC3 && ecf3 > 1e-8 ) /// Prevent div-0
    {
      C3 = ecf4 * ecf2 / pow( ecf3, 2.0 );
    }

    /// C4
    if( m_doC4 && ecf4 > 1e-8 ) /// Prevent div-0
    {
      C4 = ecf5 * ecf3 / pow( ecf4, 2.0 );
    }

    /// D2
    if( ecf2 > 1e-8 ) /// Prevent div-0
    {

      D2 = ecf3 * pow( ecf1, 3.0 ) / pow( ecf2, 3.0 );

      if( ecf2_ungroomed > 1e-8 && ecf3_ungroomed > 1e-8 )
      {
        D2_dichroic = ecf3_ungroomed * ecf1_ungroomed * pow( ecf1, 2.0 ) / ( pow( ecf2_ungroomed, 2.0 ) * ecf2 );
      }

    }

    (*moment.second.dec_C1)(jet) = C1;
    (*moment.second.dec_C2)(jet) = C2;
    (*moment.second.dec_C3)(jet) = C3;
    (*moment.second.dec_C4)(jet) = C4;
    
    (*moment.second.dec_D2)(jet) = D2;

    (*moment.second.dec_D2_dichroic)(jet) = D2_dichroic;

  }

  return 0;

}

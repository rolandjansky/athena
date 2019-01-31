/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCorrections/PileupScaleFactorCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEventInfo/EventInfo.h"

namespace top{

  PileupScaleFactorCalculator::PileupScaleFactorCalculator( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),
    m_pileupReweightingTool("CP::PileupReweightingTool"),
    m_systNominal( CP::SystematicSet() ),
    m_systDataSFUp( CP::SystematicSet() ),
    m_systDataSFDown( CP::SystematicSet() )
  {
    declareProperty( "config" , m_config );
  }

  StatusCode PileupScaleFactorCalculator::initialize()
  {
    ATH_MSG_INFO(" top::PileupScaleFactorCalculator initialize" );

    top::check( m_pileupReweightingTool.retrieve(), "Failed to retireve pileup reweighting tool" );

    m_systDataSFUp.insert( CP::SystematicVariation("PRW_DATASF", 1) );
    m_systDataSFDown.insert( CP::SystematicVariation("PRW_DATASF", -1) );

    m_mu_dependent_PRW = m_config->PileupMuDependent();

    return StatusCode::SUCCESS;
  }


  StatusCode PileupScaleFactorCalculator::execute()
  {

    // get the event info
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()),
	       "Failed to retrieve EventInfo");

    top::check( m_pileupReweightingTool->applySystematicVariation( m_systNominal ),
		"Failed to set pileup reweighting to nominal value" );

    top::check(m_pileupReweightingTool->apply(*eventInfo, m_mu_dependent_PRW),
	       "Failed to apply pileup weight");

    // Apply correction to actual mu for data
    if (!m_config->isMC()) {
      float actualMu = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing( *eventInfo );
      eventInfo->auxdecor<float>("corrected_actualInteractionsPerCrossing") = actualMu;
    }

    // Get hash value which can be used later for reweighting
    if (m_config->isMC()) {
      unsigned long long prw_hash = m_pileupReweightingTool->getPRWHash(*eventInfo);
      eventInfo->auxdecor<unsigned long long>("PileupWeight_Hash") = prw_hash;
    }
    
    // Now get the up and down pileup variations
    // We want the SF for MC and the mu value for data

    // Switch tool to DataSFUp
    top::check( m_pileupReweightingTool->applySystematicVariation( m_systDataSFUp ),
		"Failed to switch pileup reweighting tool to data SF up" );
    
    if( m_config->isMC() ){
      // The apply method above has pileupWeight as
      // a double (although the function returns a float)
      // so here we do the same for consistency
      float pileupWeight = m_pileupReweightingTool->getCombinedWeight( *eventInfo );
      eventInfo->auxdecor<float>("PileupWeight_UP") = pileupWeight;
    }
    else {
      float lumiBlockMu{1};
      if (m_config->isAFII() && m_config->PileupActualMu_AF().size() > 0) {
        lumiBlockMu = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing( *eventInfo );
      } else if (!m_config->isAFII() && m_config->PileupActualMu_FS().size() > 0) {
        lumiBlockMu = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing( *eventInfo );
      } else {
        lumiBlockMu = m_pileupReweightingTool->getCorrectedAverageInteractionsPerCrossing( *eventInfo );
      }
      eventInfo->auxdecor<float>("corrected_averageInteractionsPerCrossing_UP") = lumiBlockMu;
    }
    
    // Switch tool to DataSFDown
    top::check( m_pileupReweightingTool->applySystematicVariation( m_systDataSFDown ),
		"Failed to switch pileup reweighting tool to data SF down" );

    if( m_config->isMC() ){
      float pileupWeight = m_pileupReweightingTool->getCombinedWeight( *eventInfo );
      eventInfo->auxdecor<float>("PileupWeight_DOWN") = pileupWeight;
    }
    else {
      float lumiBlockMu{1};
      if (m_config->isAFII() && m_config->PileupActualMu_AF().size() > 0) {
        lumiBlockMu = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing( *eventInfo );
      } else if (!m_config->isAFII() && m_config->PileupActualMu_FS().size() > 0) {
        lumiBlockMu = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing( *eventInfo );
      } else {
        lumiBlockMu = m_pileupReweightingTool->getCorrectedAverageInteractionsPerCrossing( *eventInfo );
      }
      eventInfo->auxdecor<float>("corrected_averageInteractionsPerCrossing_DOWN") = lumiBlockMu;
    }
    
    // Switch tool to back to nominal to be extra careful...
    top::check( m_pileupReweightingTool->applySystematicVariation( m_systNominal ),
		"Failed to switch pileup reweighting tool back to nominal" );

    return StatusCode::SUCCESS;
  }

}

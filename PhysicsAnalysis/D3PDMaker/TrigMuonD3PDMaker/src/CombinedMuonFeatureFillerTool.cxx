/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombinedMuonFeatureFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "CombinedMuonFeatureFillerTool.h"

namespace D3PD {

   CombinedMuonFeatureFillerTool::CombinedMuonFeatureFillerTool( const std::string& type,
                                                                 const std::string& name,
                                                                 const IInterface* parent )
      : BlockFillerTool< CombinedMuonFeature >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode CombinedMuonFeatureFillerTool::book() {

      CHECK( addVariable( "sigma_pt", m_sigma_pt,
                          "Sigma pT in MeV from the packed word" ) );
      CHECK( addVariable( "comb_strategy", m_comb_strategy,
                          "Combination strategy identifier" ) );
      CHECK( addVariable( "comb_errorFlag", m_comb_errorFlag,
                          "Description of error(s) happening during the combination?" ) );
      CHECK( addVariable( "comb_matchFlag", m_comb_matchFlag,
                          "Additional information about the combination?" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode CombinedMuonFeatureFillerTool::fill( const CombinedMuonFeature& muon ) {

      *m_sigma_pt       = static_cast< float >( muon.sigma_pt() );
      *m_comb_strategy  = muon.comb_strategy();
      *m_comb_errorFlag = muon.comb_errorFlag();
      *m_comb_matchFlag = muon.comb_matchFlag();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD

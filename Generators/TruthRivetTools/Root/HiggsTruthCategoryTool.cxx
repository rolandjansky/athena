/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TruthRivetTools/HiggsTruthCategoryTool.h"

namespace {

   template< class T >
   TLorentzVector makeTLV( const T& p4 ) {

      return TLorentzVector( p4.px(), p4.py(), p4.pz(), p4.E() );
   }

   template< class T >
   std::vector< TLorentzVector > makeTLVs( const T& p4vec ) {

      std::vector< TLorentzVector > result;
      for( const auto& p4 : p4vec ) {
         result.push_back( makeTLV( p4 ) );
      }
      return result;
   }

   HTXS::HiggsClassification
   rivet2Root( const Rivet::HiggsClassification& cat ) {

      HTXS::HiggsClassification ret;

      ret.prodMode = cat.prodMode;
      ret.errorCode = cat.errorCode;
      ret.higgs = makeTLV( cat.higgs );
      ret.V = makeTLV( cat.V );
      ret.p4decay_higgs = makeTLV( cat.p4decay_higgs );
      ret.p4decay_V = makeTLV( cat.p4decay_V );
      ret.jets25 = makeTLVs( cat.jets25 );
      ret.jets30 = makeTLVs( cat.jets30 );
      ret.stage0_cat = cat.stage0_cat;
      ret.stage1_cat_pTjet25GeV = cat.stage1_cat_pTjet25GeV;
      ret.stage1_cat_pTjet30GeV = cat.stage1_cat_pTjet30GeV;

      return ret;
    }

}

HiggsTruthCategoryTool::HiggsTruthCategoryTool( const std::string& name) 
  : asg::AsgTool( name ),
    m_isInitialized( false ),
    m_anaHandler(),
    m_higgsTCS() {

   // cannot be set to true until the issue with the beam protons in the truth
   // event record is fixed..
   // see JIRA ticket: https://its.cern.ch/jira/browse/ATLASRECTS-3072?filter=-2
   declareProperty( "OutputHistos", m_outHistos = false );
   declareProperty( "ProductionMode", m_prodMode = HTXS::UNKNOWN );
}


StatusCode HiggsTruthCategoryTool::initialize() {

   // Greet the user:
   ATH_MSG_INFO( "Initializing ..." );

   // Set up the helper objects:
   const HTXS::HiggsProdMode prodMode =
      static_cast< HTXS::HiggsProdMode >( m_prodMode );
   m_higgsTCS.setHiggsProdMode( prodMode );
   m_anaHandler.addAnalysis( &m_higgsTCS );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

StatusCode HiggsTruthCategoryTool :: finalize () {

   // Finalise the helper object(s):
   if( m_outHistos ) {
      m_anaHandler.finalize();
      m_anaHandler.writeData( "HiggsTruthCategoryTool.yoda" );
   } else {
      m_higgsTCS.finalize();
   }

   // Return gracefully:
   return StatusCode::SUCCESS;  
}

HTXS::HiggsClassification
HiggsTruthCategoryTool::getHiggsTruthCategoryObject( const HepMC::GenEvent& ev ) {

  if( ! m_isInitialized ) {
     m_anaHandler.init( ev ); 
     m_isInitialized = true;
  }

  // fill histos if flag is specified
  if( m_outHistos ) {
     m_anaHandler.analyze( ev );
  }

  // get the category output object containing the template cross section
  // category, and Higgs, V-boson, jets 4-vectors
  const HTXS::HiggsProdMode prodMode =
     static_cast< HTXS::HiggsProdMode >( m_prodMode );
  const Rivet::HiggsClassification htxs_cat_rivet =
     m_higgsTCS.classifyEvent( ev, prodMode );
  return rivet2Root( htxs_cat_rivet );
}

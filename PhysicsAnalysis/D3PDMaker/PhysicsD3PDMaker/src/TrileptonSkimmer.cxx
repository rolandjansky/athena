/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
 *   TrileptonSkimmer.cxx
 *
 *   authors: Steven Kaneti <Steven.KanetiSPAMNOT.cern.ch>
 *            Nick Edwards  
 * 
 *   version: $Id: TrileptonSkimmer.cxx 606012 2014-07-11 09:40:36Z krasznaa $
 *
 *   This is a C++ athena-based algorithm for a three lepton skim to be used
 *   by the Standard Model ZZ->4l and Standard Model WZ groups.
 *
 *   
 ******************************************************************************/

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "TrileptonSkimmer.h"

TrileptonSkimmer::TrileptonSkimmer( const std::string& name, 
                                    ISvcLocator* pSvcLocator )
   : AthFilterAlgorithm( name, pSvcLocator ) {

   // Property declaration 
   declareProperty("lowPtCut"             ,    m_lowPtCut         = 4.e3  );
   declareProperty("highPtCut"            ,    m_highPtCut        = 20.e3 );
   declareProperty("fwdElPtCut"           ,    m_fwdElPtCut       = 15.e3 );
   declareProperty("caloMuonPtCut"        ,    m_caloMuPtCut      = 12.e3 );
   declareProperty("allowMultiLepton"     ,    m_allowMultiLepton = true );
   declareProperty("inputCollectionStaco" ,    m_inCollKey_muons  = "Muons",
                   "StoreGate key of the input container/collection for Muons");
   declareProperty("inputCollectionElecs" ,    m_inCollKey_elecs  =
                   "ElectronAODCollection",
                   "StoreGate key of the input container/collection for "
                   "Standard Electrons");
}

StatusCode TrileptonSkimmer::initialize() {

   ATH_MSG_INFO( "Initializing " << name() << "...");
   ATH_MSG_DEBUG( "lowPtCut    = " << m_lowPtCut );
   ATH_MSG_DEBUG( "highPtCut   = " << m_highPtCut );
   ATH_MSG_DEBUG( "caloMuPtCut = " << m_caloMuPtCut );
   ATH_MSG_DEBUG( "fwdElPtCut  = " << m_fwdElPtCut );

   return StatusCode::SUCCESS;
}

StatusCode TrileptonSkimmer::execute() {

   ATH_MSG_DEBUG ("Executing " << name() << "...");

   // Retrieve the muons:
   const xAOD::MuonContainer* muons = 0;
   ATH_CHECK( evtStore()->retrieve( muons, m_inCollKey_muons ) );

   // Retrieve the electrons:
   const xAOD::ElectronContainer* electrons = 0;
   ATH_CHECK( evtStore()->retrieve( electrons, m_inCollKey_elecs ) );

   //---------------------------------------------------------------------------
   // Loop over Muons
   //
   // We do 1 loop and count the following

   /// Combined muon with |eta|<2.5 and pT > m_highPtCut
   int nHighPtCentralMuons = 0;
   /// Combined/ST/SA muon with |eta|<2.5 and pT > m_lowPtCut
   int nLowPtCentralMuons = 0;
   /// Combined/ST/SA muon and pT > m_lowPtCut
   int nLowPtMuons = 0;

   xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
   xAOD::MuonContainer::const_iterator mu_end = muons->end();
   for( ; mu_itr != mu_end; ++mu_itr ) {

      if( ( ( *mu_itr )->pt() > m_highPtCut ) &&
          ( ( *mu_itr )->muonType() == xAOD::Muon::Combined ) &&
          ( fabs( ( *mu_itr )->eta() ) < 2.5 ) ) {
         ++nHighPtCentralMuons;
      }
      if( ( ( *mu_itr )->pt() > m_lowPtCut ) &&
          ( ( ( *mu_itr )->muonType() == xAOD::Muon::Combined ) ||
            ( ( *mu_itr )->muonType() == xAOD::Muon::SegmentTagged ) ||
            ( ( *mu_itr )->muonType() == xAOD::Muon::MuonStandAlone ) ) ) {

         if( fabs( ( *mu_itr )->eta() ) < 2.5 ) {
            ++nLowPtCentralMuons;
         } 
         nLowPtMuons++;
      }
   }

   //---------------------------------------------------------------------------
   // Loop over electrons
   //
   // We do 1 loop and count the following

   /// author 1 || 3, Medium++, et>highPtCut, no specific eta cut,
   /// but is implicit in author
   int nHighPtCentralElectrons = 0;
   /// author 1 || 3, Loose++, et>lowPtCut, no specific eta cut, but is
   /// implicit in author
   int nLowPtCentralElectronsLoosePP = 0;
   /// author 1 || 3, et>lowPtCut, no specific eta cut, but is implicit in
   /// author
   int nLowPtCentralElectrons = 0;
   /// author 8, et>fwdElpTCut, no specific eta cut, but is implicit in author
   int nFwdElectrons = 0;

   xAOD::ElectronContainer::const_iterator el_itr = electrons->begin();
   xAOD::ElectronContainer::const_iterator el_end = electrons->end();
   for( ; el_itr != el_end; ++el_itr ) {

      if( ( ( *el_itr )->author() == 1 ) ||
          ( ( *el_itr )->author() == 3 ) ) {

         bool medium = false;
         ( *el_itr )->passSelection( medium, "Medium" );
         bool loose = false;
         ( *el_itr )->passSelection( loose, "Loose" );

         if( ( ( *el_itr )->pt() > m_highPtCut ) && medium ) {
            ++nHighPtCentralElectrons;
         }
         if( ( *el_itr )->pt() > m_lowPtCut ) {
            nLowPtCentralElectrons++;
            if( loose ) {
               ++nLowPtCentralElectronsLoosePP;
            }
         }
      }
   }

   //
   // The forward electron cut should be implemented. That needs a separate
   // container from StoreGate though...
   //

   //
   // Also not sure about the calorimeter muons...
   //
   int nCaloMuons = 0;

   // --------------------------------------------------------------------------
   // Decision logic
   if( ( nHighPtCentralMuons + nHighPtCentralElectrons >= 1 ) &&
       ( nLowPtCentralMuons + nLowPtCentralElectronsLoosePP >= 2 ) &&
       ( nLowPtMuons + nLowPtCentralElectrons + nFwdElectrons +
         nCaloMuons >= 3 ) ) {

      setFilterPassed( true );
   } else {
      setFilterPassed( false );
   }
   // --------------------------------------------------------------------------

   // Return gracefully:
   return StatusCode::SUCCESS;
}

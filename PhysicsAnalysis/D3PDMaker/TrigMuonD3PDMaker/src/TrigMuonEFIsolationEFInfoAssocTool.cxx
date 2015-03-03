/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationEFInfoAssocTool.cxx 499444 2012-05-07 12:10:20Z krasznaa $

// STL include(s):
#include <algorithm>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

// Local include(s):
#include "TrigMuonEFIsolationEFInfoAssocTool.h"

namespace D3PD {

   TrigMuonEFIsolationEFInfoAssocTool::
   TrigMuonEFIsolationEFInfoAssocTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : SingleAssociationTool< TrigMuonEFIsolation,
                               TrigMuonEFInfo >( type, name, parent ),
        m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ) {

      declareProperty( "UseTrigNav", m_useTrigNav = false,
                       "Use the trigger navigation to find the correct "
                       "TrigMuonEFInfo object" );
      declareProperty( "TrigDecTool", m_trigDec,
                       "TrigDecisionTool handle" );
   }

   StatusCode TrigMuonEFIsolationEFInfoAssocTool::initialize() {

      // Retrieve the TrigDecisionTool if we'll be using it later on:
      if( m_useTrigNav ) {
         ATH_MSG_INFO( "Will get the object pointer from the trigger navigation" );
         CHECK( m_trigDec.retrieve() );
         m_trigDec->ExperimentalAndExpertMethods()->enable();
      } else {
         ATH_MSG_INFO( "Will get the object pointer from the TrigMuonEFIsolation "
                       "ElementLink" );
      }

      return SingleAssociationTool< TrigMuonEFIsolation,
                                    TrigMuonEFInfo >::initialize();
   }

   const TrigMuonEFInfo*
   TrigMuonEFIsolationEFInfoAssocTool::get( const TrigMuonEFIsolation& muon ) {

      // If the navigation shouldn't be used, just do the simplest thing
      // possible:
      if( ! m_useTrigNav ) {
         return muon.getMuonInfo();
      }

      // Access the navigation. We have to do this ugly const-cast, because
      // some of the functions we need to use are non-const.
      HLT::NavigationCore* nav =
         const_cast< HLT::NavigationCore* >( m_trigDec->
                                             ExperimentalAndExpertMethods()->
                                             getNavigation() );

      // Retrieve all the TEs. Unfortunately this results in a *lot* of TEs.
      // But to stay as general as possible, we have to do this. Just asking
      // TrigDecisionTool for all the TrigMuonEFIsolation objects actually
      // misses a few objects unfortunately.
      std::vector< HLT::TriggerElement* > TEs;
      nav->getAll( TEs, false );

      // Search for this muon amongst the TEs:
      HLT::TriggerElement* TE = 0;
      std::vector< HLT::TriggerElement* >::const_iterator te_itr = TEs.begin();
      std::vector< HLT::TriggerElement* >::const_iterator te_end = TEs.end();
      for( ; te_itr != te_end; ++te_itr ) {
         // Retrieve all EF isolation objects from this TE:
         std::vector< const TrigMuonEFIsolationContainer* > muons;
         if( ! nav->getFeatures( *te_itr, muons ) ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Couldn't retrieve TrigMuonEFIsolation objects from a TE";
            return 0;
         }
         // Loop over the returned containers:
         std::vector< const TrigMuonEFIsolationContainer* >::const_iterator
            cont_itr = muons.begin();
         std::vector< const TrigMuonEFIsolationContainer* >::const_iterator
            cont_end = muons.end();
         for( ; cont_itr != cont_end; ++cont_itr ) {
            // Check if the muon is in this container:
            if( std::find( ( *cont_itr )->begin(), ( *cont_itr )->end(),
                           &muon ) != ( *cont_itr )->end() ) {
               TE = *te_itr;
               break;
            }
         }
         // Stop the loop if we already found the right TE:
         if( TE ) break;
      }

      // Check that we were successful:
      if( ! TE ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "No TriggerElement found with the received TrigMuonEFIsolation "
            << "object attached to it";
         return 0;
      }

      // Find the TrigMuonEFInfoContainer that has our object of interest:
      Trig::Feature< TrigMuonEFInfoContainer > infoCont =
         m_trigDec->ancestor< TrigMuonEFInfoContainer >( TE );

      // Isolation objects should only point at containers returned by the TDT,
      // which have exactly one element. (Even if not, they should have *at least*
      // one element.)
      if( ! infoCont.cptr()->size() ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "Empty TrigMuonEFInfoContainer found";
         return 0;
      }
      if( infoCont.cptr()->size() > 1 ) {
         REPORT_MESSAGE( MSG::WARNING )
            << "More than one element found in TrigMuonEFInfoContainer. "
            << "Returning the first one.";
      }

      // Return the object:
      return infoCont.cptr()->front();
   }

} // namespace D3PD

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventInfoSelectorTool.cxx
 *  @brief This file contains the implementation for the EventInfoSelectorTool class.
 *  $Id: EventInfoSelectorTool.cxx,v 1.5 2009-03-17 09:44:46 cranshaw Exp $
 **/

#include "EventInfoSelectorTool.h"
#include "EventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

namespace xAODMaker {

   //___________________________________________________________________________
   EventInfoSelectorTool::EventInfoSelectorTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  	   EventInfoCnvTool(type, name, parent) {
      declareInterface<IAthenaSelectorTool>(this);
      declareProperty( "AODKey", m_aodKey = "" );
      declareProperty( "xAODKey", m_xaodKey = "EventInfo" );
   }

   //___________________________________________________________________________
   EventInfoSelectorTool::~EventInfoSelectorTool() {
   }

   //__________________________________________________________________________
   StatusCode EventInfoSelectorTool::postNext() const {
      ATH_MSG_DEBUG("EventInfoSelectorTool::postNext");

      // Check if anything needs to be done:
      if( evtStore()->contains< xAOD::EventInfo >( m_xaodKey ) ) { 
         ATH_MSG_DEBUG( "xAOD::EventInfo with key \"" << m_xaodKey
                        << "\" is already in StoreGate" );
         return StatusCode::SUCCESS;
      }

      const DataHandle<EventInfo> aod;
      if( m_aodKey.empty() ) {
         CHECK( evtStore()->retrieve( aod ) );
      } else {
         CHECK( evtStore()->retrieve( aod, m_aodKey ) );
      }

      // Create the xAOD object(s):
      xAOD::EventInfo* xaod = new xAOD::EventInfo();
      CHECK( evtStore()->record( xaod, m_xaodKey ) );
      
      xAOD::EventAuxInfo* aux = new xAOD::EventAuxInfo();
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      
      xaod->setStore( aux );

      // Do the translation:
      CHECK( const_cast<EventInfoSelectorTool*>(this)->convert( aod, xaod ) );
      return StatusCode::SUCCESS;
   }
   //__________________________________________________________________________
   StatusCode EventInfoSelectorTool::preNext() const {
      return(StatusCode::SUCCESS);
   }
   //__________________________________________________________________________
}

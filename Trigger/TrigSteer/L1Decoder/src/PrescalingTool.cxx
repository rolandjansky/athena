/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "PrescalingTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"

const std::function< CLHEP::HepRandomEngine*(void) > PSTRanluxFactory = [](void)->CLHEP::HepRandomEngine*{
   return new CLHEP::Ranlux64Engine();
};

PrescalingTool::PrescalingTool( const std::string& type, 
                                const std::string& name, 
                                const IInterface* parent ) :
   base_class(type, name, parent),
   m_RNGEngines( PSTRanluxFactory, SG::getNSlots() )
{}

PrescalingTool::~PrescalingTool()
{}

StatusCode
PrescalingTool::initialize()
{
   CHECK(m_hltPrescaleSetInputKey.initialize( ! m_hltPrescaleSetInputKey.key().empty() ));
   return StatusCode::SUCCESS;
}

StatusCode PrescalingTool::prescaleChains( const EventContext& ctx,  
                                           const HLT::IDVec& initiallyActive,
                                           HLT::IDVec& remainActive ) const
{
   if ( initiallyActive.empty() )
      return StatusCode::SUCCESS;

   // clear the output just in case
   remainActive.clear();

   if ( m_hltPrescaleSetInputKey.key().empty() ) {
      // if no prescaling key is configured, treat all prescales according to propery KeepUnknownChains
      if( m_keepUnknownChains ) {
         remainActive.reserve( initiallyActive.size() );
         for( const auto & ch : initiallyActive ) {
            remainActive.push_back(ch);
         }
      }
      return StatusCode::SUCCESS;
   }

   SG::ReadCondHandle<TrigConf::HLTPrescalesSet> hltpssRH(m_hltPrescaleSetInputKey, ctx);
   const TrigConf::HLTPrescalesSet* hltPrescaleSet{*hltpssRH};
   if(hltPrescaleSet == nullptr) {
      ATH_MSG_ERROR("Failed to retrieve HLTPrescalesSet " << hltPrescaleSet->name());
      return StatusCode::FAILURE;
   }

   // access to psk
   ATH_MSG_DEBUG("Using HLT PSK " << hltPrescaleSet->psk());

   // prepare the result
   remainActive.reserve( initiallyActive.size() );

   // create the seed from the event time
   size_t seed = ctx.eventID().time_stamp() ^ ctx.eventID().time_stamp_ns_offset();
   CLHEP::HepRandomEngine* engine = m_RNGEngines.getEngine( ctx );
   engine->setSeed( seed, 0 );

   // go through all active chains
   for ( const auto & ch: initiallyActive ) {
      bool decisionToKeep { false };
      try {
         const auto & prescale = hltPrescaleSet->prescale( ch.numeric() );
         if( prescale.enabled ) {
            decisionToKeep = engine->flat() < 1./prescale.prescale;
            ATH_MSG_DEBUG("Prescaling decision for chain " << ch << " " << decisionToKeep );
         } else {
            decisionToKeep = false;
            ATH_MSG_DEBUG("Chain " << ch << " is disabled, won't keep" );
         }
      }
      catch(std::out_of_range & ex) {
         // if chain with that name is not found in the prescale set
         decisionToKeep = m_keepUnknownChains.value();
         ATH_MSG_DEBUG("No prescale value for chain " << ch << ", will " << (decisionToKeep ? "" : "not ") << "keep it.");
      }
      // keep all active chains according to prescaling result
      if ( decisionToKeep ) {
         remainActive.push_back( ch );
      }
   }
   return StatusCode::SUCCESS;
}



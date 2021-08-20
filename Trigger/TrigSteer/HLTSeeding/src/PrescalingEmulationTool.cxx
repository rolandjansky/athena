/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "PrescalingEmulationTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"


const std::function< CLHEP::HepRandomEngine*(void) > RanluxFactory = []()->CLHEP::HepRandomEngine*{
  return new CLHEP::Ranlux64Engine();
};


PrescalingEmulationTool::PrescalingEmulationTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : base_class(type, name, parent),
    m_RNGEngines( RanluxFactory, SG::getNSlots() ) { }


StatusCode PrescalingEmulationTool::initialize() {
  for ( const std::string& confElement: m_prescalingConfig ) {
    std::string chainName( confElement, 0, confElement.find(':') );
    std::string psValue( confElement, confElement.find(':')+1 );
    ATH_MSG_DEBUG( "Decoded part of the config " << chainName << " " << psValue );
    m_prescalingInfo[HLT::Identifier( chainName ) ] = 1./std::stof( psValue );
  }
  return StatusCode::SUCCESS;
}


StatusCode PrescalingEmulationTool::prescaleChains( const EventContext& ctx,
                                                    const HLT::IDVec& initiallyActive,
                                                    HLT::IDVec& remainActive ) const {
  if ( initiallyActive.empty() ) {
    return StatusCode::SUCCESS;
  }

  // obtain CTP time
  remainActive.reserve( initiallyActive.size() );

  // create the seed from the event time
  size_t seed = ctx.eventID().time_stamp() ^ ctx.eventID().time_stamp_ns_offset();
  CLHEP::HepRandomEngine* engine = m_RNGEngines.getEngine( ctx );
  engine->setSeed( seed, 0 );
  for ( auto ch: initiallyActive ) {
    PrescalingInfo::const_iterator chainPS = m_prescalingInfo.find( ch );
    const bool decision = (chainPS != m_prescalingInfo.end()) ?
                          engine->flat() < chainPS->second :
                          bool(m_keepUnknownChains);
    ATH_MSG_DEBUG("Prescaling decision for chain " << ch << " " << decision );
    if ( decision ) {
      remainActive.push_back( ch );
    }
  }
  return StatusCode::SUCCESS;
}

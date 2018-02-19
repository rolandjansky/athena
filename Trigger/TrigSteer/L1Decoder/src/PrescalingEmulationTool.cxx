/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "PrescalingEmulationTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CLHEP/Random/Ranlux64Engine.h"

std::function< CLHEP::HepRandomEngine*(void) > RanluxFactory = [](void)->CLHEP::HepRandomEngine*{
  return new CLHEP::Ranlux64Engine();
};


PrescalingEmulationTool::PrescalingEmulationTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) 
  : base_class(type, name, parent),
    m_RNGEngines( RanluxFactory, SG::getNSlots() ) { }

PrescalingEmulationTool::~PrescalingEmulationTool() { }

StatusCode PrescalingEmulationTool::initialize() {
  
  CHECK( m_eventInfo.initialize( not m_eventInfo.key().empty() ) );

  for ( const std::string& confElement: m_prescalingConfig ) {
    std::string chainName( confElement, 0, confElement.find(':') );
    std::string psValue( confElement, confElement.find(':')+1 );
    ATH_MSG_DEBUG( "Decoded part of the config " << chainName << " " << psValue );
    m_prescalingInfo[HLT::Identifier( chainName ) ] = 1./std::stof( psValue );
  }
  return StatusCode::SUCCESS;
}

StatusCode PrescalingEmulationTool::prescaleChains( const EventContext& ctx,  
						    const HLT::IDVec& initialyActive,
						    HLT::IDVec& remainActive ) const {
  if ( initialyActive.empty() ) 
    return StatusCode::SUCCESS;

  // obtain CTP time
  remainActive.reserve( initialyActive.size() );
  size_t seed =  initialyActive[0].numeric();


  if ( not m_eventInfo.key().empty() ) {
    auto handle = SG::makeHandle( m_eventInfo, ctx );
    const xAOD::EventInfo* event = handle.cptr();    
    // not sure we should mimick something we had before, will not be abel to reproduce it exactly anyways
    seed = event->timeStamp() ^ event->timeStampNSOffset();
  }
  CLHEP::HepRandomEngine* engine = m_RNGEngines.getEngine( ctx );
  engine->setSeed( seed, 0 );
  for ( auto ch: initialyActive ) {
    PrescalingInfo::const_iterator chainPS = m_prescalingInfo.find( ch );
    const bool decision = ( chainPS != m_prescalingInfo.end() ) ? engine->flat() < chainPS->second : bool(m_keepUnknownChains);
    ATH_MSG_DEBUG("Prescaling decision for chain " << ch << " " << decision );
    if ( decision ) 
      remainActive.push_back( ch );
  }
  return StatusCode::SUCCESS;
}


StatusCode PrescalingEmulationTool::finalize()
{
  return StatusCode::SUCCESS;
}



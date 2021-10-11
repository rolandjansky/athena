/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// Local includes:
#include "RingerSelectorTools/AsgRingerSelectorTool.h"
#include "RingerSelectorTools/tools/onnx/RingerSelector.h"


namespace Ringer {



//==============================================================================
// Standard constructor
//==============================================================================
AsgRingerSelectorTool::AsgRingerSelectorTool(std::string name) :
  AsgTool( name ),
  m_selector( name )
{}

AsgRingerSelectorTool::~AsgRingerSelectorTool()
{}



//==============================================================================
// Athena initialize method
//==============================================================================
StatusCode AsgRingerSelectorTool::initialize()
{
  
  for( auto& configFile : m_configFiles )
  {
    if( m_selector.read_from( configFile, &*m_onnxSvc ).isFailure() ){
        ATH_MSG_ERROR( "It's not possible to read all tuning files from " << configFile );
        return StatusCode::FAILURE;
    } 
  }

  m_accept.addCut("Pass", "Aproved by ringer");
  return StatusCode::SUCCESS;
}

//==============================================================================

StatusCode AsgRingerSelectorTool::finalize()
{
  return StatusCode::SUCCESS;
}

//==============================================================================

asg::AcceptData  AsgRingerSelectorTool::accept(const xAOD::IParticle *) const
{
  asg::AcceptData acceptData(&m_accept);
  return acceptData; 
}

// =============================================================================

asg::AcceptData AsgRingerSelectorTool::accept( const xAOD::TrigRingerRings *ringsCluster, float discr, float mu ) const
{
  asg::AcceptData acceptData(&m_accept);
  bool pass = m_selector.accept( ringsCluster, discr, mu );
  acceptData.setCutResult("Pass", pass); 
  return acceptData;
}

// =============================================================================

float AsgRingerSelectorTool::predict( const xAOD::TrigRingerRings* ringsCluster ) const 
{
  return m_selector.predict( ringsCluster , nullptr);
}

// =============================================================================

float AsgRingerSelectorTool::predict( const xAOD::TrigRingerRings* ringsCluster,
                                      const xAOD::TrigElectron *el ) const 
{
  return m_selector.predict( ringsCluster , el);
}

// =============================================================================





}// namespace

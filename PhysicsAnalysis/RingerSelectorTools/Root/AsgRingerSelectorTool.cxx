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
  
  if( m_selector.read_from( m_configFile, &*m_onnxSvc ).isFailure() ){
      ATH_MSG_ERROR( "It's not possible to read all tuning files from " << m_configFile );
      return StatusCode::FAILURE;
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
asg::AcceptData AsgRingerSelectorTool::accept( const xAOD::TrigRingerRings *ringerShape, float discr, float mu ) const
{
  asg::AcceptData acceptData(&m_accept);
  bool pass = m_selector.accept( ringerShape, discr, mu );
  acceptData.setCutResult("Pass", pass); 
  return acceptData;
}


// =============================================================================
float AsgRingerSelectorTool::predict( const xAOD::TrigRingerRings* ringerShape, std::vector<std::vector<float>> &inputs ) const 
{
  return m_selector.predict( ringerShape , inputs);
}


std::vector< std::vector<float> > AsgRingerSelectorTool::prepare_inputs( const xAOD::TrigRingerRings* ringerShape,
                                                                         const xAOD::TrigElectron *el) const
{
  return m_selector.prepare_inputs( ringerShape, el );
}



}// namespace

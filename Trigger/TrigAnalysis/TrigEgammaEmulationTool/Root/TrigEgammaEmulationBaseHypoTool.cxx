

#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationBaseHypoTool::TrigEgammaEmulationBaseHypoTool( const std::string& myname )
    : asg::AsgTool( myname ),
      m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
      m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
{}

//**********************************************************************
StatusCode TrigEgammaEmulationBaseHypoTool::initialize() {

  ATH_CHECK(m_trigdec.retrieve() );
  ATH_CHECK( m_lumiBlockMuTool.retrieve() );
  return StatusCode::SUCCESS;
}

//**********************************************************************

float TrigEgammaEmulationBaseHypoTool::avgmu() const 
{
  return m_lumiBlockMuTool->averageInteractionsPerCrossing();
}



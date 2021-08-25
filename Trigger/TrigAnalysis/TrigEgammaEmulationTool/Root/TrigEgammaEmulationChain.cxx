/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#include "TrigEgammaEmulationTool/TrigEgammaEmulationChain.h"

using namespace Trig;



//**********************************************************************

TrigEgammaEmulationChain::TrigEgammaEmulationChain( const std::string& myname )
    : asg::AsgTool( myname )
{}


//**********************************************************************
StatusCode TrigEgammaEmulationChain::initialize() 
{

    ATH_CHECK( m_steps.retrieve() );

    //add cuts into TAccept
    m_accept.addCut("L1Calo"  , "Trigger L1Calo step"     );
    m_accept.addCut("L2Calo"  , "Trigger L2Calo step"     );
    m_accept.addCut("L2"      , "Trigger L2Electron step" );
    m_accept.addCut("EFCalo"  , "Trigger EFCalo step"     );
    m_accept.addCut("EFTrack" , "Trigger EFTrack step"    );
    m_accept.addCut("HLT"     , "Trigger HLT decision"    );

    return StatusCode::SUCCESS;
}

//**********************************************************************

asg::AcceptData TrigEgammaEmulationChain::emulate(const Trig::TrigData &input) const
{
  asg::AcceptData acceptData (&m_accept);

  ATH_MSG_DEBUG( "Emulate " << m_chain);
 
  // Emulate L1 seed
  bool passedL1Calo = false;
  m_l1Seed->emulate(input, passedL1Calo);
  acceptData.setCutResult( "L1Calo", passedL1Calo );

  if(passedL1Calo){
    for (size_t step=0; step < m_steps.size(); ++step)
    {
      bool passed=false;
      m_steps[step]->emulate( input, passed);
      acceptData.setCutResult( step + 1, passed);
      if(!passed)
        break;
    }
  }
  
  ATH_MSG_DEBUG("Accept results:");
  ATH_MSG_DEBUG("L1      : " << acceptData.getCutResult(0)); // Step 0
  ATH_MSG_DEBUG("L2Calo  : " << acceptData.getCutResult(1)); // Step 1
  ATH_MSG_DEBUG("L2      : " << acceptData.getCutResult(2)); // Step 2
  ATH_MSG_DEBUG("EFCalo  : " << acceptData.getCutResult(3)); // Step 3
  ATH_MSG_DEBUG("EFTrack : " << acceptData.getCutResult(4)); // Step 4
  ATH_MSG_DEBUG("HLT     : " << acceptData.getCutResult(5)); // Step 5

  return acceptData;
}
//!==========================================================================



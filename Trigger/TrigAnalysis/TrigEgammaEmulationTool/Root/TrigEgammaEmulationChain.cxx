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
    // Step 0
    ATH_CHECK(m_l1caloTool.retrieve());
    // Step 1
    ATH_CHECK(m_fastCaloTool.retrieve()); 
    // Step 2
    ATH_CHECK(m_fastTool.retrieve()); 
    // Step 3
    ATH_CHECK(m_precisionCaloTool.retrieve()); 
    // Step 4
    ATH_CHECK(m_precisionTool.retrieve()); 


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

  ATH_MSG_INFO( "Emulate " << m_chain);
  bool passedL1Calo, passedL2Calo, passedL2, passedEFCalo, passedEFTrack, passedHLT=false;

  m_l1caloTool->emulate(input, passedL1Calo);
  acceptData.setCutResult("L1Calo", passedL1Calo);
  if (passedL1Calo){
    m_fastCaloTool->emulate(input, passedL2Calo);
    acceptData.setCutResult("L2Calo", passedL2Calo);
    if (passedL2Calo){
      m_fastTool->emulate(input, passedL2);
      acceptData.setCutResult("L2", passedL2);
      if (passedL2){
        m_precisionCaloTool->emulate(input, passedEFCalo);    
        acceptData.setCutResult("EFCalo", passedEFCalo);
        if( passedEFCalo ){
          m_precisionTrackingTool->emulate(input, passedEFTrack);
          acceptData.setCutResult("EFTrack", passedEFTrack);
          if (passedEFTrack){
            m_precisionTool->emulate(input, passedHLT);
            acceptData.setCutResult("HLT", passedHLT);
          }
        }
      }
    }
  }

  ATH_MSG_INFO("Accept results:");
  ATH_MSG_INFO("L1     : " << passedL1Calo );
  ATH_MSG_INFO("L2Calo : " << passedL2Calo );
  ATH_MSG_INFO("L2     : " << passedL2     );
  ATH_MSG_INFO("EFCalo : " << passedEFCalo );
  ATH_MSG_INFO("HLT    : " << passedHLT    );
  
  return acceptData;
}
//!==========================================================================



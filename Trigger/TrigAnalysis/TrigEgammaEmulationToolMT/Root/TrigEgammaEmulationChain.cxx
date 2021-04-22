/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#include "TrigEgammaEmulationToolMT/TrigEgammaEmulationChain.h"

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

  bool L1CaloAccept, L2CaloAccept, L2Accept, EFCaloAccept, HLTAccept=false;

  m_l1caloTool->emulate(input, L1CaloAccept);
  acceptData.setCutResult("L1Calo", L1CaloAccept);
  if (L1CaloAccept){
    m_fastCaloTool->emulate(input, L2CaloAccept);
    acceptData.setCutResult("L2Calo", L2CaloAccept);
    if (L2CaloAccept){
      m_fastTool->emulate(input, L2Accept);
      acceptData.setCutResult("L2", L2Accept);
      if (L2Accept){
        m_precisionCaloTool->emulate(input, EFCaloAccept);    
        acceptData.setCutResult("EFCalo", EFCaloAccept);
        if( EFCaloAccept ){
          m_precisionTool->emulate(input, HLTAccept);
          acceptData.setCutResult("HLT", HLTAccept);
        }
      }
    }
  }
  return acceptData;
}

//!==========================================================================



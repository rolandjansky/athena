/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : LvlTopoConverter
 *
 * @brief see header file!
 *
 * @author Ingrid Deigaard - Nikhef, The Netherlands
 *
 * File and Version Information:
 * $Id: LvlTopoConverter.cxx 637123 2014-12-19 18:40:21Z fwinkl $
 **********************************************************************************/

#include "TrigSteering/LvlTopoConverter.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "L1TopoEvent/TopoInputEvent.h"

using namespace HLT;

LvlTopoConverter::LvlTopoConverter(const std::string& name, const std::string& type,
                                   const IInterface* parent) :
   LvlConverter(name, type, parent),
   //m_storeGate("StoreGateSvc",name),
   m_emtauInputProvider("LVL1::EMTauInputProvider/EMTauInputProvider", this),
   m_jetInputProvider("LVL1::JetInputProvider/JetInputProvider", this),
   m_energyInputProvider("LVL1::EnergyInputProvider/EnergyInputProvider", this),
   m_muonInputProvider("LVL1::MuonInputProvider/MuonInputProvider", this)
{}


ErrorCode
LvlTopoConverter::hltInitialize() {
   return HLT::OK;
}


ErrorCode
LvlTopoConverter::hltFinalize() {
   return HLT::OK;
}


ErrorCode LvlTopoConverter::hltExecute(std::vector<HLT::SteeringChain*>&) {
   return HLT::OK;
}

StatusCode
LvlTopoConverter::FillInputEvent(TCS::TopoInputEvent & inputEvent) {
   // EM TAU
   CHECK(m_emtauInputProvider->fillTopoInputEvent(inputEvent));

   // JET
   CHECK(m_jetInputProvider->fillTopoInputEvent(inputEvent));

   // ET sum, ET miss
   CHECK(m_energyInputProvider->fillTopoInputEvent(inputEvent));

   // ET sum, ET miss
   CHECK(m_muonInputProvider->fillTopoInputEvent(inputEvent));

   return StatusCode::SUCCESS;
}

ErrorCode
LvlTopoConverter::hltExecute(const std::vector<HLT::SteeringChain*>& /*chainToRun*/, TCS::TopoInputEvent & inputEvent) {

   ATH_MSG_DEBUG("Fill L1Topo InputEvent");

   StatusCode sc = FillInputEvent(inputEvent);
   if( sc.isFailure() ){
      ATH_MSG_ERROR("Failed to fill L1Topo InputEvent");
      return HLT::ERROR;
   }

   ATH_MSG_DEBUG("" << inputEvent);

   return HLT::OK;
}

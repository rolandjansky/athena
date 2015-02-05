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
 * $Id: LvlTopoConverter.cxx 641540 2015-01-25 13:39:47Z stelzer $
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

namespace {
   StatusCode max(StatusCode x, StatusCode y) {
      return StatusCode ( x.getCode() >= y.getCode() ? x.getCode() : y.getCode() );
   }
}

StatusCode
LvlTopoConverter::FillInputEvent(TCS::TopoInputEvent & inputEvent) {
   // EM TAU
   StatusCode sc = m_emtauInputProvider->fillTopoInputEvent(inputEvent);

   // JET
   sc = max( sc, m_jetInputProvider->fillTopoInputEvent(inputEvent) );

   // ET sum, ET miss
   sc = max( sc, m_energyInputProvider->fillTopoInputEvent(inputEvent) );

   // muons
   sc = max( sc, m_muonInputProvider->fillTopoInputEvent(inputEvent) );

   if(sc.isRecoverable()) {
      ATH_MSG_WARNING("One or more inputs to L1Topo simulation at the HLT are not available");
   }

   return sc == StatusCode::FAILURE ? StatusCode::FAILURE : StatusCode::SUCCESS; // if recoverable, then success
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

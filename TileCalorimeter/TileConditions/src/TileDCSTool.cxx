/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileDCSTool.h"

// Athena incldues
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"


//
//____________________________________________________________________
TileDCSTool::TileDCSTool(const std::string& type, const std::string& name, const IInterface* parent)
    : base_class(type, name, parent)
{

}


//
//____________________________________________________________________
TileDCSTool::~TileDCSTool() {
}

//
//____________________________________________________________________
StatusCode TileDCSTool::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== Initialize conditions data key with DCS status
  ATH_CHECK( m_dcsStateKey.initialize() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileDCSTool::finalize() {

  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}


float TileDCSTool::getChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->getChannelHV(ros, drawer, channel);

}

float TileDCSTool::getChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->getChannelHVSet(ros, drawer, channel);

}

int TileDCSTool::getDrawerStates(unsigned int ros,  unsigned int drawer) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->getDrawerStates(ros, drawer);

}


TileDCSState::TileDCSStatus TileDCSTool::getDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->getDCSHVStatus(ros, drawer, channel);

}

TileDCSState::TileDCSStatus TileDCSTool::getDCSStatus(unsigned int ros, unsigned int drawer) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->getDCSStatus(ros, drawer);

}

TileDCSState::TileDCSStatus TileDCSTool::getDCSStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->getDCSStatus(ros, drawer, channel);

}


bool TileDCSTool::isStatusHVBad(unsigned int ros, unsigned int drawer, unsigned int channel) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->isStatusHVBad(ros, drawer, channel);

}

bool TileDCSTool::isStatusBad (unsigned int ros, unsigned int drawer) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->isStatusBad(ros, drawer);

}

bool TileDCSTool::isStatusBad(unsigned int ros, unsigned int drawer, unsigned int channel) const {

  SG::ReadCondHandle<TileDCSState> dcsState(m_dcsStateKey);
  return dcsState->isStatusBad(ros, drawer, channel);

}

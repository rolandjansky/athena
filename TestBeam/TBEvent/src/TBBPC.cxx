/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TBEvent/TBBPC.h"


/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

TBBPC::TBBPC()
  : TBBeamDetector()
{
  this->reset();
}

TBBPC::TBBPC(const std::string& thisBPCName)
  : TBBeamDetector(thisBPCName)
{
  this->reset();
}

TBBPC::~TBBPC()
{ }

///////////////////////
// Data Manipulation //
///////////////////////

void TBBPC::reset()
{
  m_xErr = 0;
  m_yErr = 0;
  m_hitnumber = 0;

  // default signals
  m_xPos = m_yPos = 0;
  m_xPulse = m_yPulse = 0.;

  // default overflow conditions
  m_xPosOverflow   = m_yPosOverflow   = false;
  m_xPulseOverflow = m_yPulseOverflow = false;
  // internal controller
  m_overflowSetFlag = false;
  // global overflow indicator (from TBBeamDetector)
  m_overflow = false;
}

void TBBPC::setXPos(TBBPC::signal_type theSignal)
{
  m_xPos = theSignal;
}

void TBBPC::setYPos(TBBPC::signal_type theSignal)
{
  m_yPos = theSignal;
}

void TBBPC::setXErr(TBBPC::signal_type theSignal)
{
  m_xErr = theSignal;
}

void TBBPC::setYErr(TBBPC::signal_type theSignal)
{
  m_yErr = theSignal;
}

void TBBPC::setXPulse(TBBPC::signal_type theSignal)
{
  m_xPulse = theSignal;
}

void TBBPC::setYPulse(TBBPC::signal_type theSignal)
{
  m_yPulse = theSignal;
}

void TBBPC::setHitNbr(int theSignal)
{
  m_hitnumber = theSignal;
}

void TBBPC::setXPosOverflow(bool overflow)
{
  m_xPosOverflow = overflow;
}

void TBBPC::setYPosOverflow(bool overflow)
{
  m_yPosOverflow = overflow;
}

void TBBPC::setXPulseOverflow(bool overflow)
{
  m_xPulseOverflow = overflow;
}

void TBBPC::setYPulseOverflow(bool overflow)
{
  m_yPulseOverflow = overflow;
}


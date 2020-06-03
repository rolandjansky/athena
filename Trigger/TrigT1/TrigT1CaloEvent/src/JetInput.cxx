/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetInput.cpp  -  description
                             -------------------
    begin                : Tue 17 Jan 2006
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/JetInput.h"
#else
#include "JetInput.h"
#endif

namespace LVL1 {

JetInput::JetInput():
  m_energy(0),
  m_phi(0.0),
  m_eta(0.0),
  m_key(0)
{	
}

JetInput::~JetInput(){
}

/** constructs a trigger tower and sets the key. */
LVL1::JetInput::JetInput::JetInput(double phi, double eta,
                                   int energy, int key):
  m_energy(energy),
  m_phi(phi),
  m_eta(eta),
  m_key(key)
{
}


/** returns eta coord */
double LVL1::JetInput::eta() const{
	return m_eta;
}

/** returns phi coord of tower */
double LVL1::JetInput::phi() const{
	return m_phi;
}

/** returns ET */
int LVL1::JetInput::energy() const{
	return m_energy;
}


/** returns TT key - that is the key the TT had
when it was created. Added for bug-testing.*/
unsigned int LVL1::JetInput::key(){
  return m_key;
}

/** Returns flag to indicate whether JetInput ET is saturated */
bool LVL1::JetInput::isSaturated() const {
  return (m_energy >= m_saturationThreshold);
}

}// end of LVL1 namespace


/** return coord of JE */
LVL1::Coordinate LVL1::JetInput::coord() const{
  return Coordinate(m_phi,m_eta);
}

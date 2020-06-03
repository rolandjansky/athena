/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          TriggerTower.cpp  -  description
                             -------------------
    begin                : Fri Sep 30 2005
    email                : Alan.Watson@cern.ch
 ***************************************************************************/


#include "TrigT1CaloUtils/InternalTriggerTower.h"

namespace LVL1 {

/** constructs a trigger tower and sets the key. */
LVL1::InternalTriggerTower::InternalTriggerTower::InternalTriggerTower(double phi, double eta, int key):
	m_phi(phi),
	m_eta(eta),
        m_key(key),
	m_EmAmps(7),
	m_HadAmps(7),
        m_EmADC(7),
        m_HadADC(7)
{
}
// default constructor 
LVL1::InternalTriggerTower::InternalTriggerTower::InternalTriggerTower():
	m_phi(0.0),
	m_eta(0.0),
        m_key(0),
	m_EmAmps(1),
	m_HadAmps(1),
        m_EmADC(1),
        m_HadADC(1)
 {
 }

InternalTriggerTower::~InternalTriggerTower(){
}

/** returns em amplitudes */
const std::vector<double> & InternalTriggerTower::EmAmps() const 
{ return m_EmAmps; }

/** returns had amplitudes */
const std::vector<double> & InternalTriggerTower::HadAmps() const 
{ return m_HadAmps; }

/** returns em digits */
const std::vector<int> & InternalTriggerTower::EmADC() const
{ return m_EmADC; }

/** returns had digits */
const std::vector<int> & InternalTriggerTower::HadADC() const
{ return m_HadADC; }


/** Methods for filling amplitude vectors */
void LVL1::InternalTriggerTower::addEMAmps(std::vector<double>& energy){ 
  m_EmAmps = energy;
  return;
}
void LVL1::InternalTriggerTower::addHadAmps(std::vector<double>& energy){
  m_HadAmps = energy;
  return;
}

/** Methods for filling digit vectors */
void LVL1::InternalTriggerTower::addEMADC(std::vector<int>& digits){
  m_EmADC = digits;
  return;
}
void LVL1::InternalTriggerTower::addHadADC(std::vector<int>& digits){
  m_HadADC = digits;
  return;
}

/** Methods for adding ET to central sample of pulse.
    Intended for use with inputs without pulse shape (CaloCell, etc) */
void LVL1::InternalTriggerTower::addEMPeak(double energy){
  int peak = m_EmAmps.size()/2;
  m_EmAmps[peak] += energy;
  return;
}
void LVL1::InternalTriggerTower::addHadPeak(double energy){
  int peak = m_HadAmps.size()/2;
  m_HadAmps[peak] += energy;
  return;
}


} // end of LVL1 namespace

/** returns coordinate of TT */
LVL1::Coordinate LVL1::InternalTriggerTower::coord() const{
  return Coordinate(m_phi, m_eta);
}

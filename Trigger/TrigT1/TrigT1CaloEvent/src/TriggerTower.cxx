/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          TriggerTower.cpp  -  description
                             -------------------
    begin                : Mon Sep 25 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/


#include "TrigT1CaloEvent/TriggerTower.h"

namespace LVL1 {

// default constructor for persistency
LVL1::TriggerTower::TriggerTower::TriggerTower():
	m_em_energy(1),
        m_had_energy(1),
	m_em_adc(1),
        m_had_adc(1),
	m_em_BCID(1),
	m_had_BCID(1),
	m_em_extBCID(1),
	m_had_extBCID(1),
	m_phi(0.0),
	m_eta(0.0),
        m_key(0),
	m_em_error(0),
	m_had_error(0),
	m_em_peak(0),
	m_em_adc_peak(0),
	m_had_peak(0),
	m_had_adc_peak(0)
 {
 }
/** constructs a trigger tower and sets the key. */
LVL1::TriggerTower::TriggerTower::TriggerTower(double phi, double eta, unsigned int key):
	m_em_energy(1),
        m_had_energy(1),
	m_em_adc(1),
        m_had_adc(1),
	m_em_BCID(1),
	m_had_BCID(1),
	m_em_extBCID(1),
	m_had_extBCID(1),
	m_phi(phi),
	m_eta(eta),
        m_key(key),
	m_em_error(0),
	m_had_error(0),
	m_em_peak(0),
	m_em_adc_peak(0),
	m_had_peak(0),
	m_had_adc_peak(0)
{
}
/** constructs a trigger tower and sets everything */
LVL1::TriggerTower::TriggerTower::TriggerTower
  (double phi, double eta, unsigned int key,
   const std::vector<int>& em_digits,
   const std::vector<int>& em_et,
   const std::vector<int>& em_digits_bcid,
   const std::vector<int>& em_bcid, 
   int em_error, int em_peak, int em_adc_peak,
   const std::vector<int>& had_digits,
   const std::vector<int>& had_et,
   const std::vector<int>& had_digits_bcid,
   const std::vector<int>& had_bcid, 
   int had_error, int had_peak, int had_adc_peak):
	m_em_energy(em_et),
        m_had_energy(had_et),
	m_em_adc(em_digits),
        m_had_adc(had_digits),
	m_em_BCID(em_bcid),
	m_had_BCID(had_bcid),
	m_em_extBCID(em_digits_bcid),
	m_had_extBCID(had_digits_bcid),
	m_phi(phi),
	m_eta(eta),
        m_key(key),
	m_em_error(em_error),
	m_had_error(had_error),
	m_em_peak(em_peak),
	m_em_adc_peak(em_adc_peak),
	m_had_peak(had_peak),
	m_had_adc_peak(had_adc_peak)
{
}

TriggerTower::~TriggerTower(){
}

// return em layer ET for the central bunch-crossing
int LVL1::TriggerTower::emEnergy() const{
  return m_em_energy[m_em_peak]; 
}

// return had layer ET for central bunch crossing
int LVL1::TriggerTower::hadEnergy() const{
  return m_had_energy[m_had_peak]; 
}

/** returns em LUT ET */
const std::vector<int> & TriggerTower::emLUT() const 
{ return m_em_energy; }

/** returns had LUT ET */
const std::vector<int> & TriggerTower::hadLUT() const 
{ return m_had_energy; }

/** returns em adc digits */
const std::vector<int> & TriggerTower::emADC() const 
{ return m_em_adc; }

/** returns had adc digits */
const std::vector<int> & TriggerTower::hadADC() const 
{ return m_had_adc; }

/** returns BCID word for peak EM sample */
int LVL1::TriggerTower::emBCID() const {
  /// Fix for misfilled MC TriggerTowers
  if (m_em_BCID.size() != m_em_energy.size() && 
      m_em_BCID.size() == m_em_adc.size()) return m_em_BCID[m_em_adc_peak];
  
  return m_em_BCID[m_em_peak]; 
}

/** returns BCID word for peak had sample */
int LVL1::TriggerTower::hadBCID() const {
  if (m_had_BCID.size() != m_had_energy.size() && 
      m_had_BCID.size() == m_had_adc.size()) return m_had_BCID[m_had_adc_peak];
  
  return m_had_BCID[m_had_peak]; 
}

/** returns vector of BCID words for all EM LUT samples */
const std::vector<int> & LVL1::TriggerTower::emBCIDvec() const 
{ return m_em_BCID; }

/** returns vector of BCID words for all Had LUT samples */
const std::vector<int> & LVL1::TriggerTower::hadBCIDvec() const 
{ return m_had_BCID; }
/** returns vector of BCID words for all EM ADC samples */
const std::vector<int> & LVL1::TriggerTower::emBCIDext() const 
{ return m_em_extBCID; }

/** returns vector of BCID words for all Had ADC samples */
const std::vector<int> & LVL1::TriggerTower::hadBCIDext() const 
{ return m_had_extBCID; }

/** Method for filling EM tower data */
void LVL1::TriggerTower::addEM(const std::vector<int>& digits,
                               const std::vector<int>& et, 
                               const std::vector<int>& digits_bcid,
                               const std::vector<int>& bcid, 
			       int error, int peak, int adc_peak) { 
  m_em_adc = digits;
  m_em_energy = et;
  m_em_BCID = bcid;
  m_em_extBCID = digits_bcid;
  m_em_error = error;
  m_em_peak = peak;
  m_em_adc_peak = adc_peak;
  return;
}
/** Method for filling Had tower data */
void LVL1::TriggerTower::addHad(const std::vector<int>& digits, const std::vector<int>& et, 
                                const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
				int error, int peak, int adc_peak) { 
  m_had_adc = digits;
  m_had_energy = et;
  m_had_BCID = bcid;
  m_had_extBCID = digits_bcid;
  m_had_error = error;
  m_had_peak = peak;
  m_had_adc_peak = adc_peak;
  return;
}

/** Method to set coordinate of tower */
void LVL1::TriggerTower::setCoord(double eta, double phi) {
  m_eta = eta;
  m_phi = phi;
  return;
}

/** Method to set or update key */
void LVL1::TriggerTower::setKey(unsigned int key) {
  m_key = key;
  return;
}
/** returns TRUE if the TriggerTower is saturated
(i.e. it has an energy greater than the saturation
threshold) and FALSE otherwise. */
bool LVL1::TriggerTower::isEMSaturated() const{
  if (m_em_energy[m_em_peak]>=m_saturation){
    return true;
  }else{
    return false;
  }
}

/** returns TRUE or FALSE depending on whether
the TriggerTower's hadronic energy is greater
than it's hadronic saturation threshold. */
bool LVL1::TriggerTower::isHadSaturated() const{
  if (m_had_energy[m_had_peak]>=m_saturation){
    return true;
  }else{
    return false;
  }
}

}

/** returns coordinate of TT */
LVL1::Coordinate LVL1::TriggerTower::coord() const{
  return Coordinate(m_phi, m_eta);
}

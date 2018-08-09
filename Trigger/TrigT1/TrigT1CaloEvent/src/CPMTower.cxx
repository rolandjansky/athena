/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CPMTower.cpp  -  description
                             -------------------
        begin                : Thurs Dec 14 2006
        email                : Alan.Watson@cern.ch
 ***************************************************************************/


#include "TrigT1CaloEvent/CPMTower.h"

namespace LVL1 {

// default constructor for persistency
LVL1::CPMTower::CPMTower::CPMTower():
	m_em_energy(1),
        m_had_energy(1),
	m_em_error(1),
	m_had_error(1),
	m_phi(0.0),
	m_eta(0.0),
	m_peak(0)
 {
 }
/** constructs a trigger tower and sets the key. */
LVL1::CPMTower::CPMTower::CPMTower(double phi, double eta):
	m_em_energy(1),
        m_had_energy(1),
	m_em_error(1),
	m_had_error(1),
	m_phi(phi),
	m_eta(eta),
	m_peak(0)
{
}
/** constructs a trigger tower and sets everything */
LVL1::CPMTower::CPMTower::CPMTower(double phi, double eta,
                                   const std::vector<int>& em_et,
                                   const std::vector<int>& em_error,
                                   const std::vector<int>& had_et,
                                   const std::vector<int>& had_error,
                                   int peak):
	m_em_energy(em_et),
        m_had_energy(had_et),
	m_em_error(em_error),
	m_had_error(had_error),
	m_phi(phi),
	m_eta(eta),
	m_peak(peak)
{
}

CPMTower::~CPMTower(){
}

/** Method for filling tower data */
void LVL1::CPMTower::fill(const std::vector<int>& em_et,
                          const std::vector<int>& em_error, 
                          const std::vector<int>& had_et,
                          const std::vector<int>& had_error,
                          int peak)
{ 
  m_em_energy = em_et;
  m_em_error = em_error;
  m_had_energy = had_et;
  m_had_error = had_error;
  m_peak = peak;
  return;
}

/** Methods to return ET in specified slice. Return 0 if slice is out of range */
int LVL1::CPMTower::emSliceEnergy(int slice) const {
  if (slice >= 0 && slice < (int)m_em_energy.size()) return m_em_energy[slice];
  else return 0;
}
int LVL1::CPMTower::hadSliceEnergy(int slice) const {
  if (slice >= 0 && slice < (int)m_had_energy.size()) return m_had_energy[slice];
  else return 0;
}

/** Methods to return error words for specified slice. Return 0 if slice is out of range */
int LVL1::CPMTower::emSliceError(int slice) const {
  if (slice >= 0 && slice < (int)m_em_error.size()) return m_em_error[slice];
  else return 0;
}
int LVL1::CPMTower::hadSliceError(int slice) const {
  if (slice >= 0 && slice < (int)m_had_error.size()) return m_had_error[slice];
  else return 0;
}

}

/** returns coordinate of TT */
LVL1::Coordinate LVL1::CPMTower::coord() const{
  return Coordinate(m_phi, m_eta);
}

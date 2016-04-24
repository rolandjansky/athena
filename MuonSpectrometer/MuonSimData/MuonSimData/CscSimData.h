/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscSimData.h

// CSC simulation data object associated with an MuonRawData object

// D. Adams
// March 2007
//
// Modification of a copy of MuonSimData.

#ifndef MUONSIMDATA_CscSimData_H
#define MUONSIMDATA_CscSimData_H

// An object of this type will be constructed for each step
// contributing to each hit strip.
//       energy = energy deposited in step
// (ypos, zpos) = transverse position where step crosses wire plane
//       charge = charge on strip for the first hit for the strip;
//                otherwise zero
class CscMcData {
public:
  CscMcData() : m_energy(-9999.), m_ypos(-9999.), m_zpos(-9999.), m_charge(0.0) { }
  CscMcData(float energy, float ypos, float zpos)
  : m_energy(energy), m_ypos(ypos), m_zpos(zpos), m_charge(0.0) { }
  void setCharge(float chg) { m_charge = chg; }
  float energy() const { return m_energy; }
  float ypos() const { return m_ypos; }
  float zpos() const { return m_zpos; }
  float charge() const { return m_charge; }
private:
  float m_energy;
  float m_ypos;
  float m_zpos;
  float m_charge;
};

#include <utility>
#include <vector>
#include "GeneratorObjects/HepMcParticleLink.h"

class CscSimData {
    
public:
  typedef std::pair<HepMcParticleLink, CscMcData> Deposit;

public:
  CscSimData();
  CscSimData (const std::vector<Deposit>& deposits, int simDataWord =0);
  CscSimData (      std::vector<Deposit>&& deposits, int simDataWord =0);
  CscSimData (const CscSimData& rhs);
  virtual ~CscSimData();
  int word() const;                       // Get the packed simdata word
  void deposits(std::vector<Deposit>& deposits) const; // Get the Deposits
  const std::vector< Deposit >& getdeposits() const;

private:
  int m_word;  
  std::vector<Deposit> m_deposits;
};


inline int CscSimData::word() const {
  return m_word & 0x1fffffff;
}

inline const std::vector<CscSimData::Deposit>& CscSimData::getdeposits() const {
  return m_deposits;
}

inline void CscSimData::deposits(std::vector<CscSimData::Deposit>& deposits) const {
  deposits = m_deposits;
  return;
}

#endif // MUONSIMDATA_CscSimData_H


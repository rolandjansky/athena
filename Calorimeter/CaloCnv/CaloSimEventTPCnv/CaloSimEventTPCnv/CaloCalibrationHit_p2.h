// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHIT_P2_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHIT_P2_H

/** @class CaloCalibrationHit_p2
    @brief Persistent CaloCalibrationHit class, version p2.
*/

#include "Identifier/Identifier.h"

class CaloCalibrationHit_p2
{
 public:
  CaloCalibrationHit_p2()
    : m_energy0(0),
      m_energy1(0),
      m_energy2(0),
      m_energy3(0),
      m_particleID(0)
  {}
  friend class CaloCalibrationHitCnv_p2;

 private:
  /** identifier of the cell in which this hit occured. */
  Identifier m_ID;

  /** energies (in MeV) deposited in this hit.  In order, they represent:
   * EM energy deposited
   * non-EM energy deposited
   * "invisible" energy deposited
   * escaped energy  */
  float m_energy0;
  float m_energy1;
  float m_energy2;
  float m_energy3;

  /** identifier of Primary Particle which caused this hit */
  unsigned int m_particleID;
};

#endif

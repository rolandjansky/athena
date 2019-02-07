/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CrateEnergy.h  -  description
                             -------------------
    begin                : 05/09/2007
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef CRATEENERGY_H
#define CRATEENERGY_H

#include "TrigT1CaloUtils/ModuleEnergy.h"
#include "TrigT1CaloEvent/EnergyCMXData.h"


/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**CrateEnergy class declaration  Simulates behaviour of the Crate-CMM
System CMM logic is done in EnergyTrigger*/

class CrateEnergy {

public:
  CrateEnergy(unsigned int crate, const DataVector<ModuleEnergy>* modules, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false);
  CrateEnergy(unsigned int crate, const DataVector<EnergyCMXData>* modules, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false);
  CrateEnergy(unsigned int crate, unsigned int et, unsigned int exTC,
              unsigned int eyTC, unsigned int overflowT, unsigned int overflowX,
	      unsigned int overflowY, bool restricted = false);
  ~CrateEnergy();

public:
  unsigned int crate() const;
  /** return et, ex, ey sums */
  int et() const;
  int ex() const;
  int ey() const;
  /** 15 bit twos-complement format */
  unsigned int exTC() const;
  unsigned int eyTC() const;
  /** Overflow bits */
  unsigned int exOverflow() const;
  unsigned int eyOverflow() const;
  unsigned int etOverflow() const;
  /** Full or restricted eta range? */
  bool restricted() const;
private:
  unsigned int m_crate;
  unsigned int m_crateEt;
  int m_crateEx;
  int m_crateEy;
  unsigned int m_overflowT;
  unsigned int m_overflowX;
  unsigned int m_overflowY;
  bool m_restricted;
  bool m_debug;
  static const unsigned int m_sumBitsTC=15;
  static const unsigned int m_sumBits=14;
  static const unsigned int m_jemEtSaturation= 0x3fff; // was 4032
private:
  unsigned int encodeTC(int input) const;
  int decodeTC(unsigned int input) const;

};

} // end of namespace bracket


#endif

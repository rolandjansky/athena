/***************************************************************************
                          CrateEnergy.h  -  description
                             -------------------
    begin                : 05/09/2007
    copyright            : (C) 2007 by Alan Watson
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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
  CrateEnergy(unsigned int crate, const DataVector<ModuleEnergy>* modules, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false);
  CrateEnergy(unsigned int crate, const DataVector<EnergyCMXData>* modules, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false);
  CrateEnergy(unsigned int crate, unsigned int et, unsigned int exTC,
              unsigned int eyTC, unsigned int overflowT, unsigned int overflowX,
	      unsigned int overflowY, bool restricted = false);
  ~CrateEnergy();

public:
  unsigned int crate();
  /** return et, ex, ey sums */
  int et();
  int ex();
  int ey();
  /** 15 bit twos-complement format */
  unsigned int exTC();
  unsigned int eyTC();
  /** Overflow bits */
  unsigned int exOverflow();
  unsigned int eyOverflow();
  unsigned int etOverflow();
  /** Full or restricted eta range? */
  bool restricted();
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
  static const unsigned int m_jemEtSaturation=4032;

private:
  unsigned int encodeTC(int input);
  int decodeTC(unsigned int input);

};

} // end of namespace bracket


#endif

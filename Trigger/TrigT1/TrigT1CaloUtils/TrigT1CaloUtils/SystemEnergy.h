/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          SystemEnergy.h  -  description
                             -------------------
    begin                : 06/09/2007
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef SYSTEMENERGY_H
#define SYSTEMENERGY_H
#include "TrigT1CaloUtils/CrateEnergy.h"

 // Include for the configuration service:
#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"



/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**SystemEnergy class declaration  Simulates behaviour of the Crate-CMM
System CMM logic is done in EnergyTrigger*/

class SystemEnergy {

public:
  SystemEnergy(const DataVector<CrateEnergy>* crates, ServiceHandle<TrigConf::ILVL1ConfigSvc> config);
  SystemEnergy(unsigned int et, unsigned int exTC, unsigned int eyTC,
               unsigned int overflowT, unsigned int overflowX,
	       unsigned int overflowY, unsigned int restricted,
	       ServiceHandle<TrigConf::ILVL1ConfigSvc> config);
  ~SystemEnergy();

public:
  /** return global et, ex, ey sums */
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
  /** Trigger Results */
  unsigned int etMissHits() const;
  unsigned int etSumHits() const;
  unsigned int metSigHits() const;
  /** RoI words */
  unsigned int roiWord0() const;
  unsigned int roiWord1() const;
  unsigned int roiWord2() const;
  
private:
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  
  int m_systemEx;
  int m_systemEy;
  int m_systemEt;
  unsigned int m_overflowX;
  unsigned int m_overflowY;
  unsigned int m_overflowT;
  unsigned int m_restricted;
  unsigned int m_etMissHits;
  unsigned int m_etSumHits;
  unsigned int m_metSigHits;
  uint32_t m_etMissQ;
  static const unsigned int m_sumBits=15;
  static const int m_maxEtSumThr=0x3fff;
  static const int m_etSumOverflow=0x7fff;
  bool m_debug;

private:
  void etMissTrigger();
  void etSumTrigger();
  void metSigTrigger();
  unsigned int encodeTC(int input) const;
  int decodeTC(unsigned int input) const;

};

} // end of namespace bracket

#endif

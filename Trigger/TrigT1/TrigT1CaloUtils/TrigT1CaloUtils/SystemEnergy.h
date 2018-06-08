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
#include "TrigConfInterfaces/ITrigConfigSvc.h"



/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**SystemEnergy class declaration  Simulates behaviour of the Crate-CMM
System CMM logic is done in EnergyTrigger*/

class SystemEnergy {

public:
  SystemEnergy(const DataVector<CrateEnergy>* crates, ServiceHandle<TrigConf::ITrigConfigSvc> config);
  SystemEnergy(unsigned int et, unsigned int exTC, unsigned int eyTC,
               unsigned int overflowT, unsigned int overflowX,
	       unsigned int overflowY, unsigned int restricted,
	       ServiceHandle<TrigConf::ITrigConfigSvc> config);
  ~SystemEnergy();

public:
  /** return global et, ex, ey sums */
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
  /** Trigger Results */
  unsigned int etMissHits();
  unsigned int etSumHits();
  unsigned int metSigHits();
  /** RoI words */
  unsigned int roiWord0();
  unsigned int roiWord1();
  unsigned int roiWord2();
  
private:
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
  
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
  unsigned int encodeTC(int input);
  int decodeTC(unsigned int input);

};

} // end of namespace bracket

#endif

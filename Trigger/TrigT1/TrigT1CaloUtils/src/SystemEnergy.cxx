/***************************************************************************
                          SystemEnergy.h  -  description
                             -------------------
    begin                : 06/09/2007
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

#include "TrigT1CaloUtils/SystemEnergy.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/L1METvalue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/METSigParam.h"

namespace LVL1 {

SystemEnergy::SystemEnergy(const DataVector<CrateEnergy>* crates, ServiceHandle<TrigConf::ITrigConfigSvc> config):
  m_configSvc(config),
  m_systemEx(0),
  m_systemEy(0),
  m_systemEt(0),
  m_overflowX(0),
  m_overflowY(0),
  m_overflowT(0),
  m_etMissHits(0),
  m_etSumHits(0),
  m_metSigHits(0),
  m_metSig(0),
  m_debug(false)
  {

  /** Get Ex, Ey, ET sums from crates and form global sums <br>
      Propagate overflows and test for new ones <br> */

  DataVector<CrateEnergy>::const_iterator it = crates->begin();
  for ( ; it != crates->end(); it++) {
    if ((*it)->crate() == 0) {
      m_systemEx += (*it)->ex();
      m_systemEy += (*it)->ey();
      m_systemEt += (*it)->et();
    }
    else if ((*it)->crate() == 1) {
      m_systemEx -= (*it)->ex();
      m_systemEy += (*it)->ey();
      m_systemEt += (*it)->et();
    }
    m_overflowX = m_overflowX|(*it)->exOverflow();
    m_overflowY = m_overflowY|(*it)->eyOverflow();
    m_overflowT = m_overflowT|(*it)->etOverflow();
  }
  
  /** Check for EtSum overflow */
  if (m_overflowT != 0) m_systemEt = m_etSumOverflow;
  /** Check for overflow of Ex, Ey sums */
  int xyMax = (1<<(m_sumBits-1)) - 1;
  if (abs(m_systemEx) > xyMax) {
    m_overflowX = 1;
    m_systemEx = (m_systemEx > 0 ? 1 : -1) * ( abs(m_systemEx)&xyMax );
  }
  if (abs(m_systemEy) > xyMax) {
    m_overflowY = 1;
    m_systemEy = (m_systemEy > 0 ? 1 : -1) * ( abs(m_systemEy)&xyMax );
  }
 

  if (m_debug) {
    std::cout << "SystemEnergy results: " << std::endl
              << "   Et "  << m_systemEt << " overflow " << m_overflowT << std::endl
              << "   Ex "  << m_systemEx << " overflow " << m_overflowX << std::endl
              << "   Ey "  << m_systemEy << " overflow " << m_overflowY << std::endl;
              
  }

  /** Having completed sums, we can now compare with thresholds */
  etMissTrigger();
  etSumTrigger();
  metSigTrigger();
  
}

SystemEnergy::SystemEnergy(unsigned int et, unsigned int exTC, unsigned int eyTC,
                           unsigned int overflowT, unsigned int overflowX,
			   unsigned int overflowY,
			   ServiceHandle<TrigConf::ITrigConfigSvc> config):
  m_configSvc(config),
  m_systemEx(0),
  m_systemEy(0),
  m_systemEt(et),
  m_overflowX(overflowX),
  m_overflowY(overflowY),
  m_overflowT(overflowT),
  m_etMissHits(0),
  m_etSumHits(0),
  m_metSigHits(0),
  m_metSig(0),
  m_debug(false)
{
  m_systemEx = decodeTC(exTC);
  m_systemEy = decodeTC(eyTC);

  if (m_debug) {
    std::cout << "SystemEnergy results: " << std::endl
              << "   Et "  << m_systemEt << " overflow " << m_overflowT << std::endl
              << "   Ex "  << m_systemEx << " overflow " << m_overflowX << std::endl
              << "   Ey "  << m_systemEy << " overflow " << m_overflowY << std::endl;
              
  }

  /** Having completed sums, we can now compare with thresholds */
  etMissTrigger();
  etSumTrigger();
  metSigTrigger();

}

SystemEnergy::~SystemEnergy(){
}

/** return crate Et */
int SystemEnergy::et() {
  return m_systemEt;
}

/** return crate Ex */
int SystemEnergy::ex() {
  return m_systemEx;
}

/** return crate Ey */
int SystemEnergy::ey() {
  return m_systemEy;
}

/** return Et overflow bit */
unsigned int SystemEnergy::etOverflow() {
  return m_overflowT & 0x1;
}

/** return Ex overflow bit */
unsigned int SystemEnergy::exOverflow() {
  return m_overflowX & 0x1;
}

/** return Ey overflow bit */
unsigned int SystemEnergy::eyOverflow() {
  return m_overflowY & 0x1;
}

/** return crate Ex in 15-bit twos-complement format (hardware format) */
unsigned int SystemEnergy::exTC() {
  return encodeTC(m_systemEx) ;
}

/** return crate Ey in 15-bit twos-complement format (hardware format) */
unsigned int SystemEnergy::eyTC() {
  return encodeTC(m_systemEy) ;
}

/** return EtMiss hits */
unsigned int SystemEnergy::etMissHits() {
  return m_etMissHits;
}

/** return EtSum hits */
unsigned int SystemEnergy::etSumHits() {
  return m_etSumHits;
}

/** return MEtSig hits */
unsigned int SystemEnergy::metSigHits() {
  return m_metSigHits;
}

/** return MEtSig value * 1000 */
float SystemEnergy::metSig() {
  return m_metSig;
}

/** return RoI word 0 (Ex value & overflow) */
unsigned int SystemEnergy::roiWord0() {
  // Start by setting up header
  unsigned int word = TrigT1CaloDefs::energyRoIType<<30 ;
  word += TrigT1CaloDefs::energyRoI0<<28;
  // add MET Significance hits
  word += metSigHits()<<16;
  // add Ex overflow bit
  word += exOverflow()<<15;
  // add Ex value (twos-complement)
  word += exTC();
  return word;
}

/** return RoI word 1 (Ey value & overflow, EtSum hits) */
unsigned int SystemEnergy::roiWord1() {
  // Start by setting up header
  unsigned int word = TrigT1CaloDefs::energyRoIType<<30 ;
  word += TrigT1CaloDefs::energyRoI1<<28;
  // add EtSum hits
  word += etSumHits()<<16;
  // add Ey overflow bit
  word += eyOverflow()<<15;
  // add Ey value (twos-complement)
  word += eyTC();
  return word;
}

/** return RoI word 2 (Et value & overflow, EtMiss hits) */
unsigned int SystemEnergy::roiWord2() {
  // Start by setting up header
  unsigned int word = TrigT1CaloDefs::energyRoIType<<30 ;
  word += TrigT1CaloDefs::energyRoI2<<28;
  // add EtMiss hits
  word += etMissHits()<<16;
  // add Et overflow bit
  word += etOverflow()<<15;
  // add Et value (unsigned)
  word += et();
  return word;
}

/** Test Ex, Ey sums against ETmiss thresholds <br>
    Regrettably not as simple as it sounds if we emulate hardware! <br>
    See CMM specificiation from L1Calo web pages for details */
void SystemEnergy::etMissTrigger() {
  /// Start cleanly
  m_etMiss     = 0;
  m_etMissHits = 0;
  
  /// Ex or Ey overflow automatically fires all thresholds, sets MET value to maximum
  if ( (m_overflowX != 0) || (m_overflowY != 0) ) {
    m_metRange = 3;
    m_etMiss = 63<<m_metRange;
    m_etMissHits = (1<<TrigT1CaloDefs::numOfMissingEtThresholds) - 1;
    return;
  }
  
  // Use L1METvalue tool to computer ETmiss with effective precision of LUT
  L1METvalue METvalue;
  bool overflow;
  METvalue.calcL1MET(ex(),ey(),m_etMiss,m_metRange,overflow);

  // LUT overflow also fires all thresholds
  if (overflow) {
    m_metRange = 3;
    m_etMiss = 63<<m_metRange;
    m_etMissHits = (1<<TrigT1CaloDefs::numOfMissingEtThresholds) - 1;
    return;
  }
  
  // Otherwise expand the MET value
  int missingET = m_etMiss << m_metRange;  
  
  // Get thresholds
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  
  // get Threshold values and test
  L1DataDef def;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.xeType() ) {
      TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0);       
      int thresholdValue = (*tv).thresholdValueCount();
      int threshNumber = (*it)->thresholdNumber();
      if (missingET > thresholdValue )
           m_etMissHits = m_etMissHits|(1<<threshNumber);
    }
  }

  return;
  
}

/** Test Et sum against ETsum thresholds */
void SystemEnergy::etSumTrigger() {
  // Start cleanly
  m_etSumHits = 0;

  // Overflow of any sort automatically fires all thresholds
  if (m_overflowT != 0 || m_systemEt > m_maxEtSumThr) {
    m_etSumHits = (1<<TrigT1CaloDefs::numOfSumEtThresholds) - 1;
    return;
  }
  
  // Get thresholds
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  
  // get Threshold values and test
  // Since eta-dependent values are being used to disable TE in regions, must find lowest value for each threshold
  L1DataDef def;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.teType() ) {
      int threshNumber = (*it)->thresholdNumber();
      int thresholdValue = m_maxEtSumThr;
      std::vector<TriggerThresholdValue*> tvv = (*it)->thresholdValueVector();
      for (std::vector<TriggerThresholdValue*>::const_iterator ittvv = tvv.begin(); ittvv != tvv.end(); ++ittvv) 
	if ((*ittvv)->thresholdValueCount() < thresholdValue) thresholdValue = (*ittvv)->thresholdValueCount();
      if (static_cast<int>(m_systemEt) > thresholdValue ) m_etSumHits = m_etSumHits|(1<<threshNumber);
    }
  }

  return;
}

/** Test MEt Significance against METSig thresholds */
void SystemEnergy::metSigTrigger() {
  /// Start cleanly
  m_metSig     = 0.;
  m_metSigHits = 0;
  
  /// Obtain parameters from configuration service
  METSigParam params = m_configSvc->thresholdConfig()->caloInfo().metSigParam();
  float a = params.xsSigmaScale()/1000.;
  float b = params.xsSigmaOffset()/1000.;
  int XEmin = params.xeMin();
  int XEmax = params.xeMax();
  int sqrtTEmin = params.teSqrtMin();
  int sqrtTEmax = params.teSqrtMax();
  
  /// Calculate MET Significance
  L1METvalue METvalue;
  int outOfRange = 0;
  METvalue.calcL1METSig(m_etMiss, m_metRange, m_systemEt, a, b, XEmin, XEmax, sqrtTEmin, sqrtTEmax,
               m_metSig, outOfRange);

  /// Check inputs within range and set hits accordingly
  if (outOfRange >= 0) {
    
    // Get thresholds
    std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
    std::vector<TriggerThreshold*>::const_iterator it;
  
    // get Threshold values and test
    L1DataDef def;
    for (it = thresholds.begin(); it != thresholds.end(); ++it) {
      if ( (*it)->type() == def.xsType() ) {
        TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0);       
        float thresholdValue = float((*tv).thresholdValueCount())/10.;
        int threshNumber = (*it)->thresholdNumber();
        if ( m_metSig > thresholdValue || outOfRange > 0 ) m_metSigHits = m_metSigHits|(1<<threshNumber);
      }
    }
    
  }
  
  return;
}
/** encode int as 15-bit twos-complement format (hardware Ex/Ey format) */
unsigned int SystemEnergy::encodeTC(int input) {
  unsigned int value;

  if (input > 0) {
    value = input;
  }
  else {
    value = (1<<m_sumBits) + input;
  }

  int mask = (1<<m_sumBits) - 1;
  return value&mask ;
}

/** decode 15-bit twos-complement format (hardware Ex/Ey format) as int */
int SystemEnergy::decodeTC(unsigned int input) {

  int mask = (1<<m_sumBits) - 1;
  int value = input&mask;

  if ((value >> (m_sumBits - 1))) {
    value += (-1) << m_sumBits;
  }

  return value;
}

} // end of ns

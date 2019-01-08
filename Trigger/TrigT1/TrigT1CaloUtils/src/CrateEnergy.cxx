/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CrateEnergy.h  -  description
                             -------------------
    begin                : 06/09/2007
    email                : Alan.Watson@cern.ch
 ***************************************************************************/


#include "TrigT1CaloUtils/CrateEnergy.h"

namespace LVL1 {

CrateEnergy::CrateEnergy(unsigned int crate, const DataVector<ModuleEnergy>* JEMs, uint32_t maskXE, uint32_t maskTE, bool restricted):
  m_crate(crate),
  m_crateEt(0),
  m_crateEx(0),
  m_crateEy(0),
  m_overflowT(0),
  m_overflowX(0),
  m_overflowY(0),
  m_restricted(restricted),
  m_debug(false)
  {
  /** Check crate in range */
  if (m_crate > 1) return;

  /** Added for restricted eta triggers in Run 2 */
  if ((maskXE&0xff) != 0xff || (maskTE&0xff) != 0xff) m_restricted = true;

  /** Summing within a crate proceeds as follows: <br>
         unsigned 14 bit Ex, Ey, Et sums are formed for each half crate<br>
         these sums (= opposite quadrants) are added (Et) or subtracted (Ex, Ey)<br>
         to give the crate totals<br>
         overflows are propagated using separate bits */
  
  /** Sum ET from modules belonging to this crate in two halves */
  unsigned int eT[2] = {0,0};
  unsigned int eX[2] = {0,0};
  unsigned int eY[2] = {0,0};
  DataVector<ModuleEnergy>::const_iterator it = JEMs->begin();
  for ( ; it != JEMs->end(); it++) {
    int moduleInQuad = (*it)->module() % 8;
    if ((*it)->crate() == m_crate) {
      int quad = ( (*it)->module() < 8 ? 0 : 1 );
      if ((maskTE>>moduleInQuad)&1) {
        eT[quad] += (*it)->et();
        if ( (*it)->et() >= m_jemEtSaturation ) m_overflowT = 1;
      }
      if ((maskXE>>moduleInQuad)&1) {
        eX[quad] += (*it)->ex();
        eY[quad] += (*it)->ey();
        if ( (*it)->ex() >= m_jemEtSaturation ) m_overflowX = 1;
        if ( (*it)->ey() >= m_jemEtSaturation ) m_overflowY = 1;
      } 
    }  // Right crate?
  }   // Loop over JEMs
  
  /** Check for overflows then truncate quadrant sums*/
  unsigned int mask = (1 << m_sumBits) - 1;

  /** Form crate sums */
  /** For total ET we must check for further overflows */
  m_crateEt = eT[0] + eT[1];
  if (m_crateEt >= mask){
    m_overflowT = 1;
    m_crateEt = mask;
  }

 
  if (!m_overflowX){
    m_crateEx = eX[0] - eX[1];
  } else{
    m_crateEx = -(mask + 1);
  }
  if (!m_overflowY){
    m_crateEy = eY[0] - eY[1];
  }else{
    m_crateEy = -(mask + 1);
  }
  

  if (m_debug) {
    std::cout << "CrateEnergy: crate " << m_crate << " results " << std::endl
              << "   Et "  << m_crateEt << " overflow " << m_overflowT << std::endl
              << "   Ex "  << m_crateEx << " overflow " << m_overflowX << std::endl
              << "   Ey "  << m_crateEy << " overflow " << m_overflowY << std::endl;
              
  }
  
}

CrateEnergy::CrateEnergy(unsigned int crate, const DataVector<EnergyCMXData>* JEMs, uint32_t maskXE, uint32_t maskTE, bool restricted):
  m_crate(crate),
  m_crateEt(0),
  m_crateEx(0),
  m_crateEy(0),
  m_overflowT(0),
  m_overflowX(0),
  m_overflowY(0),
  m_restricted(restricted),
  m_debug(false)
  {
  /** Check crate in range */
  if (m_crate > 1) return;

  /** Added for restricted eta triggers in Run 2 */
  if ((maskXE&0xff) != 0xff || (maskTE&0xff) != 0xff) m_restricted = true;

  /** Summing within a crate proceeds as follows: <br>
         unsigned 14 bit Ex, Ey, Et sums are formed for each half crate<br>
         these sums (= opposite quadrants) are added (Et) or subtracted (Ex, Ey)<br>
         to give the crate totals<br>
         overflows are propagated using separate bits */
  
  /** Sum ET from modules belonging to this crate in two halves */
  unsigned int eT[2] = {0,0};
  unsigned int eX[2] = {0,0};
  unsigned int eY[2] = {0,0};
  DataVector<EnergyCMXData>::const_iterator it = JEMs->begin();
  for ( ; it != JEMs->end(); it++) {
    int moduleInQuad = (*it)->module() % 8;
    if ((unsigned int)(*it)->crate() == m_crate) {
      int quad = ( (*it)->module() < 8 ? 0 : 1 );

      if ((maskTE>>moduleInQuad)&1) {
        eT[quad] += (*it)->Et();
        if ( (*it)->Et() >= m_jemEtSaturation ) m_overflowT = 1;
      }
      if ((maskXE>>moduleInQuad)&1) {
        eX[quad] += (*it)->Ex();
        eY[quad] += (*it)->Ey();
        if ( (*it)->Ex() >= m_jemEtSaturation ) m_overflowX = 1;
        if ( (*it)->Ey() >= m_jemEtSaturation ) m_overflowY = 1;
      } 
    }
  }
  
  /** Check for overflows then truncate quadrant sums*/
  unsigned int mask = (1<<m_sumBits) - 1;


  /** Form crate sums */
  /** For total ET we must check for further overflows */
  m_crateEt = eT[0] + eT[1];
  if (m_crateEt >= mask){
    m_overflowT = 1;
    m_crateEt = mask;
  }

  if (!m_overflowX){
    m_crateEx = eX[0] - eX[1];
  } else{
    m_crateEx = -(mask + 1);
  }
  if (!m_overflowY){
    m_crateEy = eY[0] - eY[1];
  }else{
    m_crateEy = -(mask + 1);
  }

  if (m_debug) {
    std::cout << "CrateEnergy: crate " << m_crate << " results " << std::endl
              << "   Et "  << m_crateEt << " overflow " << m_overflowT << std::endl
              << "   Ex "  << m_crateEx << " overflow " << m_overflowX << std::endl
              << "   Ey "  << m_crateEy << " overflow " << m_overflowY << std::endl;
              
  }
  
}

CrateEnergy::CrateEnergy(unsigned int crate, unsigned int et, unsigned int exTC,
             unsigned int eyTC, unsigned int overflowT, unsigned int overflowX,
	     unsigned int overflowY, bool restricted) :
  m_crate(crate),
  m_crateEt(0),
  m_crateEx(0),
  m_crateEy(0),
  m_overflowT(0),
  m_overflowX(0),
  m_overflowY(0),
  m_restricted(restricted),
  m_debug(false)
{
  /** Check crate in range */
  if (m_crate > 1) return;
  m_crateEt = et;
  m_crateEx = decodeTC(exTC);
  m_crateEy = decodeTC(eyTC);
  m_overflowT = overflowT;
  m_overflowX = overflowX;
  m_overflowY = overflowY;

  if (m_debug) {
    std::cout << "CrateEnergy: crate " << m_crate << " results " << std::endl
              << "   Et "  << m_crateEt << " overflow " << m_overflowT << std::endl
              << "   Ex "  << m_crateEx << " overflow " << m_overflowX << std::endl
              << "   Ey "  << m_crateEy << " overflow " << m_overflowY << std::endl;
              
  }
}


CrateEnergy::~CrateEnergy(){
}

/** return crate number */
unsigned int CrateEnergy::crate() const {
  return m_crate;
}

/** return crate Et */
int CrateEnergy::et() const {
  return m_crateEt;
}

/** return crate Ex */
int CrateEnergy::ex() const {
  return m_crateEx;
}

/** return crate Ey */
int CrateEnergy::ey() const {
  return m_crateEy;
}

/** return Et overflow bit */
unsigned int CrateEnergy::etOverflow() const {
  return m_overflowT;
}

/** return Ex overflow bit */
unsigned int CrateEnergy::exOverflow() const {
  return m_overflowX;
}

/** return Ey overflow bit */
unsigned int CrateEnergy::eyOverflow() const {
  return m_overflowY;
}

/** Full or Restricted eta range */
bool CrateEnergy::restricted() const {
  return m_restricted;
}

/** return crate Ex in 15-bit twos-complement format (hardware format) */
unsigned int CrateEnergy::exTC() const {
  return encodeTC(m_crateEx);
}

/** return crate Ey in 15-bit twos-complement format (hardware format) */
unsigned int CrateEnergy::eyTC() const {
  return encodeTC(m_crateEy);
}

/** encode int as 15-bit twos-complement format (hardware Ex/Ey format) */
unsigned int CrateEnergy::encodeTC(int input) const {
  unsigned int value;

  if (input > 0) {
    value = input;
  }
  else {
    value = (1<<m_sumBitsTC) + input;
  }

  int mask = (1<<m_sumBitsTC) - 1;
  return value&mask ;
}

/** decode 15-bit twos-complement format (hardware Ex/Ey format) as int */
int CrateEnergy::decodeTC(unsigned int input) const {

  int mask = (1<<m_sumBitsTC) - 1;
  int value = input&mask;

  if ((value >> (m_sumBitsTC - 1))) {
    value += (~0U) << m_sumBitsTC;
  }

  return value;
}
} // end of ns

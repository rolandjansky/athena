/***************************************************************************
                          JEMHits.cpp  -  description
                             -------------------
    begin                : 25 05 2006
    copyright            : (C) 2006 by Alan Watson
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



#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/JEMHits.h"
#else
#include "JEMHits.h"
#endif

namespace LVL1 {

JEMHits::JEMHits():
  m_crate(0),
  m_module(0),
  m_peak(0),
  m_JetHits(1)
{
}

JEMHits::~JEMHits(){
}

/** constructs a JEMHits object, specifying crate & module. */
LVL1::JEMHits::JEMHits::JEMHits(int crate, int module):
  m_crate(crate),
  m_module(module),
  m_peak(0),
  m_JetHits(1)
{
}

/** constructs a JEMHits object and fill all data members */
LVL1::JEMHits::JEMHits::JEMHits(int crate, int module, 
                            const std::vector<unsigned int>& JetHits,
			    int peak):
  m_crate(crate),
  m_module(module),
  m_peak(peak),
  m_JetHits(JetHits)
{
}


/** returns crate number */
int LVL1::JEMHits::crate() const{
	return m_crate;
}

/** returns module number */
int LVL1::JEMHits::module() const{
	return m_module;
}

/** returns jet hits for peak sample */
unsigned int LVL1::JEMHits::JetHits() const{
	return m_JetHits[m_peak];
}

/** returns jet hits */
const std::vector<unsigned int>& LVL1::JEMHits::JetHitsVec() const{
	return m_JetHits;
}

/** Central or FCAL JEM? */
bool LVL1::JEMHits::forward() const{
	return ( ((m_module%8) == 0) || ((m_module%8) == 7) );
}

/** Set Jet hits */
void LVL1::JEMHits::addJetHits(const std::vector<unsigned int>& hits) {
  m_JetHits = hits;
}

/** Specify peak slice  */
void LVL1::JEMHits::setPeak(int peak) {
  m_peak = peak;
}
                                                                                
/** returns peak slice number */
int LVL1::JEMHits::peak() const{
        return m_peak;
}

} // end of namespace bracket

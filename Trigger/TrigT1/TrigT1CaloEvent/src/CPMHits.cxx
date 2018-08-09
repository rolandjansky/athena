/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CPMHits.cpp  -  description
                             -------------------
    begin                : 25 05 2006
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/CPMHits.h"
#else
#include "CPMHits.h"
#endif

namespace LVL1 {

CPMHits::CPMHits():
  m_crate(0),
  m_module(0),
  m_peak(0),
  m_Hits0(1),
  m_Hits1(1)
{
}

CPMHits::~CPMHits(){
}

/** constructs a CPMHits object, specifying crate & module. */
LVL1::CPMHits::CPMHits::CPMHits(int crate, int module):
  m_crate(crate),
  m_module(module),
  m_peak(0),
  m_Hits0(1),
  m_Hits1(1)
{
}

/** constructs a CPMHits object and fill all data members */
LVL1::CPMHits::CPMHits::CPMHits(int crate, int module, 
                                const std::vector<unsigned int>& EMTauHits0,
                                const std::vector<unsigned int>& EMTauHits1, int peak):
  m_crate(crate),
  m_module(module),
  m_peak(peak),
  m_Hits0(EMTauHits0),
  m_Hits1(EMTauHits1)
{
}


/** returns crate number */
int LVL1::CPMHits::crate() const{
	return m_crate;
}

/** returns module number */
int LVL1::CPMHits::module() const{
	return m_module;
}

/** returns hits for peak sample */
unsigned int LVL1::CPMHits::HitWord0() const{
	return m_Hits0[m_peak];
}
unsigned int LVL1::CPMHits::HitWord1() const{
	return m_Hits1[m_peak];
}

/** returns hits */
const std::vector<unsigned int>& LVL1::CPMHits::HitsVec0() const{
	return m_Hits0;
}
const std::vector<unsigned int>& LVL1::CPMHits::HitsVec1() const{
	return m_Hits1;
}

/** Set hits */
void LVL1::CPMHits::addHits(const std::vector<unsigned int>& hits0, 
                            const std::vector<unsigned int>& hits1) {
  m_Hits0 = hits0;
  m_Hits1 = hits1;
}

/** Specify peak slice  */
void LVL1::CPMHits::setPeak(int peak) {
  m_peak = peak;
}
                                                                                
/** returns peak slice number */
int LVL1::CPMHits::peak() const{
        return m_peak;
}

} // end of namespace bracket

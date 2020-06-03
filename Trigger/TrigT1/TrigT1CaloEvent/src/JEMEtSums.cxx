/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JEMEtSums.cpp  -  description
                             -------------------
    begin                : 25 05 2006
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/JEMEtSums.h"
#else
#include "JEMEtSums.h"
#endif

namespace LVL1 {

JEMEtSums::JEMEtSums():
  m_crate(0),
  m_module(0),
  m_peak(0),
  m_Et(1),
  m_Ex(1),
  m_Ey(1)
{
}

JEMEtSums::~JEMEtSums(){
}

/** constructs a JEMEtSums object, specifying crate & module. */
LVL1::JEMEtSums::JEMEtSums::JEMEtSums(int crate, int module):
  m_crate(crate),
  m_module(module),
  m_peak(0),
  m_Et(1),
  m_Ex(1),
  m_Ey(1)
{
}

/** constructs a JEMEtSums object and fill all data members */
LVL1::JEMEtSums::JEMEtSums::JEMEtSums(int crate, int module, 
                            const std::vector<unsigned int>& Et, 
			    const std::vector<unsigned int>& Ex, 
			    const std::vector<unsigned int>& Ey,
			    int peak):
  m_crate(crate),
  m_module(module),
  m_peak(peak),
  m_Et(Et),
  m_Ex(Ex),
  m_Ey(Ey)
{
}


/** returns crate number */
int LVL1::JEMEtSums::crate() const{
	return m_crate;
}

/** returns module number */
int LVL1::JEMEtSums::module() const{
	return m_module;
}

/** returns module ET sum for peak sample */
unsigned int LVL1::JEMEtSums::Et() const{
	return m_Et[m_peak];
}

/** returns module Ex sum for peak sample */
unsigned int LVL1::JEMEtSums::Ex() const{
	return m_Ex[m_peak];
}

/** returns module Ey sum for peak sample */
unsigned int LVL1::JEMEtSums::Ey() const{
	return m_Ey[m_peak];
}

/** returns module ET sum */
const std::vector<unsigned int>& LVL1::JEMEtSums::EtVec() const{
	return m_Et;
}

/** returns module Ex sum */
const std::vector<unsigned int>& LVL1::JEMEtSums::ExVec() const{
	return m_Ex;
}

/** returns module Ey sum */
const std::vector<unsigned int>& LVL1::JEMEtSums::EyVec() const{
	return m_Ey;
}

/** Central or FCAL JEM? */
bool LVL1::JEMEtSums::forward() const{
	return ( ((m_module%8) == 0) || ((m_module%8) == 7) );
}

/** Update ET sum */
void LVL1::JEMEtSums::addEt(const std::vector<unsigned int>& Et) {
  m_Et = Et;
}

/** Update Ex sum */
void LVL1::JEMEtSums::addEx(const std::vector<unsigned int>& Ex) {
  m_Ex = Ex;
}

/** Update Ey sum */
void LVL1::JEMEtSums::addEy(const std::vector<unsigned int>& Ey) {
  m_Ey = Ey;
}

/** Specify peak slice (Same for all vectors) */
void LVL1::JEMEtSums::setPeak(int peak) {
  m_peak = peak;
}
                                                                                
/** returns peak slice number */
int LVL1::JEMEtSums::peak() const{
        return m_peak;
}

} // end of namespace bracket

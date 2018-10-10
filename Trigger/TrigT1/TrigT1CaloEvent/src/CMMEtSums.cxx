/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CMMEtSums.cpp  -  description
                             -------------------
    begin                : 25 05 2006
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/CMMEtSums.h"
#else
#include "CMMEtSums.h"
#endif

namespace LVL1 {

CMMEtSums::CMMEtSums():
  m_crate(0),
  m_dataID(0),
  m_peak(0),
  m_Et(1),
  m_Ex(1),
  m_Ey(1),
  m_EtError(1),
  m_ExError(1),
  m_EyError(1)
{
}

CMMEtSums::~CMMEtSums(){
}

/** constructs a CMMEtSums object, specifying crate, and data ID. */
CMMEtSums::CMMEtSums(int crate, int dataID):
  m_crate(crate),
  m_dataID(dataID),
  m_peak(0),
  m_Et(1),
  m_Ex(1),
  m_Ey(1),
  m_EtError(1),
  m_ExError(1),
  m_EyError(1)
{
}

/** constructs a CMMEtSums object and fill all data members */
CMMEtSums::CMMEtSums(int crate, int dataID,
                     const std::vector<unsigned int>& Et, 
		     const std::vector<unsigned int>& Ex, 
		     const std::vector<unsigned int>& Ey,
		     const std::vector<int>& EtError,
		     const std::vector<int>& ExError,
		     const std::vector<int>& EyError,
		     int peak):
  m_crate(crate),
  m_dataID(dataID),
  m_peak(peak),
  m_Et(Et),
  m_Ex(Ex),
  m_Ey(Ey),
  m_EtError(EtError),
  m_ExError(ExError),
  m_EyError(EyError)
{
}


/** returns crate number */
int CMMEtSums::crate() const{
	return m_crate;
}

/** returns data ID */
int CMMEtSums::dataID() const{
        return m_dataID;
}

/** returns module ET sum for peak sample */
unsigned int CMMEtSums::Et() const{
	return m_Et[m_peak];
}

/** returns module Ex sum for peak sample */
unsigned int CMMEtSums::Ex() const{
	return m_Ex[m_peak];
}

/** returns module Ey sum for peak sample */
unsigned int CMMEtSums::Ey() const{
	return m_Ey[m_peak];
}

/** returns module ET sum error for peak sample */
int CMMEtSums::EtError() const{
	return m_EtError[m_peak];
}

/** returns module Ex sum error for peak sample */
int CMMEtSums::ExError() const{
	return m_ExError[m_peak];
}

/** returns module Ey sum error for peak sample */
int CMMEtSums::EyError() const{
	return m_EyError[m_peak];
}

/** returns module ET sum */
const std::vector<unsigned int>& CMMEtSums::EtVec() const{
	return m_Et;
}

/** returns module Ex sum */
const std::vector<unsigned int>& CMMEtSums::ExVec() const{
	return m_Ex;
}

/** returns module Ey sum */
const std::vector<unsigned int>& CMMEtSums::EyVec() const{
	return m_Ey;
}

/** returns module ET errors */
const std::vector<int>& CMMEtSums::EtErrorVec() const{
	return m_EtError;
}

/** returns module Ex errors */
const std::vector<int>& CMMEtSums::ExErrorVec() const{
	return m_ExError;
}

/** returns module Ey errors */
const std::vector<int>& CMMEtSums::EyErrorVec() const{
	return m_EyError;
}

/** Update ET sum */
void CMMEtSums::addEt(const std::vector<unsigned int>& Et,
                      const std::vector<int>& EtError) {
  m_Et = Et;
  m_EtError = EtError;
}

/** Update Ex sum */
void CMMEtSums::addEx(const std::vector<unsigned int>& Ex,
                      const std::vector<int>& ExError) {
  m_Ex = Ex;
  m_ExError = ExError;
}

/** Update Ey sum */
void CMMEtSums::addEy(const std::vector<unsigned int>& Ey,
                      const std::vector<int>& EyError) {
  m_Ey = Ey;
  m_EyError = EyError;
}

/** Specify peak slice (Same for all vectors) */
void CMMEtSums::setPeak(int peak) {
  m_peak = peak;
}
                                                                                
/** returns peak slice number */
int CMMEtSums::peak() const{
        return m_peak;
}

} // end of namespace bracket

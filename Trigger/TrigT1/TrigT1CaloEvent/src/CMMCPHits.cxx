/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CMMCPHits.cpp  -  description
                             -------------------
    begin                : 25 05 2006
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/CMMCPHits.h"
#else
#include "CMMCPHits.h"
#endif

namespace LVL1 {

CMMCPHits::CMMCPHits():
  m_crate(0),
  m_dataID(0),
  m_peak(0),
  m_hits0(1),
  m_hits1(1),
  m_error0(1),
  m_error1(1)
{
}

CMMCPHits::~CMMCPHits(){
}

/** constructs a CMMCPHits object, specifying crate, data ID. */
CMMCPHits::CMMCPHits(int crate, int dataID):
  m_crate(crate),
  m_dataID(dataID),
  m_peak(0),
  m_hits0(1),
  m_hits1(1),
  m_error0(1),
  m_error1(1)
{
}

/** constructs a CMMCPHits object and fill all data members */
CMMCPHits::CMMCPHits(int crate, int dataID,
                     const std::vector<unsigned int>& hits0,
                     const std::vector<unsigned int>& hits1,
		     const std::vector<int>& error0,
		     const std::vector<int>& error1, int peak):
  m_crate(crate),
  m_dataID(dataID),
  m_peak(peak),
  m_hits0(hits0),
  m_hits1(hits1),
  m_error0(error0),
  m_error1(error1)
{
}


/** returns crate number */
int CMMCPHits::crate() const{
	return m_crate;
}

/** returns data ID of data */
int CMMCPHits::dataID() const{
        return m_dataID;
}

/** returns hits0 for peak sample */
unsigned int CMMCPHits::HitWord0() const{
	return m_hits0[m_peak];
}

/** returns hits1 for peak sample */
unsigned int CMMCPHits::HitWord1() const{
	return m_hits1[m_peak];
}

/** returns error0 for peak sample */
int CMMCPHits::Error0() const{
	return m_error0[m_peak];
}

/** returns error1 for peak sample */
int CMMCPHits::Error1() const{
	return m_error1[m_peak];
}

/** returns hits0 */
const std::vector<unsigned int>& CMMCPHits::HitsVec0() const{
	return m_hits0;
}

/** returns hits1 */
const std::vector<unsigned int>& CMMCPHits::HitsVec1() const{
	return m_hits1;
}

/** returns error0 */
const std::vector<int>& CMMCPHits::ErrorVec0() const{
	return m_error0;
}

/** returns error1 */
const std::vector<int>& CMMCPHits::ErrorVec1() const{
	return m_error1;
}

/** Set hits */
void CMMCPHits::addHits(const std::vector<unsigned int>& hits0,
                        const std::vector<unsigned int>& hits1,
			const std::vector<int>& error0,
                        const std::vector<int>& error1) {
  m_hits0 = hits0;
  m_hits1 = hits1;
  m_error0 = error0;
  m_error1 = error1;
}

/** Specify peak slice  */
void CMMCPHits::setPeak(int peak) {
  m_peak = peak;
}
                                                                                
/** returns peak slice number */
int CMMCPHits::peak() const{
        return m_peak;
}

} // end of namespace bracket

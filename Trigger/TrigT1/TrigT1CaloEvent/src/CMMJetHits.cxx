/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CMMJetHits.cpp  -  description
                             -------------------
    begin                : 25 05 2006
    email                : Alan.Watson@cern.ch
 ***************************************************************************/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/CMMJetHits.h"
#else
#include "CMMJetHits.h"
#endif

namespace LVL1 {

CMMJetHits::CMMJetHits():
  m_crate(0),
  m_dataID(0),
  m_peak(0),
  m_hits(1),
  m_errors(1)
{
}

CMMJetHits::~CMMJetHits(){
}

/** constructs a CMMJetHits object, specifying crate, data ID. */
CMMJetHits::CMMJetHits(int crate, int dataID):
  m_crate(crate),
  m_dataID(dataID),
  m_peak(0),
  m_hits(1),
  m_errors(1)
{
}

/** constructs a CMMJetHits object and fill all data members */
CMMJetHits::CMMJetHits(int crate, int dataID,
                       const std::vector<unsigned int>& hits,
		       const std::vector<int>& errors, int peak):
  m_crate(crate),
  m_dataID(dataID),
  m_peak(peak),
  m_hits(hits),
  m_errors(errors)
{
}


/** returns crate number */
int CMMJetHits::crate() const{
	return m_crate;
}

/** returns data ID of data */
int CMMJetHits::dataID() const{
        return m_dataID;
}

/** returns hits for peak sample */
unsigned int CMMJetHits::Hits() const{
	return m_hits[m_peak];
}

/** returns error for peak sample */
int CMMJetHits::Error() const{
	return m_errors[m_peak];
}

/** returns hits */
const std::vector<unsigned int>& CMMJetHits::HitsVec() const{
	return m_hits;
}

/** returns errors */
const std::vector<int>& CMMJetHits::ErrorVec() const{
	return m_errors;
}

/** Set hits */
void CMMJetHits::addHits(const std::vector<unsigned int>& hits,
                      const std::vector<int>& errors) {
  m_hits = hits;
  m_errors = errors;
}

/** Specify peak slice  */
void CMMJetHits::setPeak(int peak) {
  m_peak = peak;
}
                                                                                
/** returns peak slice number */
int CMMJetHits::peak() const{
        return m_peak;
}

} // end of namespace bracket

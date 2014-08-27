/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "TrigT1Result/RecRoIBResult.h"

using namespace std;

namespace ROIB {

  RecRoIBResult::RecRoIBResult( const std::vector< LVL1::RecEmTauRoI >& recEmTauRoIs,
                                const std::vector< LVL1::RecMuonRoI >& recMuonRoIs,
                                const std::vector< LVL1::RecJetRoI >& recJetRoIs )
    : m_recRoIBResult_RecEmTauRoIVector( recEmTauRoIs ),
      m_recRoIBResult_RecMuonRoIVector( recMuonRoIs ),
      m_recRoIBResult_RecJetRoIVector( recJetRoIs ) {

  }

  RecRoIBResult::RecRoIBResult()
    : m_recRoIBResult_RecEmTauRoIVector(),
      m_recRoIBResult_RecMuonRoIVector(),
      m_recRoIBResult_RecJetRoIVector() {

  }

  RecRoIBResult::~RecRoIBResult() {

  }

  const vector< LVL1::RecEmTauRoI >& RecRoIBResult::recEmTauRoIVector() const {
    return m_recRoIBResult_RecEmTauRoIVector;
  }

  const vector< LVL1::RecMuonRoI >& RecRoIBResult::recMuonRoIVector() const {
    return m_recRoIBResult_RecMuonRoIVector;
  }

  const vector< LVL1::RecJetRoI >& RecRoIBResult::recJetRoIVector() const {
    return m_recRoIBResult_RecJetRoIVector;
  }

} // namespace ROIB

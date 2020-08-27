/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "TrigT1Result/RecRoIBResult.h"

using namespace std;

namespace ROIB {

  RecRoIBResult::RecRoIBResult( std::vector< LVL1::RecEmTauRoI >&& recEmTauRoIs,
                                std::vector< LVL1::RecMuonRoI >&& recMuonRoIs,
                                std::vector< LVL1::RecJetRoI >&& recJetRoIs )
    : m_recRoIBResult_RecEmTauRoIVector( std::move(recEmTauRoIs) ),
      m_recRoIBResult_RecMuonRoIVector( std::move(recMuonRoIs) ),
      m_recRoIBResult_RecJetRoIVector( std::move(recJetRoIs) ) {

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

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************
*
* RecRoIBResult is the L1 reconstructed RDO structure 
* Author: Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
*
********************************************************** */
#ifndef TRIGT1RESULT_RECROIBRESULT_H
#define TRIGT1RESULT_RECROIBRESULT_H

// STL include(s):
#include <vector>

// TrigT1 include(s):
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"

namespace ROIB {

  class RecRoIBResult {

  public:

    /* Empty constructor */
    RecRoIBResult() = default;

    /* Full Constructor */
    RecRoIBResult( std::vector< LVL1::RecEmTauRoI >&& recEmTauRoIs,
                   std::vector< LVL1::RecMuonRoI >&& recMuonRoIs,
                   std::vector< LVL1::RecJetRoI >&& recJetRoIs );


    /* Gets the RecEmTauRoI part of the L1 reconstructed RDO */
    const std::vector< LVL1::RecEmTauRoI >& recEmTauRoIVector() const;
    /* Gets the RecMuonRoI part of the L1 reconstructed RDO */
    const std::vector< LVL1::RecMuonRoI >& recMuonRoIVector() const;
    /* Gets the RecJetRoI part of the L1 reconstructed RDO */
    const std::vector< LVL1::RecJetRoI >& recJetRoIVector() const;

  private:
    /* The members - vectors for Muon, EmTau, Jet and Energy RoIs */
    std::vector<LVL1::RecEmTauRoI> m_recRoIBResult_RecEmTauRoIVector;
    std::vector<LVL1::RecMuonRoI> m_recRoIBResult_RecMuonRoIVector;
    std::vector<LVL1::RecJetRoI> m_recRoIBResult_RecJetRoIVector;

  }; // class RecRoIBResult

} // namespace ROIB

/* The RecRoIBResult class_def part, defining a CLID of 6001 */
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ROIB::RecRoIBResult , 6001 , 0 )

#endif // TRIGT1RESULT_RECROIBRESULT_H

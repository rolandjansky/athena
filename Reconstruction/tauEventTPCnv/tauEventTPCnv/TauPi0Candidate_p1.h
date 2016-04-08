/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of persistent class for Analysis::TauPi0Candidate
 *
 * @author Felix Friedrich
 * @date October 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUPI0CANDIDATE_P1_H
#define TAUEVENTTPCNV_TAUPI0CANDIDATE_P1_H

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"

/// forward declarations
class TauPi0CandidateCnv_p1;

/// persistent class version 1 for TauPi0Candidate
class TauPi0Candidate_p1
{
  public:
  TauPi0Candidate_p1() {};
  
  /// necessary to have member data private
  friend class TauPi0CandidateCnv_p1;

  private:
  std::vector< TPObjRef > m_pi0ClusterVector; 
  
};


#endif

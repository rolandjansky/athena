/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of persistent class for Analysis::TauPi0Details
 *
 * @author Felix Friedrich
 * @date October 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUPI0DETAILS_P2_H
#define TAUEVENTTPCNV_TAUPI0DETAILS_P2_H

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"
#include "CLHEP/Vector/LorentzVector.h"

///  forward declarations
class TauPi0DetailsCnv_p2;

/// persistent class version 1 for TauPi0Details
class TauPi0Details_p2
{

public:
  TauPi0Details_p2() {};

  /// necessary to have member data private
  friend class  TauPi0DetailsCnv_p2;

private:
   std::vector< TPObjRef > m_pi0CandContainer;
   std::vector< TPObjRef > m_pi0ClusContainer;
   std::vector< TPObjRef > m_shotContainer;

   CLHEP::HepLorentzVector m_visTau_hlv;
    
};

#endif

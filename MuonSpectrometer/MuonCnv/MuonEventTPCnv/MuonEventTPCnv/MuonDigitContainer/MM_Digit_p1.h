/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDIGIT_P1_H
#define MMDIGIT_P1_H

//-----------------------------------------------------------------------------
//
// file:   MM_Digit_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
  /**
 Persistent representation of Muon::MmDigit
  */
    class MM_Digit_p1
    {
    public:
      MM_Digit_p1() : m_muonId(0){}

        /// @name Data from Muon::MmDigit
        //@{
      Identifier32::value_type  m_muonId;

       /** strip response */
      std::vector<float> m_stripResponseTime;
      std::vector<float> m_stripResponseCharge;
      std::vector<int> m_stripResponsePosition;
       /** VMM chip response */
      std::vector<float> m_chipResponseTime;
      std::vector<float> m_chipResponseCharge;
      std::vector<int> m_chipResponsePosition;

      float m_stripTimeForTrigger;
      int m_stripPositionForTrigger;
      float m_stripChargeForTrigger;

      int m_MMFE_VMM_idForTrigger;
      int m_VMM_idForTrigger;
    };
}

#endif

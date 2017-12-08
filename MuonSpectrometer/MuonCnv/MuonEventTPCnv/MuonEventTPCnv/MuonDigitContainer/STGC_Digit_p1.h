/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCDIGIT_P1_H
#define STGCDIGIT_P1_H

//-----------------------------------------------------------------------------
//
// file:   STGC_Digit_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"
/*******************************************************************************/
namespace Muon
{
  /** 
      Persistent representation of Muon::sTgcDigit
  */
  class STGC_Digit_p1
  {
  public:
    STGC_Digit_p1() : m_muonId(0),m_bcTag(0),m_charge(0.0),m_time(0.0),m_isDead(false),m_isPileup(false) {} 
                
      /// @name Data from Muon::sTgcDigit
      //@{
      Identifier32::value_type  m_muonId;
      uint16_t  m_bcTag;
      float m_charge; 
      float m_time;
      bool m_isDead;
      bool m_isPileup;
  };
}
/*******************************************************************************/
#endif 

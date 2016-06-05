/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MmDigit.cxx

#include "MuonDigitContainer/MmDigit.h"

//**********************************************************************
// Local definitions
//**********************************************************************
namespace {

  // Range of time.
  //const int timemin = 0;
  //const int timemax = 4096;

}
//**********************************************************************
// Member functions.
//**********************************************************************

// Default constructor.

MmDigit::MmDigit()
  : MuonDigit(),
    m_stripResponseTime(0),
    m_stripResponseCharge(0),
    m_stripResponsePosition(0),
    m_chipResponseTime(0),
    m_chipResponseCharge(0),
    m_chipResponsePosition(0),
    m_stripTimeForTrigger(0),
    m_stripPositionForTrigger(0),
    m_stripChargeForTrigger(0),
    m_MMFE_VMM_idForTrigger(0),
    m_VMM_idForTrigger(0) { }
 
//**********************************************************************/
// Full constructor from Identifier.
MmDigit::MmDigit(const Identifier& id)
  : MuonDigit(id),
    m_stripResponseTime(0),
    m_stripResponseCharge(0),
    m_stripResponsePosition(0),
    m_chipResponseTime(0),
    m_chipResponseCharge(0),
    m_chipResponsePosition(0),
    m_stripTimeForTrigger(0),
    m_stripPositionForTrigger(0),
    m_stripChargeForTrigger(0),
    m_MMFE_VMM_idForTrigger(0),
    m_VMM_idForTrigger(0) { }

//**********************************************************************/
  
// Full constructor, with trigger Info

MmDigit::MmDigit(const Identifier& id,
                 const std::vector<float>& stripResponseTime,
                 const std::vector<int>& stripResponsePosition,
                 const std::vector<float>& stripResponseCharge,
                 const std::vector<float>& chipResponseTime,
                 const std::vector<int>& chipResponsePosition,
                 const std::vector<float>& chipResponseCharge,
                 const std::vector<float>& stripTimeForTrigger,
                 const std::vector<int>& stripPositionForTrigger,
                 const std::vector<float>& stripChargeForTrigger,
                 const std::vector<int>& MMFE_VMM_idForTrigger,
                 const std::vector<int>& VMM_idForTrigger)  :
    MuonDigit(id),
    m_stripResponseTime(stripResponseTime),
    m_stripResponseCharge(stripResponseCharge),
    m_stripResponsePosition(stripResponsePosition),
    m_chipResponseTime(chipResponseTime),
    m_chipResponseCharge(chipResponseCharge),
    m_chipResponsePosition(chipResponsePosition),
    m_stripTimeForTrigger(stripTimeForTrigger),
    m_stripPositionForTrigger(stripPositionForTrigger),
    m_stripChargeForTrigger(stripChargeForTrigger),
    m_MMFE_VMM_idForTrigger(MMFE_VMM_idForTrigger),
    m_VMM_idForTrigger(VMM_idForTrigger) { }
//**********************************************************************/
 
// Validity check.
bool MmDigit::is_valid(const MmIdHelper * /*mmHelper*/) const {
  return true; /*( (m_time>=timemin && m_time<=timemax) && (mmHelper->valid(m_muonId)));*/
}


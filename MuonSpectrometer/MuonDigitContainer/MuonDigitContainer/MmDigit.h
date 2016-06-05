/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MmDigit.h

#ifndef MmDigitUH
#define MmDigitUH

// Nektarios Chr. Benekos
// March 2013
// edit for micromegas needs: Karakostas Konstantinos   <Konstantinos.Karakostas@cern.ch>
//
// MicroMegas digitization.

#include <iosfwd>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/MmIdHelper.h"

class MmDigit : public MuonDigit {

 private:  // data
 /** strip response info */
  std::vector<float> m_stripResponseTime;
  std::vector<float> m_stripResponseCharge;
  std::vector<int> m_stripResponsePosition;
   /** VMM chip response info */
  std::vector<float> m_chipResponseTime;
  std::vector<float> m_chipResponseCharge;
  std::vector<int> m_chipResponsePosition;
  
  std::vector<float> m_stripTimeForTrigger;
  std::vector<int> m_stripPositionForTrigger;
  std::vector<float> m_stripChargeForTrigger;
    
  std::vector<int> m_MMFE_VMM_idForTrigger;
  std::vector<int> m_VMM_idForTrigger;
   
 public:  // functions

  /** Default constructor */
  MmDigit();

  /** Full constructor --- From Identifier. */ 
  MmDigit(const Identifier& id);

  /** Full constructor --- From Identifier and time */ 
 
  MmDigit(const Identifier& id,
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
          const std::vector<int>& VMM_idForTrigger);

  // Is this a valid digit?
  bool is_valid(const MmIdHelper * mmHelper) const;
  
   /** strip response info */
  /** Return the time */
  const std::vector<float>& stripResponseTime() const { return m_stripResponseTime; }
  /** Return the strip position */
  const std::vector<int>& stripResponsePosition() const { return m_stripResponsePosition; }
  /** Return the charge */
  const std::vector<float>& stripResponseCharge() const { return m_stripResponseCharge; }
    
  /** Return the time for trigger*/
  const std::vector<float>& stripTimeForTrigger() const { return m_stripTimeForTrigger; }
  /** Return the strip position for trigger*/
  const std::vector<int>& stripPositionForTrigger() const { return m_stripPositionForTrigger; }
  /** Return the charge for trigger*/
  const std::vector<float>& stripChargeForTrigger() const { return m_stripChargeForTrigger; }
  
  /** Retrum the VMM id*/
  const std::vector<int>& MMFE_VMM_idForTrigger() const { return m_MMFE_VMM_idForTrigger; }
  const std::vector<int>& VMM_idForTrigger() const { return m_VMM_idForTrigger; }


   /** chip response info */
  /** return the time */
  const std::vector<float>& chipResponseTime() const { return m_chipResponseTime; }
  /** Return the strip position */
  const std::vector<int>& chipResponsePosition() const { return m_chipResponsePosition; }
  /** Return the charge */
  const std::vector<float>& chipResponseCharge() const { return m_chipResponseCharge; }



};

#endif

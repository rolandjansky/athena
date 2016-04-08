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
  
  int m_stripForTrigger;
  float m_stripTimeForTrigger;
   
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
          int stripForTrigger,
          float stripTimeForTrigger);

  // Is this a valid digit?
  bool is_valid(const MmIdHelper * mmHelper) const;
  
   /** strip response info */
  /** return the time */
  const std::vector<float>& stripResponseTime() const { return m_stripResponseTime; }
  /** Return the strip position */
  const std::vector<int>& stripResponsePosition() const { return m_stripResponsePosition; }
  /** Return the charge */
  const std::vector<float>& stripResponseCharge() const { return m_stripResponseCharge; }
  /** Return the stripForTrigger */
  int stripForTrigger() const { return m_stripForTrigger; }
  /** Return the charge */
  float stripTimeForTrigger() const { return m_stripTimeForTrigger; }

   /** chip response info */
  /** return the time */
  const std::vector<float>& chipResponseTime() const { return m_chipResponseTime; }
  /** Return the strip position */
  const std::vector<int>& chipResponsePosition() const { return m_chipResponsePosition; }
  /** Return the charge */
  const std::vector<float>& chipResponseCharge() const { return m_chipResponseCharge; }



};

#endif

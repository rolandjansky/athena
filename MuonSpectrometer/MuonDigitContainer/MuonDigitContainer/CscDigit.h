/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscDigit.h

#ifndef CscDigitUH
#define CscDigitUH

// Augmented by Woochun Park
// November 3, 2010

// ketevi A. Assamagan
// September 30 2002
//
// CSC digitization. Holds a channel ID.

#include <iosfwd>
#include <vector>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/CscIdHelper.h"

class CscDigit : public MuonDigit {

private:

  int m_charge;
  float m_time;
  std::vector<float> m_sampleCharges;
  
public:

  /** Default constructor */
  CscDigit();

  /** Full constructor from Identifier */
  CscDigit(const Identifier& id, int charge);
  CscDigit(const Identifier& id, int charge, float time);
  // added 11/3/2010 for new EDM...
  CscDigit(const Identifier& id, std::vector<float> sampleCharges);


  /** Is this a valid digit */
  bool is_valid(const CscIdHelper * cscHelper) const;

  /** Return the charge */
  /** in new EDM, it's max charge **/
  int charge() const { return m_charge; }

  /** return the time */
  /** in new EDM, it's obsolete (return 0)  **/ 
  float time() const { return m_time; }

  const std::vector<float>  sampleCharges() const { return m_sampleCharges; }
  
};

#endif







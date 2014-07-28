/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTDIGIT_H
#define TRTDIGIT_H

/**
 * Class for TRT digits
 */
class TRTDigit {
public:

  /** Construct new digit */
  TRTDigit(const int& strawID, const unsigned& digit)
    : m_strawID(strawID), m_digit(digit) {}

  /** Empty, useless digit */
  TRTDigit() : m_strawID(0), m_digit(0) {}

  ~TRTDigit() {}

  /** Get straw ID */
  int GetStrawID() const { return m_strawID; }

  /** Get digit */
  unsigned GetDigit() const { return m_digit; }

private:
  int m_strawID;    /**< Straw ID                      */
  unsigned m_digit; /**< Digit (encoded 27 bit word)   */
};

#endif

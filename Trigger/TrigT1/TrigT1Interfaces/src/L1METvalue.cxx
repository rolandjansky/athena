/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          L1METvalue.cxx  -  description
                             -------------------
    begin                : 19-10-2007
    email                : Alan.Watson@cern.ch
 
    A simple set of functions to calculate ETmiss at the precision available
    in the L1 thresholding logic (depending on values of Ex & Ey).

    Because of the way the thresholding logic works, Ex or Ey values outside
    the defined range will simply cause all thresholds to be set (irrespective
    of values). In this case there is no "correct" ETmiss value to return, and
    so we (a) return a boolean flag to indicate that overflow has occurred,
    and (b) set the returned ETmiss value to 1 TeV (outside the threshold
    range). The flag should however always be checked before using the ETmiss
    value.
 ***************************************************************************/

#include "TrigT1Interfaces/L1METvalue.h"
#include <cmath>

namespace LVL1 {


void LVL1::L1METvalue::calcL1METQ(int Ex, int Ey, int& METQ, bool& Overflow) {

  /** Greater of 2 values determines range of bits used */
  unsigned int absEx = std::abs(Ex);
  unsigned int absEy = std::abs(Ey);
  int max = (absEx > absEy ? absEx : absEy);

  /** If Ex/Ey overflows the LUT input range, trigger fires all thresholds.
      Indicate this with overflow flag and an out of range ETmiss value */
  if ( max >= (1<<(m_nBits+m_nRanges-1)) )  {
    METQ = 16777216; // 4096**2
    Overflow = true;
  }
  /** Otherwise, emulate precision by checking which ET range we are in and
      zeroing bits below that. */
  else {
    for (unsigned int range = 0; range < m_nRanges; ++range) {
      if ( max < (1<<(m_nBits+range)) ) {
        absEx &= (m_mask<<range);
        absEy &= (m_mask<<range);
        break;
      }
    }
    METQ = absEx*absEx + absEy*absEy;
    Overflow = false;
  }
   
}

void LVL1::L1METvalue::calcL1MET(int Ex, int Ey, float& MET, bool& Overflow) {
  int METQ;
  calcL1METQ(Ex, Ey, METQ, Overflow);
  MET = sqrt(static_cast<float>(METQ));
}

void LVL1::L1METvalue::calcL1MET(int Ex, int Ey, int& MET, unsigned int& Range, bool& Overflow) {
  
  /** Greater of 2 values determines range of bits used */
  unsigned int absEx = std::abs(Ex);
  unsigned int absEy = std::abs(Ey);
  int max = (absEx > absEy ? absEx : absEy);

  /** If Ex/Ey overflows the LUT input range, trigger fires all thresholds.
      Indicate this with overflow flag and an out of range ETmiss value */
  if ( max >= (1<<(m_nBits+m_nRanges-1)) )  {
    MET = m_rangeMask + m_valueMask; // 7 bit ET + 2 bit range, all bits set
    Overflow = true;
  }
  /** Otherwise, emulate precision by checking which ET range we are in and
      zeroing bits below that. */
  else {
    Range = 0;
    for (unsigned int range = 0; range < m_nRanges; ++range) {
      if ( max < (1<<(m_nBits+range)) ) {
        absEx &= (m_mask<<range);
        absEy &= (m_mask<<range);
        Range = range;
        break;
      }
    }
    // Shift Ex/Ey values to lowest 6 bits
    absEx = (absEx >> Range);
    absEy = (absEy >> Range);
    // Calculate 7 bit MET value, rounded up to the ceiling 
    int METQ = absEx*absEx + absEy*absEy;
    MET = ceil(sqrt(static_cast<float>(METQ)));
    Overflow = false;
  }

}

void LVL1::L1METvalue::calcL1METSig(int Ex, int Ey, int SumET, float a, float b, int XEmin, int XEmax, int sqrtTEmin, int sqrtTEmax,
                                    float& XS, int& OutOfRange) {
  
  /// Calculate MET value with correct precision and format
  bool overflow = false;
  int MET = 0;
  unsigned int Range = 0;
  calcL1MET(Ex, Ey, MET, Range, overflow);
  
  ///Use that plus provided SumET and coefficients to calculate METSig
  calcL1METSig(MET, Range, SumET, a, b, XEmin, XEmax, sqrtTEmin, sqrtTEmax, XS, OutOfRange);
  
}

void LVL1::L1METvalue::calcL1METSig(int MET, unsigned int Range, int SumET, float a, float b, int XEmin, int XEmax, int sqrtTEmin, int sqrtTEmax,
                                    float& XS, int& OutOfRange) {
 
  /// Initialise default values
  XS = -1.;
  OutOfRange = 0;
    
  /// Impose firmware limitations on specified ranges
  if (XEmax > m_xsXEmax)          XEmax     = m_xsXEmax;
  if (sqrtTEmax > m_xsSqrtTEmax)  sqrtTEmax = m_xsSqrtTEmax;
  
  /// Convert MET value to single linear scale
  int XE = (MET<<Range);
  
  /// Impose hardware limits on MET input to LUT
  if (XE > m_xsXEmax) XE = m_xsXEmax;

  /// MET overflow check takes precedence over all other conditions
  if (XE >= XEmax) {
    XS = 9999.;
    OutOfRange = 1;
    return;
  }

  /// Protection against invalid input (can never happen in reality, but this is software)
  if (SumET < 0) {
     OutOfRange = -1;
     return;
  }
  
  /// Calculate sqrt(SumET) with integer precision & hardware limits
  int sqrtSumET = sqrt(SumET);
  if (sqrtSumET > m_xsSqrtTEmax) sqrtSumET = m_xsSqrtTEmax;
 
  if (XE < XEmin || sqrtSumET >= sqrtTEmax || sqrtSumET < sqrtTEmin) {
    //XS = 0.;
    OutOfRange = -1;
  }
  /// If in range, calculate XS
  else { 
    OutOfRange = 0;
    if (sqrtSumET > b) XS = float(XE)/(a*(sqrtSumET-b));
  }

}

} // end of namespace bracket

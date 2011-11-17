///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcDigits_p1.h 
// Header file for class ZdcDigits_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef ZDCTPCNV_ZDCDIGITS_P1_H
#define ZDCTPCNV_ZDCDIGITS_P1_H

#include <vector>

// forward declarations
class ZdcDigitsCnv_p1;

class ZdcDigits_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class ZdcDigitsCnv_p1;

public:

  /** Default constructor: 
   */
  ZdcDigits_p1() {m_id=0;}

private:

  unsigned int m_id;
  std::vector<int> m_digits_gain0_delay0;
  std::vector<int> m_digits_gain0_delay1;
  std::vector<int> m_digits_gain1_delay0;
  std::vector<int> m_digits_gain1_delay1;

};

#endif //> ZDCTPCNV_ZDCDIGITS_P1_H


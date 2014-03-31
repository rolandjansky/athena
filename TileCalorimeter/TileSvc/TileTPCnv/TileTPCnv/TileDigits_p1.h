///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigits_p1.h 
// Header file for class TileDigits_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEDIGITS_P1_H
#define TILETPCNV_TILEDIGITS_P1_H

#include <vector>

// forward declarations
class TileDigitsCnv_p1;

class TileDigits_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileDigitsCnv_p1;

public:

  /** Default constructor: 
   */
  TileDigits_p1() : m_channelID(0), m_digits() {}

private:

  unsigned int m_channelID;
  std::vector<short> m_digits;

};

#endif //> TILETPCNV_TILEDIGITS_P1_H


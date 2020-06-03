///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigits_p2.h 
// Header file for class TileDigits_p2
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEDIGITS_P2_H
#define TILETPCNV_TILEDIGITS_P2_H

#include <vector>

// forward declarations
class TileDigitsCnv_p2;

class TileDigits_p2 {

  // Make the AthenaPoolCnv class our friend
  friend class TileDigitsCnv_p2;

public:

  /** Default constructor: 
   */
  TileDigits_p2() : m_channelID(0), m_digits() {}

private:

  unsigned int m_channelID;
  std::vector<short> m_digits;

};

#endif //> TILETPCNV_TILEDIGITS_P2_H


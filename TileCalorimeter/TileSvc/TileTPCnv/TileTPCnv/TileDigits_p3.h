///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigits_p3.h 
// Header file for class TileDigits_p3
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEDIGITS_P3_H
#define TILETPCNV_TILEDIGITS_P3_H

#include <vector>

// forward declarations
class TileDigitsCnv_p3;

class TileDigits_p3 {

  // Make the AthenaPoolCnv class our friend
  friend class TileDigitsCnv_p3;

public:

  /** Default constructor: 
   */
  TileDigits_p3() : m_channelID(0), m_digits() {}

private:

  unsigned int m_channelID;
  std::vector<float> m_digits;

};

#endif //> TILETPCNV_TILEDIGITS_P3_H


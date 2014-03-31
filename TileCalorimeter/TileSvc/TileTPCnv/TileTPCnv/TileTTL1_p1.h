///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1_p1.h 
// Header file for class TileTTL1_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILETTL1_P1_H
#define TILETPCNV_TILETTL1_P1_H

#include <vector>

// forward declarations
class TileTTL1Cnv_p1;

class TileTTL1_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileTTL1Cnv_p1;

public:

  /** Default constructor: 
   */
  TileTTL1_p1() : m_channelID(0), m_TTL1digits() {}

private:

  unsigned int m_channelID;
  std::vector<float> m_TTL1digits;

};

#endif //> TILETPCNV_TILETTL1_P1_H


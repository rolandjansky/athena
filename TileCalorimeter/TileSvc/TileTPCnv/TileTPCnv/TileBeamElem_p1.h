///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileBeamElem_p1.h 
// Header file for class TileBeamElem_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEBEAMELEM_P1_H
#define TILETPCNV_TILEBEAMELEM_P1_H

#include <vector>
#include <inttypes.h>

// forward declarations
class TileBeamElemCnv_p1;

class TileBeamElem_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileBeamElemCnv_p1;

public:

  /** Default constructor: 
   */
  TileBeamElem_p1() : m_channelID(0), m_digits() {}

private:

  unsigned int m_channelID;
  std::vector<uint16_t> m_digits;

};

#endif //> TILETPCNV_TILEBEAMELEM_P1_H


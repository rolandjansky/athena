///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1Cell_p1.h 
// Header file for class TileTTL1Cell_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILETTL1CELL_P1_H
#define TILETPCNV_TILETTL1CELL_P1_H

#include <vector>

// forward declarations
class TileTTL1CellCnv_p1;

class TileTTL1Cell_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileTTL1CellCnv_p1;

public:

  /** Default constructor: 
   */
  TileTTL1Cell_p1() : m_channelID(0), m_eneTower(0.0), 
                      m_timeAve(0.0), m_corrFactor(0.0),
                      m_quality(0) {}

private:

  unsigned int m_channelID;
  float m_eneTower;
  float m_timeAve;
  float m_corrFactor;
  uint16_t m_quality;

};

#endif //> TILETPCNV_TILETTL1CELL_P1_H


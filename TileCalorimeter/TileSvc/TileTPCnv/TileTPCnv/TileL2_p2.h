///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileL2_p2.h 
// Header file for class TileL2_p2
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEL2_P2_H
#define TILETPCNV_TILEL2_P2_H

#include <vector>

// forward declarations
class TileL2Cnv_p2;

class TileL2_p2 {

  // Make the AthenaPoolCnv class our friend
  friend class TileL2Cnv_p2;

public:

  /** Default constructor: 
   */
  TileL2_p2() : m_ID(0), m_ival(), m_fval() {}

private:

  int m_ID;
  std::vector<unsigned int> m_ival;
  std::vector<float> m_fval;

};

#endif //> TILETPCNV_TILEL2_P2_H

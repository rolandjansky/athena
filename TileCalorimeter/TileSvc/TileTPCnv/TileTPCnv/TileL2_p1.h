///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileL2_p1.h 
// Header file for class TileL2_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEL2_P1_H
#define TILETPCNV_TILEL2_P1_H

#include <vector>

// forward declarations
class TileL2Cnv_p1;

class TileL2_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileL2Cnv_p1;

public:

  /** Default constructor: 
   */
  TileL2_p1() : m_ID(0), m_val(), m_eta(), m_phi(0.0),
                m_enemu0(), m_enemu1(), m_enemu2(), 
                m_quality_factor(), m_Et(0.0) {}

private:

  int m_ID;
  std::vector<unsigned int> m_val;
  std::vector<float> m_eta;
  float m_phi;
  std::vector<float> m_enemu0;
  std::vector<float> m_enemu1;
  std::vector<float> m_enemu2;
  std::vector<unsigned int> m_quality_factor;
  float m_Et;

};

#endif //> TILETPCNV_TILEL2_P1_H

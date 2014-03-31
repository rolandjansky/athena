///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMu_p1.h 
// Header file for class TileMu_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEMU_P1_H
#define TILETPCNV_TILEMU_P1_H

#include <vector>

// forward declarations
class TileMuCnv_p1;

class TileMu_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileMuCnv_p1;

public:

  /** Default constructor: 
   */
    TileMu_p1() : m_eta(0.0), m_phi(0.0), m_energy_deposited(), m_quality_factor() {}  

private:

  float m_eta;
  float m_phi;
  std::vector<float> m_energy_deposited;
  float m_quality_factor;

};

#endif //> TILETPCNV_TILEMU_P1_H

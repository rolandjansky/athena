/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATA_P2_H
#define INDETSIMDATA_P2_H

#include <vector>                                                      
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"
                                                                                                       
class InDetSimData_p2 {
 public:
  InDetSimData_p2()  {};
// List of Cnv classes that convert this into SimData objects
  friend class InDetSimDataCnv_p2;
 private:
  unsigned int m_word; // sim data word
  std::vector<HepMcParticleLink_p2> m_links; // HepMCPLs
  std::vector<float> m_enDeposits; //  energy deposits
};
                                                                                                                                                             
#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATA_P1_H
#define INDETSIMDATA_P1_H

#include <vector>                                                      
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"
                                                                                                       
class InDetSimData_p1 {
 public:
  InDetSimData_p1()  {};
// List of Cnv classes that convert this into SimData objects
  friend class InDetSimDataCnv_p1;
  //private:
  unsigned int m_word; // sim data word
  std::vector<HepMcParticleLink_p1> m_links; // HepMCPLs
  std::vector<float> m_enDeposits; //  energy deposits
};
                                                                                                                                                             
#endif


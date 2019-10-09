/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATA_P3_H
#define INDETSIMDATA_P3_H

#include <vector>                                                      
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p3.h"
                                                                                                       
class InDetSimData_p3 {
 public:
  InDetSimData_p3()  {};
// List of Cnv classes that convert this into SimData objects
  friend class InDetSimDataCnv_p3;
 private:
  unsigned int m_word; // sim data word
  std::vector<HepMcParticleLink_p3> m_links; // HepMCPLs
  std::vector<float> m_enDeposits; //  energy deposits
};
                                                                                                                                                             
#endif


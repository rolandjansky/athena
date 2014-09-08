/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPSYMMATRIX_P1_TRK_H
#define HEPSYMMATRIX_P1_TRK_H

#include <vector>

namespace Trk {

   class HepSymMatrix_p1
   {
  public:
     HepSymMatrix_p1() : m_nrow(0) {}

     #ifdef UseFloatsIn_HepSymMatrix
     std::vector<float>	m_matrix_val;
     #else
     std::vector<double> m_matrix_val;
     #endif
     int                   	m_nrow;
   };
}

#endif

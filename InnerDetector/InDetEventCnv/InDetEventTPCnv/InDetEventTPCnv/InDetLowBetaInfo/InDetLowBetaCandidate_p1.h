/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetLowBetaCandidate_P1_H
#define InDetLowBetaCandidate_P1_H

#include <vector>
 class InDetLowBetaCandidate_p1
 {
  public:
   InDetLowBetaCandidate_p1()  {};
  
   //private:
 
   std::vector<float> m_data;
   int	m_TRTNLastBits;
  
 };
 
#endif

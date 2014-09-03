/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCLUSTERMOMENTCONTAINER_P2_H
#define CALOATHENAPOOL_CALOCLUSTERMOMENTCONTAINER_P2_H
#include <vector>
//#include "CaloTPCnv/Compressor.h"

class CaloClusterMomentContainer_p2 {
public:
   
  unsigned short m_nMoments; 
//  typedef std::vector<float>::const_iterator const_iterator;
  typedef std::vector<unsigned int>::const_iterator const_iterator;
 
//std::vector<float> m_Mvalue;
std::vector<unsigned int> m_Mvalue;
std::vector<unsigned short> m_Mkey;
		
  
};

#endif




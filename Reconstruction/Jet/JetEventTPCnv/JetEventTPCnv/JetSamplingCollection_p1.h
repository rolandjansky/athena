///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollection_p1.h 
// Header file for class JetSamplingCollection_p1
// Author: S.Binet<binet@cern.ch>
// Date:   July 2007
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLINGCOLLECTION_P1_H 
#define JETEVENTTPCNV_JETSAMPLINGCOLLECTION_P1_H 

// STL includes
#include <vector>

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSampling_p1.h"

// forward declare
class JetSamplingCollectionCnv_p1;

class JetSamplingCollection_p1 : public std::vector<JetSampling_p1>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetSamplingCollectionCnv_p1;

};

#endif //> JETEVENTTPCNV_JETSAMPLINGCOLLECTION_P1_H

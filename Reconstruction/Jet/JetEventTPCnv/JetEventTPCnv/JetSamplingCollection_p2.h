///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollection_p2.h 
// Header file for class JetSamplingCollection_p2
// Author: Belen.Salvachua@cern.ch
// Date:   Dec 2008
//    Based on JetSamplingCollection_p1.h
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLINGCOLLECTION_P2_H 
#define JETEVENTTPCNV_JETSAMPLINGCOLLECTION_P2_H 

// STL includes
#include <vector>

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSampling_p2.h"

// forward declare
class JetSamplingCollectionCnv_p2;

class JetSamplingCollection_p2 : public std::vector<JetSampling_p2>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetSamplingCollectionCnv_p2;

};

#endif //> JETEVENTTPCNV_JETSAMPLINGCOLLECTION_P2_H

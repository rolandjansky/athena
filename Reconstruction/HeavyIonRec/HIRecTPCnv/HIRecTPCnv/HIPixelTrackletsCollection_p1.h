/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSCOLLECTION_P1_H
#define HIPIXELTRACKLETSCOLLECTION_P1_H

//#include "HIRecTPCnv/HIPixelTracklets_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class HIPixelTrackletsCollection_p1 
: public std::vector<TPObjRef>  {
  
 public:
  //default constructor
  HIPixelTrackletsCollection_p1() {};
  
};



#endif

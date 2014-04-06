/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOP_OVERLAP_VECTOR_H
#define TOP_OVERLAP_VECTOR_H

#include "TopInputs/TopOverlap.h"
#include "CLIDSvc/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  class TopOverlapVector : public std::vector<TopOverlap> {};

}

//CLASS_DEF( TopRec::TopOverlapVector, 2403670, 1 )

#endif // TOP_OVERLAP_VECTOR_H

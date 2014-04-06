/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOP_OVERLAP_TRIGGER_VECTOR_H
#define TOP_OVERLAP_TRIGGER_VECTOR_H

#include "TopInputs/TopOverlapTrigger.h"
#include "CLIDSvc/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  class TopOverlapTriggerVector : public std::vector<TopOverlapTrigger> {};

}

//CLASS_DEF( TopRec::TopOverlapTriggerVector, 202084599, 1 )

#endif // TOP_OVERLAP_TRIGGER_VECTOR_H

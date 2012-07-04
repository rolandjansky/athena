/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArSimEvent_LArHitFloatContainer_h
#define LArSimEvent_LArHitFloatContainer_h

#include "LArSimEvent/LArHitFloat.h"
#include "CLIDSvc/CLASS_DEF.h" 
#include <vector>

/**
@class LArHitFloatContainer
@brief Container for LArHitFloat
*/

class LArHitFloatContainer: public std::vector<LArHitFloat>
{

 public:
  /** constructor */
  LArHitFloatContainer() {};

   /** desctructor */
  virtual ~LArHitFloatContainer() {};

  inline void add (const LArHitFloat& hit)
     {this->push_back(hit);}

};

CLASS_DEF(LArHitFloatContainer,1247689295,0)

#endif

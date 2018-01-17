/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBHITPLANECONT_H
#define TBHITPLANECONT_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TBEvent/TBHitPlane.h"

/**
@class TBHitPlaneCont TBHitPlaneCont.h "TBEvent/TBHitPlaneCont.h"
Holds a set of TBHitPlane objects.

@author M. Schram
*/

class TBHitPlaneCont : public DataVector< TBHitPlane > {
 public:
  /** Constructor */
  TBHitPlaneCont() : DataVector< TBHitPlane >() { }

  /** destructor  */
 virtual ~TBHitPlaneCont() { }

} ;
CLASS_DEF( TBHitPlaneCont , 226587971 , 1 )

#endif 

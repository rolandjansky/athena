/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TBEvent/TBHit.h"

#ifndef TBHITCONT_H
#define TBHITCONT_H

/** @class TBHitCont TBHitCont.h 
    "TBEvent/TBHitCont.h"
    
    @author M. Schram
*/

class TBHitCont : public DataVector< TBHit > {
 public:
  /** Constructor */
  TBHitCont() : DataVector< TBHit >() { }
    
  /** destructor  */
 virtual ~TBHitCont() { } 

} ;
CLASS_DEF( TBHitCont , 84246700 , 1 )

#endif

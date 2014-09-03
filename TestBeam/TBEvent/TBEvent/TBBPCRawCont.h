/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TBEvent/TBBPCRaw.h"

#ifndef TBBPCRAWCONT_H
#define TBBPCRAWCONT_H

/** @class TBBPCRawCont TBBPCRawCont.h 
    "TBEvent/TBBPCRawCont.h"
    
    @author P.A. Delsart
*/
class TBBPCRawCont : public DataVector<TBBPCRaw> {
 public:
  /** Constructor */
  TBBPCRawCont() : DataVector<TBBPCRaw>() { }
  

  
  /** destructor  */
  virtual ~TBBPCRawCont() { } 

} ;

CLASS_DEF( TBBPCRawCont , 60289547 , 1 )
#endif

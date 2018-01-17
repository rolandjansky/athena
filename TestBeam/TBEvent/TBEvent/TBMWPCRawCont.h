/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TBEvent/TBMWPCRaw.h"

#ifndef TBMWPCRAWCONT_H
#define TBMWPCRAWCONT_H

/** @class TBMWPCRawCont TBMWPCRawCont.h 
    "TBEvent/TBMWPCRawCont.h"
    
    @author P.A. Delsart
*/
class TBMWPCRawCont : public DataVector<TBMWPCRaw> {
 public:
  /** Constructor */
  TBMWPCRawCont() : DataVector<TBMWPCRaw>() { }
  

  
  /** destructor  */
  virtual ~TBMWPCRawCont() { } 

} ;

CLASS_DEF( TBMWPCRawCont , 221991264 , 1 )
#endif

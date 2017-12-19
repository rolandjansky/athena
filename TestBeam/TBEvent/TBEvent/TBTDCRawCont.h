/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBTDCRAWCONT_H
#define TBTDCRAWCONT_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TBEvent/TBTDCRaw.h"



/** @class TBTDCRawCont TBTDCRawCont.h 
    "TBEvent/TBTDCRawCont.h"
    
    @author M. Schram
*/

class TBTDCRawCont : public DataVector< TBTDCRaw > {
 public:
  /** Constructor */
  TBTDCRawCont() : DataVector< TBTDCRaw >() { }
    
  /** destructor  */
 virtual ~TBTDCRawCont() { } 

} ;

CLASS_DEF( TBTDCRawCont , 199428793 , 1 )

#endif

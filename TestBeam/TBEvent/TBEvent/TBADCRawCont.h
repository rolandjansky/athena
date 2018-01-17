/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBADCRAWCONT_H
#define TBADCRAWCONT_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TBEvent/TBADCRaw.h"


/** @class TBADCRawCont TBADCRawCont.h 
    "TBEvent/TBADCRawCont.h"
    
    @author M. Schram
*/

class TBADCRawCont : public DataVector< TBADCRaw > {
 public:
  /** Constructor */
  TBADCRawCont() : DataVector< TBADCRaw >() { }
    
  /** destructor  */
 virtual ~TBADCRawCont() { } 

} ;
CLASS_DEF( TBADCRawCont , 206065774 , 1 )

#endif

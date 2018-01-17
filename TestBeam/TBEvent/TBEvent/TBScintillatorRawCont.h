/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TBEvent/TBScintillatorRaw.h"

#ifndef TBSCINTILLATORRAWCONT_H
#define TBSCINTILLATORRAWCONT_H

/** @class TBScintillatorRawCont TBScintillatorRawCont.h 
    "TBEvent/TBScintillatorRawCont.h"
    
    @author P.A. Delsart
*/

class TBScintillatorRawCont : public DataVector< TBScintillatorRaw > {
 public:
  /** Constructor */
  TBScintillatorRawCont() : DataVector< TBScintillatorRaw >() { }
  

  
  /** destructor  */
  virtual ~TBScintillatorRawCont() { } 

} ;
CLASS_DEF( TBScintillatorRawCont , 119910655 , 1 )

#endif

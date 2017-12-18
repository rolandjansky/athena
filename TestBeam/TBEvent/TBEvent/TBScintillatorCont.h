/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TBEvent/TBScintillator.h"


#ifndef TBSCINTILLATORCONT_H
#define TBSCINTILLATORCONT_H

/** @class TBScintillatorCont TBScintillatorCont.h 
    "TBEvent/TBScintillatorCont.h"
    
    @author P.A. Delsart
*/

class TBScintillatorCont : public DataVector< TBScintillator > {
 public:
  /** Constructor */
  TBScintillatorCont() : DataVector< TBScintillator >() { }
  

  
  /** destructor  */
  virtual ~TBScintillatorCont() { } 

} ;
CLASS_DEF( TBScintillatorCont , 180154734 , 1 )

#endif

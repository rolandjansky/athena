/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TBEvent/TBMWPC.h"

#ifndef TBMWPCCONT_H
#define TBMWPCCONT_H

/** @class TBMWPCCont TBMWPCCont.h 
    "TBEvent/TBMWPCCont.h"
    
    @author P.A. Delsart
*/
class TBMWPCCont : public DataVector<TBMWPC> {
 public:
  /** Constructor */
  TBMWPCCont() : DataVector<TBMWPC>() { }
  

  
  /** destructor  */
  virtual ~TBMWPCCont() { } 

} ;

CLASS_DEF( TBMWPCCont , 34162951 , 1 )

#endif

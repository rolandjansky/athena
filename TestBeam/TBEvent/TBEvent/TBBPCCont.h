/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TBEvent/TBBPC.h"

#ifndef TBBPCCONT_H
#define TBBPCCONT_H

/** @class TBBPCCont TBBPCCont.h 
    "TBEvent/TBBPCCont.h"
    
    @author P.A. Delsart
*/
class TBBPCCont : public DataVector<TBBPC> {
 public:
  /** Constructor */
  TBBPCCont() : DataVector<TBBPC>() { }
  

  
  /** destructor  */
  virtual ~TBBPCCont() { } 

} ;
CLASS_DEF( TBBPCCont , 245525186 , 1 )
#endif

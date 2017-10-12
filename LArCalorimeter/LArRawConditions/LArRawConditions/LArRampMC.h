/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARRAMPMC_H
#define LARRAWCONDITIONS_LARRAMPMC_H

#include "LArRawConditions/LArRampComplete.h"



class LArRampMC: public LArRampComplete {
  /** Empty shell around LArRampComplete
   * Kept for bw compatibility since LArRampMC is stored as such in COOL  
   * The MT-complaint object is LArRampSym
   */

 public: 
  LArRampMC();
  virtual ~LArRampMC( );
  
 private: 
};

CLASS_DEF( LArRampMC, 42062668,1)

//ConditionsContainer clid for athenaMT
#include "AthenaKernel/CondCont.h"
CLASS_DEF(  CondCont<LArRampMC> ,104173262 , 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( CondCont<LArRampMC>, CondContBase );


#endif 

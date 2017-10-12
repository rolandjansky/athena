/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARUA2MEVMC_H
#define LARRAWCONDITIONS_LARUA2MEVMC_H

#include "LArRawConditions/LAruA2MeVComplete.h"

class LAruA2MeVMC: public LAruA2MeVComplete {
  /** Empty shell around LAruA2MeVComplete
   * Kept for bw compatibility since LAruA2MeV is stored as such in COOL  
   * The MT-complaint object is LAruA2MeVSym
   */
  
 public:
  LAruA2MeVMC();
  virtual ~LAruA2MeVMC();
   
 private: 
};

#include "AthenaKernel/CondCont.h"
CONDCONT_BASE(LAruA2MeVMC,ILAruA2MeV);
CLASS_DEF( LAruA2MeVMC, 172345330,1)
CLASS_DEF( CondCont<LAruA2MeVMC> , 245266716 , 1 )
#endif 

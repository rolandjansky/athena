/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigMinBias/versions/TrigHisto2DAuxContainer_v1.h"

namespace xAOD {
  
  TrigHisto2DAuxContainer_v1::TrigHisto2DAuxContainer_v1 ()
  : AuxContainerBase() {
    
    AUX_VARIABLE ( contents );
    
    AUX_VARIABLE (nbinsX);
    AUX_VARIABLE (minX);
    AUX_VARIABLE (maxX);
    
    AUX_VARIABLE (nbinsY);
    AUX_VARIABLE (minY);
    AUX_VARIABLE (maxY);
  }
  
}

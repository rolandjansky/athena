/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerAuxContainer_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v2.h"

namespace xAOD {

  TriggerTowerAuxContainer_v2::TriggerTowerAuxContainer_v2() :
    AuxContainerBase()
  {
    
    ///@name IParticle variables            
    AUX_VARIABLE( eta );           
    AUX_VARIABLE( phi );
    AUX_VARIABLE( coolId );
    AUX_VARIABLE( lut_cp );
    AUX_VARIABLE( lut_jep );
    AUX_VARIABLE( correction );
    AUX_VARIABLE( correctionEnabled );
    AUX_VARIABLE( bcidVec );
    AUX_VARIABLE( adc );
    AUX_VARIABLE( bcidExt );
    AUX_VARIABLE( sat80Vec );
    AUX_VARIABLE( error );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( adcPeak );
  }

} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v1.h"

namespace xAOD {

  TriggerTowerAuxContainer_v1::TriggerTowerAuxContainer_v1() :
    AuxContainerBase()
  {
    AUX_VARIABLE( emLUT );
    AUX_VARIABLE( hadLUT );
    AUX_VARIABLE( emADC );
    AUX_VARIABLE( hadADC );   
    AUX_VARIABLE( emBCIDvec );
    AUX_VARIABLE( hadBCIDvec );
    AUX_VARIABLE( emBCIDext );
    AUX_VARIABLE( hadBCIDext );
    AUX_VARIABLE( phi );
    AUX_VARIABLE( eta );
    AUX_VARIABLE( key );
    AUX_VARIABLE( emError );
    AUX_VARIABLE( hadError );
    AUX_VARIABLE( emPeak );
    AUX_VARIABLE( emADCPeak );
    AUX_VARIABLE( hadPeak );
    AUX_VARIABLE( hadADCPeak );
  }

} // namespace xAOD

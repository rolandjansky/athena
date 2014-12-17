/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuAuxContainer_v1.cxx 631658 2014-11-27 18:53:39Z lheinric $

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuAuxContainer_v1.h"

namespace xAOD {

   TriggerMenuAuxContainer_v1::TriggerMenuAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( smk );
      AUX_VARIABLE( l1psk );
      AUX_VARIABLE( hltpsk );

      AUX_VARIABLE( itemCtpIds );
      AUX_VARIABLE( itemNames );
      AUX_VARIABLE( itemPrescales );

      AUX_VARIABLE( chainIds );
      AUX_VARIABLE( chainNames );
      AUX_VARIABLE( chainParentNames );
      AUX_VARIABLE( chainPrescales );
      AUX_VARIABLE( chainRerunPrescales );
      AUX_VARIABLE( chainPassthroughPrescales );
      AUX_VARIABLE( chainSignatureCounters );
      AUX_VARIABLE( chainSignatureLogics );
      AUX_VARIABLE( chainSignatureOutputTEs );
      AUX_VARIABLE( chainSignatureLabels );

      AUX_VARIABLE( sequenceInputTEs );
      AUX_VARIABLE( sequenceOutputTEs );
      AUX_VARIABLE( sequenceAlgorithms );
      
      AUX_VARIABLE( bunchGroupBunches );
   }

} // namespace xAOD

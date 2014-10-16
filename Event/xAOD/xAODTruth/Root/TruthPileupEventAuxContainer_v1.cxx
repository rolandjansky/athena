/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthPileupEventAuxContainer_v1.cxx 622193 2014-10-16 16:08:34Z krasznaa $

// Local include(s):
#include "xAODTruth/versions/TruthPileupEventAuxContainer_v1.h"

namespace xAOD {

   TruthPileupEventAuxContainer_v1::TruthPileupEventAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( truthVertexLinks );
      AUX_VARIABLE( truthParticleLinks );
   }

} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventAuxContainer_v1.cxx 622193 2014-10-16 16:08:34Z krasznaa $

// Local include(s):
#include "xAODTruth/versions/TruthEventAuxContainer_v1.h"

namespace xAOD {

   TruthEventAuxContainer_v1::TruthEventAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( signalProcessVertexLink );
      AUX_VARIABLE( beamParticle1Link );
      AUX_VARIABLE( beamParticle2Link );

      AUX_VARIABLE( weights );

      AUX_VARIABLE( crossSection );
      AUX_VARIABLE( crossSectionError );

      AUX_VARIABLE( truthVertexLinks );
      AUX_VARIABLE( truthParticleLinks );
   }

} // namespace xAOD

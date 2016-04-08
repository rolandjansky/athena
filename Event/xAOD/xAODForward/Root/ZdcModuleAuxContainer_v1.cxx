/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataAuxContainer_v1.cxx 693858 2015-09-09 10:30:15Z
// krasznaa $

// Local include(s):
#include "xAODForward/versions/ZdcModuleAuxContainer_v1.h"

namespace xAOD {

   ZdcModuleAuxContainer_v1::ZdcModuleAuxContainer_v1()
      : AuxContainerBase() {

      // Digit collectoins
      AUX_VARIABLE( id );
      AUX_VARIABLE( side );
      AUX_VARIABLE( zdcModule );
      AUX_VARIABLE( type );
      AUX_VARIABLE( channel );

      AUX_VARIABLE( energy );
      AUX_VARIABLE( amplitude );
      AUX_VARIABLE( time );
      AUX_VARIABLE( qual );

      AUX_VARIABLE( energyG0 );
      AUX_VARIABLE( amplitudeG0 );
      AUX_VARIABLE( timeG0 );
      AUX_VARIABLE( qualG0 );

      AUX_VARIABLE( energyG1 );
      AUX_VARIABLE( amplitudeG1 );
      AUX_VARIABLE( timeG1 );
      AUX_VARIABLE( qualG1 );

      AUX_VARIABLE( waveformTime );
      AUX_VARIABLE( waveformADC );
      AUX_VARIABLE( waveformTimeG0 );
      AUX_VARIABLE( waveformADCG0 );
      AUX_VARIABLE( waveformTimeG1 );
      AUX_VARIABLE( waveformADCG1 );

      AUX_VARIABLE( TTg0d0Link );
      AUX_VARIABLE( TTg0d1Link );
      AUX_VARIABLE( TTg1d0Link );
      AUX_VARIABLE( TTg1d1Link );

   }

} // namespace xAOD

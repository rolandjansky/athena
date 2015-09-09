/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataAuxContainer_v1.cxx 693858 2015-09-09 10:30:15Z krasznaa $

// Local include(s):
#include "xAODForward/versions/ALFADataAuxContainer_v1.h"

namespace xAOD {

   ALFADataAuxContainer_v1::ALFADataAuxContainer_v1()
      : AuxContainerBase() {

      // LocRecEvCollection & LocRecODEvCollection
      AUX_VARIABLE( detectorPartID );
      AUX_VARIABLE( maxTrackCnt );
      AUX_VARIABLE( overU );
      AUX_VARIABLE( overV );
      AUX_VARIABLE( overY );
      AUX_VARIABLE( numU );
      AUX_VARIABLE( numV );
      AUX_VARIABLE( numY );
      AUX_VARIABLE( mdFibSel );
      AUX_VARIABLE( odFibSel );
      AUX_VARIABLE( xDetCS );
      AUX_VARIABLE( yDetCS );

      // LocRecCorrEvCollection & LocRecCorrODEvCollection
      AUX_VARIABLE( xLhcCS );
      AUX_VARIABLE( yLhcCS );
      AUX_VARIABLE( zLhcCS );
      AUX_VARIABLE( xRPotCS );
      AUX_VARIABLE( yRPotCS );
      AUX_VARIABLE( xStatCS );
      AUX_VARIABLE( yStatCS );
      AUX_VARIABLE( xBeamCS );
      AUX_VARIABLE( yBeamCS );

      // RawDataContainer
      AUX_VARIABLE( scaler );
      AUX_VARIABLE( trigPat );

      // DigitCollection
      AUX_VARIABLE( mdFiberHits );
      AUX_VARIABLE( mdMultiplicity );

      // ODDigitCollection
      AUX_VARIABLE( odFiberHitsPos );
      AUX_VARIABLE( odFiberHitsNeg );
      AUX_VARIABLE( odMultiplicityPos );
      AUX_VARIABLE( odMultiplicityNeg );
   }

} // namespace xAOD

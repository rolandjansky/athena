/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFAData_v1.cxx 717508 2016-01-13 12:32:59Z nozka $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/ALFAData_v1.h"

namespace xAOD {

   ALFAData_v1::ALFAData_v1()
      : SG::AuxElement() {

   }

   // LocRecEvCollection & LocRecODEvCollection
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >, detectorPartID, setDetectorPartID )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( ALFAData_v1, int, maxTrackCnt, setMaxTrackCnt )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      overU, setOverU )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      overV, setOverV )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      overY, setOverY )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      numU, setNumU )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      numV, setNumV )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      numY, setNumY )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      mdFibSel, setMDFibSel )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      odFibSel, setODFibSel )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      xDetCS, setXDetCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      yDetCS, setYDetCS )

   // LocRecCorrEvCollection & LocRecCorrODEvCollection
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      xLhcCS, setXLhcCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      yLhcCS, setYLhcCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      zLhcCS, setZLhcCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      xRPotCS, setXRPotCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      yRPotCS, setYRPotCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      xStatCS, setXStatCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      yStatCS, setYStatCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      xBeamCS, setXBeamCS )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< float >,
                                      yBeamCS, setYBeamCS )

   // RawDataContainer
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      scaler, setScaler )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      trigPat, setTrigPat )

   // DigitCollection
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      mdFiberHits, setMDFiberHits )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      mdMultiplicity, setMDMultiplicity )

   // ODDigitCollection
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      odFiberHitsPos, setODFiberHitsPos )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      odFiberHitsNeg, setODFiberHitsNeg )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      odMultiplicityPos, setODMultiplicityPos )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( ALFAData_v1, std::vector< int >,
                                      odMultiplicityNeg, setODMultiplicityNeg )

} // namespace xAOD

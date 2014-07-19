/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORRECTIONCODE_CXX
#define CORRECTIONCODE_CXX

#include "TauAnalysisTools/CorrectionCode.h"

using namespace TauAnalysisTools;

CorrectionCode CorrectionCodeMin(CorrectionCode c1, CorrectionCode c2){
  // return worst case
  // ordering: Error - OutOfValidityRange - OK
  if (c1 == CorrectionCode::Error or c2 == CorrectionCode::Error)
    return CorrectionCode::Error;
  if (c1 == CorrectionCode::OutOfValidityRange or c2 == CorrectionCode::OutOfValidityRange)
    return CorrectionCode::OutOfValidityRange;
  return CorrectionCode::OK;
}
  
CorrectionCode CorrectionCodeMax(CorrectionCode c1, CorrectionCode c2){
  // return best case
  // ordering: OK - OutOfValidityRange - Error
  if (c1 == CorrectionCode::OK or c2 == CorrectionCode::OK)
    return CorrectionCode::OK;
  if (c1 == CorrectionCode::OutOfValidityRange or c2 == CorrectionCode::OutOfValidityRange)
    return CorrectionCode::OutOfValidityRange;
  return CorrectionCode::Error;
}


#endif // CORRECTIONCODE_CXX

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDTPCNV_LARHVSCALECORRCOMPLETECNV_H
#define LARCONDTPCNV_LARHVSCALECORRCOMPLETECNV_H

#include "LArRawConditions/LArHVScaleCorrComplete.h" 
#include "LArSingleFloatConverter.h" 

class LArHVScaleCorrCompleteCnv : public LArSingleFloatConverter<LArHVScaleCorrComplete> {
  friend class CnvFactory<LArHVScaleCorrCompleteCnv>;
 protected: 
  LArHVScaleCorrCompleteCnv (ISvcLocator* svcloc) :
    LArSingleFloatConverter<LArHVScaleCorrComplete>(svcloc) {}
}; 

#endif 


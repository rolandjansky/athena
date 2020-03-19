/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
 * @file LArDAC2uACompleteCnv.h
 * @brief AthenaPool converter LArDAC2uAComplete
 * @author Walter Lampl <walter.lampl@ cern.ch>
 */

#ifndef LArDAC2uACompleteCnv_H
#define LArDAC2uACompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "LArRawConditions/LArDAC2uAComplete.h"
#include "LArRawConditions/LArDAC2uAP.h"
#include "LArSingleFloatConverter.h"
 
typedef LArSingleFloatConverter<LArDAC2uAComplete> LArDAC2uACompleteCnvBase;

class LArDAC2uACompleteCnv : public LArDAC2uACompleteCnvBase {
friend class CnvFactory<LArDAC2uACompleteCnv >;
protected:
public:
    LArDAC2uACompleteCnv (ISvcLocator* svcloc) : LArDAC2uACompleteCnvBase(svcloc) {}
protected:
    virtual LArConditionsSubset<LArSingleFloatP>*  createTransient ();
    LArConditionsSubset<LArSingleFloatP>*          createTransient(LArConditionsSubset<LArDAC2uAP>* orig);
};

#endif

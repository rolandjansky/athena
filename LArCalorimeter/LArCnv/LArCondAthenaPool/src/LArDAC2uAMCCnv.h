/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
 * @file LArDAC2uAMCCnv.h
 * @brief AthenaPool converter LArDAC2uAMC
 * @author Walter Lampl <walter.lampl@ cern.ch>
 */

#ifndef LArDAC2uAMCCnv_H
#define LArDAC2uAMCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#include "LArRawConditions/LArDAC2uAMC.h"
#undef private
#include "LArRawConditions/LArDAC2uAP.h"
#include "LArSingleFloatConverter.h"
 
typedef LArSingleFloatConverter<LArDAC2uAMC> LArDAC2uAMCCnvBase;

class LArDAC2uAMCCnv : public LArDAC2uAMCCnvBase {
friend class CnvFactory<LArDAC2uAMCCnv >;
protected:
    LArDAC2uAMCCnv (ISvcLocator* svcloc) : LArDAC2uAMCCnvBase(svcloc) {}
    virtual LArConditionsSubset<LArSingleFloatP>*  createTransient ();
    LArConditionsSubset<LArSingleFloatP>*          createTransient(LArConditionsSubset<LArDAC2uAP>* orig);
};

#endif

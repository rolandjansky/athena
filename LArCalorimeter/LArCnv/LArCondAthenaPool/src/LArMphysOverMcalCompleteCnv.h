/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
 * @file LArMphysOverMcalCompleteCnv.h
 * @brief AthenaPool converter LArMphysOverMcalComplete
 * @author Walter Lampl <walter.lampl@ cern.ch>
 */

#ifndef LArMphysOverMcalCompleteCnv_H
#define LArMphysOverMcalCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#undef private
#include "LArRawConditions/LArMphysOverMcalP.h"
#include "LArRawConditions/LArMphysOverMcalP1.h"
#include "LArRawConditions/LArSingleFloatP.h"
#include "LArCondTPCnv/LArMphysOverMcalSubset_p1.h"
#include "LArSingleFloatConverter.h"
 
typedef LArSingleFloatConverter<LArMphysOverMcalComplete> LArMphysOverMcalCompleteCnvBase;

class LArMphysOverMcalCompleteCnv : public LArMphysOverMcalCompleteCnvBase {
friend class CnvFactory<LArMphysOverMcalCompleteCnv >;
protected:
    LArMphysOverMcalCompleteCnv (ISvcLocator* svcloc) : LArMphysOverMcalCompleteCnvBase(svcloc) {}
    virtual LArConditionsSubset<LArSingleFloatP>*  createTransient ();
    LArConditionsSubset<LArSingleFloatP>*          createTransient(LArConditionsSubset<LArMphysOverMcalP>* orig);
};

#endif


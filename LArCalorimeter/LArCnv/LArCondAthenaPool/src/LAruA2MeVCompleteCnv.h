/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
 * @file LAruA2MeVCompleteCnv.h
 * @brief AthenaPool converter LAruA2MeVComplete
 * @author Walter Lampl <walter.lampl@ cern.ch>
 */

#ifndef LAruA2MeVCompleteCnv_H
#define LAruA2MeVCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#include "LArRawConditions/LAruA2MeVComplete.h"
#undef private
#include "LArRawConditions/LAruA2MeVP.h"
#include "LArSingleFloatConverter.h"
 
typedef LArSingleFloatConverter<LAruA2MeVComplete> LAruA2MeVCompleteCnvBase;

class LAruA2MeVCompleteCnv : public LAruA2MeVCompleteCnvBase {
friend class CnvFactory<LAruA2MeVCompleteCnv >;
protected:
    LAruA2MeVCompleteCnv (ISvcLocator* svcloc) : LAruA2MeVCompleteCnvBase(svcloc) {}
    virtual LArConditionsSubset<LArSingleFloatP>*  createTransient ();
    LArConditionsSubset<LArSingleFloatP>*          createTransient(LArConditionsSubset<LAruA2MeVP>* orig);
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
 * @file LAruA2MeVMCCnv.h
 * @brief AthenaPool converter LAruA2MeVMC
 * @author Walter Lampl <walter.lampl@ cern.ch>
 */

#ifndef LAruA2MeVMCCnv_H
#define LAruA2MeVMCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#include "LArRawConditions/LAruA2MeVMC.h"
#undef private
#include "LArRawConditions/LAruA2MeVP.h"
#include "LArSingleFloatConverter.h"
 
typedef LArSingleFloatConverter<LAruA2MeVMC> LAruA2MeVMCCnvBase;

class LAruA2MeVMCCnv : public LAruA2MeVMCCnvBase {
friend class CnvFactory<LAruA2MeVMCCnv >;
protected:
    LAruA2MeVMCCnv (ISvcLocator* svcloc) : LAruA2MeVMCCnvBase(svcloc) {}
    virtual LArConditionsSubset<LArSingleFloatP>*  createTransient ();
    LArConditionsSubset<LArSingleFloatP>*          createTransient(LArConditionsSubset<LAruA2MeVP>* orig);
};

#endif

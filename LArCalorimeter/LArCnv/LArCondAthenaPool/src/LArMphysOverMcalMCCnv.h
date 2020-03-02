/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
 * @file LArMphysOverMcalMCCnv.h
 * @brief AthenaPool converter LArMphysOverMcalMC
 * @author Walter Lampl <walter.lampl@ cern.ch>
 * $Id: LArMphysOverMcalMCCnv.h,v 1.2 2008-04-10 14:54:33 wlampl Exp $
 */

#ifndef LArMphysOverMcalMCCnv_H
#define LArMphysOverMcalMCCnv_H
#include "LArMphysOverMcalCompleteCnv.h"
#include "LArRawConditions/LArMphysOverMcalMC.h"
 
typedef LArSingleFloatConverter<LArMphysOverMcalMC> LArMphysOverMcalMCCnvBase;

class LArMphysOverMcalMCCnv : public LArMphysOverMcalMCCnvBase {
friend class CnvFactory<LArMphysOverMcalMCCnv >;
protected:
public:
    LArMphysOverMcalMCCnv (ISvcLocator* svcloc) : LArMphysOverMcalMCCnvBase(svcloc) {}
protected:
    virtual LArConditionsSubset<LArSingleFloatP>*  createTransient ();
    LArConditionsSubset<LArSingleFloatP>*          createTransient(LArConditionsSubset<LArMphysOverMcalP>* orig);
};


#endif 



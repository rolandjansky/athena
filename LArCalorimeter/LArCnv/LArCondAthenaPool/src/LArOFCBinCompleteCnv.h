/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArOFCBinCompleteCnv.h
 * @brief AthenaPool converter LArOFCBinComplete
 * @author Walter Lampl <walter.lampl@ cern.ch>
 */

#ifndef LArOFCBinCompleteCnv_H
#define LArOFCBinCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
//#define private public
//#define protected public
#include "LArRawConditions/LArOFCBinComplete.h"
//#undef private
//#undef protected
#include "LArRawConditions/LArOFCBinP.h"
#include "LArCondTPCnv/LArOFCBinSubsetCnv_p1.h"
 
// the latest persistent representation type of LArOFCBinComplete
typedef LArConditionsSubset<LArOFCBinP> LArOFCBinTransType;
typedef LArSingleIntSubset_p1           LArOFCBinPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArOFCBinComplete, LArOFCBinTransType, LArOFCBinPersType > LArOFCBinCompleteCnvBase;

class LArOFCBinCompleteCnv : public LArOFCBinCompleteCnvBase {
friend class CnvFactory<LArOFCBinCompleteCnv >;
protected:
    LArOFCBinCompleteCnv (ISvcLocator* svcloc) : LArOFCBinCompleteCnvBase(svcloc) {}
    virtual LArOFCBinPersType*   createPersistent (LArOFCBinTransType* transObj);
    virtual LArOFCBinTransType*  createTransient ();
 private:
    LArOFCBinSubsetCnv_p1 m_TPconverter;
};


#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVECCNV_H
#define ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVECCNV_H

#include "AthenaPoolTestAthenaPool/PixelRDOElemLinkVec_p1.h"
#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "PixelRDOElemLinkVecCnv_p1.h"

typedef PixelRDOElemLinkVec_p1 PixelRDOElemLinkVecPERS;
typedef T_AthenaPoolCustomCnv<PixelRDOElemLinkVec,PixelRDOElemLinkVecPERS> PixelRDOElemLinkVecCnvBase;

class PixelRDOElemLinkVecCnv : public PixelRDOElemLinkVecCnvBase 
{
    friend class CnvFactory<PixelRDOElemLinkVecCnv>;
protected:
    PixelRDOElemLinkVecCnv(ISvcLocator*);
    virtual PixelRDOElemLinkVec*      createTransient();
    virtual PixelRDOElemLinkVecPERS*  createPersistent(PixelRDOElemLinkVec*);
private:
    PixelRDOElemLinkVecCnv_p1 m_converter_p1;
    pool::Guid                m_guid1;
};

#endif

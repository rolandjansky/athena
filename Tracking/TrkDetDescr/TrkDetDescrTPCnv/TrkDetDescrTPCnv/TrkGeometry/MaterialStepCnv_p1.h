/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStepCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIAL_STEP_CNV_P1_H
#define MATERIAL_STEP_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/MaterialStep.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStep_p1.h"

class MsgStream;

class MaterialStepCnv_p1 : public T_AthenaPoolTPCnvBase<Trk::MaterialStep, Trk::MaterialStep_p1>
{
    public:
        MaterialStepCnv_p1() {}
        
        virtual void persToTrans( const Trk::MaterialStep_p1 *persObj,
                                        Trk::MaterialStep    *transObj,
                                        MsgStream                 &log );
        virtual void transToPers( const Trk::MaterialStep    *transObj,
                                        Trk::MaterialStep_p1 *persObj,
                                        MsgStream                 &log );
};

#endif // MATERIAL_STEP_CNV_P1_H
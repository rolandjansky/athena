/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialPropertiesCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIAL_CNV_P1_H
#define MATERIAL_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/Material.h"
#include "TrkDetDescrTPCnv/TrkGeometry/Material_p1.h"

class MsgStream;

class MaterialCnv_p1 : public T_AthenaPoolTPCnvBase<Trk::Material, Trk::Material_p1>
{
    public:
        MaterialCnv_p1() {}
        
        virtual void persToTrans( const Trk::Material_p1 *persObj,
                                        Trk::Material    *transObj,
                                        MsgStream                 &log );
        virtual void transToPers( const Trk::Material    *transObj,
                                        Trk::Material_p1 *persObj,
                                        MsgStream                 &log );
};

#endif // MATERIAL_CNV_P1_H

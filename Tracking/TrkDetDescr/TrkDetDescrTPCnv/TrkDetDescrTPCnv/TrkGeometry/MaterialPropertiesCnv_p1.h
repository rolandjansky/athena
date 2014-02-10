/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialPropertiesCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef MATERIALPROPERTIES_CNV_P1_H
#define MATERIALPROPERTIES_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/MaterialProperties.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialProperties_p1.h"

class MsgStream;

class MaterialPropertiesCnv_p1 : public T_AthenaPoolTPCnvBase<Trk::MaterialProperties, Trk::MaterialProperties_p1>
{
    public:
        MaterialPropertiesCnv_p1() {}
        
        virtual void persToTrans( const Trk::MaterialProperties_p1 *persObj,
                                        Trk::MaterialProperties    *transObj,
                                        MsgStream                 &log );
        virtual void transToPers( const Trk::MaterialProperties    *transObj,
                                        Trk::MaterialProperties_p1 *persObj,
                                        MsgStream                 &log );
};

#endif // MATERIALPROPERTIES_CNV_P1_H
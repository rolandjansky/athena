/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStepCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/Material.h"
#include "TrkGeometry/MaterialStep.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCnv_p1.h"

void MaterialStepCnv_p1::persToTrans( const Trk::MaterialStep_p1 *persObj,
                                            Trk::MaterialStep    *transObj,
                                            MsgStream&)
{
    transObj->m_steplength   = persObj->materialData[0];
    transObj->m_hitX         = persObj->materialData[1];
    transObj->m_hitY         = persObj->materialData[2];
    transObj->m_hitZ         = persObj->materialData[3];
    transObj->m_hitR         = sqrt(transObj->m_hitX*transObj->m_hitX+transObj->m_hitY*transObj->m_hitY);
    transObj->m_material.X0  = persObj->materialData[4];
    transObj->m_material.L0  = persObj->materialData[5];
    transObj->m_material.A   = persObj->materialData[6];
    transObj->m_material.Z   = persObj->materialData[7];
    transObj->m_material.rho = persObj->materialData[8];
    
    // recreate the composition vector if possible
    if ( persObj->elements.size() && persObj->fractions.size() ) {
        transObj->m_material.composition = new Trk::MaterialComposition(persObj->elements, persObj->fractions);
    }
    
}

void MaterialStepCnv_p1::transToPers( const Trk::MaterialStep    *transObj,
                                            Trk::MaterialStep_p1 *persObj,
                                            MsgStream& )
{    
    persObj->materialData[0] = transObj->m_steplength  ;
    persObj->materialData[1] = transObj->m_hitX        ;
    persObj->materialData[2] = transObj->m_hitY        ;
    persObj->materialData[3] = transObj->m_hitZ        ;
    persObj->materialData[4] = transObj->m_material.X0 ;
    persObj->materialData[5] = transObj->m_material.L0 ;
    persObj->materialData[6] = transObj->m_material.A  ;
    persObj->materialData[7] = transObj->m_material.Z  ;
    persObj->materialData[8] = transObj->m_material.rho;
    
    // write the composition vector if present
    // disentangle the ElementFraction for columnwise write support (compression optimised)
    if ( transObj->m_material.composition ){
        persObj->elements.reserve(transObj->m_material.composition->size());
        persObj->fractions.reserve(transObj->m_material.composition->size());
        for (auto& iel: (*transObj->m_material.composition)){
            persObj->elements.push_back(iel.first);
            persObj->fractions.push_back(iel.second);
        }
    }
}

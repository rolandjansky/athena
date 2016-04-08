/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialPropertiesCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/Material.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"

void MaterialPropertiesCnv_p1::persToTrans( const Trk::MaterialProperties_p1 *persObj,
                                            Trk::MaterialProperties    *transObj,
                                            MsgStream&)
{
    transObj->m_material.X0   = persObj->materialData[0];
    transObj->m_material.L0   = persObj->materialData[1];
    transObj->m_material.A    = persObj->materialData[2];
    transObj->m_material.Z    = persObj->materialData[3];
    transObj->m_material.rho  = persObj->materialData[4];

    transObj->m_material.zOaTr =  transObj->m_material.A *transObj->m_material.A  > 10e-10 ?
       (transObj->m_material.Z/transObj->m_material.A*transObj->m_material.rho) : 0.; 
    
    if ( persObj->materialData.size() > 6 )
        transObj->m_material.dEdX = persObj->materialData[6];
    
    // recreate the composition vector if possible
    if ( persObj->elements.size() && persObj->fractions.size() ) {
        transObj->m_material.composition = new Trk::MaterialComposition(persObj->elements, persObj->fractions);
    }
    
    // the derived quantities - to be recreated
    float d = ( persObj->materialData.size()>5 ) ? persObj->materialData[5] : 1.;
    transObj->m_dInX0 = transObj->m_material.X0*transObj->m_material.X0 > 10e-10 ? (d/transObj->m_material.X0)  : 0.;
    transObj->m_dInL0 = transObj->m_material.L0*transObj->m_material.L0 > 10e-10 ? (d/transObj->m_material.L0)  : 0.;           
    transObj->m_zOaTrTd = transObj->m_material.A *transObj->m_material.A  > 10e-10 ? (d*transObj->m_material.Z
							       /transObj->m_material.A*transObj->m_material.rho) : 0.;    
    
}

void MaterialPropertiesCnv_p1::transToPers( const Trk::MaterialProperties    *transObj,
                                            Trk::MaterialProperties_p1 *persObj,
                                            MsgStream& )
{    
    if ( transObj->m_material.dEdX != 0. ){
        persObj->materialData.resize(7);
        persObj->materialData[6] = transObj->m_material.dEdX;
    }
    
    persObj->materialData[0] = transObj->m_material.X0 ;
    persObj->materialData[1] = transObj->m_material.L0 ;
    persObj->materialData[2] = transObj->m_material.A  ;
    persObj->materialData[3] = transObj->m_material.Z  ;
    persObj->materialData[4] = transObj->m_material.rho;
    persObj->materialData[5] = transObj->m_dInX0*transObj->m_material.X0;
    
    // write the composition vector if present, decompose ElementFraction object
    if ( transObj->m_material.composition ){
        persObj->elements.reserve(transObj->m_material.composition->size());
        persObj->fractions.reserve(transObj->m_material.composition->size());
        for (auto& iel: (*transObj->m_material.composition)){
            persObj->elements.push_back(iel.first);
            persObj->fractions.push_back(iel.second);
        }
    }
}

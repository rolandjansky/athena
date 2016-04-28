/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/Material.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialCnv_p1.h"

void MaterialCnv_p1::persToTrans( const Trk::Material_p1 *persObj,
                                            Trk::Material    *transObj,
                                            MsgStream&)
{
    transObj->X0   = persObj->materialData[0];
    transObj->L0   = persObj->materialData[1];
    transObj->A    = persObj->materialData[2];
    transObj->Z    = persObj->materialData[3];
    transObj->rho  = persObj->materialData[4];
    
    if ( persObj->materialData.size() > 5 )
        transObj->dEdX = persObj->materialData[5];
    
    // recreate the composition vector if possible
    if ( persObj->elements.size() && persObj->fractions.size() ) {
        transObj->composition = new Trk::MaterialComposition(persObj->elements, persObj->fractions);
    }
                              
    transObj->zOaTr = transObj->A *transObj->A  > 10e-10 ? transObj->Z/transObj->A*transObj->rho : 0.;    
    
}

void MaterialCnv_p1::transToPers( const Trk::Material    *transObj,
				  Trk::Material_p1 *persObj,
				  MsgStream& )
{    
    if ( transObj->dEdX != 0. ){
        persObj->materialData.resize(6);
        persObj->materialData[5] = transObj->dEdX;
    }
    
    persObj->materialData[0] = transObj->X0 ;
    persObj->materialData[1] = transObj->L0 ;
    persObj->materialData[2] = transObj->A  ;
    persObj->materialData[3] = transObj->Z  ;
    persObj->materialData[4] = transObj->rho;
    
    // write the composition vector if present
    // disentangle the ElementFraction for columnwise write support (compression optimised)
    if ( transObj->composition ){
        persObj->elements.reserve(transObj->composition->size());
        persObj->fractions.reserve(transObj->composition->size());
        for (auto& iel: (*transObj->composition)){
            persObj->elements.push_back(iel.first);
            persObj->fractions.push_back(iel.second);
        }  
    }
}

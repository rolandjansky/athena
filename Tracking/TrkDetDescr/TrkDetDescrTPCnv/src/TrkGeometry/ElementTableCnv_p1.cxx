/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTableCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/Material.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTable_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialCnv_p1.h"
#include "CxxUtils/make_unique.h"
        
void ElementTableCnv_p1::persToTrans( const Trk::ElementTable_p1 *persObj,
                                      Trk::ElementTable    *transObj,
                                      MsgStream& mlog)
{
    // create the transient representation of the element table
    for (size_t im = 0; im < persObj->table.size(); ++im){
        auto tMaterial = CxxUtils::make_unique<Trk::Material>();
        m_materialCnv.persToTrans(  &persObj->table[im], tMaterial.get(), mlog );
        transObj->addElement (std::move (tMaterial), persObj->names[im]);
    }
}

void ElementTableCnv_p1::transToPers( const Trk::ElementTable *transObj,
                                      Trk::ElementTable_p1 *persObj,
                                      MsgStream& mlog)
{
    // create the persistent representation of the element table - only write out what you have
    for (size_t im = 0; im < transObj->size(); ++im){
        if (transObj->contains(im)){
            Trk::Material_p1 pMaterial;
            m_materialCnv.transToPers( transObj->element(im), &pMaterial, mlog );
            persObj->table.push_back( pMaterial );
            persObj->names.push_back( transObj->elementName(im) );     
        }
    }
}

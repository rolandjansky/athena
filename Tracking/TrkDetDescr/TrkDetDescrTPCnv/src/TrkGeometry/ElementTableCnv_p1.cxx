/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTableCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#define private public
#define protected public
#include "TrkGeometry/ElementTable.h"
#undef private
#undef protected

#include "TrkGeometry/Material.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTable_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialCnv_p1.h"
        
void ElementTableCnv_p1::persToTrans( const Trk::ElementTable_p1 *persObj,
                                      Trk::ElementTable    *transObj,
                                      MsgStream& mlog)
{
    // create the transient representation of the element table
    for (size_t im = 0; im < persObj->table.size(); ++im){
        Trk::Material* tMaterial = new Trk::Material(); 
        m_materialCnv.persToTrans(  &persObj->table[im], tMaterial, mlog );
        std::string name = persObj->names[im];
        unsigned char iZ = static_cast<unsigned char>(int(tMaterial->Z));
        transObj->m_table[iZ] = tMaterial;
        transObj->m_names[iZ] = name;
    }
}

void ElementTableCnv_p1::transToPers( const Trk::ElementTable *transObj,
                                      Trk::ElementTable_p1 *persObj,
                                      MsgStream& mlog)
{
    // create the persistent representation of the element table - only write out what you have
    for (size_t im = 0; im < transObj->m_table.size(); ++im){
        if (transObj->m_table[im]){
            Trk::Material_p1 pMaterial;
            m_materialCnv.transToPers( transObj->m_table[im], &pMaterial, mlog );
            persObj->table.push_back( pMaterial );
            persObj->names.push_back( transObj->m_names[im] );     
        }
    }
}

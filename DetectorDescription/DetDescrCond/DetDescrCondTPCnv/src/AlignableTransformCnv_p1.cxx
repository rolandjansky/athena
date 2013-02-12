/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "DetDescrConditions/AlignableTransform.h"
#undef private
#undef protected

#include "DetDescrCondTPCnv/AlignableTransformCnv_p1.h"

#include "CLHEP/Geometry/Transform3D.h"

void AlignableTransformCnv_p1::transToPers(const AlignableTransform* trans, AlignableTransform_p1* pers, MsgStream & /*log*/) {

    pers->m_tag = trans->m_tag;
    pers->m_ids.reserve(trans->m_vec.size());
    pers->m_trans.reserve(12*trans->m_vec.size());
    for (unsigned int i = 0; i < trans->m_vec.size(); ++i) {
        pers->m_ids.push_back(trans->m_vec[i].m_ident.get_identifier32().get_compact());
        pers->m_trans.push_back( trans->m_vec[i].m_trans.xx() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.xy() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.xz() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.dx() );
             
        pers->m_trans.push_back( trans->m_vec[i].m_trans.yx() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.yy() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.yz() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.dy() );
 
        pers->m_trans.push_back( trans->m_vec[i].m_trans.zx() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.zy() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.zz() );
        pers->m_trans.push_back( trans->m_vec[i].m_trans.dz() );
    }
}

void AlignableTransformCnv_p1::persToTrans(const AlignableTransform_p1* pers, AlignableTransform* trans, MsgStream &log)  {

    trans->m_tag = pers->m_tag;
    trans->m_vec.reserve(pers->m_ids.size());
    unsigned int j = 0;
    // Check on size for HepTransform info
    if (pers->m_trans.size() != 12*pers->m_ids.size()) {
        log << MSG::ERROR << "AlignableTransformCnv_p1::persToTrans Incorrect size for transform info: number of ids: " 
            << pers->m_ids.size() << " Number of transform elements: " 
            << pers->m_trans.size() << " Should be 12*nids!! "
            << endreq;
    }
    for (unsigned int i = 0; i < pers->m_ids.size(); ++i) {
        Identifier     id(pers->m_ids[i]);
        HepGeom::Transform3D tf;
        tf.setTransform( pers->m_trans[j + 0],    // xx
                         pers->m_trans[j + 1],    // xy
                         pers->m_trans[j + 2],    // xz
                         pers->m_trans[j + 3],    // dx

                         pers->m_trans[j + 4],    // yx
                         pers->m_trans[j + 5],    // yy
                         pers->m_trans[j + 6],    // yz
                         pers->m_trans[j + 7],    // dy

                         pers->m_trans[j + 8],    // zx
                         pers->m_trans[j + 9],    // zy
                         pers->m_trans[j + 10],   // zz
                         pers->m_trans[j + 11] ); // dz
        j += 12;
        trans->m_vec.push_back(AlignableTransform::AlignTransMember(id, tf));
    }
}


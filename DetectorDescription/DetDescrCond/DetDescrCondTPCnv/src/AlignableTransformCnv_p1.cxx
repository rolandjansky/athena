/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DetDescrConditions/AlignableTransform.h"
#include "DetDescrCondTPCnv/AlignableTransformCnv_p1.h"

#include "CLHEP/Geometry/Transform3D.h"

namespace {

class Transform3DElements
  : public HepGeom::Transform3D
{
public:
  using HepGeom::Transform3D::setTransform;
};

}

void AlignableTransformCnv_p1::transToPers(const AlignableTransform* trans, AlignableTransform_p1* pers, MsgStream & /*log*/) {

    pers->m_tag = trans->tag();
    pers->m_ids.reserve(trans->size());
    pers->m_trans.reserve(12*trans->size());
    for (const AlignableTransform::AlignTransMember& mem : trans->range()) {
        pers->m_ids.push_back(mem.identify().get_identifier32().get_compact());
        const HepGeom::Transform3D& tf = mem.transform();
        pers->m_trans.push_back( tf.xx() );
        pers->m_trans.push_back( tf.xy() );
        pers->m_trans.push_back( tf.xz() );
        pers->m_trans.push_back( tf.dx() );
             
        pers->m_trans.push_back( tf.yx() );
        pers->m_trans.push_back( tf.yy() );
        pers->m_trans.push_back( tf.yz() );
        pers->m_trans.push_back( tf.dy() );
 
        pers->m_trans.push_back( tf.zx() );
        pers->m_trans.push_back( tf.zy() );
        pers->m_trans.push_back( tf.zz() );
        pers->m_trans.push_back( tf.dz() );
    }
}

void AlignableTransformCnv_p1::persToTrans(const AlignableTransform_p1* pers, AlignableTransform* trans, MsgStream &log)  {

    trans->m_tag = pers->m_tag;
    trans->m_vec.clear();
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
        Transform3DElements tf;
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
        trans->add (id, tf);
    }
}


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DetDescrConditions/DetCondKeyTrans.h"

#include "DetDescrCondTPCnv/DetCondKeyTransCnv_p1.h"

#include "CLHEP/Geometry/Transform3D.h"

namespace {


class Transform3DComponents
  : public HepGeom::Transform3D
{
public:
  using HepGeom::Transform3D::setTransform;
};


}

void DetCondKeyTransCnv_p1::transToPers(const DetCondKeyTrans* trans, DetCondKeyTrans_p1* pers, MsgStream &/*log*/)
{
    size_t sz = trans->keyTrans().size();
    pers->m_trans.reserve(12*sz);

    for (const DetCondKeyTrans::KeyTrans::value_type& trans : trans->keyTrans()) {
        pers->m_keys.push_back(trans.first);
        const HepGeom::Transform3D& tf = trans.second;
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

void DetCondKeyTransCnv_p1::persToTrans(const DetCondKeyTrans_p1* pers, DetCondKeyTrans* trans, MsgStream &/*log*/)  {

    // Copy stored vec into map 
    unsigned int j = 0;
    for (unsigned int i = 0; i < pers->m_keys.size(); ++i) {
        Transform3DComponents tf;
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
        trans->setTransform (pers->m_keys[i], tf);
    }
}


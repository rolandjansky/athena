/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "DetDescrConditions/DetCondKeyTrans.h"
#undef private
#undef protected

#include "DetDescrCondTPCnv/DetCondKeyTransCnv_p1.h"

#include "CLHEP/Geometry/Transform3D.h"

void DetCondKeyTransCnv_p1::transToPers(const DetCondKeyTrans* trans, DetCondKeyTrans_p1* pers, MsgStream &/*log*/) {
    
    pers->m_trans.reserve(12*trans->m_keytrans.size());
    DetCondKeyTrans::KeyTrans::const_iterator it   = trans->m_keytrans.begin();
    DetCondKeyTrans::KeyTrans::const_iterator last = trans->m_keytrans.end();
    for (; it != last; ++it) {
        pers->m_keys.push_back(it->first);
        const HepGeom::Transform3D& tf = it->second;
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
    typedef DetCondKeyTrans::KeyTrans::value_type value_type;
    DetCondKeyTrans::KeyTrans::iterator it   = trans->m_keytrans.begin();
    unsigned int j = 0;
    for (unsigned int i = 0; i < pers->m_keys.size(); ++i) {
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
        it = trans->m_keytrans.insert(it, value_type(pers->m_keys[i], tf));
    }
}


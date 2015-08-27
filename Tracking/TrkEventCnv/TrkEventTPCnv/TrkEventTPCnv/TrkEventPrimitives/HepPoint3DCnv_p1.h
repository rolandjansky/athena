/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HepPoint3D_Cnv_P1_H
#define HepPoint3D_Cnv_P1_H

//-----------------------------------------------------------------------------
//
// file:   HepPoint3DCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "CLHEP/Geometry/Point3D.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3D_p1.h"

class MsgStream;

class HepPoint3DCnv_p1
   : public T_AthenaPoolTPCnvBase< HepGeom::Point3D<double>, HepPoint3D_p1 >
{
public:
  HepPoint3DCnv_p1() {}
  
  virtual void persToTrans( const HepPoint3D_p1 *persObj, HepGeom::Point3D<double> *transObj, MsgStream &log ); 
  virtual void transToPers( const HepGeom::Point3D<double> *transObj, HepPoint3D_p1 *persObj, MsgStream &log );
};


inline
void
HepPoint3DCnv_p1::persToTrans( const HepPoint3D_p1 *persObj, HepGeom::Point3D<double> *transObj, MsgStream &) {
   transObj->setX( persObj->m_x );
   transObj->setY( persObj->m_y );
   transObj->setZ( persObj->m_z );
}

inline
void
HepPoint3DCnv_p1::transToPers( const HepGeom::Point3D<double> *transObj, HepPoint3D_p1 *persObj, MsgStream & ) {
   persObj->m_x = transObj->x();
   persObj->m_y = transObj->y();
   persObj->m_z = transObj->z();
}

#endif 


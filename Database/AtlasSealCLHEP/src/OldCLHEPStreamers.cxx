/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AtlasSealCLHEP/OldCLHEPStreamers.h"
#include <iostream>


void
CLHEPMatrixStreamer::CopyOldToNew(const OldHepMatrix &old_matx, CLHEP::HepMatrix &new_matx )
{
//      std::cout << "  ****  CLHEPMatrixStreamer::CopyOldToNew() " << std::endl;
//      std::cout << "  matrix size="<< old_matx.size  << std::endl;
//      std::cout << "  matrix nrow="<< old_matx.nrow  << std::endl;
//      std::cout << "  matrix ncol="<< old_matx.ncol  << std::endl;
//      std::cout << "  matrix m-size="<< old_matx.m.size()  << std::endl;

     // intentional swap!   somehow necessary for reading root3 data 
     CLHEP::HepMatrix mat (old_matx.ncol, old_matx.nrow);
     double* m = &mat[0][0];
     std::copy (old_matx.m.begin(), old_matx.m.end(), m);
     std::swap (mat, new_matx);
} 



void
CLHEPPoint3dStreamer::CopyOldToNew(const OldHepPoint3D &old_point, HepGeom::Point3D<double> &new_point )
{
//    std::cout << "  ****  CLHEPPoint3dStreamer::CopyOldToNew() " << std::endl;
//    std::cout << "  Point(x,y,z) = "  << old_point.x() << "," << old_point.y() << "," <<  old_point.z() << std::endl;
   HepGeom::Point3D<double>	point( old_point.x(), old_point.y(), old_point.z() );
   new_point = point;
} 



void
CLHEPLorVecStreamer::CopyOldToNew(const OldHepLorentzVector &old_vec, CLHEP::HepLorentzVector &new_vec )
{
//    std::cout << "  ****  CLHEPLorVecStreamer::CopyOldToNew() " << std::endl;
//    std::cout << "  LVec(x,y,z,t) = "  << old_vec.x() << "," << old_vec.y() << "," << old_vec.z() << "," << old_vec.t() << std::endl;
   CLHEP::HepLorentzVector	vector( old_vec.x(),  old_vec.y(),  old_vec.z(),  old_vec.t() );
   new_vec = vector;
}


void
CLHEP3VectorStreamer::CopyOldToNew(const OldHep3Vector &old_vec, CLHEP::Hep3Vector &new_vec )
{
//    std::cout << "  ****  CLHEP3VectorStreamer::CopyOldToNew() " << std::endl;
//    std::cout << "  Vec(x,y,z) = "  << old_vec.x() << "," << old_vec.y() << "," << old_vec.z()  << std::endl;
   CLHEP::Hep3Vector	vector( old_vec.x(),  old_vec.y(),  old_vec.z() );
   new_vec = vector;
}


void
CLHEPBasicVectorStreamer::CopyOldToNew(const OldBasicVector3D &old_vec, HepGeom::BasicVector3D<double> &new_vec )
{
//   std::cout << "  ****  CLHEPBasicVectorStreamer::CopyOldToNew() " << std::endl;
//   std::cout << "  Vec(x,y,z) = "  << old_vec.x() << "," << old_vec.y() << "," << old_vec.z()  << std::endl;
   HepGeom::BasicVector3D<double>	vector( old_vec.x(),  old_vec.y(),  old_vec.z() );
   new_vec = vector;
}



namespace {
class Transform3DInit 
  : public HepGeom::Transform3D
{
public:
  using HepGeom::Transform3D::setTransform;
};
}
  


#define TRANSF_DBGL 0
void
CLHEPTransform3DStreamer::CopyOldToNew(const OldHepTransform3D &old_tr, HepGeom::Transform3D &new_tr )
{
#if( TRANSF_DBGL > 0 ) 
  std::cout << "  ****  CLHEPTransform3DStreamer::CopyOldToNew() " << std::endl;
#endif
#if( TRANSF_DBGL > 1 )   
  std::cout << "  TRFM= " << old_tr.m1 <<"," << old_tr.m2 <<"," << old_tr.m3 <<"," << old_tr.m4 << std::endl;
  std::cout << "  TRFM= " << old_tr.m5 <<"," << old_tr.m6 <<"," << old_tr.m7 <<"," << old_tr.m8 << std::endl;
  std::cout << "  TRFM= " << old_tr.m9 <<"," << old_tr.m10<<"," << old_tr.m11<<"," << old_tr.m12<< std::endl;
#endif
  Transform3DInit& new_init = static_cast<Transform3DInit&> (new_tr);
  new_init.setTransform
    (old_tr.m4,  old_tr.m5,  old_tr.m6,  old_tr.m1,   // xx, xy, xz, dx
     old_tr.m7,  old_tr.m8,  old_tr.m9,  old_tr.m2,   // yx, yy, yz, dy
     old_tr.m10, old_tr.m11, old_tr.m12, old_tr.m3);  // zx, zy, zz, dz
#if( TRANSF_DBGL > 1 ) 
  std::cout << "  TRFM new= " << new_tr.xx() <<"," << new_tr.xy() <<"," << new_tr.xz() <<"," << new_tr.dx() << std::endl;
  std::cout << "  TRFM new= " << new_tr.yx() <<"," << new_tr.yy() <<"," << new_tr.yz() <<"," << new_tr.dy() << std::endl;
  std::cout << "  TRFM new= " << new_tr.zx() <<"," << new_tr.zy() <<"," << new_tr.zz() <<"," << new_tr.dz() << std::endl;
#endif
}

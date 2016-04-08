/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MdtStationGeometryRow_h
#define MuonCalib_MdtStationGeometryRow_h

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TTree.h"

namespace MuonCalib {

class MdtStationGeometryRow {
 public:
  Float_t xx, xy, xz,
    yx, yy, yz,
    zx, zy, zz;
  Float_t dx, dy, dz;
	
  inline void CreateBranches(TTree * tree) {
    tree->Branch("xx", &xx, "xx/F");
    tree->Branch("xy", &xy, "xy/F");
    tree->Branch("xz", &xz, "xz/F");
    tree->Branch("yx", &yx, "yx/F");
    tree->Branch("yy", &yy, "yy/F");
    tree->Branch("yz", &yz, "yz/F");
    tree->Branch("zx", &zx, "zx/F");
    tree->Branch("zy", &zy, "zy/F");
    tree->Branch("zz", &zz, "zz/F");
    tree->Branch("dx", &dx, "dx/F");
    tree->Branch("dy", &dy, "dy/F");
    tree->Branch("dz", &dz, "dz/F");
  }
	
  inline void ReadHepTransform(const Amg::Transform3D &hep_transform) {
    Amg::RotationMatrix3D hep_rot(hep_transform.rotation());
    Amg::Vector3D hep_trans(hep_transform.translation());
    xx = hep_rot.col(0).x();
    xy = hep_rot.col(0).y();
    xz = hep_rot.col(0).z();
    yx = hep_rot.col(1).x();
    yy = hep_rot.col(1).y();
    yz = hep_rot.col(1).z();
    zx = hep_rot.col(2).x();
    zy = hep_rot.col(2).y();
    zz = hep_rot.col(2).z();
    dx = hep_trans.x();
    dy = hep_trans.y();
    dz = hep_trans.z();
  }
		
  inline void SetBranchAddress(TTree * tree) {
    tree->SetBranchAddress("xx", &xx);
    tree->SetBranchAddress("xy", &xy);
    tree->SetBranchAddress("xz", &xz);
    tree->SetBranchAddress("yx", &yx);
    tree->SetBranchAddress("yy", &yy);
    tree->SetBranchAddress("yz", &yz);
    tree->SetBranchAddress("zx", &zx);
    tree->SetBranchAddress("zy", &zy);
    tree->SetBranchAddress("zz", &zz);
    tree->SetBranchAddress("dx", &dx);
    tree->SetBranchAddress("dy", &dy);
    tree->SetBranchAddress("dz", &dz);
  }
	  
  inline  HepGeom::Transform3D GetTransform() {
    CLHEP::Hep3Vector colx( xx ,xy, xz );
    CLHEP::Hep3Vector coly( yx, yy, yz );
    CLHEP::Hep3Vector colz( zx, zy, zz );
    CLHEP::HepRotation rotation( colx, coly, colz );
    CLHEP::Hep3Vector  translation(dx, dy, dz);
    HepGeom::Transform3D trans( rotation, translation );	    
    return trans;
  }
  
};

}  //namespace MuonCalib
#endif

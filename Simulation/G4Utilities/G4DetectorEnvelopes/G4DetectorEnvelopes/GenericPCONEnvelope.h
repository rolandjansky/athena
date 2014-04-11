/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericPCONEnvelope_H
#define GenericPCONEnvelope_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>
#include <assert.h>
#include <string>
class G4LogicalVolume;

class GenericPCONEnvelope: public FADS::DetectorFacility {
public:
  GenericPCONEnvelope(G4String);
  void BuildGeometry();
  void SetMaterial(std::string);
  void SetNSurfaces(int i) {nsurfaces=i;}
  void SetPhiMin(double p) {phimin=p;}
  void SetDeltaPhi(double p) {deltaphi=p;}
  void SetZSurface(int i,double z)
  {
    assert (i>=0 && (unsigned int)i<nsurfaces);
    if (!z_surface) z_surface=new double[nsurfaces];
    *(z_surface+i)=z;
  }
  void SetRinSurface(int i,double r)
  {
    assert (i>=0 && (unsigned int)i<nsurfaces);
    if (!rin_surface) rin_surface=new double[nsurfaces];
    *(rin_surface+i)=r;
  }
  void SetRouSurface(int i,double r)
  {
    assert (i>=0 && (unsigned int)i<nsurfaces);
    if (!rou_surface) rou_surface=new double[nsurfaces];
    *(rou_surface+i)=r;
  }
private:
  unsigned int nsurfaces;
  double phimin;
  double deltaphi;
  double *z_surface;
  double *rin_surface;
  double *rou_surface;
  G4LogicalVolume *theVolume;
  std::string material;
};
#endif

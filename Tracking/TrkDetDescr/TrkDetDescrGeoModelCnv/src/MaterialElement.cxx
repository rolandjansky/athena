/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialElement.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDetDescrGeoModelCnv/MaterialElement.h"

double Trk::MaterialElement::mass()
{ 
  if (!material) return 0.;
  return material->getDensity()/CLHEP::g*CLHEP::mm3*geoVolume;
}

double Trk::MaterialElement::volume() 
{ 
  double dphi = phimax-phimin;
  if (dphi<=0 || dphi>2*acos(-1.)) dphi = 2*acos(-1.) ; 
  return (zmax-zmin)*dphi/2*(rmax*rmax-rmin*rmin);
}

double Trk::MaterialElement::thMax() 
{ 
  if (zmin==0.) return acos(-1.)/2.;
  double r = zmin<0 ? rmin : rmax;
  return (zmin>0.? atan(r/zmin) : atan(r/zmin)+acos(-1.)) ; 
}
    
double Trk::MaterialElement::thMin() 
{ 
  if (zmax==0.) return acos(-1.)/2.;
  double r = zmax<0 ? rmax : rmin;
  return (zmax>0.? atan(r/zmax) : atan(r/zmax)+acos(-1.)) ; 
}
    
double Trk::MaterialElement::etaMin() 
{ 
  if (zmin==0.) return 0;
  double r = zmin<0 ? rmin : rmax;
  double th = zmin>0.? atan(r/zmin) : atan(r/zmin)+acos(-1.) ;
  return -log(tan(th/2.)); 
}
    
double Trk::MaterialElement::etaMax() 
{ 
  if (zmax==0.) return 0;
  double r = zmax<0 ? rmax : rmin;
  double th = zmax>0.? atan(r/zmax) : atan(r/zmax)+acos(-1.) ;
  return -log(tan(th/2.)); 
}

void Trk::MaterialElement::setMaterial(Trk::Material mp) { matProp = Trk::Material(mp); };

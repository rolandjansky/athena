/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCalibAlgs/PixelBarrelSurveyUtils.h"
#include <cmath>

using namespace std;

namespace PixelBarrelSurveyUtils {


ModuleStruct* ReadModule(istream &input) {
  ModuleStruct *newModule = new ModuleStruct;
  input >> newModule->serialNumber;
  // cerr << newModule->serialNumber << endl;
  for (int i=0; i<3; i++) input >> newModule->xcenter[i];
  for (int i=0; i<3; i++) input >> newModule->angles[i];
  for (int i=0; i<3; i++) input >> newModule->disto[i];
  if ( input.bad() || input.eof() ) {
    delete newModule; 
    return 0;
  } else return newModule;
}

StaveStruct* ReadStave(istream &input) {
  StaveStruct *newStave=new StaveStruct;
  input >> newStave->serialNumber;
  // cerr << newStave->serialNumber << endl;
  for (int i=0; i<3; i++) input >> newStave->sphereA[i];
  for (int i=0; i<3; i++) input >> newStave->sphereC[i];
  input >> newStave->position;
  for (int i=0; i<3; i++) input >> newStave->sphereA1[i];
  for (int i=0; i<3; i++) input >> newStave->sphereC1[i];
  string dummy; input >> dummy;
  for (int i=0; i<4; i++) input >> newStave->plane[i];
  for (int i=0; i<ModulesOnStave; i++) {
    ModuleStruct *temp=ReadModule(input);
    if (temp==0) break;
    newStave->module[i]=*temp;
    delete temp;
  }
  if ( input.bad() || input.eof() ) {
    delete newStave;
    return 0;
  } else return newStave;
}

bool OutputDistorsion(ostream& out, ModuleStruct& module) {
  ios_base::fmtflags original_flags = out.flags();
  out << module.offlineID << "\t" << module.hashID;
  out.precision(7);
  for (int i=0; i<3; i++) out << "\t" << module.disto[i];
  out << endl;
  out.flags(original_flags);
  return out.good();
}


HepGeom::Transform3D GetModuleTransform(ModuleStruct& module) {
  // In local frame the zero point is in the center of the active region.
  // In survey data, z is measured at the surface of the front end chip:
  static const CLHEP::HepRotation id;
  static const double thickness = 250.*CLHEP::micrometer; // active area thickness
  static const double bumpheight=   5.*CLHEP::micrometer; // bump height
  static const double sensorlen = 60.8*CLHEP::millimeter; // active area length
  double height=thickness/2.+bumpheight;
  // Additional correcttion to have average delta z correction to zero. 
  const double m = tan(0.5*module.disto[2]*CLHEP::degree);
  const double z1= ((1+m*m+m*m*m*m)/sqrt(1+m*m))*sensorlen*sensorlen*(module.disto[0]/CLHEP::meter)/24.;
  const double z2= ((1+m*m+m*m*m*m)/sqrt(1+m*m))*sensorlen*sensorlen*(module.disto[1]/CLHEP::meter)/24.;
  height+=0.5*(z1+z2);
  HepGeom::Vector3D<double> d(0.,0.,-height);
  HepGeom::Transform3D offset(id,d);
  // local module module reference frame is oriented with
  // y axis along the long pixel direction, and z axis outgoing 
  // from the module, while the stave frame has the y axis outgoing
  // from the stave plane and the z axis along ATLAS z 
  // (~module y axis).
  // newAxis is a rotation that reorder that set of axis 
  static const HepGeom::RotateX3D r1(M_PI/2.);
  // static const RotateY3D r2(M_PI);
  // static const Transform3D newAxis=r2*r1;
  HepGeom::Transform3D newAxis=r1*offset;
  // return newAxis;
  HepGeom::RotateX3D rx(-module.angles[0]*CLHEP::degree); // CLHEP::degree is declared in CLHEP
  HepGeom::RotateY3D ry(module.angles[1]*CLHEP::degree);
  HepGeom::RotateZ3D rz(module.angles[2]*CLHEP::degree);
  HepGeom::Transform3D rot=rz*ry*rx*newAxis;
  HepGeom::Vector3D<double> v(module.xcenter[0],module.xcenter[1],module.xcenter[2]);
  HepGeom::Transform3D tr(id,v);
  return tr*rot;
}

HepGeom::Transform3D GetStaveTransform(StaveStruct& stave) {
  // use the transformation which transform three space points
  // in one system to three space point in the other.
  // first of all, project spheres from halfshell survey on stave plane
  double dist, spA[3], spC[3];
  dist = stave.plane[3];
  for (int i=0; i<3; i++) dist+=stave.sphereA[i]*stave.plane[i];
  for (int i=0; i<3; i++) spA[i]=stave.sphereA[i]-dist*stave.plane[i];
  dist = stave.plane[3];
  for (int i=0; i<3; i++) dist+=stave.sphereC[i]*stave.plane[i];
  for (int i=0; i<3; i++) spC[i]=stave.sphereC[i]-dist*stave.plane[i];
  // 1) center of spheres
  HepGeom::Point3D<double> x0h(0.5*(spA[0]+spC[0]),
		      0.5*(spA[1]+spC[1]),
		      0.5*(spA[2]+spC[2]));
  HepGeom::Point3D<double> x0s(0.5*(stave.sphereA1[0]+stave.sphereC1[0]),
		      0.,
		      0.5*(stave.sphereA1[2]+stave.sphereC1[2]));
  // 2) moving vertically by 1 mm
  HepGeom::Point3D<double> x1h(x0h.x()+stave.plane[0],
		      x0h.y()+stave.plane[1],
		      x0h.z()+stave.plane[2]);
  HepGeom::Point3D<double> x1s(x0s.x(),x0s.y()+1.,x0s.z());
  // 3) movine by 1 mm along the line connecting the two spheres
  dist=0.;
  for (int i=0; i<3; i++) dist+=pow(spA[i]-spC[i],2.);
  dist=sqrt(dist);
  HepGeom::Point3D<double> x2h(x0h.x()+(spA[0]-spC[0])/dist,
		      x0h.y()+(spA[1]-spC[1])/dist,
		      x0h.z()+(spA[2]-spC[2])/dist);
  dist=0.;
  for (int i=0; i<3; i+=2) // skip y component, which is zero after projection
    dist+=pow(stave.sphereA1[i]-stave.sphereC1[i],2.); 
  dist=sqrt(dist);
  HepGeom::Point3D<double> x2s(x0s.x()+(stave.sphereA1[0]-stave.sphereC1[0])/dist,
		      x0s.y(),
		      x0s.z()+(stave.sphereA1[2]-stave.sphereC1[2])/dist);
  HepGeom::Transform3D m(x0s,x1s,x2s,x0h,x1h,x2h);
  return m;
}

// eta_index goes from -6 (module M6C) to +6 (module M6A)
HepGeom::Transform3D GetModuleTransform(StaveStruct& stave,int eta_index) {
  HepGeom::Transform3D modToStave = GetModuleTransform(stave.module[eta_index+6]);
  HepGeom::Transform3D staveToATLAS = GetStaveTransform(stave);
  return staveToATLAS*modToStave;
}



}

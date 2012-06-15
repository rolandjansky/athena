/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OLD_CLHEP_STREAMERS_H_
#define OLD_CLHEP_STREAMERS_H_

/*
  2006 CERN  Marcin Nowak

  Athena/ROOT custom streamers for CLHEP v.1.8.2 classes
  
*/

#include "AthenaPoolServices/T_AthenaRootConverter.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "OldCLHEP.h"


class CLHEPVec3dStreamer  : public T_AthenaRootConverter<HepGeom::Vector3D<double>, OldHepVector3D > {  
public:
  CLHEPVec3dStreamer() {
     SetClassNameAndChecksum("HepVector3D", 358881035U);
  }          
  void CopyOldToNew(const OldHepVector3D &old_vec, HepGeom::Vector3D<double> &new_vec )
  {
     HepGeom::Vector3D<double>	vec( old_vec.x(), old_vec.y(), old_vec.z() );
     new_vec = vec;
   } 
};



class CLHEPPoint3dStreamer  : public T_AthenaRootConverter<HepGeom::Point3D<double>, OldHepPoint3D > {  
public:
  CLHEPPoint3dStreamer() {
     SetClassNameAndChecksum("HepPoint3D", 1634550480U);
  }          
  void CopyOldToNew(const OldHepPoint3D &old_point, HepGeom::Point3D<double> &new_point );
};



class CLHEPRotationStreamer  : public T_AthenaRootConverter<CLHEP::HepRotation, OldHepRotation >
{
public:
  CLHEPRotationStreamer() {
     SetClassNameAndChecksum("HepRotation", 4141898558U);
  }
};


template<>
void
T_AthenaRootConverter<CLHEP::HepGenMatrix, OldHepGenMatrix >
::CopyOldToNew(const OldHepGenMatrix &, CLHEP::HepGenMatrix & ) {}

class CLHEPGenMatrixStreamer  : public T_AthenaRootConverter<CLHEP::HepGenMatrix, OldHepGenMatrix >
{
public:
  CLHEPGenMatrixStreamer() {  SetClassNameAndChecksum("HepGenMatrix", 21721098U);  }
};



class CLHEPMatrixStreamer  : public T_AthenaRootConverter<CLHEP::HepMatrix, OldHepMatrix >
{
public:
  CLHEPMatrixStreamer() {
     SetClassNameAndChecksum("HepMatrix", 3811046672U);
  }
  void CopyOldToNew(const OldHepMatrix &old_matx, CLHEP::HepMatrix &new_matx );
};


class CLHEPLorVecStreamer  : public T_AthenaRootConverter<CLHEP::HepLorentzVector, OldHepLorentzVector >
{
public:
  CLHEPLorVecStreamer() {
     SetClassNameAndChecksum("HepLorentzVector", 3077056266U);
  }
  void CopyOldToNew(const OldHepLorentzVector &old_vec, CLHEP::HepLorentzVector &new_vec );
};


class CLHEP3VectorStreamer  : public T_AthenaRootConverter<CLHEP::Hep3Vector, OldHep3Vector >
{
public:
  CLHEP3VectorStreamer() {
     SetClassNameAndChecksum("Hep3Vector", 760000369U);
  }
  void CopyOldToNew(const OldHep3Vector &old_vec, CLHEP::Hep3Vector &new_vec );
};



class CLHEPBasicVectorStreamer  : public T_AthenaRootConverter<HepGeom::BasicVector3D<double>, OldBasicVector3D >
{
public:
  CLHEPBasicVectorStreamer() {
     SetClassNameAndChecksum("BasicVector3D", 2681080162U);
  }
  void CopyOldToNew(const OldBasicVector3D &old_vec, HepGeom::BasicVector3D<double> &new_vec );
};




namespace HepGeom { class Transform3D; }
class CLHEPTransform3DStreamer  : public T_AthenaRootConverter<HepGeom::Transform3D, OldHepTransform3D >
{
public:
  CLHEPTransform3DStreamer() {
     SetClassNameAndChecksum("HepTransform3D", 520750269U);
  }
  void CopyOldToNew(const OldHepTransform3D &old_, HepGeom::Transform3D &new_);
};

#endif

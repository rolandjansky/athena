/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_ALINEPAR_H
#define MUONALIGNMENTDATA_ALINEPAR_H

#include "CLHEP/Geometry/Transform3D.h"

#include "MuonAlignmentData/MuonAlignmentPar.h"

class ALinePar : public MuonAlignmentPar {

 public:

  // Default constructor
  ALinePar();
  // destructor
  ~ALinePar();

  // Full constructor


  void setParameters(float s, float z, float t, 
		     float rotS, float rotZ, float rotT);

  void getParameters(float& s, float& z, float& t, 
		     float& rotS, float& rotZ, float& rotT);
  
  HepGeom::Transform3D deltaTransform();
  
 private:

  // traslation parameters
  float m_S;
  float m_Z;
  float m_T;
  // rotation parameters
  float m_rotS;
  float m_rotZ;
  float m_rotT;


};


#endif  // MUONALIGNMENTDATA_ALINEPAR_H

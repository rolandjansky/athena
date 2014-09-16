/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_CSCLINEPAR_H
#define MUONALIGNMENTDATA_CSCLINEPAR_H


#include "MuonAlignmentData/MuonAlignmentPar.h"

class CscInternalAlignmentPar : public MuonAlignmentPar {

 public:

  // Default constructor
  CscInternalAlignmentPar();
  // destructor
  ~CscInternalAlignmentPar();

  // Full constructor

  void setAmdbId(std::string type, int jff, int jzz, int job, int wireLayer);
  void getAmdbId(std::string& type, int& jff, int& jzz, int& job, int& wireLayer);

  void setParameters(float s, float z, float t, 
		     float rotS, float rotZ, float rotT);

  void getParameters(float& s, float& z, float& t, 
		     float& rotS, float& rotZ, float& rotT);
  
 private:
    // wire layer identifier
  int m_wireLayer;  

  // traslation parameters
  float m_S;
  float m_Z;
  float m_T;
  // rotation parameters
  float m_rotS;
  float m_rotZ;
  float m_rotT;


};


#endif  // MUONALIGNMENTDATA_CSCLINEPAR_H

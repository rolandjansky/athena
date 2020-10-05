/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_CSCLINEPAR_H
#define MUONALIGNMENTDATA_CSCLINEPAR_H


#include "MuonAlignmentData/MuonAlignmentPar.h"

class CscInternalAlignmentPar : public MuonAlignmentPar {

 public:

  // Default constructor
  CscInternalAlignmentPar();
  // destructor
  virtual ~CscInternalAlignmentPar() override = default;

  void setAmdbId(const std::string& type, int jff, int jzz, int job, int wireLayer);
  void getAmdbId(std::string& type, int& jff, int& jzz, int& job, int& wireLayer) const;

  void setParameters(float s, float z, float t, 
		     float rotS, float rotZ, float rotT);

  void getParameters(float& s, float& z, float& t, 
		     float& rotS, float& rotZ, float& rotT) const;
  
 private:
    // wire layer identifier
  int m_wireLayer;  

  // translation parameters
  float m_S;
  float m_Z;
  float m_T;
  // rotation parameters
  float m_rotS;
  float m_rotZ;
  float m_rotT;


};


#endif  // MUONALIGNMENTDATA_CSCLINEPAR_H

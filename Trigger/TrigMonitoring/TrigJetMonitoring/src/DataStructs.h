/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_DATASTRUCTS_H
#define TRIGJETMONITORING_DATASTRUCTS_H

#include <string>

class JetData {
public:
JetData(float et, float eta, float phi, std::string et_label) :
  m_et{et}, m_eta{eta}, m_phi{phi}, m_et_label{et_label}{}
  
  float m_et;
  float m_eta;
  float m_phi;
  std::string m_et_label;  //"et", "et8x8" ...
};

class JetMatchData {
public:
JetMatchData(float dPt, float dEnergy, float dMass,
	     float rPt, float rEnergy, float rMass,
	     float ptRef, float etaRef) :
  
  m_dPt{dPt}, m_dEnergy{dEnergy}, m_dMass{dMass},
    m_rPt{rPt}, m_rEnergy{rEnergy}, m_rMass{rMass},
    m_ptRef{ptRef}, m_etaRef{etaRef}{}

  
  float m_dPt;
  float m_dEnergy;
  float m_dMass;
  float m_rPt;
  float m_rEnergy;
  float m_rMass;
  float m_ptRef;
  float m_etaRef;
};

#endif

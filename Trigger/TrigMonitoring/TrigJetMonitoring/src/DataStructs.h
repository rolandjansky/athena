/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_DATASTRUCTS_H
#define TRIGJETMONITORING_DATASTRUCTS_H

struct JetData {
JetData(float et, float eta, float phi) : m_et{et}, m_eta{eta}, m_phi{phi}{}
  
  float m_et;
  float m_eta;
  float m_phi;
};

struct MatchedJetData {
MatchedJetData(float dPt, float dEnergy, float dMass,
	       float rPt, float rEnergy, float rMass,
	       float ptRef, float etRef) :
    
  m_dPt{dPt}, m_dEnergy{dEnergy}, m_dMass{dMass},
    m_rPt{rPt}, m_rEnergy{rEnergy}, m_rMass{rMass},
    m_ptRef{ptRef}, m_etRef{etRef}{}

  
  float m_dPt;
  float m_dEnergy;
  float m_dMass;
  float m_rPt;
  float m_rEnergy;
  float m_rMass;
  float m_ptRef;
  float m_etRef;
};

#endif

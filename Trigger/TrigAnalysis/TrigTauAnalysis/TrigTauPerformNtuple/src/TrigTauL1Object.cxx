/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigTauPerformNtuple/TrigTauL1Object.h"
ClassImp(TrigTauL1Object )

TrigTauL1Object::TrigTauL1Object()
  :  TLorentzVector(0,0,0,0)
  ,  m_EmEnergy(0) 
  ,  m_TauEnergy(0)
  ,  m_EmIsol(0)   
  ,  m_HdIsol(0)   
  ,  m_EmCore(0)   
  ,  m_HdCore(0)   
  ,  m_EmThresh(0) 
  ,  m_TauThresh(0)
  ,  m_roiWord(0)
{}

TrigTauL1Object::TrigTauL1Object(const TrigTauL1Object & other)
  : TLorentzVector(other)
  ,  m_EmEnergy(other.m_EmEnergy) 
  ,  m_TauEnergy(other.m_TauEnergy)
  ,  m_EmIsol(other.m_EmIsol)   
  ,  m_HdIsol(other.m_HdIsol)   
  ,  m_EmCore(other.m_EmCore)   
  ,  m_HdCore(other.m_HdCore)   
  ,  m_EmThresh(other.m_EmThresh) 
  ,  m_TauThresh(other.m_TauThresh)
  ,  m_roiWord(other.m_roiWord)
{}

 TrigTauL1Object::~TrigTauL1Object() {}
 void TrigTauL1Object::reset()
{
  SetPtEtaPhiM(0,0,0,0);
  m_EmEnergy = 0;
  m_TauEnergy= 0;
  m_EmIsol   = 0;
  m_HdIsol   = 0;
  m_EmCore   = 0;
  m_HdCore   = 0;
  m_EmThresh = 0;
  m_TauThresh= 0;
  m_roiWord = 0;
}


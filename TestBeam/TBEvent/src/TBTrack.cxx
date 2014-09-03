/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBTrack.h"

TBTrack::TBTrack(int hitNumberU,int hitNumberV){
  m_hitNumberU = hitNumberU;
  m_hitNumberV = hitNumberV;
  m_residualu.resize(m_hitNumberU);
  m_residualv.resize(m_hitNumberV);
  m_chi2 = 0;
  m_chi2u = 0;
  m_chi2v = 0;
  m_angle = 0;
  m_uslope = 0;
  m_vslope = 0;
  m_uintercept = 0;
  m_vintercept = 0;
  m_cryou = 0;
  m_cryov = 0;
  m_cryow = 0;

}

TBTrack::~TBTrack(){}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBTrack.h"

TBTrack::TBTrack(int hitNumberU,int hitNumberV){
  m_hitNumberU = hitNumberU;
  m_hitNumberV = hitNumberV;
  m_residualu.resize(m_hitNumberU);
  m_residualv.resize(m_hitNumberV);
}

TBTrack::TBTrack (const std::vector<double>& residualU,
                  const std::vector<double>& residualV)
  : m_hitNumberU (residualU.size()),
    m_hitNumberV (residualV.size()),
    m_residualu (residualU),
    m_residualv (residualV)
{
}

TBTrack::~TBTrack(){}

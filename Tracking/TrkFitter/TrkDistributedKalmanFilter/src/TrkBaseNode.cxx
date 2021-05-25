/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkBaseNode.cpp
//    Implementation of class TrkBaseNode
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkPrepRawData/PrepRawData.h"

namespace Trk {

  TrkBaseNode::TrkBaseNode():m_chi2Cut{},m_nodeType{},m_pSurface{},m_pPRD{}
  {
    m_ndof=0;m_dChi2=0.0;
    m_nodeState=1;
    m_pTrackState=nullptr;
  }
	
  bool TrkBaseNode::isValidated()
  {
    return (m_nodeState!=0);
  }

  TrkPlanarSurface* TrkBaseNode::getSurface()
  {
    return m_pSurface;
  }

  const PrepRawData* TrkBaseNode::getPrepRawData()
  {
    return m_pPRD;
  }
  
  TrkTrackState* TrkBaseNode::getTrackState()
  {
    return m_pTrackState;
  }
  
  TrkBaseNode::~TrkBaseNode()
  {

  }

  void TrkBaseNode::setNodeState(int s)
  {
    m_nodeState=s;
  }

  int TrkBaseNode::getNodeState()
  {
    return m_nodeState;
  }

  void TrkBaseNode::setNodeType(char s)
  {
    m_nodeType=s;
  }

  char TrkBaseNode::getNodeType()
  {
    return m_nodeType;
  }
  
  int TrkBaseNode::getNdof() const
  {
    return m_ndof;
  }

  double TrkBaseNode::getChi2() const
  {
    return m_dChi2;
  }

  void TrkBaseNode::updateInternal()
  {

  }

}

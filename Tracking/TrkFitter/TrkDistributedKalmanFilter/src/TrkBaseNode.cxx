/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
	
  bool TrkBaseNode::m_isValidated()
  {
    return (m_nodeState!=0);
  }

  TrkPlanarSurface* TrkBaseNode::m_getSurface()
  {
    return m_pSurface;
  }

  const PrepRawData* TrkBaseNode::m_getPrepRawData()
  {
    return m_pPRD;
  }
  
  TrkTrackState* TrkBaseNode::m_getTrackState()
  {
    return m_pTrackState;
  }
  
  TrkBaseNode::~TrkBaseNode()
  {

  }

  void TrkBaseNode::m_setNodeState(int s)
  {
    m_nodeState=s;
  }

  int TrkBaseNode::m_getNodeState()
  {
    return m_nodeState;
  }

  void TrkBaseNode::m_setNodeType(char s)
  {
    m_nodeType=s;
  }

  char TrkBaseNode::m_getNodeType()
  {
    return m_nodeType;
  }
  
  int TrkBaseNode::m_getNdof()
  {
    return m_ndof;
  }

  double TrkBaseNode::m_getChi2()
  {
    return m_dChi2;
  }

  void TrkBaseNode::m_updateInternal()
  {

  }

}

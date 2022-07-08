//////////////////////////////////////////////////////////////////
// TrkBaseNode.cpp
//    Implementation of class TrkBaseNode
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#include "TrkBaseNode.h"
#include "TrkPlanarSurface.h"
#include "TrkTrackState.h"

TrkBaseNode::TrkBaseNode() : m_chi2Cut(0.0),m_nodeType(0),m_pSurface(0)
{
  m_ndof=0;m_dChi2=0.0;
  m_nodeState=1;
  m_pTrackState=0;
}

bool TrkBaseNode::isValidated()
{
  return (m_nodeState!=0);
}

TrkPlanarSurface* TrkBaseNode::getSurface()
{
  return m_pSurface;
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

int TrkBaseNode::getNdof()
{
  return m_ndof;
}

double TrkBaseNode::getChi2()
{
  return m_dChi2;
}

void TrkBaseNode::updateInternal()
{
}

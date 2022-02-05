/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWAsBuilt/PcbElement.h"
using namespace NswAsBuilt;


//===============================================================================
PcbElement::PcbElement(stripConfiguration_t config, std::reference_wrapper<Element> element)
  : m_config(config), m_element(element)
{ }


//===============================================================================
PcbElement::strip_t PcbElement::getStrip(ParameterClass iclass, int stripNumber) const 
{
  int npitch = stripNumber - m_config.centralStripNumber;
  Eigen::Matrix3d vectorset;
  vectorset.col(0) = m_config.centerPoint.pos + npitch * m_config.centerPoint.pitchvector;
  vectorset.col(1) = m_config.leftPoint.pos + npitch * m_config.leftPoint.pitchvector;
  vectorset.col(2) = m_config.rightPoint.pos + npitch * m_config.rightPoint.pitchvector;

  Element& element = m_element;
  element.transformToFrame(iclass, vectorset, nullptr);

  strip_t ret;
  ret.center = vectorset.col(0);
  ret.left   = vectorset.col(1);
  ret.right  = vectorset.col(2);
  return ret;
}


//===============================================================================
Amg::Vector3D PcbElement::getPositionAlongStrip(ParameterClass iclass, int stripNumber, double sx, double sy) const 
{
  // sx is expected to be in [-1, 1]
  // sy is expected to be in [-0.5, 0.5] (i.e. in a dx band of 1 pitch)
  double npitch = sy + (stripNumber - m_config.centralStripNumber);
  Amg::Vector3D center = m_config.centerPoint.pos + npitch * m_config.centerPoint.pitchvector;
  Amg::Vector3D left   = m_config.leftPoint.pos   + npitch * m_config.leftPoint.pitchvector;
  Amg::Vector3D right  = m_config.rightPoint.pos  + npitch * m_config.rightPoint.pitchvector;

  // Get strip-local coordinate of point along strip
  // Note: left, center and right are exactly in line in local coordinates
  // (i.e. strip is not deformed in local coordinates)
  Amg::Vector3D ret;
  if (sx < 0.0) 
    ret = (sx+1.0)*center - sx*left;
  else
    ret = (1.0-sx)*center + sx*right;

  Element& element = m_element;
  element.transformToFrame(iclass, ret, nullptr);
  return ret;
}




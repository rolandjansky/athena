/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMExtra.h"

void BCMExtra::SetVertex(double X, double Y, double Z)
{
  m_vertexX = X;
  m_vertexY = Y;
  m_vertexZ = Z;
}

double BCMExtra::GetVertexX()
{
  return m_vertexX;
}

double BCMExtra::GetVertexY()
{
  return m_vertexY;
}

double BCMExtra::GetVertexZ()
{
  return m_vertexZ;
}


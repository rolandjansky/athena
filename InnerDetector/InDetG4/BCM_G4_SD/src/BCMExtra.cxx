/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BCM_G4_SD/BCMExtra.h"

void BCMExtra::SetVertex(double X, double Y, double Z)
{
  vertexX = X;
  vertexY = Y;
  vertexZ = Z;
}

double BCMExtra::GetVertexX()
{
  return vertexX;
}

double BCMExtra::GetVertexY()
{
  return vertexY;
}

double BCMExtra::GetVertexZ()
{
  return vertexZ;
}


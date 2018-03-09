/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMEXTRA_H
#define BCMEXTRA_H

class BCMExtra
{
public:
  BCMExtra(): m_vertexX(0.0),m_vertexY(0.0),m_vertexZ(0.0) {};
  virtual ~BCMExtra() {};
  
  void SetVertex(double X, double Y, double Z);
  double GetVertexX();
  double GetVertexY();
  double GetVertexZ();
private:
  double m_vertexX;
  double m_vertexY;
  double m_vertexZ;
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(BCMExtra, 207661756, 1)

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWVERTEXCORRECTION_H
#define EFLOWVERTEXCORRECTION_H

//Author Mark Hodgkinson April 2013 - note the vertex correction algorithm is taken from the CaloObject class.

#include "TVector3.h"

class eflowVertexCorrection {

 public:
  eflowVertexCorrection () {};
  ~eflowVertexCorrection () {};
  void VertexCorrection(TVector3 vertexToCorrectTo, double &clusterEta, double &clusterPhi, double centerMag);

};
#endif

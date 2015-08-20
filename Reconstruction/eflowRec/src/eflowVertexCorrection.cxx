/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowVertexCorrection.h"
#include <cmath>

void eflowVertexCorrection::VertexCorrection(TVector3 vertexToCorrectTo, double &clusterEta, double &clusterPhi, double centerMag){

  double radius = centerMag/cosh(clusterEta);

  double EtaVertexCorr = 0.0, PhiVertexCorr = 0.0;
  
  if (radius > 1.0 && centerMag > 1e-3){
    EtaVertexCorr = (-vertexToCorrectTo.Z()/cosh(clusterEta) + (vertexToCorrectTo.X()*cos(clusterPhi) + vertexToCorrectTo.Y()*sin(clusterPhi))*tanh(clusterEta))/radius;
    PhiVertexCorr = (vertexToCorrectTo.X()*sin(clusterPhi) - vertexToCorrectTo.Y()*cos(clusterPhi))/radius;
  }

  clusterEta += EtaVertexCorr;
  clusterPhi += PhiVertexCorr;

}

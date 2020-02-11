/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CANDIDATEMATCHHELPERS_H
#define CANDIDATEMATCHHELPERS_H

#include <cmath>
#include <iostream>
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace CandidateMatchHelpers{
/**Function to get the (x,y,z) of the cluster wrt to a point (x0,y0,z0)*/
Amg::Vector3D approxXYZwrtPoint (const xAOD::CaloCluster& cluster, 
                                 const Amg::Vector3D& point, 
                                                                  const bool isEndCap);


/**Function to get the (x,y,z) of the cluster in the ATLAS frame origin (0,0,0)*/
Amg::Vector3D approxXYZwrtATLAS (const xAOD::CaloCluster& clusterEta, 
                                 const bool isEndCap);

/** Function to calculate the approximate rotation in phi/bending of a track until it reaches the calo*/
double PhiROT(const double pt,const double eta, const int charge, 
              const double r_start,const bool isEndCap);


double CorrectedEta(const double clusterEta,const double z_first,const bool isEndCap);

}


#endif

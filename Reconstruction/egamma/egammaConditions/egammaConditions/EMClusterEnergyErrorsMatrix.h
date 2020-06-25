/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMClusterEnergyErrorsMatrix

	The EMClusterEnergyErrorsMatrix class is derived of EMClusterErrorsMatrix and was
	designed to represents the cluster energy resulutions. The current binning is in eta.

    @author Jovan.Mitrevski@cern.ch
**/
/////////////////////////////////////////////////////////////////////////////

#ifndef EMCLUSTERENERGYERRORSMATRIX_H
#define EMCLUSTERENERGYERRORSMATRIX_H
#include "egammaConditions/EMClusterErrorsMatrix.h"

class EMClusterEnergyErrorsMatrix : public EMClusterErrorsMatrix
{
public :
  // Constructors
  EMClusterEnergyErrorsMatrix(); // for use when afterwards read from db
  EMClusterEnergyErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
			      const std::string& textDescription = "");
  /** Default destructor*/
  ~EMClusterEnergyErrorsMatrix() {};
  
  StatusCode setError(double eta, float a, float b, float c);
  /** Get (reconstruction/trigger) efficiency for a track
      @param vec: Vector with eta, phi and pt information of track*/
  double getError(double eta, double energyCluster) const;
  
};

#endif

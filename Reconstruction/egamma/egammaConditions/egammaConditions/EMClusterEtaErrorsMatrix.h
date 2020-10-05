/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMClusterEtaErrorsMatrix

	The EMClusterEtaErrorsMatrix class is derived of EMClusterErrorsMatrix and was
	designed to represents the cluster eta resulutions. The current binning is in eta.

    @author Jovan.Mitrevski@cern.ch
**/
/////////////////////////////////////////////////////////////////////////////

#ifndef EMCLUSTETAERRORSMATRIX_H
#define EMCLUSTETAERRORSMATRIX_H
#include "egammaConditions/EMClusterErrorsMatrix.h"

class EMClusterEtaErrorsMatrix : public EMClusterErrorsMatrix
{
public :
  // Constructors
  EMClusterEtaErrorsMatrix(); // for use when afterwards read from db
  EMClusterEtaErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
			   const std::string& textDescription = "");
  /** Default destructor*/
  ~EMClusterEtaErrorsMatrix() {};
  
  // need to figure out which eta to use.
  StatusCode setError(double eta, float a);
  /** Get (reconstruction/trigger) efficiency for a track
      @param vec: Vector with eta, phi and pt information of track*/
  double getError(double eta, double energyCluster) const;

};

#endif

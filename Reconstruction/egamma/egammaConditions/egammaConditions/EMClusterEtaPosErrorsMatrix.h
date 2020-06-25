/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMClusterEtaPosErrorsMatrix

	The EMClusterEtaPosErrorsMatrix class is derived of EMClusterErrorsMatrix and was
	designed to represents the cluster eta resulutions. The current binning is in eta.

    @author T. Cuhadar Donszelmann, Jovan.Mitrevski@cern.ch
**/
/////////////////////////////////////////////////////////////////////////////

#ifndef EMCLUSTETAPOSERRORSMATRIX_H
#define EMCLUSTETAPOSERRORSMATRIX_H
#include "egammaConditions/EMClusterErrorsMatrix.h"

class EMClusterEtaPosErrorsMatrix : public EMClusterErrorsMatrix
{
public :
  // Constructors
  EMClusterEtaPosErrorsMatrix(); // for use when afterwards read from db
  EMClusterEtaPosErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
			   const std::string& textDescription = "");
  /** Default destructor*/
  ~EMClusterEtaPosErrorsMatrix() {};
  
  // need to figure out which eta to use.
  StatusCode setError(double eta, float a, float b);
  /** Get (reconstruction/trigger) efficiency for a track
      @param vec: Vector with eta, phi and pt information of track*/
  double getError(double eta, double energyCluster) const;

};

#endif

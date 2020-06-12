/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMClusterPhiErrorsMatrix

	The EMClusterPhiErrorsMatrix class is derived of EMClusterErrorsMatrix and was
	designed to represents the cluster phi resulutions. The current binning is in eta.

    @author Jovan.Mitrevski@cern.ch
**/
/////////////////////////////////////////////////////////////////////////////

#ifndef EMCLUSTPHIERRORSMATRIX_H
#define EMCLUSTPHIERRORSMATRIX_H
#include "egammaConditions/EMClusterErrorsMatrix.h"

class EMClusterPhiErrorsMatrix : public EMClusterErrorsMatrix
{
public :
  // Constructors
  EMClusterPhiErrorsMatrix(); // for use when afterwards read from db
  EMClusterPhiErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
			   const std::string& textDescription = "");
  /** Default destructor*/
  ~EMClusterPhiErrorsMatrix() {};

  // need to figure out which eta to use.
  StatusCode setError(double eta, float a);
  /** Get (reconstruction/trigger) efficiency for a track
      @param vec: Vector with eta, phi and pt information of track*/
  double getError(double eta, double energyCluster) const;

};

#endif

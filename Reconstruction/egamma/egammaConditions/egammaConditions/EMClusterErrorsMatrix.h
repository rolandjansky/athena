/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APEMClusterErrorsMatrix

    The APEMClusterErrorsMatrix class is derived of APMatrix and was
    designed to represents the electron cluster  uncertainties.

    @author Jovan.Mitrevski@cern.ch
**/
/////////////////////////////////////////////////////////////////////////////

#ifndef EMCLUSTERERRORSMATRIX_H
#define EMCLUSTERERRORSMATRIX_H

#include "egammaConditions/EMAPMatrix.h"
#include <string>
#include <utility>
#include <vector>

typedef std::vector<float> EMClusterErrorsEntry;

class EMClusterErrorsMatrix : public EMAPMatrix< EMClusterErrorsEntry >
{
  friend class EMInsituDatabaseEntry;

public:
  // Constructors
  /** Defaul constructor*/
  EMClusterErrorsMatrix();
  /** Constructor with std::vector of axes to define dimensions and binnging of this matrix
      @param axes: std::vector of APMatrixAxis objects
  */
  EMClusterErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, const std::string& textDescription);
  /** Default destructor*/
  ~EMClusterErrorsMatrix() {};

  // Acces Operators
  /** Sets bin-content to "apEntry" in bin which corresponds to value std::vector x. Can only used for n-dimensional 
      matrix if the length of the std::vector x ind n.
      @param x: std::vector of length n which represents the values on all axes
      @param apEntry: value which should be set*/
  StatusCode setError(const std::vector<double>& x, const EMClusterErrorsEntry &apEntry);
  /** Get efficiency in bin which corresponds to value std::vector x. Can only used for n-dimensional 
      matrix if the length of the std::vector x ind n.
      @param x: std::vector of length n which represents the values on all axes
  */

  const EMClusterErrorsEntry* getError(std::vector<double> x) const {return getBinContent(std::move(x)); };
  const EMClusterErrorsEntry* getError(double x) const {return getBinContent(x); };  
  const EMClusterErrorsEntry* getError(double x, double y) const {return getBinContent(x,y); };

  void printMatrix() const;	// a function for debugging
		
};

#endif

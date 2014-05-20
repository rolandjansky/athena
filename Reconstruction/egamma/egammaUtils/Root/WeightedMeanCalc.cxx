/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/WeightedMeanCalc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "FourMomUtils/P4Helpers.h"
#include <Eigen/LU> 

/* Note that the vector is of the form:

  v[0] = d0
  v[1] = z0
  v[2] = phi
  v[3] = eta
  v[4] = E

  The matrix is the associated covariance matrix.

  Note that one has to be careful with phi wraparound when making the calculations
  June 07, 2010 change to being a function in a namespace instead of a static
*/

namespace WeightedMeanCalc {
  EigenVectorAndMatrix getWeightedMean( const EigenVectorAndMatrix& vm1, const EigenVectorAndMatrix& vm2) 
  {

    // let's take care of the input for phi wraparound

    Amg::VectorX v1 = vm1.getVector();
    Amg::VectorX v2 = vm2.getVector();

    const double diff = v1[2] - v2[2];

    if (diff != P4Helpers::deltaPhi(v1[2], v2[2])) {
      // temporarily make the smaller to be in [pi,3pi[
      if (diff > 0) {
	v2[2] += 2*M_PI;
      } else {
	v1[2] += 2*M_PI;
      }
    }
    // end of input phi wraparound 

    Amg::MatrixX sumMatrix = vm1.getMatrix()+vm2.getMatrix();
    Amg::MatrixX invSumMatrix = sumMatrix.inverse() ;
    Amg::MatrixX weight = vm1.getMatrix()*invSumMatrix;  
    
    // get weighted mean vector and weighted mean matrix
    Amg::VectorX combinedVec = vm1.getVector() + (weight*(vm2.getVector()-vm1.getVector()));      	
    Amg::MatrixX combinedMat = weight*vm2.getMatrix();
    
    // fix phi wraparound
    if (combinedVec[2] >= M_PI) {
      combinedVec[2] = fmod(combinedVec[2] + M_PI, 2*M_PI) - M_PI;
    } else if (combinedVec[2] < - M_PI) {
      combinedVec[2] = fmod(combinedVec[2] - M_PI, 2*M_PI) + M_PI;
      // make M_PI be -M_PI
      if (combinedVec[2] == M_PI) {
	combinedVec[2] = - M_PI;
      }
    } 

    //Make sure combined matrix is symmetric
    Amg::MatrixX combinedSymMatrix(combinedMat);
    
    return EigenVectorAndMatrix(combinedVec, combinedSymMatrix); 
    
  } 	
}

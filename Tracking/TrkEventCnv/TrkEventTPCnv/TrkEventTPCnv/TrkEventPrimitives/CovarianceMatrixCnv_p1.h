/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COVARIANCE_MATRIX_CNV_P1_H
#define COVARIANCE_MATRIX_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CovarianceMatrixCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
// #include "TrkEventPrimitives/CovarianceMatrix.h"

class MsgStream;

namespace Trk {
  struct CovarianceMatrix{
    std::vector<float> values;
  }; // Original EM was removed from release as part of 19.0.0 migration from CLHEP to Eigen.
}

class CovarianceMatrixCnv_p1
   : public T_AthenaPoolTPCnvBase<Trk::CovarianceMatrix, Trk::HepSymMatrix_p1>
{
public:
  CovarianceMatrixCnv_p1() {}
  
  virtual void persToTrans( const Trk::HepSymMatrix_p1 *persObj,
			    Trk::CovarianceMatrix *transObj,
			    MsgStream &log );
  
  virtual void transToPers( const Trk::CovarianceMatrix *transObj,
			    Trk::HepSymMatrix_p1 *persObj,
			    MsgStream &log );
};

#endif // COVARIANCE_MATRIX_CNV_P1_H

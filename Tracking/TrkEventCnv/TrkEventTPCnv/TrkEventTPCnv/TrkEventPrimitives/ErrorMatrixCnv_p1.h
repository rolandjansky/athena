/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ERROR_MATRIX_CNV_P1_H
#define ERROR_MATRIX_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   ErrorMatrixCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"
#include <vector>                                                                                             \

class MsgStream;

namespace Trk {
  class ErrorMatrix{
  public:
    std::vector<float> values;
  }; // Original EM was removed from release as part of 19.0.0 migration from CLHEP to Eigen.
}

class ErrorMatrixCnv_p1
   : public T_AthenaPoolTPCnvBase<Trk::ErrorMatrix, Trk::HepSymMatrix_p1>
{
public:
  ErrorMatrixCnv_p1() {}
  
  virtual void persToTrans( const Trk::HepSymMatrix_p1*, Trk::ErrorMatrix*, MsgStream& ); 
  virtual void transToPers( const Trk::ErrorMatrix*, Trk::HepSymMatrix_p1*, MsgStream& );
  
private:
  CovarianceMatrixCnv_p1 m_covarianceMatrixCnv;
};

#endif // ERROR_MATRIX_CNV_P1_H

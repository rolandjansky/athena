/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CovarianceMatrixCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"
#include  <stdexcept>

void CovarianceMatrixCnv_p1::persToTrans( const Trk::HepSymMatrix_p1 *persObj, Trk::CovarianceMatrix *transObj, MsgStream &) {
  transObj->values.resize(persObj->m_matrix_val.size());
  transObj->values.insert(transObj->values.begin(),persObj->m_matrix_val.begin(),persObj->m_matrix_val.end());
}


void CovarianceMatrixCnv_p1::transToPers( const Trk::CovarianceMatrix * /**transObj*/, Trk::HepSymMatrix_p1 * /**persObj*/, MsgStream &) {
  throw std::runtime_error("CovarianceMatrixCnv_p1::transToPers is deprecated!");
}

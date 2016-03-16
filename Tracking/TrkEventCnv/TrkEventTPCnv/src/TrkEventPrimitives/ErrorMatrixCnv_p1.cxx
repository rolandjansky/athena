/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   ErrorMatrixCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include  <stdexcept>

void ErrorMatrixCnv_p1::persToTrans( const Trk::HepSymMatrix_p1 *persObj,
				     Trk::ErrorMatrix *transObj,
				     MsgStream & /**log*/ ) 
{
   transObj->values = persObj->m_matrix_val;
}

void ErrorMatrixCnv_p1::transToPers( const Trk::ErrorMatrix * transObj,
				     Trk::HepSymMatrix_p1 * persObj,
				     MsgStream & /**log*/) 
{
  persObj->m_matrix_val = transObj->values;
}

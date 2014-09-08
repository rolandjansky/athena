/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MeasuredNeutralPerigeeCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "TrkNeutralParameters/NeutralParameters.h"
#undef private
#undef protected
#include "TrkEventTPCnv/TrkNeutralParameters/MeasuredNeutralPerigeeCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void MeasuredNeutralPerigeeCnv_p1::persToTrans( const Trk::MeasuredPerigee_p1 *persObj,
					 Trk::NeutralPerigee *transObj,
					 MsgStream &log ) 
{
   fillTransFromPStore( &m_perigeeConverter, persObj->m_perigee, transObj, log );
   Trk::ErrorMatrix dummy;
   fillTransFromPStore( &m_emConverter, persObj->m_errorMatrix, &dummy, log );
   EigenHelpers::vectorToEigenMatrix(dummy.values, *transObj->m_covariance, "MeasuredNeutralAtaSurfaceCnv_p1");
}

void MeasuredNeutralPerigeeCnv_p1::transToPers( const Trk::NeutralPerigee * /**transObj*/,
					 Trk::MeasuredPerigee_p1 * /**persObj*/,
					 MsgStream & /**log*/ ) 
{
  throw std::runtime_error("MeasuredNeutralPerigeeCnv_p1::transToPers is deprecated!");
}



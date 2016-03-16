/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MeasuredPerigeeCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigeeCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"
#include "TrackParametersCovarianceCnv.h"

void MeasuredPerigeeCnv_p1::persToTrans( const Trk::MeasuredPerigee_p1 *persObj,
					 Trk::MeasuredPerigee *transObj,
					 MsgStream &log ) 
{
   fillTransFromPStore( &m_perigeeConverter, persObj->m_perigee, transObj, log );
   
   Trk::ErrorMatrix dummy;
   fillTransFromPStore( &m_emConverter, persObj->m_errorMatrix, &dummy, log );
   TrackParametersCovarianceCnv<Trk::MeasuredPerigee>::setCovariance (transObj, dummy);
}

void MeasuredPerigeeCnv_p1::transToPers( const Trk::MeasuredPerigee *,
					 Trk::MeasuredPerigee_p1 *,
					 MsgStream & ) 
{
  throw std::runtime_error("No more 'measured' track parameters, so MeasuredAtaSurfaceCnv_p1::transToPers is deprecated!");
   // persObj->m_perigee = baseToPersistent( &m_perigeeConverter, transObj, log );
   // persObj->m_errorMatrix = toPersistent( &m_emConverter, &(transObj->localErrorMatrix()), log );  
}



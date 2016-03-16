/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigeeCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"
#include "TrackParametersCovarianceCnv.h"

void MeasuredPerigeeCnv_p2::persToTrans( const Trk::MeasuredPerigee_p1 *persObj, Trk::MeasuredPerigee *transObj, MsgStream &log ) {
    fillTransFromPStore( &m_perigeeConverter, persObj->m_perigee, transObj, log );
    Trk::ErrorMatrix dummy;
    fillTransFromPStore( &m_emConverter, persObj->m_errorMatrix, &dummy, log );
    TrackParametersCovarianceCnv<Trk::MeasuredPerigee>::setCovariance (transObj, dummy);
}

void MeasuredPerigeeCnv_p2::transToPers( const Trk::MeasuredPerigee * /**transObj*/ , Trk::MeasuredPerigee_p1 * /**persObj*/, MsgStream & /**log*/ ) {
  throw std::runtime_error("No more 'measured' track parameters, so MeasuredAtaSurfaceCnv_p1::transToPers is deprecated!");
}



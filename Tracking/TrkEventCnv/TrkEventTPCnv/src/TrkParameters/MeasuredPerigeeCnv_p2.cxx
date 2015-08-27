/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrkParameters/TrackParameters.h"
#undef private
#undef protected
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigeeCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void MeasuredPerigeeCnv_p2::persToTrans( const Trk::MeasuredPerigee_p1 *persObj, Trk::MeasuredPerigee *transObj, MsgStream &log ) {
    fillTransFromPStore( &m_perigeeConverter, persObj->m_perigee, transObj, log );
    Trk::ErrorMatrix dummy;
    fillTransFromPStore( &m_emConverter, persObj->m_errorMatrix, &dummy, log );

    if (!transObj->m_covariance || transObj->m_covariance->size() != 5) {
      delete transObj->m_covariance;
      transObj->m_covariance = new AmgSymMatrix(5);
    }

    EigenHelpers::vectorToEigenMatrix(dummy.values, *transObj->m_covariance, "MeasuredPerigeeCnv_p2");
    
    // std::cout<<"MeasuredPerigee reading ... "<<*transObj<<std::endl;
}

void MeasuredPerigeeCnv_p2::transToPers( const Trk::MeasuredPerigee * /**transObj*/ , Trk::MeasuredPerigee_p1 * /**persObj*/, MsgStream & /**log*/ ) {
  throw std::runtime_error("No more 'measured' track parameters, so MeasuredAtaSurfaceCnv_p1::transToPers is deprecated!");
}



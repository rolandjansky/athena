/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtTwinPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/MdtTwinPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtTwinPrepDataCnv_p1.h"

void MdtTwinPrepDataCnv_p1::
persToTrans( const Muon::MdtTwinPrepData_p1 *, Muon::MdtTwinPrepData *,MsgStream & log ) 
{
  log<<MSG::WARNING<<"MdtTwinPrepDataCnv_p1::persToTrans is currently disabled. Contact Muon developers"<<endmsg;
  
    // // Fill localposition
    // transObj->m_localPos.setX(persObj->m_driftRadius);
    // transObj->m_localPos.setY(persObj->m_xTwin);
    // // Error Matrix
    // // Trk::CovarianceMatrix *cmat = new Trk::CovarianceMatrix(1);
    // cmat->m[0]                  = persObj->m_errorDriftRadius;
    // cmat->m[3]                  = persObj->m_errorXTwin;         // TWIN TUBES
    // transObj->m_localErrMat     = new Trk::ErrorMatrix(cmat);
    // //MdtPrepData - specific
    // transObj->m_tdc             = persObj->m_tdc;
    // transObj->m_adc             = persObj->m_adc;
    // transObj->m_tdcTwin         = persObj->m_tdcTwin;            // TWIN TUBES
    // transObj->m_adcTwin         = persObj->m_adcTwin;            // TWIN TUBES
    // transObj->m_status          = static_cast<const Muon::MdtDriftCircleStatus>(persObj->m_status);
}

void MdtTwinPrepDataCnv_p1::
transToPers( const Muon::MdtTwinPrepData *, Muon::MdtTwinPrepData_p1 *, MsgStream & log )
{
  log<<MSG::WARNING<<"MdtTwinPrepDataCnv_p1::transToPers is currently disabled. Contact Muon developers"<<endmsg;
    // persObj->m_driftRadius      = transObj->m_localPos.x();
    // persObj->m_errorDriftRadius = transObj->m_localErrMat->covariance().m[0];
    // persObj->m_tdc              = transObj->m_tdc;
    // persObj->m_adc              = transObj->m_adc;
    // persObj->m_status           = transObj->m_status;
    // // TWIN TUBES
    // persObj->m_xTwin            = transObj->m_localPos.y();
    // persObj->m_errorXTwin       = transObj->m_localErrMat->covariance().m[3];
    // persObj->m_tdcTwin          = transObj->m_tdcTwin;
    // persObj->m_adcTwin          = transObj->m_adcTwin;
}



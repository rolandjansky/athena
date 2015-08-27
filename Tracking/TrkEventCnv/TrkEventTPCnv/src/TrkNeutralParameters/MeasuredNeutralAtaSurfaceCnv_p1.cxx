/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MeasuredNeutralAtaSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "TrkNeutralParameters/NeutralParameters.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkNeutralParameters/MeasuredNeutralAtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


template< class SURFACE_CNV, class ATA_SURFACE >
void
MeasuredNeutralAtaSurfaceCnv_p1< SURFACE_CNV, ATA_SURFACE >::
persToTrans( const Trk::MeasuredAtaSurface_p1 *persObj, ATA_SURFACE *transObj, MsgStream &log )
{
   m_surfaceCnv.persToTrans( persObj, transObj, log );
   
   Trk::ErrorMatrix dummy;
   this->fillTransFromPStore( &m_errorMxCnv, persObj->m_errorMatrix, &dummy, log );
   EigenHelpers::vectorToEigenMatrix(dummy.values, *transObj->m_covariance, "MeasuredNeutralAtaSurfaceCnv_p1");
   
}


template< class SURFACE_CNV, class ATA_SURFACE >
void
MeasuredNeutralAtaSurfaceCnv_p1< SURFACE_CNV, ATA_SURFACE >::transToPers( const ATA_SURFACE * /**transObj*/, Trk::MeasuredAtaSurface_p1 * /**persObj*/, MsgStream & /**log*/ )
{
  throw std::runtime_error("MeasuredNeutralAtaSurfaceCnv_p1::transToPers is deprecated!");
}



namespace {
   MeasuredNeutralAtaDiscCnv_p1	inst1;
   MeasuredNeutralAtaCylinderCnv_p1	inst2;
   MeasuredNeutralAtaPlaneCnv_p1	inst3;
   MeasuredNeutralAtaStraightLineCnv_p1 isnt4;
}


/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MeasuredNeutralAtaSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkNeutralParameters/NeutralParameters.h"
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
   AmgSymMatrix(ATA_SURFACE::dim)* cov = transObj->covariance();
   EigenHelpers::vectorToEigenMatrix(dummy.values, *cov, "MeasuredNeutralAtaSurfaceCnv_p1");

}


template< class SURFACE_CNV, class ATA_SURFACE >
void
MeasuredNeutralAtaSurfaceCnv_p1< SURFACE_CNV, ATA_SURFACE >::transToPers( const ATA_SURFACE * /**transObj*/, Trk::MeasuredAtaSurface_p1 * /**persObj*/, MsgStream & /**log*/ )
{
  throw std::runtime_error("MeasuredNeutralAtaSurfaceCnv_p1::transToPers is deprecated!");
}



template class MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaDiscCnv_p1, Trk::NeutralAtaDisc >;
template class MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaCylinderCnv_p1, Trk::NeutralAtaCylinder >;
template class MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaPlaneCnv_p1, Trk::NeutralAtaPlane >;
template class MeasuredNeutralAtaSurfaceCnv_p1< NeutralAtaStraightLineCnv_p1, Trk::NeutralAtaStraightLine >;

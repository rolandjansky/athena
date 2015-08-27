/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MeasuredAtaSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "TrkParameters/TrackParameters.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"
#include <typeinfo>

template< class SURFACE_CNV, class ATA_SURFACE >
void
MeasuredAtaSurfaceCnv_p1< SURFACE_CNV, ATA_SURFACE >::
persToTrans( const Trk::MeasuredAtaSurface_p1 *persObj, ATA_SURFACE *transObj, MsgStream &log )
{
  // std::cout<<" Trying to persToTrans a "<<typeid(*transObj).name()<<std::endl;
   m_surfaceCnv.persToTrans( persObj, transObj, log );
   Trk::ErrorMatrix dummy;
   this->fillTransFromPStore( &m_errorMxCnv, persObj->m_errorMatrix, &dummy, log );
   if (!transObj->m_covariance || transObj->m_covariance->size() != 5) {
     delete transObj->m_covariance;
     transObj->m_covariance = new AmgSymMatrix(5);
   }
   EigenHelpers::vectorToEigenMatrix(dummy.values, *transObj->m_covariance, "MeasuredAtaSurfaceCnv_p1");
}


template< class SURFACE_CNV, class ATA_SURFACE >
void
MeasuredAtaSurfaceCnv_p1< SURFACE_CNV, ATA_SURFACE >::transToPers( const ATA_SURFACE * /**transObj*/, Trk::MeasuredAtaSurface_p1 * /**persObj*/, MsgStream & /**log*/ )
{
  // No more measured Parameters! Deprecated.
  throw std::runtime_error("No more 'measured' track parameters, so MeasuredAtaSurfaceCnv_p1::transToPers is deprecated!");
}



namespace {
   MeasuredAtaDiscCnv_p1	inst1;
   MeasuredAtaCylinderCnv_p1	inst2;
   MeasuredAtaPlaneCnv_p1	inst3;
   MeasuredAtaStraightLineCnv_p1 isnt4;
   MeasuredCurvilinearParametersCnv_p1 isnt5; 
}


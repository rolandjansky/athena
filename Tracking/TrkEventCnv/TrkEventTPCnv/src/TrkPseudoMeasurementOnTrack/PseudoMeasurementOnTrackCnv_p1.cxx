/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p1.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void PseudoMeasurementOnTrackCnv_p1::persToTrans( const Trk :: PseudoMeasurementOnTrack_p1 *persObj,
                                                        Trk :: PseudoMeasurementOnTrack    *transObj, MsgStream &log )
{
  Trk::LocalParameters localParams;
  fillTransFromPStore( &m_localParamsCnv, persObj->m_localParams, &localParams, log );
  // fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
   
  Trk::ErrorMatrix dummy;
  Amg::MatrixX localCovariance;
  fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "PseudoMeasurementOnTrackCnv_p1");
   
  ITPConverterFor<Trk::Surface>* surfaceCnv=nullptr;
  Trk::ConstSurfaceUniquePtr surf
    (this->createTransFromPStore( &surfaceCnv, persObj->m_associatedSurface, log ));

  DetElementSurfaceCnv_p1* detElCnv = dynamic_cast<DetElementSurfaceCnv_p1*>(surfaceCnv); 
  if (detElCnv) {
      // have a DetElementSurface
    surf = Trk::ConstSurfaceUniquePtr (detElCnv->createSurface(log));
  }

  *transObj = Trk::PseudoMeasurementOnTrack (localParams, localCovariance, std::move(surf));
}

void PseudoMeasurementOnTrackCnv_p1::transToPers( const Trk :: PseudoMeasurementOnTrack    * transObj,
                                                        Trk :: PseudoMeasurementOnTrack_p1 * persObj, MsgStream & log)
{
  persObj->m_localParams       = toPersistent( &m_localParamsCnv, &transObj->localParameters(), log );

  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "PseudoMeasurementOnTrackCnv_p1");
  persObj->m_localErrMat = toPersistent( &m_localErrMatCnv, &pMat, log );

  ITPConverterFor<Trk::Surface>* surfaceCnv=nullptr;
  if (transObj->hasSurface())
  {
  // FIXME - this is to handle curvilinear parameters, by forcing them to build a surface. In fact they need a different
  //persistent type and should store their GP and not surface.
    if (transObj->associatedSurface().isFree() ) { // if this is a free surface, write it out 'as is'
      persObj->m_associatedSurface = toPersistent(&surfaceCnv, transObj->associatedSurface().baseSurface(), log);
    } else {
        // else, make it into a DetElementSurface, to allow the correct convertor to be called
        Trk::DetElementSurface dummySurf( transObj->associatedSurface());
        persObj->m_associatedSurface = this->toPersistent(&surfaceCnv, &dummySurf, log );
        // Need to do this->toPersistent() to get around 'error: there are no arguments of 'toPersistent' that depend on a template parameter'
    }
  }
  else {
  log<<MSG::WARNING<<"TrackParameter doesn't have a surface! Dumping: "<<*transObj<<endmsg;
  }
}

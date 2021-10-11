/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p2.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void PseudoMeasurementOnTrackCnv_p2::persToTrans( const Trk :: PseudoMeasurementOnTrack_p2 *persObj,
                                                        Trk :: PseudoMeasurementOnTrack    *transObj, MsgStream &log )
{
  // std::cout<<"PseudoMeasurementOnTrackCnv_p2::persToTrans for transObj="<<transObj<<std::endl;
  Trk::LocalParameters localParams;
  fillTransFromPStore( &m_localParamsCnv, persObj->m_localParams, &localParams, log );
  // fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
   
  Trk::ErrorMatrix dummy;
  Amg::MatrixX localCovariance;
  fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "PseudoMeasurementOnTrackCnv_p2");

  Trk::ConstSurfaceUniquePtr surf
    (this->createTransFromPStore( (ITPConverterFor<Trk::Surface>**)nullptr, persObj->m_associatedSurface, log ));
  if (!surf){
    log<<MSG::WARNING<<"PseudoMeasurementOnTrackCnv_p2: Could not recreate Surface (null pointer)"<<endmsg;
    log<<MSG::VERBOSE<<(*transObj)<<endmsg;    
   } 

  *transObj = Trk::PseudoMeasurementOnTrack (localParams, localCovariance, std::move(surf));
}

void PseudoMeasurementOnTrackCnv_p2::transToPers( const Trk :: PseudoMeasurementOnTrack    * transObj,
                                                        Trk :: PseudoMeasurementOnTrack_p2 * persObj, MsgStream & log)
{
  // std::cout<<"PseudoMeasurementOnTrackCnv_p2::transToPers for transObj="<<transObj<<std::endl;
  persObj->m_localParams       = toPersistent( &m_localParamsCnv, &transObj->localParameters(), log );

  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "PseudoMeasurementOnTrackCnv_p2");
  persObj->m_localErrMat = toPersistent( &m_localErrMatCnv, &pMat, log );

  if (transObj->hasSurface())
  {
      // std::cout<<"PseudoMeasurementOnTrackCnv_p2::transToPers Have base surface ["<<(transObj->associatedSurface().baseSurface())
      //     << "] id= ["<<transObj->associatedSurface().baseSurface()->associatedDetectorElementIdentifier()
      //          <<"] which has isFree()="<<transObj->associatedSurface().baseSurface()->isFree()<<std::endl;
    // std::cout<<*transObj<<std::endl;
    persObj->m_associatedSurface = toPersistent((ITPConverterFor<Trk::Surface>**)nullptr, transObj->associatedSurface().baseSurface(), log);
  }
  else {
    log<<MSG::WARNING<<"TrackParameter doesn't have a surface! Dumping: "<<*transObj<<endmsg;
  }
}

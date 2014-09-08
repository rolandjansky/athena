/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p2.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void PseudoMeasurementOnTrackCnv_p2::persToTrans( const Trk :: PseudoMeasurementOnTrack_p2 *persObj,
                                                        Trk :: PseudoMeasurementOnTrack    *transObj, MsgStream &log )
{
  // std::cout<<"PseudoMeasurementOnTrackCnv_p2::persToTrans for transObj="<<transObj<<std::endl;
  fillTransFromPStore( &m_localParamsCnv, persObj->m_localParams, &transObj->m_localParams, log );
  // fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
   
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "PseudoMeasurementOnTrackCnv_p2");
   
  transObj->m_associatedSurface = this->createTransFromPStore( (ITPConverterFor<Trk::Surface>**)0, persObj->m_associatedSurface, log );
  if (transObj->m_associatedSurface==0){
    log<<MSG::WARNING<<"PseudoMeasurementOnTrackCnv_p2: Could not recreate Surface (null pointer)"<<endreq;
    log<<MSG::VERBOSE<<(*transObj)<<endreq;    
   } 
}

void PseudoMeasurementOnTrackCnv_p2::transToPers( const Trk :: PseudoMeasurementOnTrack    * transObj,
                                                        Trk :: PseudoMeasurementOnTrack_p2 * persObj, MsgStream & log)
{
  // std::cout<<"PseudoMeasurementOnTrackCnv_p2::transToPers for transObj="<<transObj<<std::endl;
  persObj->m_localParams       = toPersistent( &m_localParamsCnv, &transObj->m_localParams, log );

  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "PseudoMeasurementOnTrackCnv_p2");
  persObj->m_localErrMat = toPersistent( &m_localErrMatCnv, &pMat, log );

  if (&(transObj->associatedSurface())!=0){
      // std::cout<<"PseudoMeasurementOnTrackCnv_p2::transToPers Have base surface ["<<(transObj->associatedSurface().baseSurface())
      //     << "] id= ["<<transObj->associatedSurface().baseSurface()->associatedDetectorElementIdentifier()
      //          <<"] which has isFree()="<<transObj->associatedSurface().baseSurface()->isFree()<<std::endl;
    // std::cout<<*transObj<<std::endl;
    persObj->m_associatedSurface = toPersistent((ITPConverterFor<Trk::Surface>**)0, transObj->m_associatedSurface->baseSurface(), log);
  } else {
    log<<MSG::WARNING<<"TrackParameter doesn't have a surface! Dumping: "<<*transObj<<endreq;
  }
}

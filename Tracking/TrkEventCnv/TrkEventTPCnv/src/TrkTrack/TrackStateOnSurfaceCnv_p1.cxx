/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p1.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"


void TrackStateOnSurfaceCnv_p1::
persToTrans( const Trk::TrackStateOnSurface_p1 *persObj, Trk::TrackStateOnSurface *transObj, MsgStream &log )
{
  ITPConverterFor<Trk::MeasurementBase>	*measureCnv = nullptr;
  const Trk::MeasurementBase* meas =  createTransFromPStore( &measureCnv, persObj->m_measurementOnTrack, log );

  ITPConverterFor<Trk::TrackParameters>	*paramsCnv = nullptr;
  const Trk::TrackParameters* trackParameters = dynamic_cast<const Trk::TrackParameters*>(createTransFromPStore( &paramsCnv, persObj->m_trackParameters, log ));

  const Trk::FitQualityOnSurface* fitQoS = createTransFromPStore( &m_fitQCnv, persObj->m_fitQualityOnSurface, log );

  const Trk::MaterialEffectsBase* materialEffects = nullptr;
  if (! persObj->m_scatteringAngle.isNull() ) {
    materialEffects =
      createTransFromPStore( &m_scatCnv, persObj->m_scatteringAngle, log );
  }

   if (!persObj->m_materialEffectsOnTrack.isNull()) {
     materialEffects =
       createTransFromPStore( &m_bremCnv, persObj->m_materialEffectsOnTrack,log);
   }

#if 0
   // Taking the address of a reference cannot yield null for a valid c++ program.
   if (trackParameters!=0 && &(trackParameters->associatedSurface())==0) {
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
      if ( rot!=0 ) {
          
          log<<MSG::WARNING<<"TrackStateOnSurfaceCnv_p1: Not sure how to set the surface yet, so this Parameter is useless for most purposes."<<endmsg;
          
      } else {
          log<<MSG::WARNING<<"TrackStateOnSurfaceCnv_p1: TrackParam has no surface and no ROT available to help!"<<endmsg;
      }
   }
#endif

  *transObj = Trk::TrackStateOnSurface (std::unique_ptr<const Trk::MeasurementBase>(meas),
                                        std::unique_ptr<const Trk::TrackParameters>(trackParameters),
                                        std::unique_ptr<const Trk::FitQualityOnSurface>(fitQoS),
                                        std::unique_ptr<const Trk::MaterialEffectsBase>(materialEffects),
                                        persObj->m_typeFlags);
}


void TrackStateOnSurfaceCnv_p1::
transToPers( const Trk::TrackStateOnSurface *, Trk::TrackStateOnSurface_p1 *, MsgStream & ) 
{
  throw std::runtime_error("TrackStateOnSurfaceCnv_p1::transToPers is deprecated!");   
}


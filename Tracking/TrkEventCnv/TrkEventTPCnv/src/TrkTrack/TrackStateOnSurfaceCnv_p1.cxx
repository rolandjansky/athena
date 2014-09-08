/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkTrack/TrackStateOnSurface.h"
#undef private

#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p1.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"


void TrackStateOnSurfaceCnv_p1::
persToTrans( const Trk::TrackStateOnSurface_p1 *persObj, Trk::TrackStateOnSurface *transObj, MsgStream &log )
{
   ITPConverterFor<Trk::TrackParameters>	*paramsCnv = 0;
   transObj->m_trackParameters = dynamic_cast<const Trk::TrackParameters*>(createTransFromPStore( &paramsCnv, persObj->m_trackParameters, log ));

   transObj->m_fitQualityOnSurface = createTransFromPStore( &m_fitQCnv, persObj->m_fitQualityOnSurface, log );

   // here is schema evolution: from old SAOT/MEOT to new classes
   if (log.level() <= MSG::VERBOSE)
     log << MSG::VERBOSE << " ->>->  persTSoS->m_scat: "
       << (persObj->m_scatteringAngle.isNull()? "NO":"YES") << endreq;
   if (! persObj->m_scatteringAngle.isNull() ) {
     transObj->m_materialEffectsOnTrack =
       createTransFromPStore( &m_scatCnv, persObj->m_scatteringAngle, log );
   }

   if (log.level() <= MSG::VERBOSE)
     log << MSG::VERBOSE << " ->>->  persTSoS->m_meot: "
         << (persObj->m_materialEffectsOnTrack.isNull()? "NO":"YES") << endreq;
   if (!persObj->m_materialEffectsOnTrack.isNull()) {
     transObj->m_materialEffectsOnTrack =
       createTransFromPStore( &m_bremCnv, persObj->m_materialEffectsOnTrack,log);
   }

   ITPConverterFor<Trk::MeasurementBase>	*measureCnv = 0;
//   log << MSG::INFO << " ->>->  persTSoS->m_measurementOnTrack=(" << persObj->m_measurementOnTrack.isNull() << "," << persObj->m_measurementOnTrack.index() << ")" << endreq;
   transObj->m_measurementOnTrack =  createTransFromPStore( &measureCnv, persObj->m_measurementOnTrack, log );
//   log << MSG::INFO << " ->>->  transTSoS->m_measurementOnTrack=" << (void*)(transObj->m_measurementOnTrack) << endreq;
   transObj->m_typeFlags = persObj->m_typeFlags;

   if (transObj->m_trackParameters!=0 && &(transObj->m_trackParameters->associatedSurface())==0) {
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(transObj->m_measurementOnTrack);
      if ( rot!=0 ) {
          
          // const Trk::Surface* surface = m_eventCnvTool->getSurface(rot->identify());
          // Trk::TrackParameters * nonConst = const_cast<Trk::TrackParameters *>(transObj->m_trackParameters);
          // nonConst->ParametersT::m_surface = surface; FIXME!!
          log<<MSG::WARNING<<"TrackStateOnSurfaceCnv_p1: Not sure how to set the surface yet, so this Parameter is useless for most purposes."<<endreq;
          
      } else {
          log<<MSG::WARNING<<"TrackStateOnSurfaceCnv_p1: TrackParam has no surface and no ROT available to help!"<<endreq;
      }
   }   
}


void TrackStateOnSurfaceCnv_p1::
transToPers( const Trk::TrackStateOnSurface *, Trk::TrackStateOnSurface_p1 *, MsgStream & ) 
{
  throw std::runtime_error("TrackStateOnSurfaceCnv_p1::transToPers is deprecated!");   
}


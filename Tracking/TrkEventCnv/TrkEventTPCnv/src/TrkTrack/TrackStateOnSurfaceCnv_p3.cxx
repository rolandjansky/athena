/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p3.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"

void TrackStateOnSurfaceCnv_p3::
persToTrans( const Trk::TrackStateOnSurface_p3 *persObj, Trk::TrackStateOnSurface *transObj, MsgStream &log )
{
  ITPConverterFor<Trk::MeasurementBase>	*measureCnv = 0;
  const Trk::MeasurementBase* meas = createTransFromPStore( &measureCnv, persObj->m_measurementOnTrack, log );

  ITPConverter* dummy = topConverter ()->converterForType( typeid(Trk::TrackParameters));    
  if (!m_parametersCnv)  m_parametersCnv = dynamic_cast<TrackParametersCnv_p2*>(dummy); // FIXME - only in init?
  const Trk::TrackParameters* trackParameters = dynamic_cast<const Trk::TrackParameters*>(createTransFromPStore( &m_parametersCnv, persObj->m_trackParameters, log ));

  const Trk::FitQualityOnSurface* fitQoS = createTransFromPStore( &m_fitQCnv, persObj->m_fitQualityOnSurface, log );

  ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = 0;  
  const Trk::MaterialEffectsBase* materialEffects = createTransFromPStore( &matBaseCnv, persObj->m_materialEffects, log );

  // There were some tracks saved that violate the isSane test in
  // TrackStateOnSurface.  If we were to pass materialEffects to this ctor
  // then we would trip that assertion.  However, we want to preserve
  // the previous behavior of the tp converters, which did allow reading
  // such tracks.  So defer setting the material pointer until
  // after the checks,
  *transObj = Trk::TrackStateOnSurface (meas,
                                        trackParameters,
                                        fitQoS,
                                        nullptr,
                                        persObj->m_typeFlags);
  transObj->m_materialEffectsOnTrack = materialEffects;
}


void TrackStateOnSurfaceCnv_p3::
transToPers( const Trk::TrackStateOnSurface *transObj, Trk::TrackStateOnSurface_p3 *persObj, MsgStream &log ) 
{
  //--- Parameters
  ITPConverter* dummy = topConverter ()->converterForType( typeid(Trk::TrackParameters));    
  if (!m_parametersCnv)  m_parametersCnv = dynamic_cast<TrackParametersCnv_p2*>(dummy); // FIXME - only in init?
  persObj->m_trackParameters = toPersistent( &m_parametersCnv, transObj->trackParameters(), log );

  persObj->m_fitQualityOnSurface = toPersistent( &m_fitQCnv, transObj->fitQualityOnSurface(), log );

  ITPConverterFor<Trk::MeasurementBase>  *measureCnv = 0;
  persObj->m_measurementOnTrack = toPersistent( &measureCnv, transObj->measurementOnTrack(), log );

  ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = 0;  
  persObj->m_materialEffects = toPersistent( &matBaseCnv,
                                             transObj->materialEffectsOnTrack(), log );
  persObj->m_typeFlags = transObj->types().to_ulong();
}

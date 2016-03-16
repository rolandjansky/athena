/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p2.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"

//#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"


void TrackStateOnSurfaceCnv_p2::
persToTrans( const Trk::TrackStateOnSurface_p2 *persObj, Trk::TrackStateOnSurface *transObj, MsgStream &log )
{
  ITPConverterFor<Trk::MeasurementBase>	*measureCnv = 0;
  const Trk::MeasurementBase* meas =  createTransFromPStore( &measureCnv, persObj->m_measurementOnTrack, log );

  ITPConverterFor<Trk::TrackParameters>	*paramsCnv = 0;
  const Trk::TrackParameters* trackParameters = dynamic_cast<const Trk::TrackParameters*>(createTransFromPStore( &paramsCnv, persObj->m_trackParameters, log ));

  const Trk::FitQualityOnSurface* fitQoS = createTransFromPStore( &m_fitQCnv, persObj->m_fitQualityOnSurface, log );

  ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = 0;
  const Trk::MaterialEffectsBase* materialEffects = createTransFromPStore( &matBaseCnv, persObj->m_materialEffects, log );

  *transObj = Trk::TrackStateOnSurface (meas,
                                        trackParameters,
                                        fitQoS,
                                        materialEffects,
                                        persObj->m_typeFlags);
}


void TrackStateOnSurfaceCnv_p2::
transToPers( const Trk::TrackStateOnSurface *, Trk::TrackStateOnSurface_p2 *, MsgStream & ) 
{
  throw std::runtime_error("TrackStateOnSurfaceCnv_p2::transToPers is deprecated!");   
}

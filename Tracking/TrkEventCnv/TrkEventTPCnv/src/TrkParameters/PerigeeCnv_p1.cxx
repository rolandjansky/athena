/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PerigeeCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/PerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/mungeZeroQOverP.h"

void PerigeeCnv_p1::persToTrans(    const Trk::Perigee_p1 *persObj,
  Trk::Perigee *transObj,
  MsgStream &log ) 
{
  fillTransFromPStore( &m_trackParametersCnv, persObj->m_parameters, transObj, log );

  // Preserve previous behavior of tp converters.
  bool waszero = false;
  auto parms = transObj->parameters();
  if (parms[Trk::qOverP] == 0) {
    parms[Trk::qOverP] = 1;
    waszero = true;
  }

  Trk::SurfaceUniquePtrT<const Trk::PerigeeSurface> surf
    (createTransFromPStore( &m_perigeeSurfaceCnv, persObj->m_assocSurface, log ));
  *transObj = Trk::Perigee (parms, std::move(surf));
  if (waszero)
    TrkEventTPCnv::mungeZeroQOverP (*transObj);
}

void PerigeeCnv_p1::transToPers( const Trk::Perigee * /**transObj*/,
  Trk::Perigee_p1 * /**persObj*/,
  MsgStream & /**log*/ ) 
{
  throw std::runtime_error("PerigeeCnv_p1::transToPers is deprecated!");
}



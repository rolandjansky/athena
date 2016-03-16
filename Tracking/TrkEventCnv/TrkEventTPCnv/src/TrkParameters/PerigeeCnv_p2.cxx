/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/PerigeeCnv_p2.h"
#include "TrkEventTPCnv/TrkParameters/mungeZeroQOverP.h"

void PerigeeCnv_p2::persToTrans( const Trk::Perigee_p2 *persObj, Trk::Perigee *transObj, MsgStream &log ) {

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


void PerigeeCnv_p2::transToPers( const Trk::Perigee *, Trk::Perigee_p2 *, MsgStream & ) {
  throw std::runtime_error("PerigeeCnv_p2::transToPers is deprecated! (Use TrackParametersCnv_pX instead)");
}



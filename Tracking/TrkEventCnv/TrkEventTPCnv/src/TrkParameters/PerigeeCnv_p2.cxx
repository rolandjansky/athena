/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrkParameters/TrackParameters.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkParameters/PerigeeCnv_p2.h"

void PerigeeCnv_p2::persToTrans( const Trk::Perigee_p2 *persObj, Trk::Perigee *transObj, MsgStream &log ) {

  fillTransFromPStore( &m_trackParametersCnv, persObj->m_parameters, transObj, log );

  transObj->m_surface = createTransFromPStore( &m_perigeeSurfaceCnv, persObj->m_assocSurface, log );

  double qop=transObj->parameters()[Trk::qOverP];
  if ( qop< 0.) transObj->m_chargeDef.setCharge(-1);
    // fill momentum & then position using the surface
  double p = qop != 0. ? transObj->charge()/qop : 1.;
  double phi = transObj->parameters()[Trk::phi];
  double theta = transObj->parameters()[Trk::theta];

  transObj->m_momentum = Amg::Vector3D(p*cos(phi)*sin(theta),
    p*sin(phi)*sin(theta),
    p*cos(theta));
  transObj->m_surface->localToGlobal(transObj->localPosition(),
    transObj->m_momentum,
    transObj->m_position);
}


void PerigeeCnv_p2::transToPers( const Trk::Perigee *, Trk::Perigee_p2 *, MsgStream & ) {
  throw std::runtime_error("PerigeeCnv_p2::transToPers is deprecated! (Use TrackParametersCnv_pX instead)");
}



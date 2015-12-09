/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   NeutralPerigeeCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "TrkNeutralParameters/NeutralParameters.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkNeutralParameters/NeutralPerigeeCnv_p1.h"

void NeutralPerigeeCnv_p1::persToTrans(    const Trk::Perigee_p1 *persObj,
                                    Trk::NeutralPerigee *transObj,
                                    MsgStream &log ) 
{

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
 //   transObj->m_localPosition = createTransFromPStore( &m_localPosCnv, persObj->m_localPos, log );
}

void NeutralPerigeeCnv_p1::transToPers(    const Trk::NeutralPerigee * /**transObj*/,
                                    Trk::Perigee_p1 * /**persObj*/,
                                    MsgStream & /**log*/ ) 
{
  throw std::runtime_error("NeutralPerigeeCnv_p1::transToPers is deprecated!");

}



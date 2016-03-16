/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   NeutralPerigeeCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkEventTPCnv/TrkNeutralParameters/NeutralPerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/mungeZeroQOverP.h"

void NeutralPerigeeCnv_p1::persToTrans(    const Trk::Perigee_p1 *persObj,
                                    Trk::NeutralPerigee *transObj,
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
  *transObj = Trk::NeutralPerigee (parms, std::move(surf));
   if (waszero)
     TrkEventTPCnv::mungeZeroQOverP (*transObj);
}

void NeutralPerigeeCnv_p1::transToPers(    const Trk::NeutralPerigee * /**transObj*/,
                                    Trk::Perigee_p1 * /**persObj*/,
                                    MsgStream & /**log*/ ) 
{
  throw std::runtime_error("NeutralPerigeeCnv_p1::transToPers is deprecated!");

}



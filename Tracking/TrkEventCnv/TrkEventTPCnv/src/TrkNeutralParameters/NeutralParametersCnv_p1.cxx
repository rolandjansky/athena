/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   NeutralParametersCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkEventTPCnv/TrkNeutralParameters/NeutralParametersCnv_p1.h"

void NeutralParametersCnv_p1::persToTrans( const Trk :: TrackParameters_p1 *persObj,
					 Trk :: NeutralParameters    *transObj,
					 MsgStream& )
{
  unsigned int size=persObj->m_parameters.size();
  assert (transObj->dim==size);
  AmgVector(Trk::TrackParameters::dim) pars;
  for (unsigned int i=0; i<size; ++i) pars(i)=persObj->m_parameters[i];
  transObj->updateParameters(pars);
}


void NeutralParametersCnv_p1::transToPers( const Trk :: NeutralParameters    * /**transObj*/,
					 Trk :: TrackParameters_p1 * /**persObj*/,
					 MsgStream& )
{
  throw std::runtime_error("NeutralParametersCnv_p1::transToPers is deprecated!");

}










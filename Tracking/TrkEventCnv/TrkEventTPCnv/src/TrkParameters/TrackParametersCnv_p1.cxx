/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParametersCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "TrkParameters/TrackParameters.h"
#include "TrkParametersBase/ParametersBase.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"

void TrackParametersCnv_p1::persToTrans( const Trk :: TrackParameters_p1 *persObj, Trk :: TrackParameters    *transObj, MsgStream& ) {
    // transObj->m_parameters.m.insert(transObj->m_parameters.m.begin(),persObj->m_parameters.begin(),persObj->m_parameters.end());
  unsigned int size=persObj->m_parameters.size();
  assert (transObj->parameters().rows()==size);
  for (unsigned int i=0; i<size; ++i) const_cast<AmgVector(5)&>(transObj->parameters())[i]=persObj->m_parameters[i];
  const_cast<Amg::Vector3D&>(transObj->position()).setZero();
  const_cast<Amg::Vector3D&>(transObj->momentum()).setZero();
}

void TrackParametersCnv_p1::transToPers( const Trk :: TrackParameters    * /**transObj*/, Trk :: TrackParameters_p1 * /**persObj*/, MsgStream& ) {
  throw std::runtime_error("TrackParametersCnv_p1::transToPers is deprecated!");
}


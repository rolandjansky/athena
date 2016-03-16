/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MVFVxTrackAtVertexCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxTrackAtVertexCnv_p1.h"

void  MVFVxTrackAtVertexCnv_p1::persToTrans(const Trk::MVFVxTrackAtVertex_p1 * persObj, Trk::MVFVxTrackAtVertex * transObj, MsgStream &log)
{
 fillTransFromPStore( &m_vxTrackAtVertexConverter, persObj->m_vxTrackAtVertex, transObj, log);
}

void  MVFVxTrackAtVertexCnv_p1::transToPers(const Trk::MVFVxTrackAtVertex * transObj, Trk::MVFVxTrackAtVertex_p1 * persObj, MsgStream &log)
{
 persObj->m_vxTrackAtVertex = baseToPersistent(&m_vxTrackAtVertexConverter,  transObj, log );
}

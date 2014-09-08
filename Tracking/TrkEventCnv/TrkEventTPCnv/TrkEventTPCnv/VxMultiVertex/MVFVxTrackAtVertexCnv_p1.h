/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXTRACKATVERTEX_CNV_P1_H
#define MVFVXTRACKATVERTEX_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxMultiVertex/MVFVxTrackAtVertex.h"
// #include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxTrackAtVertex_p1.h"
#include "VxVertex/VxTrackAtVertex.h"

/**
 * Converter for Transient/Persistent separation
 * of the VxMultiVertex/MVFVxTrackAtVertexVertex class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * February 2007 modified for the new TP split schema
 */


class MsgStream;
class MVFVxTrackAtVertexCnv_p1 : public T_AthenaPoolTPPolyCnvBase< Trk::VxTrackAtVertex, Trk::MVFVxTrackAtVertex, Trk::MVFVxTrackAtVertex_p1>

{
 public:
  
   MVFVxTrackAtVertexCnv_p1() : m_vxTrackAtVertexConverter(0) {}
 
/**  
 * Conversion methods 
 */  
   void  persToTrans(const Trk::MVFVxTrackAtVertex_p1 * persObj, Trk::MVFVxTrackAtVertex * transObj, MsgStream &log);
  
   void  transToPers(const Trk::MVFVxTrackAtVertex * transObj, Trk::MVFVxTrackAtVertex_p1 * persObj, MsgStream &log);
 
 private:
   
   ITPConverterFor<Trk::VxTrackAtVertex>      *m_vxTrackAtVertexConverter;
};
  
#endif //MVFVXTRACKATVERTEX_CNV_P1_H

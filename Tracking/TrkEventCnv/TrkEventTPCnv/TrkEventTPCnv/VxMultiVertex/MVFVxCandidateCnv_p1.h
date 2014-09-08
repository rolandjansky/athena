/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXCANDIDATE_CNV_P1_H
#define MVFVXCANDIDATE_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxMultiVertex/MVFVxCandidate.h"
// #include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxCandidate_p1.h"
#include "VxVertex/VxCandidate.h"

/**
 * Converter for Transient/Persistent separation
 * of the VxMultiVertex/MVFVxTrackAtVertexVertex class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * February 2007 modified for the new TP split schema
 */


class MsgStream;
class MVFVxCandidateCnv_p1 : public T_AthenaPoolTPPolyCnvBase< Trk::VxCandidate, Trk::MVFVxCandidate, Trk::MVFVxCandidate_p1>

{
  public:
  
   MVFVxCandidateCnv_p1(): m_vxCandidateConverter(0) {}
   
/**  
 * Conversion methods 
 */  
   void  persToTrans(const Trk::MVFVxCandidate_p1 * persObj, Trk::MVFVxCandidate * transObj, MsgStream &log);
  
   void  transToPers(const Trk::MVFVxCandidate * transObj, Trk::MVFVxCandidate_p1 * persObj, MsgStream &log);
 
  private:
   
   ITPConverterFor<Trk::VxCandidate>      *m_vxCandidateConverter;
   
};//end of class definitions

#endif // MVFVXCANDIDATE_CNV_P1_H

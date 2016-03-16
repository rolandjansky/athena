/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXCANDIDATE_CNV_P1_H
#define VXCANDIDATE_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/VxCandidate.h"
#include "TrkEventTPCnv/VxVertex/VxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertexCnv_p1.h"

/**
 * Converter for Transient/Persistent separation
 * of the VxVertex/RecVertex class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * January 2007 modified for the new TP split schema
 */
 class MsgStream;
 
class VxCandidateCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::VxCandidate, Trk::VxCandidate_p1>
 {                            
  public:
   VxCandidateCnv_p1():m_recVertexConverter(0) {}
  
/**  
 * Conversion methods 
 */
    void  persToTrans(const Trk::VxCandidate_p1 * persObj, Trk::VxCandidate * transObj, MsgStream &log);
  
    void  transToPers(const Trk::VxCandidate * transObj, Trk::VxCandidate_p1 * persObj, MsgStream &log); 
    
    virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) 
    {
       m_vxTrkAtVrtCnv.setTopConverter( topCnv, 0 );
    }
    
  private:
   
   RecVertexCnv_p1 * m_recVertexConverter;
   //VxTrackAtVertexCnv_p1 * m_vxTrackAtVertexConverter;
    typedef T_AthenaPoolTPPolyVectorCnv< std::vector<Trk::VxTrackAtVertex*>,
                                     std::vector<TPObjRef>,
                                     ITPConverterFor<Trk::VxTrackAtVertex> >   VxTrkAtVrtVectorCnv_p1;
  

   VxTrkAtVrtVectorCnv_p1 m_vxTrkAtVrtCnv;

 };//end of class definitions
#endif //VXCANDIDATE_CNV_P1_H

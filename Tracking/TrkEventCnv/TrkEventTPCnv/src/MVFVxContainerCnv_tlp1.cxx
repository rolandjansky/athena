/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/MVFVxContainerCnv_tlp1.h"
#include "CxxUtils/checker_macros.h"

MVFVxContainerCnv_tlp1::MVFVxContainerCnv_tlp1()
{
// Add all converters defined in this top level converter:
// never change the order of adding converters!
  addMainTPConverter(); 		    
  					    
  addTPConverter( &m_mVFVxCandidateCnv);    
  addTPConverter( &m_mVFVxTrackAtVertexCnv) ;
}


void MVFVxContainerCnv_tlp1::setPStorage( Trk::MVFVxContainer_tlp1 *storage )
{
 setMainCnvPStorage( &storage->m_mVFVxContainers );
 
 m_mVFVxCandidateCnv.        setPStorage(&storage->m_mVFVxCandidates);
 m_mVFVxTrackAtVertexCnv.    setPStorage(&storage->m_mVFVxTrackAtVertices);

}


void T_TPCnv<MVFVxContainer, Trk::MVFVxContainer_tlp1 >::
persToTrans (const Trk::MVFVxContainer_tlp1* pers,
             MVFVxContainer* trans,
             MsgStream& msg)
{
  // FIXME: TPConverter uses the same non-const member m_pStorage
  // for both reading and writing, but we want it to be const
  // in the former case.
  Trk::MVFVxContainer_tlp1* pers_nc ATLAS_THREAD_SAFE =
    const_cast<Trk::MVFVxContainer_tlp1*> (pers);
  setPStorage (pers_nc);
  m_mainConverter.pstoreToTrans (0, trans, msg);
}

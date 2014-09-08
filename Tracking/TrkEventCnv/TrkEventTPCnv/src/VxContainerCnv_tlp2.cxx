/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/VxContainerCnv_tlp2.h"

VxContainerCnv_tlp2::VxContainerCnv_tlp2()
{

// Add all converters defined in this top level converter:
// never change the order of adding converters!
   addMainTPConverter();
   
   addTPConverter( &m_vxCandidateCnv);
   addTPConverter( &m_extendedVxCandidateCnv);
   // addTPConverterForReading( &m_vxTrackAtVertexCnv);
   addTPConverter( &m_vxTrackAtVertexCnv);
   addTPConverter( &m_recVertexCnv);
   // addTPConverter( &m_VertexCnv);

   addTPConverter( &m_trackCnv);

   addTPConverter( &m_trackParametersCnv);
   addTPConverter( &m_perigeeCnv);
   addTPConverterForReading( &m_measuredPerigeeCnv);
   
   addTPConverter( &m_perigeeSurfacesCnv );
   // addTPConverter( &m_fitQualityCnv);
   addTPConverter( &m_errorMatrixCnv);
   // addTPConverter( &m_localPositionCnv);   
   
   // addTPConverter( &m_vxTrackAtVertexCnv_p2);   
}

void VxContainerCnv_tlp2::setPStorage( Trk::VxContainer_tlp2 *storage )
{ 
// for implicit TrackCollection_p1 from the base class: 
 setMainCnvPStorage( &storage->m_vxContainers );
 
// for all converters defined in this top level converter
 m_vxCandidateCnv.          setPStorage(&storage->m_vxCandidates);
 m_extendedVxCandidateCnv.  setPStorage(&storage->m_extendedVxCandidates);
 m_vxTrackAtVertexCnv.      setPStorage(&storage->m_vxTrackAtVertices);
 m_recVertexCnv.            setPStorage(&storage->m_recVertices);
 // m_VertexCnv.               setPStorage(&storage->m_vertices);
 m_trackCnv.                setPStorage(&storage->m_tracks);

 m_trackParametersCnv.      setPStorage(&storage->m_trackParameters);
 m_perigeeCnv.              setPStorage(&storage->m_perigees); 
 m_measuredPerigeeCnv.      setPStorage(&storage->m_measPerigees);
 m_perigeeSurfacesCnv.	    setPStorage( &storage->m_surfaces ); 
 // m_fitQualityCnv.           setPStorage(&storage->m_fitQualities);
 m_errorMatrixCnv.          setPStorage(&storage->m_hepSymMatrices);
 // m_localPositionCnv.        setPStorage(&storage->m_localPositions);
 
 // m_vxTrackAtVertexCnv_p2.   setPStorage(&storage->m_vxTrackAtVertices_p2);
}

T_TPCnv<VxContainer, Trk::VxContainer_tlp2 >::T_TPCnv(){
  TPObjRef::typeID_t tid (m_MVFVxContainerCnv_tlp1.getConverterID(), 0);
  m_MVFVxContainerCnv_tlp1.setTopConverter (this, tid);
  addExtTPConverterForReading (&m_MVFVxContainerCnv_tlp1);
  m_MVFVxContainerCnv_tlp1.addTPConvertersTo (this);
}


void T_TPCnv<VxContainer, Trk::VxContainer_tlp2 >::persToTrans (const Trk::VxContainer_tlp2* pers, VxContainer* trans, MsgStream& msg){
  setPStorage (const_cast<Trk::VxContainer_tlp2*> (pers));
  m_mainConverter.pstoreToTrans (0, trans, msg);
}


void T_TPCnv<VxContainer, Trk::VxContainer_tlp2 >::transToPers (const VxContainer* trans, Trk::VxContainer_tlp2* pers, MsgStream& msg)
{
  this->setTLPersObject( pers );
  m_mainConverter.virt_toPersistent(trans, msg);
  this->clearTLPersObject();
}


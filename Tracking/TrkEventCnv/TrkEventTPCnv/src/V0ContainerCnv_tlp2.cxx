/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/V0ContainerCnv_tlp2.h"
#include "CxxUtils/checker_macros.h"


V0ContainerCnv_tlp2::V0ContainerCnv_tlp2()
{
// Add all converters defined in this top level converter:
// never change the order of adding converters!
 addMainTPConverter(); 		    
  					    
 addTPConverter( &m_v0CandidateCnv);    
 addTPConverter( &m_v0HypothesisCnv);
 
//VxVertex 
 addTPConverter( &m_extendedVxCandidateCnv);
 addTPConverter( &m_vxCandidateCnv);
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
}

void V0ContainerCnv_tlp2::setPStorage( Trk::V0Container_tlp2 *storage )
{
 setMainCnvPStorage( &storage->m_v0Containers );
 m_v0CandidateCnv.        setPStorage(&storage->m_v0Candidates);
 m_v0HypothesisCnv.       setPStorage(&storage->m_v0Hypothesises);
 
//VxVertex 
 m_extendedVxCandidateCnv.       setPStorage(&storage->m_extendedVxCandidates);
 m_vxCandidateCnv.       setPStorage(&storage->m_vxCandidates);
 m_vxTrackAtVertexCnv.    setPStorage(&storage->m_vxTracksAtVertex);
 m_recVertexCnv.            setPStorage(&storage->m_recVertices);
 // m_VertexCnv.               setPStorage(&storage->m_vertices);

 m_trackParametersCnv.      setPStorage(&storage->m_trackParameters);
 m_perigeeCnv.              setPStorage(&storage->m_perigees); 
 m_measuredPerigeeCnv.      setPStorage(&storage->m_measPerigees);
 m_perigeeSurfacesCnv.	    setPStorage( &storage->m_surfaces ); 
 // m_fitQualityCnv.           setPStorage(&storage->m_fitQualities);
 m_errorMatrixCnv.          setPStorage(&storage->m_hepSymMatrices);
 
}

void T_TPCnv<V0Container, Trk::V0Container_tlp2 >::
persToTrans (const Trk::V0Container_tlp2* pers,
             V0Container* trans,
             MsgStream& msg)
{
  // FIXME: TPConverter uses the same non-const member m_pStorage
  // for both reading and writing, but we want it to be const
  // in the former case.
  Trk::V0Container_tlp2* pers_nc ATLAS_THREAD_SAFE =
    const_cast<Trk::V0Container_tlp2*> (pers);
  setPStorage (pers_nc);
  m_mainConverter.pstoreToTrans (0, trans, msg);
}

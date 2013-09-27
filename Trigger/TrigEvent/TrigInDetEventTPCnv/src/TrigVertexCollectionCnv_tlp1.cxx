/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_tlp1.h"
  
TrigVertexCollectionCnv_tlp1::TrigVertexCollectionCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigVertexCollectionCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_VertexCnv); 
  addTPConverter( &m_TrigIDtrackCnv); 
  addTPConverter( &m_TrigIDtrackFitParCnv);    
}

void TrigVertexCollectionCnv_tlp1::setPStorage( TrigVertexCollection_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigVertexCollection );
  
  m_VertexCnv.              setPStorage(&storage->m_Vertex);
  m_TrigIDtrackCnv.         setPStorage(&storage->m_Track);
  m_TrigIDtrackFitParCnv.   setPStorage(&storage->m_TrackFitPar);
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTopLevelCnv/SegmentCollectionCnv_tlp4.h"

SegmentCollectionCnv_tlp4::SegmentCollectionCnv_tlp4( )
{
//    std::cout << "SegmentCollectionCnv_tlp4::SegmentCollectionCnv_tlp4( )" << std::endl;
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!

  addMainTPConverter();

  addTPConverter( &m_segmentCnv );

   // Surfaces
  addTPConverter( &m_coneSurfacesCnv       );
  addTPConverter( &m_cylSurfacesCnv        );
  addTPConverter( &m_discSurfacesCnv       );
  addTPConverter( &m_perigeeSurfacesCnv    );
  addTPConverter( &m_planeSurfacesCnv      );
  addTPConverter( &m_lineSurfacesCnv       );
  addTPConverter( &m_saggedLineSurfacesCnv );

  addTPConverter( &m_fitQualitiesCnv );
  addTPConverter( &m_errorMatricesCnv );   
  addTPConverter( &m_covarianceMatricesCnv );     
  addTPConverter( &m_localParametersCnv );            

  addTPConverter( &m_pseudoMeasurementOnTrackCnv );
  addTPConverter( &m_crotCnv );

  // adding extending TLP converters,
  // they don't interfere with the order of adding regular converters (above)
  m_muonMeasurementsCnv.addTPConvertersTo( this );
  m_inDetTrackCnv.addTPConvertersTo( this );
}


void SegmentCollectionCnv_tlp4::setPStorage( PERS *storage )
{
  setMainCnvPStorage( &storage->m_segmentCollections );

     // for all converters defined in this top level converter
  m_segmentCnv.                  setPStorage( &storage->m_segments );                

  m_coneSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_cylSurfacesCnv           .setPStorage( &storage->m_surfaces );
  m_discSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_perigeeSurfacesCnv       .setPStorage( &storage->m_surfaces );
  m_planeSurfacesCnv         .setPStorage( &storage->m_surfaces );
  m_lineSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_saggedLineSurfacesCnv    .setPStorage( &storage->m_surfaces );

  m_fitQualitiesCnv.             setPStorage( &storage->m_fitQualities );          
  m_covarianceMatricesCnv.       setPStorage( &storage->m_hepSymMatrices );        
  m_errorMatricesCnv.            setPStorage( &storage->m_hepSymMatrices );        
  m_localParametersCnv.          setPStorage( &storage->m_localParameters );       

  m_pseudoMeasurementOnTrackCnv. setPStorage( &storage->m_pseudoMeasurementOnTrack );  
  m_crotCnv.                     setPStorage( &storage->m_competingRotsOnTrack );          

  // when reading (createTrans) these will be non-null if extension is present
  // when writing, they will be null at first end set afterwards in createPersisten()
  if( storage->m_muonMeasurementsExt ) m_muonMeasurementsCnv.setPStorage( storage->m_muonMeasurementsExt );
  if( storage->m_inDetTrackExt )       m_inDetTrackCnv      .setPStorage( storage->m_inDetTrackExt );
}   


BaseSegmentCollectionCnv::PERS* SegmentCollectionCnv_tlp4::createPersistent(const TRANS* transObj, MsgStream &log)
 {
    PERS *pers = BaseSegmentCollectionCnv::createPersistent(transObj, log);
    pers->m_muonMeasurementsExt = m_muonMeasurementsCnv.releaseTLPersObject();
    pers->m_inDetTrackExt = m_inDetTrackCnv.releaseTLPersObject();
    return pers;
 }


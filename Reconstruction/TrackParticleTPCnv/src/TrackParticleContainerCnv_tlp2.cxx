/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp2.h"
#include "CxxUtils/checker_macros.h"

TrackParticleContainerCnv_tlp2::TrackParticleContainerCnv_tlp2()
{
    // Add all converters defined in this top level converter.
    // HEY YOU --- YES YOU --- READ THIS!!!!!!!
    // NEVER change the order of the addTPConverter calls!
    // NEVER delete an addTPConverter call!
    // NEVER add an addTPConverter call in the middle --- only at the end!

    addMainTPConverter();

    addTPConverter( &m_trackParametersCnv);
    addTPConverter( &m_perigeeCnv);
    addTPConverter( &m_ataDiscsCnv);
    addTPConverter( &m_ataCylindersCnv);
    addTPConverter( &m_ataPlanesCnv);
    addTPConverter( &m_ataStraightLinesCnv);
    addTPConverterForReading( &m_measuredPerigeeCnv);
    addTPConverterForReading( &m_measuredAtaDiscsCnv);
    addTPConverterForReading( &m_measuredAtaCylindersCnv);
    addTPConverterForReading( &m_measuredAtaPlanesCnv);
    addTPConverterForReading( &m_measuredAtaStraightLinesCnv);

    addTPConverter( &m_neutralParametersCnv);
    addTPConverter( &m_neutralPerigeeCnv);
    addTPConverter( &m_neutralAtaDiscsCnv);
    addTPConverter( &m_neutralAtaCylindersCnv);
    addTPConverter( &m_neutralAtaPlanesCnv);
    addTPConverter( &m_neutralAtaStraightLinesCnv);
    addTPConverterForReading( &m_measuredNeutralPerigeeCnv);
    addTPConverterForReading( &m_measuredNeutralAtaDiscsCnv);
    addTPConverterForReading( &m_measuredNeutralAtaCylindersCnv);
    addTPConverterForReading( &m_measuredNeutralAtaPlanesCnv);
    addTPConverterForReading( &m_measuredNeutralAtaStraightLinesCnv);

    addTPConverter( &m_perigeeSurfacesCnv);
    addTPConverter( &m_discSurfacesCnv);
    addTPConverter( &m_planeSurfacesCnv);
    addTPConverter( &m_cylinderSurfacesCnv);
    addTPConverter( &m_straightLineSurfacesCnv);

    addTPConverter( &m_surfacesCnv);
    addTPConverter( &m_cylinderBoundsCnv);
    addTPConverter( &m_diamondBoundsCnv);
    addTPConverter( &m_discBoundsCnv);
    addTPConverter( &m_rectangleBoundsCnv);
    addTPConverter( &m_trapesoidBoundsCnv);

    addTPConverter( &m_trackSummaryCnv);
    addTPConverter( &m_covarianceMatricesCnv);
    addTPConverter( &m_errorMatricesCnv);

    addTPConverter( &m_rotatedTrapesoidBoundsCnv );           
    addTPConverter( &m_detElSurfCnv );    

    addTPConverter( &m_trackParticleCnv_p2);

    addTPConverter( &m_rotatedDiamondBoundsCnv);

    // HEY YOU --- YES YOU --- READ THE COMMENT AT THE START OF THIS FUNCTION!!!
}

void TrackParticleContainerCnv_tlp2::setPStorage( Rec::TrackParticleContainer_tlp2 *storage )
{ 
    setMainCnvPStorage( &storage->m_trackParticleContainer_p2 );
    m_trackParticleCnv_p2.    setPStorage(&storage->m_trackParticle_p2);

    m_trackParametersCnv.setPStorage(&storage->m_trackParameters);
    m_perigeeCnv.setPStorage(&storage->m_perigees);
    m_measuredPerigeeCnv.setPStorage(&storage->m_measPerigees);

    m_measuredAtaDiscsCnv.	        setPStorage( &storage->m_measuredAtaSurfaces );   
    m_measuredAtaPlanesCnv.	setPStorage( &storage->m_measuredAtaSurfaces );   
    m_measuredAtaCylindersCnv.	setPStorage( &storage->m_measuredAtaSurfaces );   
    m_measuredAtaStraightLinesCnv. setPStorage( &storage->m_measuredAtaSurfaces );

    m_ataDiscsCnv.		        setPStorage( &storage->m_ataSurfaces );           
    m_ataPlanesCnv.		setPStorage( &storage->m_ataSurfaces );           
    m_ataCylindersCnv.		setPStorage( &storage->m_ataSurfaces );           
    m_ataStraightLinesCnv. 	setPStorage( &storage->m_ataSurfaces );

    m_neutralParametersCnv.        setPStorage(&storage->m_trackParameters);
    m_neutralPerigeeCnv.           setPStorage(&storage->m_perigees);
    m_measuredNeutralPerigeeCnv.   setPStorage(&storage->m_measPerigees);

    m_measuredNeutralAtaDiscsCnv.          setPStorage( &storage->m_measuredAtaSurfaces );
    m_measuredNeutralAtaPlanesCnv.         setPStorage( &storage->m_measuredAtaSurfaces );
    m_measuredNeutralAtaCylindersCnv.      setPStorage( &storage->m_measuredAtaSurfaces );
    m_measuredNeutralAtaStraightLinesCnv.  setPStorage( &storage->m_measuredAtaSurfaces );

    m_neutralAtaDiscsCnv.                  setPStorage( &storage->m_ataSurfaces );
    m_neutralAtaPlanesCnv.                 setPStorage( &storage->m_ataSurfaces );
    m_neutralAtaCylindersCnv.              setPStorage( &storage->m_ataSurfaces );
    m_neutralAtaStraightLinesCnv.          setPStorage( &storage->m_ataSurfaces ); 

    m_perigeeSurfacesCnv.setPStorage(&storage->m_surfaces);
    m_discSurfacesCnv.		setPStorage( &storage->m_boundSurfaces );         
    m_planeSurfacesCnv.	setPStorage( &storage->m_boundSurfaces );         
    m_cylinderSurfacesCnv.	setPStorage( &storage->m_boundSurfaces );         
    m_straightLineSurfacesCnv.	setPStorage( &storage->m_boundSurfaces );   
    m_detElSurfCnv.            setPStorage( &storage->m_detElementSurfaces );     


    m_surfacesCnv.		setPStorage( &storage->m_surfaces );              
    m_cylinderBoundsCnv.	setPStorage( &storage->m_cylinderBounds );        
    m_diamondBoundsCnv.	        setPStorage( &storage->m_diamondBounds );         
    m_rotatedDiamondBoundsCnv.  setPStorage( &storage->m_rotatedDiamondBounds );         
    m_discBoundsCnv.	        setPStorage( &storage->m_discBounds );            
    m_rectangleBoundsCnv.	setPStorage( &storage->m_rectangleBounds );       
    m_trapesoidBoundsCnv.	setPStorage( &storage->m_trapesoidBounds );      
    m_rotatedTrapesoidBoundsCnv.   setPStorage( &storage->m_rotatedTrapesoidBounds); 

    m_trackSummaryCnv.         setPStorage(&storage->m_trackSummaries);
    m_covarianceMatricesCnv.   setPStorage(&storage->m_hepSymMatrices);
    m_errorMatricesCnv.        setPStorage(&storage->m_hepSymMatrices);

}


void T_TPCnv<Rec::TrackParticleContainer, Rec::TrackParticleContainer_tlp2 >::
persToTrans (const Rec::TrackParticleContainer_tlp2* pers, Rec::TrackParticleContainer* trans, MsgStream& msg){
    // FIXME: TPConverter uses the same non-const member m_pStorage
    // for both reading and writing, but we want it to be const
    // in the former case.
    Rec::TrackParticleContainer_tlp2* pers_nc ATLAS_THREAD_SAFE =
      const_cast<Rec::TrackParticleContainer_tlp2*> (pers);
    setPStorage (pers_nc);
    m_mainConverter.pstoreToTrans (0, trans, msg);
}

void T_TPCnv<Rec::TrackParticleContainer, Rec::TrackParticleContainer_tlp2 >::
transToPers (const Rec::TrackParticleContainer* trans, Rec::TrackParticleContainer_tlp2* pers, MsgStream& msg)  {
    this->setTLPersObject( pers );
    m_mainConverter.virt_toPersistent(trans, msg);
    this->clearTLPersObject();
}


TrackParticleContainerARACnv_tlp2::TrackParticleContainerARACnv_tlp2()
{
    m_dumDetElSurfCnv.setTopConverter (m_detElSurfCnv.topConverter(), m_detElSurfCnv.typeID());
    addExtTPConverterForReading( &m_dumDetElSurfCnv );    
}


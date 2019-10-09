/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_p3.h"

TrackParticleTruthCollectionContainerCnv_p3::TrackParticleTruthCollectionContainerCnv_p3() :
  T_AthenaPoolTPCnvBase<TrackParticleTruthCollectionContainer,
			TrackParticleTruthCollectionContainer_p3>()
{
}

TrackParticleTruthCollectionContainerCnv_p3::~TrackParticleTruthCollectionContainerCnv_p3()
{
}

void TrackParticleTruthCollectionContainerCnv_p3::transToPers(const TrackParticleTruthCollectionContainer *transVect,
							      TrackParticleTruthCollectionContainer_p3 *persVect,
							      MsgStream &msg)
{
    ITPConverterFor<TrackParticleTruthCollection> *cnv = 0;
    persVect->clear();
    persVect->reserve( transVect->size() );
    for( TrackParticleTruthCollectionContainer::const_iterator it = transVect->begin();
	 it != transVect->end();  ++it ) {
      persVect->push_back(toPersistent(&cnv, *it, msg ));
    }
}

void TrackParticleTruthCollectionContainerCnv_p3::persToTrans(const TrackParticleTruthCollectionContainer_p3 *persVect, 
							      TrackParticleTruthCollectionContainer *transVect, 
			   
							      MsgStream &msg )
{
    ITPConverterFor<TrackParticleTruthCollection> *cnv = 0;
    transVect->clear();
    transVect->reserve( persVect->size() );
    for( TrackParticleTruthCollectionContainer_p3::const_iterator it  = persVect->begin();
         it != persVect->end();  ++it ) {
      transVect->push_back(createTransFromPStore( &cnv, *it, msg ) );
    }
}

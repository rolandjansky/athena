/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_p1.h"

TrackParticleTruthCollectionContainerCnv_p1::TrackParticleTruthCollectionContainerCnv_p1() :
  T_AthenaPoolTPCnvBase<TrackParticleTruthCollectionContainer,
			TrackParticleTruthCollectionContainer_p1>()
{
}

TrackParticleTruthCollectionContainerCnv_p1::~TrackParticleTruthCollectionContainerCnv_p1()
{
}

void TrackParticleTruthCollectionContainerCnv_p1::transToPers(const TrackParticleTruthCollectionContainer *transVect,
							      TrackParticleTruthCollectionContainer_p1 *persVect,
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

void TrackParticleTruthCollectionContainerCnv_p1::persToTrans(const TrackParticleTruthCollectionContainer_p1 *persVect, 
							      TrackParticleTruthCollectionContainer *transVect, 
			   
							      MsgStream &msg )
{
    ITPConverterFor<TrackParticleTruthCollection> *cnv = 0;
    transVect->clear();
    transVect->reserve( persVect->size() );
    for( TrackParticleTruthCollectionContainer_p1::const_iterator it  = persVect->begin();
         it != persVect->end();  ++it ) {
      transVect->push_back(createTransFromPStore( &cnv, *it, msg ) );
    }
}

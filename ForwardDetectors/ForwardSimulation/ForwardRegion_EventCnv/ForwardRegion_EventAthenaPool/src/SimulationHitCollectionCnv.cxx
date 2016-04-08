/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegion_EventTPCnv/SimulationHitCollectionCnv_p1.h"
#include "SimulationHitCollectionCnv.h"
#include "ForwardRegion_EventTPCnv/SimulationHit_p1.h"

SimulationHitCollection_PERS* SimulationHitCollectionCnv::createPersistent(SimulationHitCollection* transCont)
{
	MsgStream mlog(messageService(), "SimulationHitCollectionConverter" );

	SimulationHitCollectionCnv_p1 TPConverter;
	SimulationHitCollection_PERS *pPersColl = TPConverter.createPersistent( transCont, mlog );

	return pPersColl;
}

SimulationHitCollection* SimulationHitCollectionCnv::createTransient()
{
	MsgStream mlog(messageService(), "SimulationHitCollectionConverter" );

	//GUID of persistence collection class (see selection.xml in ForwardRegion_EventTPCnv, class item SimulationHitCollection_p1
	static const pool::Guid p1_guid("DD6CF87F-4D7E-4BF6-9FAC-156DB34C08C7");

	SimulationHitCollectionCnv_p1 TPConverter_p1;
	SimulationHitCollection *pTransColl=NULL;

	if(this->compareClassGuid(p1_guid)){
		std::auto_ptr<SimulationHitCollection_p1> col_vect(this->poolReadObject<SimulationHitCollection_p1>());
		pTransColl=TPConverter_p1.createTransient(col_vect.get(), mlog);
	}
	else{
		throw std::runtime_error("Unsupported persistent version of Data container");
	}

	return pTransColl;
}

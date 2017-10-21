/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_SIDSimHitCollectionCnv_p1.h"
#include "AFP_SIDSimHitCollectionCnv.h"
#include "AFP_EventTPCnv/AFP_SIDSimHit_p1.h"

AFP_SIDSimHitCollection_PERS* AFP_SIDSimHitCollectionCnv::createPersistent(AFP_SIDSimHitCollection* transCont)
{
	MsgStream mlog(msgSvc(), "AFP_SIDSimHitCollectionConverter" );

	AFP_SIDSimHitCollectionCnv_p1 TPConverter;
	AFP_SIDSimHitCollection_PERS *pPersColl = TPConverter.createPersistent( transCont, mlog );

	return pPersColl;
}

AFP_SIDSimHitCollection* AFP_SIDSimHitCollectionCnv::createTransient()
{
	MsgStream mlog(msgSvc(), "AFP_SIDSimHitCollectionConverter" );

	//GUID of persistence collection class (see selection.xml in AFP_EventTPCnv, class item AFP_SIDSimHitCollection_p1
	static const pool::Guid p1_guid("AAA03560-D4CC-40C3-80D6-165448375201");

	AFP_SIDSimHitCollectionCnv_p1 TPConverter_p1;
	AFP_SIDSimHitCollection *pTransColl=NULL;

	if(this->compareClassGuid(p1_guid)){
		std::auto_ptr<AFP_SIDSimHitCollection_p1> col_vect(this->poolReadObject<AFP_SIDSimHitCollection_p1>());
		pTransColl=TPConverter_p1.createTransient(col_vect.get(), mlog);
	}
	else{
		throw std::runtime_error("Unsupported persistent version of Data container");
	}

	return pTransColl;
}

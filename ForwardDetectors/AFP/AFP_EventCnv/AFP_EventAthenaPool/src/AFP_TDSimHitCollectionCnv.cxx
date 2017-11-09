/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_TDSimHitCollectionCnv_p1.h"
#include "AFP_TDSimHitCollectionCnv.h"
#include "AFP_EventTPCnv/AFP_TDSimHit_p1.h"

AFP_TDSimHitCollection_PERS* AFP_TDSimHitCollectionCnv::createPersistent(AFP_TDSimHitCollection* transCont)
{
	MsgStream mlog(msgSvc(), "AFP_TDSimHitCollectionConverter" );

	AFP_TDSimHitCollectionCnv_p1 TPConverter;
	AFP_TDSimHitCollection_PERS *pPersColl = TPConverter.createPersistent( transCont, mlog );

	return pPersColl;
}

AFP_TDSimHitCollection* AFP_TDSimHitCollectionCnv::createTransient()
{
	MsgStream mlog(msgSvc(), "AFP_TDSimHitCollectionConverter" );

	//GUID of persistence collection class (see selection.xml in AFP_EventTPCnv, class item AFP_TDSimHitCollection_p1
	static const pool::Guid p1_guid("F38ED236-94F3-483B-A015-C95EA7B194AE");

	AFP_TDSimHitCollectionCnv_p1 TPConverter_p1;
	AFP_TDSimHitCollection *pTransColl=NULL;

	if(this->compareClassGuid(p1_guid)){
		std::auto_ptr<AFP_TDSimHitCollection_p1> col_vect(this->poolReadObject<AFP_TDSimHitCollection_p1>());
		pTransColl=TPConverter_p1.createTransient(col_vect.get(), mlog);
	}
	else{
		throw std::runtime_error("Unsupported persistent version of Data container");
	}

	return pTransColl;
}

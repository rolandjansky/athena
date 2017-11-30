/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_SiDigiCollectionCnv_p1.h"
#include "AFP_SiDigiCollectionCnv.h"
#include "AFP_EventTPCnv/AFP_SiDigi_p1.h"

AFP_SiDigiCollection_PERS* AFP_SiDigiCollectionCnv::createPersistent(AFP_SiDigiCollection* transCont)
{
	MsgStream mlog(msgSvc(), "AFP_SiDigiCollectionConverter" );

	AFP_SiDigiCollectionCnv_p1 TPConverter;
	AFP_SiDigiCollection_PERS *pPersColl = TPConverter.createPersistent( transCont, mlog );

	return pPersColl;
}

AFP_SiDigiCollection* AFP_SiDigiCollectionCnv::createTransient()
{
	MsgStream mlog(msgSvc(), "AFP_SiDigiCollectionConverter" );

	//GUID of persistence collection class (see selection.xml in AFP_EventTPCnv, class item AFP_SiDigiCollection_p1
	static const pool::Guid p1_guid("513B266D-BD55-4603-A064-8E6F24042707");

	AFP_SiDigiCollectionCnv_p1 TPConverter_p1;
	AFP_SiDigiCollection *pTransColl=NULL;

	if(this->compareClassGuid(p1_guid)){
		std::auto_ptr<AFP_SiDigiCollection_p1> col_vect(this->poolReadObject<AFP_SiDigiCollection_p1>());
		pTransColl=TPConverter_p1.createTransient(col_vect.get(), mlog);
	}
	else{
		throw std::runtime_error("Unsupported persistent version of Data container");
	}

	return pTransColl;
}

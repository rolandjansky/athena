/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_TDDigiCollectionCnv_p1.h"
#include "AFP_TDDigiCollectionCnv.h"
#include "AFP_EventTPCnv/AFP_TDDigi_p1.h"

AFP_TDDigiCollection_PERS* AFP_TDDigiCollectionCnv::createPersistent(AFP_TDDigiCollection* transCont)
{
	MsgStream mlog(msgSvc(), "AFP_TDDigiCollectionConverter" );

	AFP_TDDigiCollectionCnv_p1 TPConverter;
	AFP_TDDigiCollection_PERS *pPersColl = TPConverter.createPersistent( transCont, mlog );

	return pPersColl;
}

AFP_TDDigiCollection* AFP_TDDigiCollectionCnv::createTransient()
{
	MsgStream mlog(msgSvc(), "AFP_TDDigiCollectionConverter" );

	//GUID of persistence collection class (see selection.xml in AFP_EventTPCnv, class item AFP_TDDigiCollection_p1
	static const pool::Guid p1_guid("352BE1B9-96FA-46BB-B1AE-51DDF56380EB");

	AFP_TDDigiCollectionCnv_p1 TPConverter_p1;
	AFP_TDDigiCollection *pTransColl=NULL;

	if(this->compareClassGuid(p1_guid)){
		std::auto_ptr<AFP_TDDigiCollection_p1> col_vect(this->poolReadObject<AFP_TDDigiCollection_p1>());
		pTransColl=TPConverter_p1.createTransient(col_vect.get(), mlog);
	}
	else{
		throw std::runtime_error("Unsupported persistent version of Data container");
	}

	return pTransColl;
}

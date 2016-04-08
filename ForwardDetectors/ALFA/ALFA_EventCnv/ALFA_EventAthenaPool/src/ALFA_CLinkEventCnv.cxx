/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_CLinkEventCnv_p1.h"
#include "ALFA_CLinkEventCnv.h"

ALFA_CLinkEvent_PERS* ALFA_CLinkEventCnv::createPersistent(ALFA_CLinkEvent* transCont)
{
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CLinkEventCnv_p1");
	//LogStream<<MSG::INFO<<"MARK01-ALFA_CLinkEventCnv::createPersistent (AthenaPOOL)"<<endreq;

	ALFA_CLinkEventCnv_p1   TPConverter;
	ALFA_CLinkEvent_PERS *persObj = TPConverter.createPersistent( transCont, LogStream );

	//LogStream<<MSG::INFO<<"MARK02-ALFA_CLinkEventCnv::createPersistent (AthenaPOOL)"<<endreq;
	return persObj;
}

ALFA_CLinkEvent* ALFA_CLinkEventCnv::createTransient()
{
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CLinkEventCnv_p1");
	//LogStream<<MSG::INFO<<"MARK01-ALFA_CLinkEventCnv::createTransient (AthenaPOOL)"<<endreq;

	ALFA_CLinkEventCnv_p1 TPConverter_p1;

	ALFA_CLinkEvent *trans_cont(0); // probably inicialization
	static const pool::Guid p1_guid ("D8FCB0A1-3B3E-4536-B590-1A48347B6E1A");

	if( this->compareClassGuid(p1_guid)){
		std::auto_ptr< ALFA_CLinkEvent_p1 >   col_vect( this->poolReadObject< ALFA_CLinkEvent_p1 >() );
		trans_cont = TPConverter_p1.createTransient( col_vect.get(), LogStream );
	}
	else {
		throw std::runtime_error("Unsupported persistent version of Data container");
	}
	
	//LogStream<<MSG::INFO<<"MARK02-ALFA_CLinkEventCnv::createTransient (AthenaPOOL)"<<endreq;

	return trans_cont;
}

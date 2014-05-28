/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDHandler.h"
#include "GaudiKernel/MsgStream.h"

AGDDHandler::AGDDHandler(std::string s):XMLHandler(s)
{
}

void AGDDHandler::ElementHandle()
{
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"handling for AGDD "<<endreq;
}

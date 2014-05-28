/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/sectionHandler.h"
#include "AGDD2Geo/AGDDSection.h"
#include <iostream>


sectionHandler::sectionHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for section"<<std::endl;
}

void sectionHandler::ElementHandle()
{
	bool res;
	std::string name = getAttributeAsString("name",res);
	std::string version = getAttributeAsString("version",res);
	std::string date = getAttributeAsString("date",res);
	std::string author = getAttributeAsString("author",res);
    std::string top="";
	top=getAttributeAsString("top_volume",res);
	if (msgLog().level()<=MSG::DEBUG)
	{
 	msgLog()<<MSG::DEBUG<<"\t---------------------------"<<endreq;
 	msgLog()<<MSG::DEBUG<<"\tname    "<<name<<endreq;
 	msgLog()<<MSG::DEBUG<<"\tversion "<<version<<endreq;
 	msgLog()<<MSG::DEBUG<<"\tdate    "<<date<<endreq;
 	msgLog()<<MSG::DEBUG<<"\tauthor  "<<author<<endreq;
 	msgLog()<<MSG::DEBUG<<"\ttopVolume "<<top<<endreq;
 	msgLog()<<MSG::DEBUG<<"\t---------------------------"<<endreq;
	}
	AGDDSection *sect __attribute__((__unused__));
	sect=new AGDDSection(name,version,author,date,top,res);
}

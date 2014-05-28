/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/Geo2XML.h"
#include <iostream>
#include <ctime>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

void Geo2XML::openFile(std::string dName)
{
	StoreGateSvc* pDetStore=0;
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
	if (sc.isFailure()) 
	{
		std::cout<<"AGDDController could not get at the detector store!"<<std::endl;
	}
	const DataHandle<GeoModelExperiment> theExpt;
	sc=pDetStore->retrieve( theExpt,"ATLAS");
	if (sc.isFailure()) 
	{
		std::cout<<"AGDDController could not get GeoModelExperiment!"<<std::endl;
	}
	else
	{	
		time_t date;
		time(&date);
		std::string fileName=dName+"_generated.xml";
		xmlFile.open(fileName.c_str());
		xmlFile<<"<?xml version=\"1.0\"?>"<<std::endl;
		xmlFile<<"<!DOCTYPE AGDD SYSTEM \"AGDD.dtd\" >"<<std::endl;
		xmlFile<<"<AGDD>"<<std::endl<<std::endl;
		xmlFile<<"section name= \""<<dName<<"\""<<std::endl;
		xmlFile<<" version = \"0.0\""<<std::endl;
		xmlFile<<" date = \""<<std::ctime(&date)<<"\""<<std::endl;
		xmlFile<<" author = \"John Doe\""<<std::endl;
		const GeoVDetectorManager *theManager=theExpt->getManager(dName);
		if (theManager->getNumTreeTops()>1) std::cout<<"AGDDController: more than one treetop!!!"<<std::endl;
		const GeoVPhysVol *pv= &(*(theManager->getTreeTop(0)));
		xmlFile<<" top_volume = \""<<pv->getLogVol()->getName()<<"\">"<<std::endl;
	}	
}

void Geo2XML::closeFile()
{
	xmlFile<<"</section>"<<std::endl;
	xmlFile<<"</AGDD>"<<std::endl;
}

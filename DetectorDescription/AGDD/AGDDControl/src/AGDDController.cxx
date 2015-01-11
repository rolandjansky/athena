/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/AGDDBuilder.h"
#include "AGDDControl/IAGDDParser.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/AGDD2GeoModelBuilder.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AliasStore.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoPVLink.h"
#include "GeoModelKernel/GeoPVConstLink.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>

std::vector<const GeoLogVol*> volumeMap;

void navigateVolumeContents(const GeoVPhysVol *pv, unsigned int ilev)
{
	const GeoLogVol *cvl = pv->getLogVol();
	std::string vname = cvl->getName();
	if (std::find(volumeMap.begin(),volumeMap.end(),cvl)!=volumeMap.end())
	{
	}
	else
	{
		volumeMap.push_back(cvl);
	}
// 	for (unsigned int i=0;i<ilev;i++)
// 			std::cout<<"\t";
// 	std::cout<<"----- volume "<<vname<<std::endl;
	
	unsigned int ivol=pv->getNChildVols();
	for (unsigned int i=0;i<ivol;i++)
	{
		const GeoVPhysVol *child=&(*(pv->getChildVol(i)));
		navigateVolumeContents(child,ilev+1);
	}
}

AGDDController::~AGDDController()
{
	if (theParser) delete theParser;
	if (theBuilder) delete theBuilder;
}

AGDDController::AGDDController():theBuilder(0),locked(false),disableSections(false),
								printLevel(0)
{
//	theParser=new AMDBParser;
	theParser=new XercesParser;
	theBuilder=new AGDD2GeoModelBuilder;
}

void AGDDController::SetBuilder(AGDDBuilder *b) 
{
	theBuilder=b;
}
void AGDDController::SetParser(IAGDDParser *b) 
{
	theParser=b;
}
AGDDBuilder* AGDDController::GetBuilder() 
{
	return theBuilder;
}
IAGDDParser* AGDDController::GetParser() 
{
	return theParser;
}
void AGDDController::AddFile(std::string fName) 
{
	filesToParse.push_back(fName);
}
void AGDDController::AddSection(std::string section) 
{
	sectionsToBuild.push_back(section);
}
void AGDDController::AddVolume(std::string section) 
{
	volumesToBuild.push_back(section);
}


void AGDDController::ParseFiles()
{
	if (!theParser) theParser=new XercesParser;
	for (unsigned int i=0;i<filesToParse.size();i++)
		theParser->ParseFileAndNavigate(filesToParse[i]);
}

void AGDDController::BuildSections()
{
	for (unsigned int i=0;i<sectionsToBuild.size();i++)
	{
		if (printLevel) std::cout<< " -----> now building section "<<sectionsToBuild[i]<<std::endl;
		theBuilder->BuildFromSection(sectionsToBuild[i]);
	} 
}

void AGDDController::BuildVolumes()
{
	for (unsigned int i=0;i<volumesToBuild.size();i++)
		theBuilder->BuildFromVolume(volumesToBuild[i]);
}

void AGDDController::BuildAll()
{
		BuildVolumes();
		BuildSections();
}

void AGDDController::PrintSections() const
{
	AGDDSectionStore *ss=AGDDSectionStore::GetSectionStore();
  	ss->PrintAllSections();
}

void AGDDController::ParseString(std::string s)
{
	if (!theParser) theParser=new XercesParser;
	theParser->ParseStringAndNavigate(s);
}

AGDDController* AGDDController::GetController()
{
//	std::cout<<" this is AGDDController::GetController()"<<std::endl;
	static AGDDController* theController=new AGDDController;
	return theController;
}

void AGDDController::UseGeoModelDetector(std::string name)
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
		const GeoVDetectorManager *theManager=theExpt->getManager(name);
		if (theManager->getNumTreeTops()>1) std::cout<<"AGDDController: more than one treetop!!!"<<std::endl;
		GeoPVConstLink pv=theManager->getTreeTop(0);
		GeoVPhysVol* ppv=const_cast<GeoVPhysVol*>(&(*pv));
		((AGDD2GeoModelBuilder*)(theBuilder))->SetMotherVolume((GeoPhysVol*)ppv);
	}

}

void AGDDController::PrintVolumeHierarchy(std::string name, int ilevel)
{
	AGDDVolumeStore *vs=AGDDVolumeStore::GetVolumeStore();
	AGDDVolume *vol=vs->GetVolume(name);
	int currentLevel=ilevel+1;
	for (int i=0;i<ilevel;i++) std::cout<<"    ";
	std::cout<<"|  "<<name<<std::endl;
	if (!vol->NrOfDaughter()) return;
	std::vector<std::string> nameVec;
	std::vector<std::string>::iterator it;
	for (int i=0;i<vol->NrOfDaughter();i++)
	{
		std::string nameV=vol->GetDaughter(i)->GetVolume()->GetName();
		it=find(nameVec.begin(),nameVec.end(),nameV);
		if (it!=nameVec.end()) continue;
		nameVec.push_back(nameV);
		PrintVolumeHierarchy(nameV,currentLevel);
	}
}

void AGDDController::Clean()
{
	delete theParser;
	theParser=0;
	
	filesToParse.clear();
	sectionsToBuild.clear();
	volumesToBuild.clear();
	structuresToBuild.clear();
	
	AGDDSectionStore::GetSectionStore()->Clean();
	AGDDVolumeStore::GetVolumeStore()->Clean();
}


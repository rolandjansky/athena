/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GeoModelKernel/GeoPVConstLink.h"
#include "StoreGate/StoreGateSvc.h"

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <TString.h> // for Form
#include <iostream>

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
	unsigned int ivol=pv->getNChildVols();
	for (unsigned int i=0;i<ivol;i++)
	{
		const GeoVPhysVol *child=&(*(pv->getChildVol(i)));
		navigateVolumeContents(child,ilev+1);
	}
}

AGDDController::~AGDDController()
{
	if (m_theParser) delete m_theParser;
	if (m_theBuilder) delete m_theBuilder;
}

AGDDController::AGDDController():m_theBuilder(0),m_locked(false),m_disableSections(false),
								m_printLevel(0)
{
//	m_theParser=new AMDBParser;
	m_theParser=new XercesParser;
	m_theBuilder=new AGDD2GeoModelBuilder;
}

void AGDDController::SetBuilder(AGDDBuilder *b) 
{
	m_theBuilder=b;
}
void AGDDController::SetParser(IAGDDParser *b) 
{
	m_theParser=b;
}
AGDDBuilder* AGDDController::GetBuilder() 
{
	return m_theBuilder;
}
IAGDDParser* AGDDController::GetParser() 
{
	return m_theParser;
}
void AGDDController::AddFile(std::string fName) 
{
	m_filesToParse.push_back(fName);
}
void AGDDController::AddSection(std::string section) 
{
	m_sectionsToBuild.push_back(section);
}
void AGDDController::AddVolume(std::string section) 
{
	m_volumesToBuild.push_back(section);
}


void AGDDController::ParseFiles()
{
	if (!m_theParser) m_theParser=new XercesParser;
	for (unsigned int i=0;i<m_filesToParse.size();i++) {
		if (!m_theParser->ParseFileAndNavigate(m_filesToParse[i])) throw std::runtime_error(Form("File: %s, Line: %d\nAGDDController::ParseFiles() - Could parse file %s.", __FILE__, __LINE__, m_filesToParse[i].c_str()));
	}
}

void AGDDController::BuildSections()
{
	for (unsigned int i=0;i<m_sectionsToBuild.size();i++)
	{
		if (m_printLevel) std::cout<< " -----> now building section "<<m_sectionsToBuild[i]<<std::endl;
		m_theBuilder->BuildFromSection(m_sectionsToBuild[i]);
	} 
}

void AGDDController::BuildVolumes()
{
	for (unsigned int i=0;i<m_volumesToBuild.size();i++)
		m_theBuilder->BuildFromVolume(m_volumesToBuild[i]);
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
	if (!m_theParser) m_theParser=new XercesParser;
	m_theParser->ParseStringAndNavigate(s);
}

bool AGDDController::WriteAGDDtoDBFile(std::string s)
{
	if (!m_theParser)
	{
		std::cout<< "  -----> asking for a dump without defined parser makes no sense - crashing!" <<std::endl;
		return false;
	}
	else {
		m_theParser->WriteToFile(s);
		return true;
	}
}

AGDDController* AGDDController::GetController()
{
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
	const GeoModelExperiment* theExpt = nullptr;
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
		((AGDD2GeoModelBuilder*)(m_theBuilder))->SetMotherVolume((GeoPhysVol*)ppv);
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
	delete m_theParser;
	m_theParser=0;
	
	m_filesToParse.clear();
	m_sectionsToBuild.clear();
	m_volumesToBuild.clear();
	m_structuresToBuild.clear();
	
	AGDDSectionStore::GetSectionStore()->Clean();
	AGDDVolumeStore::GetVolumeStore()->Clean();
}


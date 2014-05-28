/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"
#include "AGDD2Geo/IAGDDParser.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/IAGDD2GeoSvc.h"
#include "AGDD2Geo/AGDD2GeoModelBuilder.h"
#include "AGDD2Geo/AGDDVolumeStore.h"
#include "AGDD2Geo/AGDDVolume.h"
#include "AGDD2Geo/AGDDPositioner.h"
#include "AGDD2Geo/AliasStore.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoPVLink.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeShift.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

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


AGDDController::AGDDController():theBuilder(0),printLevel(0)
{
//	theParser=new AMDBParser;
	theParser=new XercesParser;
	theBuilder=new AGDD2GeoModelBuilder;
	ISvcLocator* svcLocator=Gaudi::svcLocator();
   	StatusCode sc;
//   log << MSG::INFO << "in initializePool()" << endreq;
   	const IAGDD2GeoSvc* geoSvc=0;
   	sc = svcLocator->service ("AGDD2GeoSvc",geoSvc);
   	if (sc.isFailure()) std::cout<<" Error retrieving the service!!!!!!!!!"<<std::endl;
	StrVecIterator it;
	for (it=geoSvc->GetFileBegin();it!=geoSvc->GetFileEnd();it++)
		AddFile((*it));
	for (it=geoSvc->GetSectionBegin();it!=geoSvc->GetSectionEnd();it++)
		AddSection((*it));
	for (it=geoSvc->GetVolumeBegin();it!=geoSvc->GetVolumeEnd();it++)
		AddVolume((*it));
	for (it=geoSvc->GetStructureBegin();it!=geoSvc->GetStructureEnd();it++)
		AddStructure((*it));
	printLevel=geoSvc->PrintLevel();
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
void AGDDController::AddStructure(std::string section) 
{
	structuresToBuild.push_back(section);
}


void AGDDController::ParseFiles()
{
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

void AGDDController::BuildStructures()
{
	ISvcLocator* svcLocator=Gaudi::svcLocator();
   	StatusCode sc;
   	const IAGDD2GeoSvc* geoSvc=0;
   	sc = svcLocator->service ("AGDD2GeoSvc",geoSvc);
   	if (sc.isFailure()) std::cout<<" Error retrieving the service!!!!!!!!!"<<std::endl;
	for (unsigned int i=0;i<structuresToBuild.size();i++)
	{
		std::string s=structuresToBuild[i];
		std::string sName=ALIAS(s);
		geoSvc->SetTagInfo(s,sName);
		if (printLevel) std::cout<<" ----> building structure "<<s<<std::endl;
		theBuilder->BuildFromSection(sName);
	}
}

void AGDDController::BuildVolumesFromFlags()
{
	ISvcLocator* svcLocator=Gaudi::svcLocator();
   	StatusCode sc;
   	const IAGDD2GeoSvc* geoSvc=0;
   	sc = svcLocator->service ("AGDD2GeoSvc",geoSvc);
   	if (sc.isFailure()) std::cout<<" Error retrieving the service!!!!!!!!!"<<std::endl;
	for (unsigned int i=0;i<structuresToBuild.size();i++)
	{
		std::string s=structuresToBuild[i];
		std::string sName=ALIAS(s);
		geoSvc->SetTagInfo(s,sName);
		if (printLevel) std::cout<<" ----> building structure "<<s<<std::endl;
		theBuilder->BuildFromVolume(sName);
	}
}

void AGDDController::BuildVolumes()
{
	for (unsigned int i=0;i<volumesToBuild.size();i++)
		theBuilder->BuildFromVolume(volumesToBuild[i]);
}

void AGDDController::BuildAll()
{
	ISvcLocator* svcLocator=Gaudi::svcLocator();
	StatusCode sc;
   	const IAGDD2GeoSvc* geoSvc=0;
   	sc = svcLocator->service ("AGDD2GeoSvc",geoSvc);
   	if (sc.isFailure()) std::cout<<" Error retrieving the service!!!!!!!!!"<<std::endl;

    if (geoSvc->IsLocked())
	{
		if (printLevel) std::cout<<" AGDD2GeoSvc is locked! trying to build from AGDD2GeoFlags..."<<std::endl;
		BuildVolumesFromFlags();
		if (!geoSvc->DisableSections())
		{
			if (printLevel) std::cout<<"...that notwithstanding, it seems there are sections "<<
					   " to be built: trying now"<<std::endl;
			BuildSections();
		}
	}
	else if (!geoSvc->DisableSections())
	{
		BuildVolumes();
		BuildSections();
	}
	else
	{
		BuildStructures();
	}
}

void AGDDController::PrintSections() const
{
	AGDDSectionStore *ss=AGDDSectionStore::GetSectionStore();
  	ss->PrintAllSections();
}

void AGDDController::ParseString(std::string s)
{
	theParser->ParseStringAndNavigate(s);
}

AGDDController* AGDDController::GetController()
{
//	std::cout<<" this is AGDDController::GetController()"<<std::endl;
	static AGDDController* theController=new AGDDController;
	return theController;
}

void AGDDController::ParseAMDC()
{
	std::string sAMDC=GetAMDC();
//	theParser->ParseFileAndNavigate("Generated_amdb_simrec_Pool");
	theParser->ParseStringAndNavigate(sAMDC);
}

std::string AGDDController::GetAMDC()
{
//	std::cout<<"-------------- this is ParseAMDC ---------------"<<std::endl;
	

//   MsgStream log(messageService(), );
   ISvcLocator* svcLocator=Gaudi::svcLocator();
   StatusCode sc;
//   log << MSG::INFO << "in initializePool()" << endreq;

   const IGeoModelSvc * geoModel;
   sc = svcLocator->service ("GeoModelSvc",geoModel);
   if (sc.isFailure()) {}
   
   const IAGDD2GeoSvc* geoSvc=0;
   sc = svcLocator->service ("AGDD2GeoSvc",geoSvc);
   if (sc.isFailure()) std::cout<<" Error retrieving the service!!!!!!!!!"<<std::endl;
   

   IRDBAccessSvc * accessSvc;
   sc = svcLocator->service("RDBAccessSvc",accessSvc);
   if (sc.isFailure()) {}

   accessSvc->connect();

   std::string AtlasVersion = geoModel->atlasVersion();
   std::string MuonVersion  = geoModel->muonVersionOverride();
   
   if (printLevel) std::cout<<"--------------- MuonVersion "<<MuonVersion<<"------------"<<std::endl;

   std::string detectorKey  = MuonVersion.empty() ? AtlasVersion : MuonVersion;
   std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";
   if ( MuonVersion == "CUSTOM"){
     detectorKey  = AtlasVersion ;
     detectorNode = "ATLAS"  ;
   } 

   const IRDBRecordset *recordsetAMDC = accessSvc->getRecordset("AMDC",detectorKey,detectorNode);

   const IRDBRecord *recordAMDC =  (*recordsetAMDC)[0];
   std::string AmdcString = recordAMDC->getString("DATA");

   const IRDBRecordset *recordsetAGDD = accessSvc->getRecordset("AGDD",detectorKey,detectorNode);

   const IRDBRecord *recordAGDD =  (*recordsetAGDD)[0];
   std::string AgddString = recordAGDD->getString("DATA");
   accessSvc->shutdown();

   size_t pos=AgddString.find("AGDD.dtd");
	if (pos!=std::string::npos) AgddString.replace(pos-21,32,"-- Reference to AGDD.dtd automatically removed -->");
     std::ofstream  GeneratedFile;
     if (geoSvc->DumpAGDDString())
     {
     	GeneratedFile.open("Generated_amdb_simrec_Pool");
     	GeneratedFile << AgddString << std::endl;
     	GeneratedFile.close();
     }
	 
	 return AgddString;

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

void AGDDController::NavigateGeoModelDetector(std::string name)
{
	volumeMap.clear();
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
	
		std::ofstream xmlFile;
		std::string fileName=name+"_generated.xml";
		xmlFile.open(fileName.c_str());
		xmlFile<<"<?xml version=\"1.0\"?>"<<std::endl;
		xmlFile<<"<!DOCTYPE AGDD SYSTEM \"AGDD.dtd\" >"<<std::endl;
		xmlFile<<"<AGDD>"<<std::endl<<std::endl;
		xmlFile<<"section name= \""<<name<<"\""<<std::endl;
		xmlFile<<" version = \"0.0\""<<std::endl;
		xmlFile<<" date = \""<<std::ctime(&date)<<"\""<<std::endl;
		xmlFile<<" author = \"John Doe\""<<std::endl;
//		std::cout<<" +++++++++++ detector "<<name<<std::endl;
		const GeoVDetectorManager *theManager=theExpt->getManager(name);
		if (theManager->getNumTreeTops()>1) std::cout<<"AGDDController: more than one treetop!!!"<<std::endl;
		const GeoVPhysVol *pv= &(*(theManager->getTreeTop(0)));
		xmlFile<<" top_volume = \""<<pv->getLogVol()->getName()<<"\">"<<std::endl;
		navigateVolumeContents(pv,0);
		
		for (unsigned int rit=volumeMap.size();rit>0;rit--)
		{
			const GeoLogVol* vv=volumeMap[rit-1];
			std::string n=vv->getName();
			std::ostringstream sname;
			sname<<n<<"_"<<vv->getShape();
			// std::cout<<" logical volume "<<n;
			std::string mat=vv->getMaterial()->getName();
			if (vv->getShape()->typeID()==GeoBox::getClassTypeID())
			{
				const GeoBox* box=(GeoBox*)(vv->getShape());
				xmlFile<<"<box name=\""<<sname.str()<<"\""<<
				  " material=\""<<mat<<"\""<<
				  " X_Y_Z=\""<<
				  2*box->getXHalfLength()<<";"<<
				  2*box->getYHalfLength()<<";"<<
				  2*box->getZHalfLength()<<";"<<
				  "\"/>"<<std::endl;
			}
			else if (vv->getShape()->typeID()==GeoTubs::getClassTypeID())
			{
				const GeoTubs* tube=(GeoTubs*)(vv->getShape());
				xmlFile<<"<tubs name=\""<<sname.str()<<"\"" <<
				  " material=\""<<mat<<"\""<<
				  " Rio_Z=\""<<
				  tube->getRMin()<<";"<<
				  tube->getRMax()<<";"<<
				  2*tube->getZHalfLength()<<";"<<
				  "\" profile=\""<<tube->getSPhi()*180./3.1415926<<";"<<
				  tube->getDPhi()*180./3.1415926<<"\" />"<<std::endl;
			}
			else if (vv->getShape()->typeID()==GeoTube::getClassTypeID())
			{
				const GeoTube* tube=(GeoTube*)(vv->getShape());
				xmlFile<<"<tubs name=\""<<sname.str()<<"\"" <<
				  " material=\""<<mat<<"\""<<
				  " Rio_Z=\""<<
				  tube->getRMin()<<";"<<
				  tube->getRMax()<<";"<<
				  2*tube->getZHalfLength()<<";"<<
				  "\"/>"<<std::endl;
			}
			else if (vv->getShape()->typeID()==GeoTrd::getClassTypeID())
				std::cout<<" shape Trd "<<std::endl;
			else if (vv->getShape()->typeID()==GeoCons::getClassTypeID())
				std::cout<<" shape Cons "<<std::endl;
			else if (vv->getShape()->typeID()==GeoShapeUnion::getClassTypeID())
				std::cout<<" shape Union "<<std::endl;
			else if (vv->getShape()->typeID()==GeoShapeIntersection::getClassTypeID())
				std::cout<<" shape Intersection "<<std::endl;
			else if (vv->getShape()->typeID()==GeoShapeSubtraction::getClassTypeID())
				std::cout<<" shape Subtraction "<<std::endl;
			else if (vv->getShape()->typeID()==GeoShapeShift::getClassTypeID())
				std::cout<<" shape Shift "<<std::endl;
			else 
				std::cout<<" shape unknown "<<std::endl;
		}
		
		xmlFile<<"</section>"<<std::endl;
		xmlFile<<"</AGDD>"<<std::endl;
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
std::map<std::string, GeoFullPhysVol*>* AGDDController::GetMSdetectors() const
{
  if (theBuilder) return ((AGDD2GeoModelBuilder*)theBuilder)->GetMSdetectors();
  else return 0;
}

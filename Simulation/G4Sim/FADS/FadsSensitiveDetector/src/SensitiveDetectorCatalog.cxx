/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
//#include "FadsSensitiveDetector/SensitiveDetectorMessenger.h"

namespace FADS {

SensitiveDetectorCatalog* SensitiveDetectorCatalog::thePointer=
	SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();

SensitiveDetectorCatalog::SensitiveDetectorCatalog()
{
//	std::cout<<"Sensitive Detector catalog being built now!!"<<std::endl;
//	theMessenger=new SensitiveDetectorMessenger(this);
}

SensitiveDetectorCatalog* SensitiveDetectorCatalog::GetSensitiveDetectorCatalog()
{
	if (!thePointer) thePointer=new SensitiveDetectorCatalog;
	return thePointer;
}

void SensitiveDetectorCatalog::RegisterEntry(SensitiveDetectorEntry *d)
{
//	cout<<"registering sensitive detector entry"<<d->GetName()<<endl;
	if (theCatalog.find(d->GetName())!=theCatalog.end())
		std::cout<<"Redefining SensitiveDetectors list "<<d->GetName()<<std::endl;
	else
		theCatalog[d->GetName()]=d;
}

void SensitiveDetectorCatalog::RegisterSensitiveDetector(FadsSensitiveDetector *d)
{
//	cout<<"registering sensitive detector "<<d->GetName()<<endl;
	if (theStore.find(d->GetName())!=theStore.end())
		std::cout<<"Redefining SensitiveDetector "<<d->GetName()<<std::endl;
	else
		theStore[d->GetName()]=d;
}

SensitiveDetectorEntry* SensitiveDetectorCatalog::GetSensitiveDetectorEntry(std::string name)
{
	if (theCatalog.find(name)==theCatalog.end())
	{
		std::cout<<"Sensitive Detector entry "<<name<<"notfound!"
		<<std::endl;
		return 0;
	}
	else 
	{
		SensitiveDetectorEntry* ds=theCatalog[name];
		return ds;
	}
}

FadsSensitiveDetector* SensitiveDetectorCatalog::GetSensitiveDetector(std::string name)
{
	if (theStore.find(name)==theStore.end())
	{
		std::cout<<"Sensitive Detector "<<name<<"notfound!"
		<<std::endl;
		return 0;
	}
	else 
	{
		FadsSensitiveDetector* ds=theStore[name];
		return ds;
	}
}

void SensitiveDetectorCatalog::PrintEntries()
{
	std::cout<<"Printing the list of pre-defined Sensitive Detector entries:"
		<<std::endl;
	std::cout<<theCatalog.size()<<std::endl;
	SDCatalog::const_iterator it;
	for (it=theCatalog.begin();it!=theCatalog.end();it++)
	{
		std::cout<<" --- Sensitive Detector entry "<<(*it).first<<std::endl;
	}
}
void SensitiveDetectorCatalog::PrintSensitiveDetectorList()
{
	std::cout<<"Printing the list of Sensitive Detectors:"
		<<std::endl;
	std::cout<<theStore.size()<<std::endl;
	SDStore::const_iterator it;
	for (it=theStore.begin();it!=theStore.end();it++)
	{
		std::cout<<" --- Sensitive Detector: "<<(*it).first<<std::endl;
	}
}

}	// end namespace

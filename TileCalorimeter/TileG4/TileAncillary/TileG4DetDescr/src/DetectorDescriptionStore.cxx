/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include "TileG4DetDescr/DetectorDescription.h"
//#include "FadsDetDescription/DescriptionMessenger.h"

#include <iostream>

namespace FADS {

DetectorDescriptionStore *p=DetectorDescriptionStore::GetDetectorDescriptionStore();

DetectorDescriptionStore::DetectorDescriptionStore() 
{
//	theMessenger=new DescriptionMessenger(this);
	theMap=new DetDescMap();
	theNameSpaces["std"]=theMap;
	oldNameSpace=currentNameSpace="std";
}

DetectorDescriptionStore* DetectorDescriptionStore::GetDetectorDescriptionStore()
{
	static DetectorDescriptionStore* thePointer=0;
	if (!thePointer) thePointer=new DetectorDescriptionStore;
	return thePointer;
}

void DetectorDescriptionStore::UseNameSpace(std::string s)
{
	if (s==currentNameSpace) return;
	oldNameSpace=currentNameSpace; 
	if (theNameSpaces.find(s)!=theNameSpaces.end())
	{
		currentNameSpace=s;
		theMap=theNameSpaces[s];
	}
	else
	{
		theMap=new DetDescMap();
		theNameSpaces[s]=theMap;
		currentNameSpace=s;
	}
}

void DetectorDescriptionStore::ResetNameSpace() 
{	
	currentNameSpace=oldNameSpace;
	theMap=theNameSpaces[currentNameSpace];
	std::cout<<" NameSpace reset to be "<<currentNameSpace<<std::endl;
}

void DetectorDescriptionStore::AddDetectorDescription(DetectorDescription *d)
{
	std::string name=d->GetName();
	std::string::size_type i=name.find("::");
	std::string nspace;
	if (i!=std::string::npos)
	{
		nspace=name.substr(0,i);
		name=name.substr(i+2,name.size()-i+2);
	}
		
	if (!nspace.empty()) UseNameSpace(nspace);
	if (theMap->find(name)!=theMap->end())
		std::cout<<"Detector Description name "<<name<<
			   " already exists in namespace "<<currentNameSpace<<
			   ": nothing done"<<std::endl;
	else
		theMap->operator[](name)=d;
	if (!nspace.empty()) ResetNameSpace();
}

void DetectorDescriptionStore::RemoveDetectorDescription(std::string nam)
{
	std::string::size_type i=nam.find("::");
	std::string nspace;
	if (i!=std::string::npos)
	{
		nspace=nam.substr(0,i);
		nam=nam.substr(i+2,nam.size()-i+2);
	}
		
	if (!nspace.empty()) UseNameSpace(nspace);
	
	if (theMap->find(nam)!=theMap->end()) theMap->erase(nam);
	
	if (!nspace.empty()) ResetNameSpace();
}

void DetectorDescriptionStore::ReplaceDetectorDescription(std::string nam,
						const DetectorDescription *d)
{
	std::string::size_type i=nam.find("::");
	std::string nspace;
	if (i!=std::string::npos)
	{
		nspace=nam.substr(0,i);
		nam=nam.substr(i+2,nam.size()-i+2);
	}
		
	if (!nspace.empty()) UseNameSpace(nspace);
	DetDescMap::iterator it;
	if ((it=theMap->find(nam))!=theMap->end()) 
		(*it).second=const_cast<DetectorDescription *>(d);
	if (!nspace.empty()) ResetNameSpace();
}

bool DetectorDescriptionStore::FindDetectorDescription(std::string name)
{
	return (theMap->find(name)!=theMap->end());
}

DetectorDescription* DetectorDescriptionStore::GetDetectorDescription(std::string name)
{
	std::string::size_type i=name.find("::");
	std::string nspace;
	if (i!=std::string::npos)
	{
		nspace=name.substr(0,i);
		name=name.substr(i+2,name.size()-i+2);
	}
		
	if (!nspace.empty()) UseNameSpace(nspace);
	DetectorDescription *dd=0;
	if (theMap->find(name)==theMap->end())
		dd=0;
//		std::cout<<"Detector Description name "<<name<<
//			   "not found! return 0"<<std::endl;
	else
		dd=theMap->operator[](name);
	if (!nspace.empty()) ResetNameSpace();
	return dd;
}

void DetectorDescriptionStore::PrintDetectorDescription(std::string name)
{
	std::string::size_type i=name.find("::");
	std::string nspace;
	if (i!=std::string::npos)
	{
		nspace=name.substr(0,i);
		name=name.substr(i+2,name.size()-i+2);
	}
		
	if (!nspace.empty()) UseNameSpace(nspace);
	if (theMap->find(name)==theMap->end())
		std::cout<<"Detector Description name "<<name<<
			   "not found! "<<std::endl;
	else
		theMap->operator[](name)->print();
	if (!nspace.empty()) ResetNameSpace();
}

DetDescIterator DetectorDescriptionStore::DetDescBegin()
{
	return theMap->begin();
}

DetDescIterator DetectorDescriptionStore::DetDescEnd()
{
	return theMap->end();
}

void DetectorDescriptionStore::PrintAll()
{
	DetDescIterator it;
	NameSpaces::const_iterator ns;
	std::cout<<std::endl;
	std::cout<<"  Listing all detector description objects currently defined"<<std::endl;
	for (ns=theNameSpaces.begin();ns!=theNameSpaces.end();ns++)
	{
		std::cout<<std::endl<<"- NameSpace "<<(*ns).first<<std::endl<<std::endl;
		for (it=((*ns).second)->begin();it!=((*ns).second)->end();it++)
		{
			std::cout<<"--->  ";
			if ((*it).second->IsPointed()) std::cout<<"\t\t";
			else std::cout<<" * ";
			std::cout<<(*it).first<<std::endl;
		}
	}
} 

}	// end namespace

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDSection.h"
#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDTechnology.h"

#include <iostream>

//using AGDDModel::AGDDTechnology;

AGDDDetectorStore::AGDDDetectorStore()
{
}
AGDDDetectorStore* AGDDDetectorStore::GetDetectorStore()
{
	static AGDDDetectorStore* theStore=new AGDDDetectorStore;
	return theStore;
}
void AGDDDetectorStore::RegisterDetector(AGDDDetector *s)
{
	std::string n=s->GetName();
	if (m_the_detectors.find(n) != m_the_detectors.end())
		std::cout<<" Detector "<<n<<" already in store "<<std::endl;
	else
                 m_the_detectors[n]=s;
}

void AGDDDetectorStore::RegisterTechnology(AGDDTechnology *s)
{
	std::string n=s->GetName();
	if (m_the_technologies.find(n) != m_the_technologies.end())
		std::cout<<" Technology "<<n<<" already in store "<<std::endl;
	else
		m_the_technologies[n]=s;
}
void AGDDDetectorStore::PrintAllDetectors()
{
    detectorList::const_iterator it = m_the_detectors.begin();
    detectorList::const_iterator end = m_the_detectors.end();
    std::cout<<"AGDDDetectorStore::PrintAllDetector"<<std::endl
             <<"[key] \t name \t subType \t tech"<<std::endl;
    for( ; it!=end; ++it) {
        std::cout<<"["<<it->first<<"] :"
                 <<" "<<it->second->GetName()
                 <<" "<<it->second->subType()
                 <<" "<<it->second->tech
                 <<std::endl;
    }
}

detectorList& AGDDDetectorStore::GetDetectorList() const
{
	return AGDDDetectorStore::GetDetectorStore()->GetDetectorList();
}

std::vector<AGDDDetector*> AGDDDetectorStore::GetDetectorsByType(std::string dt) const
{
	std::vector<AGDDDetector*> detectors;
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		AGDDDetector* det=(*it).second;
		if (dt==det->DetectorType()) detectors.push_back(det);
	}
	return detectors;
}

AGDDDetector* AGDDDetectorStore::GetDetectorByID(std::string dt) const
{
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		AGDDDetector* det=(*it).second;
		if (dt==det->DetectorID()) return det;
	}
	return 0;
}

template<class T> std::vector<T*> AGDDDetectorStore::GetDetectorsByType() const 
{
	std::vector<T*> detectors;
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		T* det=dynamic_cast<T*>((*it).second);
		if (det) detectors.push_back(det);
	}
	return detectors;
}
template<class T> T* AGDDDetectorStore::GetDetectorByID(std::string id) const 
{
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		if (id==(*it).second->DetectorID())
	    {
			T* det=dynamic_cast<T*>((*it).second);
			if (det) return det;
		}
	}
	return 0;
}

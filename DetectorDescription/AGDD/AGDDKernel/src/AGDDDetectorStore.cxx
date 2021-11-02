/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

const detectorList& AGDDDetectorStore::GetDetectorList() const
{
	return m_the_detectors;
}

std::vector<AGDDDetector*> AGDDDetectorStore::GetDetectorsByType(const std::string& dt) const
{
	std::vector<AGDDDetector*> detectors;
	const detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();++it)
	{
		AGDDDetector* det=(*it).second;
		if (dt==det->DetectorType()) detectors.push_back(det);
	}
	return detectors;
}

AGDDDetector* AGDDDetectorStore::GetDetectorByID(const std::string& dt) const
{
	const detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();++it)
	{
		AGDDDetector* det=(*it).second;
		if (dt==det->DetectorID()) return det;
	}
	return 0;
}

template<class T> std::vector<T*> AGDDDetectorStore::GetDetectorsByType() const 
{
	std::vector<T*> detectors;
	const detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();++it)
	{
		T* det=dynamic_cast<T*>((*it).second);
		if (det) detectors.push_back(det);
	}
	return detectors;
}
template<class T> T* AGDDDetectorStore::GetDetectorByID(const std::string& id) const 
{
	const detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();++it)
	{
		if (id==(*it).second->DetectorID())
	    {
			T* det=dynamic_cast<T*>((*it).second);
			if (det) return det;
		}
	}
	return 0;
}

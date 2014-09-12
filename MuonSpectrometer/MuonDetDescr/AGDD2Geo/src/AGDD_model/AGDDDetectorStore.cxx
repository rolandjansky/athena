/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDDetectorStore.h"
#include "AGDD2Geo/AGDDSection.h"
#include "AGDD2Geo/AGDDDetector.h"

#include <iostream>

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
	if (the_detectors.find(n) != the_detectors.end())
		std::cout<<" Detector "<<n<<" already in store "<<std::endl;
	else
		the_detectors[n]=s;
}
void AGDDDetectorStore::PrintAllDetectors()
{
    detectorList::const_iterator it = the_detectors.begin();
    detectorList::const_iterator end = the_detectors.end();
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


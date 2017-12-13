/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDetectorHelper_H
#define MMDetectorHelper_H

#include <map>

class MMDetectorDescription;
class AGDDDetectorPositioner;

typedef std::map<std::string,MMDetectorDescription*> MicromegasMap;
typedef std::map<std::string,MMDetectorDescription*>::const_iterator MicromegasIterator;

typedef std::pair<MMDetectorDescription*,AGDDDetectorPositioner*> AGDDPositionedDetector;

class MMDetectorHelper {
public:
	MMDetectorHelper();
	MicromegasIterator MM_begin() {return m_MicromegasList.begin();}
	MicromegasIterator MM_end()   {return m_MicromegasList.end();}
	
	MMDetectorDescription* Get_MMDetectorType(std::string type);
	MMDetectorDescription* Get_MMDetector(char type,int ieta,int iphi,int layer=1,char side='A');
	AGDDPositionedDetector Get_MMPositionedDetector(char type,int ieta,int iphi,int layer=1,char side='A');

private:
	MicromegasMap m_MicromegasList;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCDetectorHelper_H
#define sTGCDetectorHelper_H

#include <map>

class sTGCDetectorDescription;
class AGDDDetectorPositioner;

typedef std::map<std::string,sTGCDetectorDescription*> sTGCMap;
typedef std::map<std::string,sTGCDetectorDescription*>::const_iterator sTGCIterator;

typedef std::pair<sTGCDetectorDescription*,AGDDDetectorPositioner*> AGDDPositionedDetector;

class sTGCDetectorHelper {
public:
	sTGCDetectorHelper();
	sTGCIterator sTGC_begin() {return m_sTGCList.begin();}
	sTGCIterator sTGC_end()   {return m_sTGCList.end();}
	
	sTGCDetectorDescription* Get_sTGCDetector(char type,int ieta,int iphi,int layer=1,char side='A');
	sTGCDetectorDescription* Get_sTGCDetectorType(std::string type);
	AGDDPositionedDetector Get_sTGCPositionedDetector(char type,int ieta,int iphi,int layer=1,char side='A');
	
private:
	sTGCMap m_sTGCList;

};

#endif

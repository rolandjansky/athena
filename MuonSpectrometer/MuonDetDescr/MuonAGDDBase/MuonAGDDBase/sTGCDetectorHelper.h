/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCDetectorHelper_H
#define sTGCDetectorHelper_H

#include <map>

class AGDDsTGC;

typedef std::map<std::string,AGDDsTGC*> sTGCMap;
typedef std::map<std::string,AGDDsTGC*>::const_iterator sTGCIterator;

class sTGCDetectorHelper {
public:
	sTGCDetectorHelper();
	sTGCIterator sTGC_begin() {return sTGCList.begin();}
	sTGCIterator sTGC_end()   {return sTGCList.end();}
	
	AGDDsTGC* Get_sTGCDetector(char type,int ieta,int iphi,int layer=1,char side='A');
private:
	sTGCMap sTGCList;

};

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCDetectorHelper_H
#define sTGCDetectorHelper_H

#include "AGDDControl/IAGDD2GeoSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <map>
#include <string>

class sTGCDetectorDescription;
class AGDDDetectorPositioner;

typedef std::map<std::string,sTGCDetectorDescription*> sTGCMap;
typedef std::map<std::string,sTGCDetectorDescription*> sTGCMapSubType;
typedef std::map<std::string,sTGCDetectorDescription*>::const_iterator sTGCIterator;

typedef std::pair<sTGCDetectorDescription*,AGDDDetectorPositioner*> AGDDPositionedDetector;

class sTGCDetectorHelper {
public:
	sTGCDetectorHelper();
	sTGCIterator sTGC_begin() {return m_sTGCList.begin();}
	sTGCIterator sTGC_end()   {return m_sTGCList.end();}
	
	sTGCDetectorDescription* Get_sTGCDetector(char type,int ieta,int iphi,int layer=1,char side='A');
	sTGCDetectorDescription* Get_sTGCDetectorType(const std::string& type);
	sTGCDetectorDescription* Get_sTGCDetectorSubType(const std::string& type);
	sTGCDetectorDescription* Get_sTGCDetectorType(std::string_view type);
	sTGCDetectorDescription* Get_sTGCDetectorSubType(std::string_view type);
	AGDDPositionedDetector Get_sTGCPositionedDetector(char type,int ieta,int iphi,int layer=1,char side='A');
        IAGDDtoGeoSvc::LockedController Get_Controller();


private:
	sTGCMap m_sTGCList;
	sTGCMapSubType m_sTGCListSubType;
        ServiceHandle<IAGDDtoGeoSvc> m_svc {"AGDDtoGeoSvc", "MMDetectorHelper"};
};

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDetectorHelper_H
#define MMDetectorHelper_H

#include "AGDDControl/IAGDD2GeoSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <map>
#include <string>

class MMDetectorDescription;
class AGDDDetectorPositioner;

typedef std::map<std::string,MMDetectorDescription*> MicromegasMap;
typedef std::map<std::string,MMDetectorDescription*> MicromegasMapSubType;
typedef std::map<std::string,MMDetectorDescription*>::const_iterator MicromegasIterator;

typedef std::pair<MMDetectorDescription*,AGDDDetectorPositioner*> AGDDPositionedDetector;

class MMDetectorHelper {
public:
	MMDetectorHelper();
	MicromegasIterator MM_begin() {return m_MicromegasList.begin();}
	MicromegasIterator MM_end()   {return m_MicromegasList.end();}
	
	MMDetectorDescription* Get_MMDetectorType(const std::string& type);
        MMDetectorDescription* Get_MMDetectorSubType(const std::string& type);
	MMDetectorDescription* Get_MMDetector(char type,int ieta,int iphi,int layer=1,char side='A');
	AGDDPositionedDetector Get_MMPositionedDetector(char type,int ieta,int iphi,int layer=1,char side='A');

        IAGDDtoGeoSvc::LockedController Get_Controller();

private:
	MicromegasMap m_MicromegasList;
        MicromegasMapSubType m_MicromegasListSubType;
        ServiceHandle<IAGDDtoGeoSvc> m_svc {"AGDDtoGeoSvc", "MMDetectorHelper"};
};

#endif

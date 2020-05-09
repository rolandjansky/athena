/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Implement default GmxInterface, which just prints debug messages.
//    User should over-ride all three methods.
//    Use: create an instance of this, and pass a reference to it in the Gmx2Geo constructor.
//
#include "GeoModelXml/GmxInterface.h"

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/Query.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
using namespace std;

int GmxInterface::sensorId(map<string, int> &/*index*/) {
    static int sequentialId;
    return sequentialId++;
}

void GmxInterface::addSensorType(string clas, string type, map<string, string> params) {

#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");

    log << MSG::DEBUG << "GmxInterface::addSensorType called for class " << clas << "; type " << type << 
                         "\n    Parameter names and values:\n";
    for (map<string, string>::iterator p = params.begin(); p != params.end(); ++p) {
        log << "        " << p->first << " = " << p->second << endmsg;
    }
#else
    std::cout << "GmxInterface::addSensorType called for class " << clas << "; type " << type << 
                         "\n    Parameter names and values:\n";

    for (map<string, string>::iterator p = params.begin(); p != params.end(); ++p) {
        std::cout << "        " << p->first << " = " << p->second << std::endl;
    }
#endif

}

void GmxInterface::addSensor(string name, map<string, int> &index, int sequentialId, GeoVFullPhysVol *fpv) {

#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");

    log << MSG::DEBUG << "GmxInterface::addSensor called for " << fpv->getLogVol()->getName() << ", type " << name << 
                         ". Indices:   ";
#else
    std::cout<< "GmxInterface::addSensor called for " << fpv->getLogVol()->getName() << ", type " << name << 
                         ". Indices:   ";
#endif	 
		
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
#ifndef STANDALONE_GMX
        log << i->second << "   ";
    }
    log << "\nSequential ID = " << sequentialId << endmsg;
#else
    	std::cout << i->second << "   ";
    }
    std::cout << "\nSequential ID = " << sequentialId << std::endl;
#endif

}

void GmxInterface::addAlignable(int level, map<std::string, int> &index, GeoVFullPhysVol *fpv, 
                                GeoAlignableTransform */*transform*/) {
//
//    Logvol name is not very useful (usually == AssemblyLV). Get PhysVol name (seems surprisingly awkward way needed)
//
#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
/* Cannot get this to work.
    string name("New name");
    GeoPVConstLink parent = fpv->getParent();
    Query<unsigned int> cIndex = parent->indexOf(fpv);
    if (cIndex.isValid()) {
        name = parent->getNameOfChildVol(cIndex);
    }
    else {
        name = string("Something Wrong, fullPV not found in parent in GmxInterface::addAlignable");
    }
    log << MSG::DEBUG << "GmxInterface::addAlignable called for physvol name " << name << ".  Level = " << level << ". Indices:   ";
*/

    log << MSG::DEBUG << "GmxInterface::addAlignable called for a physvol. Logvol name " << fpv->getLogVol()->getName() << 
            ". Level = " << level << ". Indices:   ";
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        log << i->second << "   ";
    }
    log << endmsg;
#else
    std::cout << "GmxInterface::addAlignable called for a physvol. Logvol name " << fpv->getLogVol()->getName() << 
            ". Level = " << level << ". Indices:   ";
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        std::cout << i->second << "   ";
    }
    std::cout << std::endl;
#endif
}

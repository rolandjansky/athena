/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
//    Implement default GmxInterface, which just prints debug messages.
//    User should over-ride all three methods.
//    Use: create an instance of this, and pass a reference to it in the Gmx2Geo constructor.
//
#include "GeoModelXml/GmxInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/Query.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
using namespace std;

int GmxInterface::sensorId(map<string, int> &/*index*/) const {
    static int sequentialId;
    return sequentialId++;
}

int GmxInterface::splitSensorId(map<string, int> &/*index*/, std::pair<std::string, int> &/*extraIndex*/, map<string, int> &/*updatedIndex*/) const {
    static int sequentialId;
    return sequentialId++;
}


void GmxInterface::addSensorType(string clas, string type, map<string, string> params) {
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");

    log << MSG::DEBUG << "GmxInterface::addSensorType called for class " << clas << "; type " << type << 
                         "\n    Parameter names and values:\n";
    for (map<string, string>::iterator p = params.begin(); p != params.end(); ++p) {
        log << "        " << p->first << " = " << p->second << endmsg;
    }
}

void GmxInterface::addSensor(string name, map<string, int> &index, int sequentialId, GeoVFullPhysVol *fpv) {
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");

    log << MSG::DEBUG << "GmxInterface::addSensor called for " << fpv->getLogVol()->getName() << ", type " << name << 
                         ". Indices:   ";
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        log << i->second << "   ";
    }
    log << "\nSequential ID = " << sequentialId << endmsg;
}

void GmxInterface::addSplitSensor(string name, map<string, int> &index, std::pair<std::string, int> &extraIndex, int sequentialId, GeoVFullPhysVol *fpv) {
  ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
  MsgStream log(&(*msgh), "GeoModelXml");
  
  log << MSG::DEBUG << "GmxInterface::addSensor called for " << fpv->getLogVol()->getName() << ", type " << name << 
    ". Indices:   ";		
  for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
    log << i->second << "   ";
  }
  log << "\n and additional Indices " << extraIndex.first << " : "<<
    extraIndex.second;
  log << "\nSequential ID = " << sequentialId << endmsg;
}


void GmxInterface::addAlignable(int level, map<std::string, int> &index, GeoVFullPhysVol *fpv, 
                                GeoAlignableTransform */*transform*/) {
//
//    Logvol name is not very useful (usually == AssemblyLV). Get PhysVol name (seems surprisingly awkward way needed)
//
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
}

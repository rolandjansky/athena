/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
//    Implement default GmxInterface, which just prints debug messages.
//    User should over-ride all three methods.
//    Use: create an instance of this, and pass a reference to it in the Gmx2Geo constructor.
//
#include "GeoModelXml/GmxInterface.h"
#include "GeoModelXml/OutputDirector.h"
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
    OUTPUT_STREAM;

    msglog << MSG::DEBUG << "GmxInterface::addSensorType called for class " << clas << "; type " << type << 
                         "\n    Parameter names and values:\n";
    for (map<string, string>::iterator p = params.begin(); p != params.end(); ++p) {
        msglog << "        " << p->first << " = " << p->second << endmsg;
    }
}

void GmxInterface::addSensor(string name, map<string, int> &index, int sequentialId, GeoVFullPhysVol *fpv) {
    OUTPUT_STREAM;

    msglog << MSG::DEBUG << "GmxInterface::addSensor called for " << fpv->getLogVol()->getName() << ", type " << name << 
                         ". Indices:   ";		
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        msglog << i->second << "   ";
    }
    msglog << "\nSequential ID = " << sequentialId << endmsg;
}

void GmxInterface::addSplitSensor(string name, map<string, int> &index, std::pair<std::string, int> &extraIndex, int sequentialId, GeoVFullPhysVol *fpv) {
    OUTPUT_STREAM;

    msglog << MSG::DEBUG << "GmxInterface::addSensor called for " << fpv->getLogVol()->getName() << ", type " << name << 
                         ". Indices:   ";		
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        msglog << i->second << "   ";
    }
    msglog << "\n and additional Indices " << extraIndex.first << " : "<<
      extraIndex.second;
    msglog << "\nSequential ID = " << sequentialId << endmsg;
}

void GmxInterface::addAlignable(int level, map<std::string, int> &index, GeoVFullPhysVol *fpv, 
                                GeoAlignableTransform */*transform*/) {
//
//    Logvol name is not very useful (usually == AssemblyLV). Get PhysVol name (seems surprisingly awkward way needed)
//
    OUTPUT_STREAM;
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
    msglog << MSG::DEBUG << "GmxInterface::addAlignable called for physvol name " << name << ".  Level = " << level << ". Indices:   ";
*/

    msglog << MSG::DEBUG << "GmxInterface::addAlignable called for a physvol. Logvol name " << fpv->getLogVol()->getName() << 
            ". Level = " << level << ". Indices:   ";
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        msglog << i->second << "   ";
    }
    msglog << endmsg;
}

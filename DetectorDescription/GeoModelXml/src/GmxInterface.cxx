/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Implement default GmxInterface, which just prints debug messages.
//    User should over-ride all three methods.
//    Use: create an instance of this, and pass a reference to it in the Gmx2Geo constructor.
//
#include "GeoModelXml/GmxInterface.h"
#include <iostream>
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
    cout << "GmxInterface::addSensorType called for class " << clas << "; type " << type << endl;
    cout << "    Parameter names and values:" << endl;
    for (map<string, string>::iterator p = params.begin(); p != params.end(); ++p) {
        cout << "        " << p->first << " = " << p->second << endl;
    }
}

void GmxInterface::addSensor(string name, map<string, int> &index, int sequentialId, GeoVFullPhysVol *fpv) {

    cout << "GmxInterface::addSensor called for " << fpv->getLogVol()->getName() << ", type " << name << ". Indices:   ";
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        cout << i->second << "   ";
    }
    cout << "\nSequential ID = " << sequentialId << endl;
}

void GmxInterface::addAlignable(int level, map<std::string, int> &index, GeoVFullPhysVol *fpv, 
                                GeoAlignableTransform */*transform*/) {
//
//    Logvol name is not very useful (usually == AssemblyLV). Get PhysVol name (seems surprisingly awkward way needed)
//
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
    cout << "GmxInterface::addAlignable called for physvol name " << name << ".  Level = " << level << ". Indices:   ";
*/

    cout << "GmxInterface::addAlignable called for a physvol. Logvol name " << fpv->getLogVol()->getName() << 
            ". Level = " << level << ". Indices:   ";
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
        cout << i->second << "   ";
    }
    cout << endl;
}

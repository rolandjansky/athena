/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_GEOMODELXML_MODULETREE_H
#define PIXEL_GEOMODELXML_MODULETREE_H
//
//   For "numerology". Create a nested set of maps. Bottom layer of leaves are module information.
//   Order is barrelEndcap/layerDisk/phiModule/etaModule/side; each of these has a map of integer-key (the 
//   value of the integer index) and value -- a map to the next layer down.
//
//   User should only create the top level item, a ModuleTree, and Modules (which are copied: user deletes his own copy).
//   add(bec, layer, phi, eta, side, Module) for each module.
//
//   Then the number of items at a given layer can be found from the nXxxxxs member functions of each sub-layer.
//
#include <map>
#include <string>
#include <sstream>

// Error message build up is spread in many routines; instead of cerr, build up a string. Caller can
// print this string with Athena message service. Make it static to give it internal linkage.
static std::ostringstream errmsg;

class Module {
public:
    Module(int unsigned hashId): m_hashId(hashId) {}
    unsigned int hashId() {return m_hashId;}
    void setHashId(unsigned int hashId) {m_hashId = hashId;}
private:
    unsigned int m_hashId;
};

class Side: public std::map<int, Module> {
public:
    bool add(int side, Module &module); 
    Module & operator[](int side) {return at(side);}
    int nSides() const {return size();}
};

class PhiModule: public std::map<int, Side> {
public:
    bool add(int phi, int side, Module &module); 
    Side & operator[](int phi) {return at(phi);}
    int nPhiModules() const {return size();}
};

class EtaModule: public std::map<int, PhiModule> {
public:
    bool add(int eta, int phi, int side, Module &module); 
    PhiModule & operator[](int eta) {return at(eta);}
    int nEtaModules() const {return size();}
};

class LayerDisk: public std::map<int, EtaModule> {
public:
    bool add(int ld, int eta, int phi, int side, Module &module); 
    EtaModule & operator[](int ld) {return at(ld);}
    int nLayers() const {return size();}
};

class BarrelEndcap: public std::map<int, LayerDisk> {
public:
    bool add(int bec, int ld, int eta, int phi, int side, Module &module, std::string &errorMessage); 
    LayerDisk & operator[](int bec) {return at(bec);}
    int nParts() const {return size();}
};

class ModuleTree: public BarrelEndcap { // Just a more descriptive name for the class.
};

#endif

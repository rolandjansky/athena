/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelXml/ModuleTree.h"

using namespace std;

bool PhiModule::add(int phi, Module &module) {
    if (count(phi) == 0) {
        insert(pair<int, Module>(phi, module));
	return true;
    }
    else {
        errmsg << "ModuleTree ERROR: request to add a second copy of module ignored. Identifier = " <<
	module.hashId() <<"; phi = " << phi;
	return false;
    }
}

bool EtaModule::add(int eta, int phi, Module &module) {
    if (count(eta) == 0) {
        insert(pair<int, PhiModule>(eta, PhiModule()));
    }
    if (!(*this)[eta].add(phi, module)) {
        errmsg << "; eta = " << eta;
        return false;
    }
    return true;
}

bool LayerDisk::add(int ld, int eta, int phi, Module &module) {
    if (count(ld) == 0) {
        insert(pair<int, EtaModule>(ld, EtaModule()));
    }
    if (!(*this)[ld].add(eta, phi, module)) {
        errmsg << "; ld = " << ld;
        return false;
    }
    return true;
}

bool BarrelEndcap::add(int bec, int ld, int eta, int phi, Module &module, string &errorMessage) {
    errorMessage = "";
    errmsg.str("");
    errmsg.str().clear();
    if (count(bec) == 0) {
        insert(pair<int, LayerDisk>(bec, LayerDisk()));
    }
    if (!(*this)[bec].add(ld, eta, phi, module)) {
        errmsg << "; bec = " << bec;
        errorMessage = errmsg.str();
        return false;
    }
    return true;
}

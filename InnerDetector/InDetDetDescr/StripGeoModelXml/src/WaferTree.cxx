/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StripGeoModelXml/WaferTree.h"

using namespace std;

bool Side::add(int side, Wafer &wafer) {

    if (count(side) == 0) {
        insert(pair<int, Wafer>(side, wafer));
        return true;
    }
    else {
        errmsg << "WaferTree ERROR: request to add a second copy of a wafer ignored. Identifier = " << 
                wafer.hashId() << "; side = " << side;
        return false;
    }
}

bool PhiModule::add(int phi, int side, Wafer &wafer) {
    if (count(phi) == 0) {
        insert(pair<int, Side>(phi, Side()));
    }
    if (!(*this)[phi].add(side, wafer)) {
        errmsg << "; phi = " << phi;
        return false;
    }
    return true;
}

bool EtaModule::add(int eta, int phi, int side, Wafer &wafer) {
    if (count(eta) == 0) {
        insert(pair<int, PhiModule>(eta, PhiModule()));
    }
    if (!(*this)[eta].add(phi, side, wafer)) {
        errmsg << "; eta = " << eta;
        return false;
    }
    return true;
}

bool LayerDisk::add(int ld, int eta, int phi, int side, Wafer &wafer) {
    if (count(ld) == 0) {
        insert(pair<int, EtaModule>(ld, EtaModule()));
    }
    if (!(*this)[ld].add(eta, phi, side, wafer)) {
        errmsg << "; ld = " << ld;
        return false;
    }
    return true;
}

bool BarrelEndcap::add(int bec, int ld, int eta, int phi, int side, Wafer &wafer, string &errorMessage) {
    errorMessage = "";
    errmsg.str("");
    errmsg.str().clear();
    if (count(bec) == 0) {
        insert(pair<int, LayerDisk>(bec, LayerDisk()));
    }
    if (!(*this)[bec].add(ld, eta, phi, side, wafer)) {
        errmsg << "; bec = " << bec;
        errorMessage = errmsg.str();
        return false;
    }
    return true;
}

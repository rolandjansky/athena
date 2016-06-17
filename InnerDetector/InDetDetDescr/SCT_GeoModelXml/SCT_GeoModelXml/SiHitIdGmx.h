/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    10 March 2016. TEMPORARY software to be thrown away.
//    Package ATLAS Event depends on package ATLAS Conditions.
//    SCT_GmxInterface is in package ATLAS conditions. 
//    SCT_GmxInterface should use, and therfore depend on, InDetSimEvent/SiHitIdHelper.h. But that 
//    is in a package "ATLAS Event", so you would create a circular dependency.
//
//    As a temporary workaround, we reproduce some of the functionality of SiHitIdHelper here.
//    This is a bad way of doing it: if the field definitions in SiHitIdHelper change, 
//    those changes will have to be propagated to here by hand. 
//
#include <string>
#include <vector>

class SiHitIdGmx {
public:
    SiHitIdGmx(int pixelSCT, int barrelEndcap, int layerWheel, int eta, int phi, int side);
    int id() const;
    void decode(int &pixelSCT, int &barrelEndcap, int &layerWheel, int &eta, int &phi, int &side) const;
    std::string print() const;
private:
    SiHitIdGmx() {}; // Only construct valid identifiers
    int m_id;
    static const int s_nFields;
    static const std::string s_name[];
    static const int s_width[];
    static const int s_offset[];
};

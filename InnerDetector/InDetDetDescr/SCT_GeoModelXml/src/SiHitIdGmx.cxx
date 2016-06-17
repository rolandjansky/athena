/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModelXml/SiHitIdGmx.h"
#include <sstream>

//
//    Initialise static members which describe the bit fields 
//    (these values need to change if the definition changes in SiHitIdHelper)
//
const int SiHitIdGmx::s_nFields = 6;
const std::string SiHitIdGmx::s_name[] = {"PixelSCT", "BarrelEndcap", "LayerDisc", "EtaModule", "PhiModule", "Side"}; 
const int SiHitIdGmx::s_width[] = {1, 3, 5, 8, 8, 2};
const int SiHitIdGmx::s_offset[] = {0, -2, 0, -100, 0, 0};

SiHitIdGmx::SiHitIdGmx(int pixelSCT, int barrelEndcap, int layerWheel, int etaModule, int phiModule, int side) {
    int array[SiHitIdGmx::s_nFields];
    array[0] = pixelSCT;
    array[1] = barrelEndcap;
    array[2] = layerWheel;
    array[3] = etaModule;
    array[4] = phiModule;
    array[5] = side;

    m_id = 0; 
    int shift = 0;
    for (int i = 0; i < s_nFields; ++i) {
        int tmp = (array[i] - s_offset[i]) & ((1 << s_width[i]) - 1);
        m_id |= tmp << shift;
        shift += s_width[i];
    }
}

int SiHitIdGmx::id() const {
    return m_id;
}

void SiHitIdGmx::decode(int &pixelSCT, int &barrelEndcap, int &layerWheel, int &etaModule, int &phiModule, int &side) const {
    int array[SiHitIdGmx::s_nFields];
    int temp = m_id;

    for (int i = 0; i < s_nFields; ++i) {
        array[i] = (temp & ((1 << s_width[i]) - 1)) + s_offset[i];
        temp >>= s_width[i];
    }

    pixelSCT = array[0];
    barrelEndcap = array[1];
    layerWheel = array[2];
    etaModule = array[3];
    phiModule = array[4];
    side = array[5];

    return;
}

std::string SiHitIdGmx::print() const {
    std::ostringstream result;
    int pixelSCT, barrelEndcap, layerWheel, etaModule, phiModule, side;

    decode(pixelSCT, barrelEndcap, layerWheel, etaModule, phiModule, side);

    result << s_name[0] << " = " << pixelSCT << "\n"; 
    result << s_name[1] << " = " << barrelEndcap << "\n"; 
    result << s_name[2] << " = " << layerWheel << "\n"; 
    result << s_name[3] << " = " << etaModule << "\n"; 
    result << s_name[4] << " = " << phiModule << "\n"; 
    result << s_name[5] << " = " << side << "\n"; 

    return result.str();
}

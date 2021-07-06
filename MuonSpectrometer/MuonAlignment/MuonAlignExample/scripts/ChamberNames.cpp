/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ChamberNames.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

//________________________________________________________________________
ChamberNames::ChamberNames(std::string filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Can't open AlignmentConstantsFile" << std::endl;
        return;
    }

    char ALineMarker;
    int jff, jzz, job, ID;
    std::string type;
    double s, z, t, ths, thz, tht;

    while (file >> ALineMarker >> type >> jff >> jzz >> job >> s >> z >> t >> ths >> thz >> tht >> ID) {
        // make chamber name
        std::stringstream chname;
        chname << type << std::abs(jzz);
        if (jzz < 0)
            chname << "C";
        else
            chname << "A";
        int detphi = 2 * jff;
        if (type.substr(2, 1) == "L")
            detphi--;
        else if (type.substr(2, 1) != "S" && type != "BEE")
            detphi = jff;
        chname << std::setfill('0') << std::setw(2) << detphi;

        m_chamberIdMap[ID] = chname.str();
    }
    file.close();
}

//________________________________________________________________________
ChamberNames::~ChamberNames() {}

//________________________________________________________________________
std::string ChamberNames::chamberName(int ID) const {
    std::map<int, std::string>::const_iterator it = m_chamberIdMap.find(ID);
    if (it != m_chamberIdMap.end())
        return it->second;
    else {
        std::cout << "no chamber found for Identifier " << ID << std::endl;
        return "";
    }
}

//________________________________________________________________________
int ChamberNames::stEta(int identifier) const {
    std::string chname = this->chamberName(identifier);
    return stEta(chname);
}

//________________________________________________________________________
int ChamberNames::stEta(const std::string& chname) const {
    int jzz = atoi(chname.substr(3, 1).c_str());
    if (chname.substr(4, 1) == "C") jzz *= -1;
    return jzz;
}

//________________________________________________________________________
int ChamberNames::stPhi(int identifier) const {
    std::string chname = this->chamberName(identifier);
    return stPhi(chname);
}

//________________________________________________________________________
int ChamberNames::stPhi(const std::string& chname) const {
    int detphi = atoi(chname.substr(5, 2).c_str());
    if (chname.substr(2, 1) == "L") detphi++;
    return detphi / 2;
}

//________________________________________________________________________
int ChamberNames::chamberIdFromString(const std::string& chname) const {
    std::map<int, std::string>::const_iterator it = m_chamberIdMap.begin();
    std::map<int, std::string>::const_iterator itEnd = m_chamberIdMap.end();

    for (; it != itEnd; ++it) {
        if (it->second == chname) return it->first;
    }
    return -1;
}

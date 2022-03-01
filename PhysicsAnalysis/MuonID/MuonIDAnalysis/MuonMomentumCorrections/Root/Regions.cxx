/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMomentumCorrections/Regions.h"

#include <TString.h>  // for Form

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

ANA_MSG_SOURCE(msgMMC, "MuonMomentumCorrections")

void Regions::PrintRegions() const {
    if (m_doMacroRegions) {
        for (std::map<int, int>::const_iterator it = m_macroRegionIdxMap.begin(); it != m_macroRegionIdxMap.end(); ++it) {
            int bReg = it->first, mReg = it->second;
            msgMMC::ANA_MSG_INFO("Base region n " << bReg << ": "
                                                  << "phi_min=" << m_phi_min[bReg] << ", phi_max=" << m_phi_max[bReg]
                                                  << ", eta_min=" << m_eta_min[bReg] << ", eta_max=" << m_eta_max[bReg]);
            if (m_loadNames) msgMMC::ANA_MSG_INFO(", name=" << m_names[bReg]);
            msgMMC::ANA_MSG_INFO("included in Macro region N " << mReg << " with innerEta=" << m_macroRegionInnerEta[mReg]);
            if (m_loadNames) msgMMC::ANA_MSG_INFO(", name=" << m_macroRegionName[mReg]);
        }
    } else {
        for (int bReg = 0; bReg < m_nb_regions; ++bReg) {
            msgMMC::ANA_MSG_INFO("Base region n " << bReg << ": phi_min=" << m_phi_min[bReg] << ", phi_max=" << m_phi_max[bReg]
                                                  << ", eta_min=" << m_eta_min[bReg] << ", eta_max=" << m_eta_max[bReg]);
            if (m_loadNames) msgMMC::ANA_MSG_INFO(", name=" << m_names[bReg]);
        }
    }
}

unsigned int Regions::GetNRegions() const {
    if (m_doMacroRegions)
        return m_macroRegionName.size();
    else
        return m_nb_regions;
}

Regions::Regions(const std::string& inRegionFile, int doMacroRegionsFlag) {
    m_eta_min.clear();
    m_eta_max.clear();
    m_phi_min.clear();
    m_phi_max.clear();
    m_loadNames = false;
    m_doMacroRegions = false;
    m_useAbsEta = false;
    std::string tmpname;
    double tmpval;
    int i = 0;

    std::ifstream inFile(inRegionFile.c_str());
    if (!inFile.good()) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRegions::Regions() - File %s NOT FOUND!!", __FILE__, __LINE__, inRegionFile.c_str()));
    } else {
        while (!inFile.eof()) {
            tmpval = 0;
            tmpname = "";
            if (i == 0) {
                getline(inFile, tmpname);
                // Check if I am loading the name of the regions in the file
                if (tmpname.find("name") != std::string::npos && !m_loadNames) m_loadNames = true;
            } else {
                inFile >> tmpval;
                if (inFile.eof()) break;  // this will break immediatly as soon as the new line is empty
                m_eta_min.push_back(tmpval);
                inFile >> tmpval;
                m_eta_max.push_back(tmpval);
                inFile >> tmpval;
                m_phi_min.push_back(tmpval);
                inFile >> tmpval;
                m_phi_max.push_back(tmpval);
                if (m_loadNames) {  // Fill the name vector only if you have them!
                    inFile >> tmpname;
                    m_names.push_back(tmpname);
                }
            }
            ++i;
        }
    }
    inFile.close();
    m_nb_regions = (int)m_eta_min.size();
    if (doMacroRegionsFlag) {  // In case I want macroRegions, I need to define the criteria here
        m_doMacroRegions = true;
        switch (doMacroRegionsFlag) {
            case 1:
                // Collects all the Large and Small sectors if they belong to the same eta bin
                collectMacroRegionsSL();
                break;
            case 2:
                // Large,Small sectors split plus Feet(12+14) and 11+15 sector split in Barrel
                collectMacroRegionsSL_SplitBAR();
                break;
            case 3:
                // Collects all the Large and Small, Up and Down sectors if they belong to the same eta bin
                collectMacroRegionsSL_UpDn();
                break;
            case 4:
                m_useAbsEta = true;
                m_doMacroRegions = false;  // not really using macroRegions
                break;
            case 5:
                useSectors();
                break;
                break;
            default:
                throw std::runtime_error(Form("File: %s, Line: %d\nRegions::Regions() - doMacroRegionFlag=%i not defined!", __FILE__,
                                              __LINE__, doMacroRegionsFlag));
        }
    }
}

int Regions::GetRegion(double eta, const double phi) const {
    int ret_k = -1;
    if (m_useAbsEta) eta = std::abs(eta);
    for (int k = 0; k < m_nb_regions; ++k) {
        if (eta >= m_eta_min[k] && eta < m_eta_max[k]) {
            if (m_phi_min[k] <= m_phi_max[k]) {  // Standard case
                if (phi >= m_phi_min[k] && phi < m_phi_max[k]) {
                    ret_k = k;
                    break;
                }
            } else {  // Circular closure of Phi, min and max are excanged in the .dat file
                if (phi >= m_phi_min[k] || phi < m_phi_max[k]) {
                    ret_k = k;
                    break;
                }
            }
        }
    }

    if (ret_k == -1) {
        msgMMC::ANA_MSG_WARNING("GetRegion() - Region corresponding to Eta=" << eta << ", Phi=" << phi << " NOT FOUND!");
        return -1;
    }
    if (m_doMacroRegions) return m_macroRegionIdxMap.find(ret_k)->second;
    return ret_k;
}

float Regions::GetRegionDeltaEta(const int r_i) const {  // Return Eta range of the region
    if (m_doMacroRegions) {
        if (r_i >= 0 && r_i < (int)m_macroRegionName.size()) return m_macroRegionDeltaEta[r_i];
    } else {
        if (r_i >= 0 && r_i < m_nb_regions) return std::abs(m_eta_max[r_i] - m_eta_min[r_i]);
    }
    msgMMC::ANA_MSG_WARNING("GetRegionDeltaEta() - Region Inner Eta corresponding to Region index=" << r_i << " NOT FOUND!");
    return -999.;
}

float Regions::GetRegionInnerEta(const int r_i) const {  // Return Eta closer to the origin
    if (m_doMacroRegions) {
        if (r_i >= 0 && r_i < (int)m_macroRegionName.size()) return m_macroRegionInnerEta[r_i];
    } else {
        if (r_i >= 0 && r_i < m_nb_regions) {
            if (m_eta_min[r_i] >= 0.)
                return m_eta_min[r_i];
            else
                return m_eta_max[r_i];
        }
    }
    msgMMC::ANA_MSG_WARNING("GetRegionInnerEta() - Region Inner Eta corresponding to Region index=" << r_i << " NOT FOUND!");
    return -999.;
}

std::string Regions::GetRegionName(const int r_i) const {
    if (m_loadNames) {
        if (m_doMacroRegions) {
            if (r_i >= 0 && r_i < (int)m_macroRegionName.size()) return m_macroRegionName[r_i];
        } else {
            if (r_i >= 0 && r_i < m_nb_regions) return m_names[r_i];
        }
        msgMMC::ANA_MSG_WARNING("GetRegionName() - Region Name corresponding to Region index=" << r_i << " NOT FOUND!");
    }
    msgMMC::ANA_MSG_WARNING("GetRegionName() - Region Names not set!");
    return "NAN";
}

std::string Regions::GetRegionName(const double eta, const double phi) const {
    if (m_loadNames) return GetRegionName(GetRegion(eta, phi));
    msgMMC::ANA_MSG_WARNING("GetRegionName() - Region Names not set!");
    return "NAN";
}

// Collects all the Large and Small sectors if they belong to the same eta bin
void Regions::collectMacroRegionsSL() {
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for (int k = 0; k < m_nb_regions; ++k) {
        if (etaMin != m_eta_min[k] || etaMax != m_eta_max[k]) {
            // Build a new macroRegion
            etaMin = m_eta_min[k];
            etaMax = m_eta_max[k];
            deltaEta = std::abs(m_eta_max[k] - m_eta_min[k]);
            // One of the following is true
            std::string macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
            macroRegName = macroRegName.substr(0, m_names[k].find("BL"));
            macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
            macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
            m_macroRegionName.push_back(macroRegName + "Large");
            m_macroRegionName.push_back(macroRegName + "Small");
            // insert twice the innerEta, for Large and for Small sectors
            for (int i = 0; i < 2; ++i) {
                if (etaMin >= 0.)
                    m_macroRegionInnerEta.push_back(etaMin);
                else
                    m_macroRegionInnerEta.push_back(etaMax);
                m_macroRegionDeltaEta.push_back(deltaEta);
            }
            macroRegionIdx += 2;
        }

        if (m_names[k].find("EL") != std::string::npos || m_names[k].find("BL") != std::string::npos)
            m_macroRegionIdxMap[k] = macroRegionIdx - 2;  // Large sectors
        else if (m_names[k].find("ES") != std::string::npos || m_names[k].find("BS") != std::string::npos)
            m_macroRegionIdxMap[k] = macroRegionIdx - 1;  // Small sectors
    }
}

// Collects all the Large and Small sectors if they belong to the same eta bin
void Regions::collectMacroRegionsSL_UpDn() {
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for (int k = 0; k < m_nb_regions; ++k) {
        if (etaMin != m_eta_min[k] || etaMax != m_eta_max[k]) {
            // Build a new macroRegion
            etaMin = m_eta_min[k];
            etaMax = m_eta_max[k];
            deltaEta = std::abs(m_eta_max[k] - m_eta_min[k]);
            // One of the following is true
            std::string macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
            macroRegName = macroRegName.substr(0, m_names[k].find("BL"));
            macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
            macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
            m_macroRegionName.push_back(macroRegName + "LargeDn");
            m_macroRegionName.push_back(macroRegName + "SmallDn");
            m_macroRegionName.push_back(macroRegName + "LargeUp");
            m_macroRegionName.push_back(macroRegName + "SmallUp");
            // insert 4 time the innerEta, for Large and Small sectors times Up and Dn
            for (int i = 0; i < 4; ++i) {
                if (etaMin >= 0.)
                    m_macroRegionInnerEta.push_back(etaMin);
                else
                    m_macroRegionInnerEta.push_back(etaMax);
                m_macroRegionDeltaEta.push_back(deltaEta);
            }
            macroRegionIdx += 4;
        }
        if (m_names[k].find("EL") != std::string::npos || m_names[k].find("BL") != std::string::npos) {  // Large sectors
            if (m_names[k].find("11") != std::string::npos || m_names[k].find("13") != std::string::npos ||
                m_names[k].find("15") != std::string::npos) {
                m_macroRegionIdxMap[k] = macroRegionIdx - 4;  // Down Large sectors (within 10 to 16)
            } else {
                m_macroRegionIdxMap[k] = macroRegionIdx - 2;  // Up, large sectors
            }
        } else if (m_names[k].find("ES") != std::string::npos || m_names[k].find("BS") != std::string::npos) {  // Small sectors
            if (m_names[k].find("10") != std::string::npos || m_names[k].find("12") != std::string::npos ||
                m_names[k].find("14") != std::string::npos || m_names[k].find("16") != std::string::npos) {
                m_macroRegionIdxMap[k] = macroRegionIdx - 3;  // Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
            } else {
                m_macroRegionIdxMap[k] = macroRegionIdx - 1;
                ;  // Up, Small sectors
            }
        }
    }
}

void Regions::useSectors() {
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for (int k = 0; k < m_nb_regions; ++k) {
        if (etaMin != m_eta_min[k] || etaMax != m_eta_max[k]) {
            // Build a new macroRegion
            etaMin = m_eta_min[k];
            etaMax = m_eta_max[k];
            deltaEta = std::abs(m_eta_max[k] - m_eta_min[k]);
            // One of the following is true
            std::string macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
            macroRegName = macroRegName.substr(0, m_names[k].find("BL"));
            macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
            macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
            m_macroRegionName.push_back(macroRegName + "LargeDn");
            m_macroRegionName.push_back(macroRegName + "SmallDn");
            m_macroRegionName.push_back(macroRegName + "LargeUp");
            m_macroRegionName.push_back(macroRegName + "SmallUp");
            // insert 4 time the innerEta, for Large and Small sectors times Up and Dn
            for (int i = 0; i < 4; ++i) {
                if (etaMin >= 0.)
                    m_macroRegionInnerEta.push_back(etaMin);
                else
                    m_macroRegionInnerEta.push_back(etaMax);
                m_macroRegionDeltaEta.push_back(deltaEta);
            }
            macroRegionIdx += 4;
        }
        if (m_names[k].find("EL") != std::string::npos || m_names[k].find("BL") != std::string::npos) {  // Large sectors
            if (m_names[k].find("11") != std::string::npos || m_names[k].find("13") != std::string::npos ||
                m_names[k].find("15") != std::string::npos) {
                m_macroRegionIdxMap[k] = macroRegionIdx - 4;  // Down Large sectors (within 10 to 16)
            } else {
                m_macroRegionIdxMap[k] = macroRegionIdx - 2;  // Up, large sectors
            }
        } else if (m_names[k].find("ES") != std::string::npos || m_names[k].find("BS") != std::string::npos) {  // Small sectors
            if (m_names[k].find("10") != std::string::npos || m_names[k].find("12") != std::string::npos ||
                m_names[k].find("14") != std::string::npos || m_names[k].find("16") != std::string::npos) {
                m_macroRegionIdxMap[k] = macroRegionIdx - 3;  // Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
            } else {
                m_macroRegionIdxMap[k] = macroRegionIdx - 1;
                ;  // Up, Small sectors
            }
        }
    }
}

// Collects all the Large and Small sectors if they belong to the same eta bin and splits
// the barrel 12,14 smalls and 11+15 larges that may need different calibrations
void Regions::collectMacroRegionsSL_SplitBAR() {
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for (int k = 0; k < m_nb_regions; ++k) {
        if (etaMin != m_eta_min[k] || etaMax != m_eta_max[k]) {  // Build a new macroRegion
            etaMin = m_eta_min[k];
            etaMax = m_eta_max[k];
            deltaEta = std::abs(m_eta_max[k] - m_eta_min[k]);
            std::string macroRegName = "";
            if (m_names[k].find("EL") != std::string::npos || m_names[k].find("ES") != std::string::npos) {
                // We are in the End-Cap sectors so we split only Large and Small sectors
                macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
                macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
                // insert 4 time the innerEta, for Large and Small sectors
                m_macroRegionName.push_back(macroRegName + "Large");
                m_macroRegionName.push_back(macroRegName + "Small");
                for (int i = 0; i < 2; ++i) {
                    if (etaMin >= 0.)
                        m_macroRegionInnerEta.push_back(etaMin);
                    else
                        m_macroRegionInnerEta.push_back(etaMax);
                    m_macroRegionDeltaEta.push_back(deltaEta);
                }
                macroRegionIdx += 2;
            } else {  // We are in the Barrels sectors so we split Large, Small, 12+14, 11+15 sectors
                macroRegName = m_names[k].substr(0, m_names[k].find("BL"));
                macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
                // insert 4 time the innerEta, for Large and Small sectors times Up and Dn
                m_macroRegionName.push_back(macroRegName + "Large");
                m_macroRegionName.push_back(macroRegName + "Small");
                m_macroRegionName.push_back(macroRegName + "Large11_15");
                m_macroRegionName.push_back(macroRegName + "SmallFeet");
                for (int i = 0; i < 4; ++i) {
                    if (etaMin >= 0.)
                        m_macroRegionInnerEta.push_back(etaMin);
                    else
                        m_macroRegionInnerEta.push_back(etaMax);
                    m_macroRegionDeltaEta.push_back(deltaEta);
                }
                macroRegionIdx += 4;
            }
        }
        if (m_names[k].find("EL") != std::string::npos) {  // End-Cap Large sectors
            m_macroRegionIdxMap[k] = macroRegionIdx - 2;
        } else if (m_names[k].find("ES") != std::string::npos) {  // End-Cap Small sectors
            m_macroRegionIdxMap[k] = macroRegionIdx - 1;          //
        } else if (m_names[k].find("BL") != std::string::npos) {  // Barrel Large sectors
            if (m_names[k].find("11") != std::string::npos || m_names[k].find("15") != std::string::npos) {
                m_macroRegionIdxMap[k] = macroRegionIdx - 2;  // Barrel Large sectors with different alignment (11,15)
            } else {
                m_macroRegionIdxMap[k] = macroRegionIdx - 4;  // Standard Barrel Large sectors
            }
        } else if (m_names[k].find("BS") != std::string::npos) {  // Barrel Small sectors
            if (m_names[k].find("12") != std::string::npos || m_names[k].find("14") != std::string::npos) {
                m_macroRegionIdxMap[k] = macroRegionIdx - 1;  // Feet Small sectors (12+14, Down).
            } else {
                m_macroRegionIdxMap[k] = macroRegionIdx - 3;  // All standard Barrel Small sectors
            }
        }
    }
}

#include <math.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include "RPCpanelList.h"

void RPCpanelList::init() { return init(0); }
void RPCpanelList::init(Int_t inputLayer) {
    std::cout << "RPCpanelList::init" << std::endl;

    std::string STRING;
    stringstream ss;
    ss << inputLayer;
    std::string laySTRING = ss.str();
    if (inputLayer > 0 && inputLayer < 7) std::cout << " Will use only lines with Layer = " << laySTRING << std::endl;
    ss.clear();
    ifstream infile(
        "/afs/le.infn.it/user/s/spagnolo/atlas/rpcPerf/code/MuonSpectrometer/MuonGeoModelTest/share/rpc_current_panelID_R.07.01");
    int nPanels = 0;
    float etamin, etamax, phimin, phimax, zmin, zmax;
    std::string stName;
    std::string viewName;
    std::string stringId;
    int layerRPC, msIndex, stNameIndex, stEtaIndex, stPhiIndex, techIndex, dbrIndex, dbzIndex, dbpIndex, ggIndex, measphiIndex, stripIndex;
    unsigned long panelID;
    if (infile.is_open())  // if the file is open
    {
        while (!infile.eof())  // To get you all the lines.
        {
            ss.clear();
            getline(infile, STRING);  // Saves the line in STRING.
            // std::cout<<" now reading <"<<STRING<<">"<<std::endl; // Prints our STRING.
            if (STRING.substr(0, 1) != laySTRING) {
                // std::cout<<"not a layer 1 panel; skipping"<<std::endl;
                continue;  // skip comments
            } else {
                ++nPanels;
                // STRING = "1 BML phi [7.2.-7.1.2.1.1.1.1.1.56] 1644724956 -1.0666 -1.0289 -0.1988 -0.0013";
                // STRING = "1 BML phi [7.2.-7.1.2.1.1.1.1.1.56] 1644724956";
                ss << STRING;
                // ss>>layerRPC>>stName>>viewName>>stringId;
                std::cout << " STRING = <" << STRING << ">" << std::endl;

                ss >> layerRPC >> stName >> viewName >> stringId >> panelID >> etamin >> etamax >> phimin >> phimax >> zmin >> zmax;

                if (nPanels < 10)
                    std::cout << " Variables are <" << layerRPC << ",  " << stName << ", " << viewName << ", " << stringId << ", "
                              << panelID << " eta,phi range [" << etamin << ", " << etamax << "][" << phimin << ", " << phimax << "]"
                              << std::endl;

                unsigned int index = nPanels - 1;
                m_entryInListMap->insert(std::pair<unsigned long, unsigned int>(panelID, index));
                m_stringIdArray[index] = stringId;
                m_stNameArray[index] = stName;
                if (viewName == "phi")
                    m_viewArray[index] = 1;
                else
                    m_viewArray[index] = 0;
                m_layerArray[index] = layerRPC;
                m_boundArrayEtaMin[index] = etamin;
                m_boundArrayEtaMax[index] = etamax;
                m_boundArrayPhiMin[index] = phimin;
                m_boundArrayPhiMax[index] = phimax;
                m_boundArrayZMin[index] = zmin;
                m_boundArrayZMax[index] = zmax;

                // t->DrawBox(etamin,phimin,etamax,phimax);
                // DrawRpcPanel(cLayer,etamin,etamax,phimin,phimax);
            }
            //	    if (nPanels>1) break;
        }

    } else {
        std::cout << "a problem with input file" << std::endl;
    }
    std::cout << "Finished; n. of panels found is " << nPanels << std::endl;
}
unsigned int RPCpanelList::getIndexInList(unsigned int panelID) {
    if (panelID == panelIDinCache) return indexInCache;
    if (m_entryInListMap->find(panelID) != m_entryInListMap->end()) {
        // found
        panelIDinCache = panelID;
        int indexRequested = (m_entryInListMap->find(panelID))->second();
        indexInCache = indexRequested;
        return indexRequested;
    } else {
        // not found
        std::cout << "panelID " << panelID << " not found in the map" << std::endl;
        return 999999;
    }
}
std::string RPCpanelList::getStringId(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_stringIdArray[ind];
    else
        return "NULL";
}
std::string RPCpanelList::getStName(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_stNameArray[ind];
    else
        return "NULL";
}
short int RPCpanelList::getLayer(unsigned int panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_layerArray[ind];
    else
        return 0;
}
short int RPCpanelList::getView(unsigned int panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_viewArray[ind];
    else
        return 0;
}
double RPCpanelList::getEtaMin(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_boundArrayEtaMin[ind];
    else
        return 0.;
}
double RPCpanelList::getEtaMax(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_boundArrayEtaMax[ind];
    else
        return 0.;
}
double RPCpanelList::getPhiMin(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_boundArrayPhiMin[ind];
    else
        return 0.;
}
double RPCpanelList::getPhiMax(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_boundArrayPhiMax[ind];
    else
        return 0.;
}
double RPCpanelList::getZMin(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_boundArrayZMin[ind];
    else
        return 0.;
}
double RPCpanelList::getZMax(unsigned long panelID) {
    unsigned int ind = getIndexInList(panelID);
    if (ind < arraySize)
        return m_boundArrayZMax[ind];
    else
        return 0.;
}

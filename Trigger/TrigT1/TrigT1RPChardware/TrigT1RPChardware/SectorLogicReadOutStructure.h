/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_SectorLogicReadOutStructure_H
#define TrigT1RPChardware_SectorLogicReadOutStructure_H

#include "TrigT1RPChardware/Lvl1Def.h"
#include "TrigT1RPChardware/RPCReadOut.h"

class SectorLogicReadOutStructure : public RPCReadOut {
public:
    SectorLogicReadOutStructure();
    SectorLogicReadOutStructure(ubit16 inputData);
    ~SectorLogicReadOutStructure() = default;

    void setInit();
    //
    ubit16 makeHeader(ubit16 *inputData);
    ubit16 makeSubHeader() { return 0; };
    ubit16 makeBody(ubit16 *inputData) { return (*inputData) & 0x0; };
    ubit16 makeFooter(ubit16 errorCode) { return errorCode & 0x0; };
    ubit16 makeFooter(ubit16 *errorCode);

    //**************************************************//
    // general user methods to decode ReadOut stream    //
    //**************************************************//
    ubit16 decodeFragment(ubit16 inputWord, char &field);
    //
    bool isHeader();
    bool isBody();
    bool isFooter();
    //
private:
    //
    // header
    ubit16 m_secid{0};
    // footer
    ubit16 m_footer{0};
    //
    // internal objects;
    //
    //
    // some static parameter describing the structure
    //

    static constexpr ubit16 s_headerNum = 2;
    static constexpr std::array<ubit16, s_headerNum> s_headerPos{12, 0};
    static constexpr std::array<ubit16, s_headerNum> s_headerLen{4, 12};
    static constexpr ubit16 s_headerVal = 0x000d;

    static constexpr ubit16 s_footerNum = 2;
    static constexpr std::array<ubit16, s_footerNum> s_footerPos{12, 0};
    static constexpr std::array<ubit16, s_footerNum> s_footerLen{4, 12};
    static constexpr ubit16 s_footerVal = 0x000f;
    //
};

#endif

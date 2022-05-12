/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPChardware/SectorLogicReadOutStructure.h"

#include <fstream>
#include <iostream>
#include <string>

//----------------------------------------------------------------------------//
SectorLogicReadOutStructure::SectorLogicReadOutStructure() {
    //
    // Constructor used by the simulation program
    //
    setInit();
}
//----------------------------------------------------------------------------//
SectorLogicReadOutStructure::SectorLogicReadOutStructure(ubit16 inputData) {
    //
    // Constructor used by the simulation program
    //
    decodeFragment(inputData, m_field);
}
//----------------------------------------------------------------------------//
void SectorLogicReadOutStructure::setInit() { m_field = 'U'; }
//----------------------------------------------------------------------------//

ubit16 SectorLogicReadOutStructure::makeHeader(ubit16 *inputData) {
    m_word = set16Bits(s_headerNum, s_headerPos.data(), inputData);  // theHeader);
    return m_word;
}
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOutStructure::makeFooter(ubit16 *inputData) {
    m_word = set16Bits(s_footerNum, s_footerPos.data(), inputData);
    return m_word;
}
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOutStructure::decodeFragment(ubit16 inputWord, char &field) {
    // bool last=false;
    // Remove errorCode because it is not used
    // ubit16 errorCode=0;
    setInit();
    m_word = inputWord;
    // errorCode=0;
    if (isHeader()) {
        m_field = 'H';
        m_secid = get16Bits(inputWord, s_headerPos[1], s_headerLen[1]);
    } else if (isFooter()) {
        m_field = 'F';
        m_footer = get16Bits(inputWord, s_footerPos[1], s_footerLen[1]);
    } else {
        m_field = 'B';
    }
    field = m_field;
    return 0;  // temporary: force return to 0
}

//----------------------------------------------------------------------------//
bool SectorLogicReadOutStructure::isBody() {
    ubit16 theword = (m_word & 0xf000) >> 12;
    if ((theword != s_headerVal) && (theword != s_footerVal)) return true;
    return false;
}
//----------------------------------------------------------------------------//
bool SectorLogicReadOutStructure::isHeader() {
    bool status = false;
    ubit16 theHeader[s_headerNum] = {s_headerVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_headerPos.data(), theHeader)) status = true;
    return status;
}

//-----------------------------------------------------------------------//
bool SectorLogicReadOutStructure::isFooter() {
    bool status = false;
    ubit16 theFooter[s_footerNum] = {s_footerVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_footerPos.data(), theFooter)) status = true;
    return status;
}

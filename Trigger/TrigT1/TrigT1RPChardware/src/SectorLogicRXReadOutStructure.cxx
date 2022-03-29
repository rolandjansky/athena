/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPChardware/SectorLogicRXReadOutStructure.h"

#include <fstream>
#include <iostream>
#include <string>

//----------------------------------------------------------------------------//
SectorLogicRXReadOutStructure::SectorLogicRXReadOutStructure() {
    //
    // Constructor used by the simulation program
    //
    setInit();
}
//----------------------------------------------------------------------------//
SectorLogicRXReadOutStructure::SectorLogicRXReadOutStructure(ubit16 inputData) {
    //
    // Constructor used by the simulation program
    //
    decodeFragment(inputData, m_field);
}
//----------------------------------------------------------------------------//
void SectorLogicRXReadOutStructure::setInit() { m_field = 'U'; }
//----------------------------------------------------------------------------//

ubit16 SectorLogicRXReadOutStructure::makeHeader(ubit16 *inputData) {
    m_word = set16Bits(s_headerNum, s_headerPos.data(), inputData);  // theHeader);
    return m_word;
}
//----------------------------------------------------------------------------//
ubit16 SectorLogicRXReadOutStructure::makeFooter(ubit16 *inputData) {
    m_word = set16Bits(s_footerNum, s_footerPos.data(), inputData);
    return m_word;
}
//----------------------------------------------------------------------------//
ubit16 SectorLogicRXReadOutStructure::decodeFragment(ubit16 inputWord, char &field) {
    //  bool last=false;
    //  Remove errorCode because it is not used
    //  ubit16 errorCode=0;
    setInit();
    m_word = inputWord;
    //  errorCode=0;
    if (isHeader()) {
        m_field = 'H';
        m_fel1id = get16Bits(inputWord, s_headerPos[2], s_headerLen[2]);
        m_slid = get16Bits(inputWord, s_headerPos[1], s_headerLen[1]);
    } else if (isFooter()) {
        m_field = 'F';
        m_status = get16Bits(inputWord, s_footerPos[1], s_footerLen[1]);
        m_crc = get16Bits(inputWord, s_footerPos[2], s_footerLen[2]);
    } else if (isSubHeader()) {
        m_field = 'S';
        m_slbcid = get16Bits(inputWord, s_subHeaderPos[1], s_subHeaderLen[1]);
    } else if (isInputHeader()) {
        m_field = 'I';
        m_inputPadId = get16Bits(inputWord, s_inputHeaderPos[2], s_inputHeaderLen[2]);
        m_inputRowinBcid = get16Bits(inputWord, s_inputHeaderPos[1], s_inputHeaderLen[1]);
    } else if (isInputBody()) {
        m_field = 'B';
        m_inputRoi = get16Bits(inputWord, s_inputBodyPos[6], s_inputBodyLen[6]);
        m_inputThreshold = get16Bits(inputWord, s_inputBodyPos[5], s_inputBodyLen[5]);
        m_inputOuterPlane = get16Bits(inputWord, s_inputBodyPos[4], s_inputBodyLen[4]);
        m_inputOverlapPhi = get16Bits(inputWord, s_inputBodyPos[3], s_inputBodyLen[3]);
        m_inputOverlapEta = get16Bits(inputWord, s_inputBodyPos[2], s_inputBodyLen[2]);
        m_inputTriggerBcid = get16Bits(inputWord, s_inputBodyPos[1], s_inputBodyLen[1]);

    } else if (isOutputHeader()) {
        m_field = 'O';
        m_outputRowinBcid = get16Bits(inputWord, s_outputHeaderPos[1], s_outputHeaderLen[1]);
        ubit16 outTrigger = get16Bits(inputWord, s_outputHeaderPos[2], s_outputHeaderLen[2]);
        m_outTriggerWord.clear();
        m_outTriggerWord.push_back(outTrigger);
    } else if (isOutputBody()) {
        m_field = 'M';
        ubit16 outTrigger = get16Bits(inputWord, s_outputBodyPos[1], s_outputBodyLen[1]);
        m_outTriggerWord.push_back(outTrigger);
        if (m_outTriggerWord.size() == 3) {
            uint32_t muctpiWord =
                ((m_outTriggerWord[0] & 0x00ff) << 24) | ((m_outTriggerWord[1] & 0x0fff) << 12) | ((m_outTriggerWord[2] & 0x0fff));

            m_moreThan2TriggerCand = (muctpiWord & 0x1);
            m_outputTriggerBcid = (muctpiWord >> 27) & 0x7;
            m_nTriggerCand = 0;
            int iTriggerCand = 0;
            if (((muctpiWord >> 19) & 0x7) != 0x7) {
                m_nTriggerCand++;
                iTriggerCand = m_nTriggerCand - 1;
                m_outputThreshold[iTriggerCand] = (muctpiWord >> 19) & 0x7;
                m_outputRoi[iTriggerCand] = (muctpiWord >> 1) & 0x1f;
                m_outputOverlap[iTriggerCand] = (muctpiWord >> 8) & 0x3;
            }
            if (((muctpiWord >> 22) & 0x7) != 0x7) {
                m_nTriggerCand++;
                iTriggerCand = m_nTriggerCand - 1;
                m_outputThreshold[iTriggerCand] = (muctpiWord >> 22) & 0x7;
                m_outputRoi[iTriggerCand] = (muctpiWord >> 10) & 0x1f;
                m_outputOverlap[iTriggerCand] = (muctpiWord >> 17) & 0x3;
            }
        }
    }

    field = m_field;
    return 0;  // temporary: force return to 0
}

//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isHeader() {
    bool status = false;
    ubit16 theHeader[s_headerNum] = {s_headerVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_headerPos.data(), theHeader)) status = true;
    return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isSubHeader() {
    bool status = false;
    ubit16 theSubHeader[s_subHeaderNum] = {s_subHeaderVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_subHeaderPos.data(), theSubHeader)) status = true;
    return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isInputHeader() {
    bool status = false;
    ubit16 theInputHeader[s_inputHeaderNum] = {s_inputHeaderVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_inputHeaderPos.data(), theInputHeader)) status = true;
    return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isOutputHeader() {
    bool status = false;
    ubit16 theOutputHeader[s_outputHeaderNum] = {s_outputHeaderVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_outputHeaderPos.data(), theOutputHeader)) status = true;
    return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isInputBody() {
    bool status = false;
    ubit16 theInputBody[s_inputBodyNum] = {s_inputBodyVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_inputBodyPos.data(), theInputBody)) status = true;
    return status;
}

//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isOutputBody() {
    bool status = false;
    ubit16 theOutputBody[s_outputBodyNum] = {s_outputBodyVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_outputBodyPos.data(), theOutputBody)) status = true;
    return status;
}

//-----------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isFooter() {
    bool status = false;
    ubit16 theFooter[s_footerNum] = {s_footerVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_footerPos.data(), theFooter)) status = true;
    return status;
}

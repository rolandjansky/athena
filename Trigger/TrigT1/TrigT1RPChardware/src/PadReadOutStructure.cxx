/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPChardware/PadReadOutStructure.h"

#include <fstream>
#include <iostream>
#include <string>

//  header structure
const ubit16 PadReadOutStructure::s_headerPos[s_headerNum] = {12, 9, 0};
// const ubit16 PadReadOutStructure::s_headerPos[s_headerNum] ={ 12, 8, 0};
const ubit16 PadReadOutStructure::s_headerLen[s_headerNum] = {4, 3, 9};
// const ubit16 PadReadOutStructure::s_headerLen[s_headerNum] ={  4, 4, 8};
const ubit16 PadReadOutStructure::s_headerVal = 0x0005;
//  subheader structure
const ubit16 PadReadOutStructure::s_subHeaderPos[s_subHeaderNum] = {12, 0};
const ubit16 PadReadOutStructure::s_subHeaderLen[s_subHeaderNum] = {4, 12};
const ubit16 PadReadOutStructure::s_subHeaderVal = 0x0006;
//  prefooter structure
const ubit16 PadReadOutStructure::s_prefooterPos[s_prefooterNum] = {12, 3, 2, 1, 0};
const ubit16 PadReadOutStructure::s_prefooterLen[s_prefooterNum] = {4, 1, 1, 1, 1};
const ubit16 PadReadOutStructure::s_prefooterVal = 0x000a;
//  footer structure
const ubit16 PadReadOutStructure::s_footerPos[s_footerNum] = {12, 11, 10, 9, 8, 0};
// const ubit16 PadReadOutStructure::s_footerPos[s_footerNum]={ 12,0};
const ubit16 PadReadOutStructure::s_footerLen[s_footerNum] = {4, 1, 1, 1, 1, 8};
// const ubit16 PadReadOutStructure::s_footerLen[s_footerNum]={  4,12};
const ubit16 PadReadOutStructure::s_footerVal = 0x0007;

//----------------------------------------------------------------------------//
PadReadOutStructure::PadReadOutStructure() {
    //
    // Constructor used by the simulation program
    //
    m_padid = 0xffff;
    m_l1id = 0xffff;
    m_bcid = 0xffff;
    m_fifoCM = 0xffff;
    m_fifoOR = 0xffff;
    m_fifoL1 = 0xffff;
    m_fifoPAD = 0xffff;
    m_status = 0xffff;
    m_errorSH = 0xffff;
    m_errorL1 = 0xffff;
    m_errorH = 0xffff;
    m_errorCM = 0xffff;
    m_errorCMID = 0xffff;
    m_errorCode = 0xffff;
    m_vectorStruct = 0;
    setInit();
}
//----------------------------------------------------------------------------//
PadReadOutStructure::PadReadOutStructure(ubit16 inputData) {
    //
    // Constructor used by the simulation program
    //
    m_vectorStruct = 0;
    decodeFragment(inputData, m_field);
}
//----------------------------------------------------------------------------//

void PadReadOutStructure::setInit() {
    m_field = 'U';
    //  m_word      =0xffff;
    // m_padid     =0xffff;
    //  m_status    =0xffff;
    // m_errorCode =0xffff;
}

//----------------------------------------------------------------------------//

ubit16 PadReadOutStructure::makeHeader(ubit16 *inputData) {
    //  m_status = *(inputData+2);
    m_padid = *(inputData + 1);
    m_l1id = *(inputData + 2);
    //  ubit16 theHeader[s_headerNum] = *inputData;
    //={s_headerVal,*(inputData+1),*(inputData+2)};
    m_vectorStruct = inputData;
    *(m_vectorStruct) = s_headerVal;
    m_word = set16Bits(s_headerNum, s_headerPos, m_vectorStruct);  // theHeader);
    return m_word;
}
//----------------------------------------------------------------------------//
/*
ubit16 PadReadOutStructure::makeFooter_new(ubit16* inputData) {
  //  m_errorCode= *inputData;
  m_errorSH =  *(inputData+1);
  m_errorL1 =  *(inputData+2);
  m_errorH  =  *(inputData+3);
  m_errorCM =  *(inputData+4);
  m_errorCMID =*(inputData+5);
  m_vectorStruct = inputData;
  *(m_vectorStruct)= s_footerVal;
  //  ubit16 theFooter[s_footerNum]={s_footerVal};
  //for(int i=1;i<5;i++) *(theFooter+i) = *(inputData+i);
  m_word =  set16Bits(s_footerNum,s_footerPos,m_vectorStruct);//theFooter);
  return m_word;
}
*/
//----------------------------------------------------------------------------//
ubit16 PadReadOutStructure::makeFooter(ubit16 *inputData) {
    //  m_errorCode= *inputData;
    m_errorSH = *(inputData + 1);
    m_errorL1 = *(inputData + 2);
    m_errorH = *(inputData + 3);
    m_errorCM = *(inputData + 4);
    m_errorCMID = *(inputData + 5);
    m_vectorStruct = inputData;
    *(m_vectorStruct) = s_footerVal;
    //  ubit16 theFooter[s_footerNum]={s_footerVal};
    // for(int i=1;i<5;i++) *(theFooter+i) = *(inputData+i);
    m_word = set16Bits(s_footerNum, s_footerPos, m_vectorStruct);  // theFooter);
    return m_word;
}
//----------------------------------------------------------------------------//
ubit16 PadReadOutStructure::decodeFragment(ubit16 inputWord, char &field) {
    //  Removed last because it is not used
    //  bool last=false;
    ubit16 errorCode = 0;
    setInit();
    m_word = inputWord;
    errorCode = 0;
    if (isHeader()) {
        m_field = 'H';
        m_padid = get16Bits(inputWord, s_headerPos[1], s_headerLen[1]);
        m_l1id = get16Bits(inputWord, s_headerPos[2], s_headerLen[2]);
    } else if (isSubHeader()) {
        m_field = 'S';
        m_bcid = get16Bits(inputWord, s_subHeaderPos[1], s_subHeaderLen[1]);
    } else if (isPreFooter()) {
        m_field = 'P';
        m_fifoCM = get16Bits(inputWord, s_prefooterPos[1], s_prefooterLen[1]);
        m_fifoOR = get16Bits(inputWord, s_prefooterPos[2], s_prefooterLen[2]);
        m_fifoL1 = get16Bits(inputWord, s_prefooterPos[3], s_prefooterLen[3]);
        m_fifoPAD = get16Bits(inputWord, s_prefooterPos[4], s_prefooterLen[4]);
        m_status = inputWord & 0x0fff;
    } else if (isFooter()) {
        m_field = 'F';
        m_errorSH = get16Bits(inputWord, s_footerPos[1], s_footerLen[1]);
        m_errorL1 = get16Bits(inputWord, s_footerPos[2], s_footerLen[2]);
        m_errorH = get16Bits(inputWord, s_footerPos[3], s_footerLen[3]);
        m_errorCM = get16Bits(inputWord, s_footerPos[4], s_footerLen[4]);
        m_errorCMID = get16Bits(inputWord, s_footerPos[5], s_footerLen[5]);
        m_errorCode = inputWord & 0x0fff;
    } else {
        m_field = 'B';
        if ((m_word & 0xf000) == (m_MRS.getFooterVal() << 12)) {
            /* last=true; */
        } else {
            /* last=false; */
        }
    }
    field = m_field;
    return errorCode;  // temporary: force return to 0
}

//----------------------------------------------------------------------------//
bool PadReadOutStructure::isBody() {
    ubit16 theword = (m_word & 0xf000) >> 12;
    if ((theword != s_headerVal) && (theword != s_subHeaderVal) && (theword != s_prefooterVal) && (theword != s_footerVal)) return true;
    return false;
}
//----------------------------------------------------------------------------//
bool PadReadOutStructure::isHeader() {
    bool status = false;
    ubit16 theHeader[s_headerNum] = {s_headerVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_headerPos, theHeader)) status = true;
    return status;
}

//-----------------------------------------------------------------------//
bool PadReadOutStructure::isSubHeader() {
    bool status = false;
    ubit16 theSubHeader[s_subHeaderNum] = {s_subHeaderVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_subHeaderPos, theSubHeader)) status = true;
    return status;
}

//-----------------------------------------------------------------------//
bool PadReadOutStructure::isPreFooter() {
    bool status = false;
    ubit16 thepreFooter[s_prefooterNum] = {s_prefooterVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_prefooterPos, thepreFooter)) status = true;
    return status;
}

//-----------------------------------------------------------------------//
bool PadReadOutStructure::isFooter() {
    bool status = false;
    ubit16 theFooter[s_footerNum] = {s_footerVal};
    if ((m_word & s_last4bitsON) == set16Bits(1, s_footerPos, theFooter)) status = true;
    return status;
}

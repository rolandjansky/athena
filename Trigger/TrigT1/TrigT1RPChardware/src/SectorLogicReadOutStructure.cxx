/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<string>
#include "TrigT1RPChardware/SectorLogicReadOutStructure.h"

//  header structure
const ubit16 SectorLogicReadOutStructure::s_headerPos[s_headerNum] ={ 12, 0};
const ubit16 SectorLogicReadOutStructure::s_headerLen[s_headerNum] ={  4,12};
const ubit16 SectorLogicReadOutStructure::s_headerVal=0x000d;  
//  footer structure
const ubit16 SectorLogicReadOutStructure::s_footerPos[s_footerNum]={ 12, 0};
const ubit16 SectorLogicReadOutStructure::s_footerLen[s_footerNum]={  4,12};
const ubit16 SectorLogicReadOutStructure::s_footerVal=0x000f;

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
  decodeFragment(inputData,m_field);
}
//----------------------------------------------------------------------------//
void SectorLogicReadOutStructure::setInit() {
  m_field     = 'U';
}
//----------------------------------------------------------------------------//

ubit16 SectorLogicReadOutStructure::makeHeader(ubit16 *inputData) {
  m_word   = set16Bits(s_headerNum, s_headerPos, inputData);//theHeader);
  return m_word;
}
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOutStructure::makeFooter(ubit16 *inputData) {
  m_word =  set16Bits(s_footerNum,s_footerPos,inputData);
  return m_word;
}
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOutStructure::decodeFragment(ubit16 inputWord, char &field) {
  //bool last=false;
  // Remove errorCode because it is not used
  // ubit16 errorCode=0;
  setInit();
  m_word = inputWord;
  // errorCode=0;
  if(isHeader()) {
    m_field = 'H';
    m_secid     = get16Bits(inputWord,s_headerPos[1],s_headerLen[1]);
  } else  if(isFooter()) {
    m_field = 'F';
    m_footer = get16Bits(inputWord,s_footerPos[1],s_footerLen[1]);
 } else {
    m_field = 'B';
  }
  field = m_field;
  return 0; // temporary: force return to 0
}

//----------------------------------------------------------------------------//
bool SectorLogicReadOutStructure::isBody()
{
  ubit16 theword = (m_word&0xf000)>>12;
  if( (theword != s_headerVal) && 
      (theword != s_footerVal) ) return true;
  return false;
}
//----------------------------------------------------------------------------//
bool SectorLogicReadOutStructure::isHeader()
{
  bool status= false;
  ubit16 theHeader[s_headerNum]={s_headerVal};
  if( (m_word&s_last4bitsON)== set16Bits(1,s_headerPos,theHeader)) status=true;
  return status;
}

//-----------------------------------------------------------------------//
bool SectorLogicReadOutStructure::isFooter()
{
  bool status= false;
  ubit16 theFooter[s_footerNum]={s_footerVal};
  if( (m_word&s_last4bitsON)== set16Bits(1,s_footerPos,theFooter)) status=true;
  return status;
}

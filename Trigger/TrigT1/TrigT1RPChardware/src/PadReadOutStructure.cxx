/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<string>
#include "TrigT1RPChardware/PadReadOutStructure.h"

//  header structure
const ubit16 PadReadOutStructure::headerPos[headerNum] ={ 12, 9, 0};
//const ubit16 PadReadOutStructure::headerPos[headerNum] ={ 12, 8, 0};
const ubit16 PadReadOutStructure::headerLen[headerNum] ={  4, 3, 9};
//const ubit16 PadReadOutStructure::headerLen[headerNum] ={  4, 4, 8};
const ubit16 PadReadOutStructure::headerVal=0x0005;  
  //  subheader structure
const ubit16 PadReadOutStructure::subHeaderPos[subHeaderNum]={ 12,  0};
const ubit16 PadReadOutStructure::subHeaderLen[subHeaderNum]={  4, 12};
const ubit16 PadReadOutStructure::subHeaderVal=0x0006;
//  prefooter structure
const ubit16 PadReadOutStructure::prefooterPos[prefooterNum]={ 12, 3, 2, 1, 0};
const ubit16 PadReadOutStructure::prefooterLen[prefooterNum]={  4, 1, 1, 1, 1};
const ubit16 PadReadOutStructure::prefooterVal=0x000a;
//  footer structure
const ubit16 PadReadOutStructure::footerPos[footerNum]={ 12, 11, 10, 9, 8, 0};
//const ubit16 PadReadOutStructure::footerPos[footerNum]={ 12,0};
const ubit16 PadReadOutStructure::footerLen[footerNum]={  4, 1, 1, 1, 1, 8};
//const ubit16 PadReadOutStructure::footerLen[footerNum]={  4,12};
const ubit16 PadReadOutStructure::footerVal=0x0007;

//----------------------------------------------------------------------------//
PadReadOutStructure::PadReadOutStructure() {
  //
  // Constructor used by the simulation program
  //
  setInit();
}
//----------------------------------------------------------------------------//
PadReadOutStructure::PadReadOutStructure(ubit16 inputData) {
  //
  // Constructor used by the simulation program
  //
  decodeFragment(inputData,m_field);
}
//----------------------------------------------------------------------------//

void PadReadOutStructure::setInit() {
  m_field     = 'U';
  //  m_word      =0xffff;
  //m_padid     =0xffff;
  //  m_status    =0xffff;
  //m_errorCode =0xffff;
}

//----------------------------------------------------------------------------//

ubit16 PadReadOutStructure::makeHeader(ubit16 *inputData) {
  //  m_status = *(inputData+2);
  m_padid  = *(inputData+1);
  m_l1id   = *(inputData+2);
  //  ubit16 theHeader[headerNum] = *inputData;
    //={headerVal,*(inputData+1),*(inputData+2)};
  vectorStruct = inputData;
  *(vectorStruct) = headerVal;
  m_word   = set16Bits(headerNum, headerPos, vectorStruct);//theHeader);
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
  vectorStruct = inputData;
  *(vectorStruct)= footerVal;
  //  ubit16 theFooter[footerNum]={footerVal};
  //for(int i=1;i<5;i++) *(theFooter+i) = *(inputData+i);
  m_word =  set16Bits(footerNum,footerPos,vectorStruct);//theFooter);
  return m_word;
}
*/
//----------------------------------------------------------------------------//
ubit16 PadReadOutStructure::makeFooter(ubit16* inputData) {
  //  m_errorCode= *inputData;
  m_errorSH =  *(inputData+1);
  m_errorL1 =  *(inputData+2);
  m_errorH  =  *(inputData+3);
  m_errorCM =  *(inputData+4);
  m_errorCMID =*(inputData+5);
  vectorStruct = inputData;
  *(vectorStruct)= footerVal;
  //  ubit16 theFooter[footerNum]={footerVal};
  //for(int i=1;i<5;i++) *(theFooter+i) = *(inputData+i);
  m_word =  set16Bits(footerNum,footerPos,vectorStruct);//theFooter);
  return m_word;
}
//----------------------------------------------------------------------------//
ubit16 PadReadOutStructure::decodeFragment(ubit16 inputWord, char &field) {
  //  Removed last because it is not used
  //  bool last=false;
  ubit16 errorCode=0;
  setInit();
  m_word = inputWord;
  errorCode=0;
  if(isHeader()) {
    m_field = 'H';
    m_padid     = get16Bits(inputWord,headerPos[1],headerLen[1]);
    m_l1id      = get16Bits(inputWord,headerPos[2],headerLen[2]);
  } else  if(isSubHeader()) {
    m_field = 'S';
    m_bcid     = get16Bits(inputWord,subHeaderPos[1],subHeaderLen[1]);;
  } else  if(isPreFooter()) {
    m_field = 'P';
    m_fifoCM  = get16Bits(inputWord,prefooterPos[1],prefooterLen[1]);
    m_fifoOR  = get16Bits(inputWord,prefooterPos[2],prefooterLen[2]);
    m_fifoL1  = get16Bits(inputWord,prefooterPos[3],prefooterLen[3]);
    m_fifoPAD = get16Bits(inputWord,prefooterPos[4],prefooterLen[4]);
    m_status = inputWord & 0x0fff;
  } else  if(isFooter()) {
    m_field = 'F';
    m_errorSH   = get16Bits(inputWord,footerPos[1],footerLen[1]);
    m_errorL1   = get16Bits(inputWord,footerPos[2],footerLen[2]);
    m_errorH    = get16Bits(inputWord,footerPos[3],footerLen[3]);
    m_errorCM   = get16Bits(inputWord,footerPos[4],footerLen[4]);
    m_errorCMID = get16Bits(inputWord,footerPos[5],footerLen[5]);
    m_errorCode = inputWord & 0x0fff; 
 } else {
    m_field = 'B';
    if((m_word&0xf000)==(MRS.getFooterVal()<<12)) {
      /* last=true; */
    } else {
      /* last=false; */
    }
  }
  field = m_field;
  return errorCode; // temporary: force return to 0
}

//----------------------------------------------------------------------------//
bool PadReadOutStructure::isBody()
{
  ubit16 theword = (m_word&0xf000)>>12;
  if( (theword != headerVal) && 
      (theword != subHeaderVal) &&
      (theword != prefooterVal) &&
      (theword != footerVal) )       return true;
  return false;
}
//----------------------------------------------------------------------------//
bool PadReadOutStructure::isHeader()
{
  bool status= false;
  ubit16 theHeader[headerNum]={headerVal};
  if( (m_word&last4bitsON)== set16Bits(1,headerPos,theHeader)) status=true;
  return status;
}

//-----------------------------------------------------------------------//
bool PadReadOutStructure::isSubHeader()
{
  bool status= false;
  ubit16 theSubHeader[subHeaderNum]={subHeaderVal};
  if( (m_word&last4bitsON)== set16Bits(1,subHeaderPos,theSubHeader)) status=true;
  return status;
}

//-----------------------------------------------------------------------//
bool PadReadOutStructure::isPreFooter()
{
  bool status= false;
  ubit16 thepreFooter[prefooterNum]={prefooterVal};
  if( (m_word&last4bitsON)== set16Bits(1,prefooterPos,thepreFooter)) status=true;
  return status;
}

//-----------------------------------------------------------------------//
bool PadReadOutStructure::isFooter()
{
  bool status= false;
  ubit16 theFooter[footerNum]={footerVal};
  if( (m_word&last4bitsON)== set16Bits(1,footerPos,theFooter)) status=true;
  return status;
}

















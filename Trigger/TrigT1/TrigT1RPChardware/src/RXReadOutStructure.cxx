/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<cstring>
#include "TrigT1RPChardware/RXReadOutStructure.h"
#include "TrigT1RPChardware/RPCReadOut.h"

using namespace std;

//  header structure
const ubit16 RXReadOutStructure::headerPos[headerNum] ={ 12, 8, 0};
const ubit16 RXReadOutStructure::headerLen[headerNum] ={  4, 4, 8};
const ubit16 RXReadOutStructure::headerVal=0x0009;  
//  footer structure
const ubit16 RXReadOutStructure::footerPos[footerNum]={ 12, 0};
const ubit16 RXReadOutStructure::footerLen[footerNum]={  4,12};
const ubit16 RXReadOutStructure::footerVal=0x000b;
//----------------------------------------------------------------------------//
RXReadOutStructure::RXReadOutStructure( RXReadOutStructure::DataVersion vers)
 :
 m_dataVersion(vers)
 {
//
// Constructor used by the simulation program
//
setInit();
}//end-of-RXReadOutStructure::RXReadOutStructure()
//----------------------------------------------------------------------------//
RXReadOutStructure::RXReadOutStructure(ubit16 inputData, RXReadOutStructure::DataVersion vers)
 :
 m_dataVersion(vers)
 {
//
// Constructor used by the simulation program
//

decodeFragment(inputData, m_field);
}//end-of-RXReadOutStructure::RXReadOutStructure()
//----------------------------------------------------------------------------//
RXReadOutStructure::~RXReadOutStructure() {
}//end-of-RXReadOutStructure::~RXReadOutStructure
//----------------------------------------------------------------------------//
void RXReadOutStructure::setInit() {
  m_field     = 'U';
  m_word      =0xffff;
  m_secid     =0xffff;
  m_status    =0xffff;
  m_errorCode =0xffff;
}//end-of-RXReadOutStructure::setInit()
//----------------------------------------------------------------------------//
unsigned short int RXReadOutStructure::makeHeader(ubit16 *inputData) {
const ubit16 theHeader[headerNum]={headerVal,*(inputData+1),*(inputData+2)};
m_secid  = *(inputData+1);
m_status = *(inputData+2);
m_word   = set16Bits(headerNum, headerPos, theHeader);
return m_word;
}//end-of-RXReadOutStructure::makeHeader
//----------------------------------------------------------------------------//
unsigned short int RXReadOutStructure::makeFooter(ubit16 errorCode) {
m_errorCode=errorCode;
const ubit16 theFooter[footerNum]={footerVal, m_errorCode};
m_word =  set16Bits(footerNum,footerPos,theFooter);
return m_word;
}//end-of-RXReadOutStructure::makeFooter
//----------------------------------------------------------------------------//
unsigned short int RXReadOutStructure::makeFooter(ubit16 *errorCode) {
return (*errorCode)&0x0;
}//end-of-RXReadOutStructure::makeFooter
//----------------------------------------------------------------------------//
bool RXReadOutStructure::isHeader() {
  bool status= false;
  ubit16 theHeader[headerNum]={headerVal};
  if( (m_word&last4bitsON)== set16Bits(1,headerPos,theHeader)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool RXReadOutStructure::isFooter() {
  bool status= false;
  ubit16 theFooter[footerNum]={footerVal};
  if( (m_word&last4bitsON)== set16Bits(1,footerPos,theFooter)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
unsigned short int RXReadOutStructure::decodeFragment(ubit16 inputWord,
                                                       char &field) {
MatrixReadOutStructure MRS;
//  Removed last because it is not used
//  bool last=false;
ubit16 errorCode=0;
setInit();
m_word = inputWord;
//
 errorCode=0;
 if(isHeader()) {
   m_field = 'H';
   if (m_dataVersion==Atlas || m_dataVersion==Simulation1) {
       m_secid    = get16Bits(inputWord,headerPos[1],headerLen[1])>>3;
       m_status   = get16Bits(inputWord,headerPos[2],headerLen[2]);
   } else if (m_dataVersion==Simulation0){
       m_status    = get16Bits(inputWord,headerPos[1],headerLen[1]);
       m_secid     = get16Bits(inputWord,headerPos[2],headerLen[2]);
   } else {
      // Unknown format, so leave data invalid.
       m_field='U';  
   }
 } else 
 if(isFooter()) {
   m_field = 'F';
   m_errorCode = get16Bits(inputWord,footerPos[1],footerLen[1]);
 } else {
   m_field = 'B';
   if((m_word&0xf000)==(MRS.getFooterVal()<<MRS.getFooterPos())) {
    /* last=true; */
   } else {
    /* last=false; */
   }//end-of-if((m_word
 }
field=m_field;
return errorCode; // temporary: force return to 0
}//end-of-decodeFragment

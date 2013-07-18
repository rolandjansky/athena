/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<cstring>
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "TrigT1RPChardware/RPCReadOut.h"

using namespace std;

  //  header structure
const ubit16 MatrixReadOutStructure::headerPos[headerNum] ={ 12, 9, 0};
const ubit16 MatrixReadOutStructure::headerLen[headerNum] ={  4, 3, 9};
const ubit16 MatrixReadOutStructure::headerVal= 0x000c;
  //  subheader structure
const ubit16 MatrixReadOutStructure::subHeaderPos[subHeaderNum]={ 12,  0};
const ubit16 MatrixReadOutStructure::subHeaderLen[subHeaderNum]={  4, 12};
const ubit16 MatrixReadOutStructure::subHeaderVal=8;
  //  body structure
const ubit16 MatrixReadOutStructure::CMABodyPos[CMABodyNum]={ 14, 11,  8,  5,  0};
const ubit16 MatrixReadOutStructure::CMABodyLen[CMABodyNum]={  2,  3,  3,  3,  5};
const ubit16 MatrixReadOutStructure::CMABodyVal=0;
  //  footer structure
const ubit16 MatrixReadOutStructure::footerPos[footerNum]={ 14,  8,  0};
const ubit16 MatrixReadOutStructure::footerLen[footerNum]={  2,  6,  8};
const ubit16 MatrixReadOutStructure::footerVal=1;
//
// old structure: valid till march 2006; replaced with the new
// footer length: 2 bits instead of 4 (see above)
//
// const ubit16 MatrixReadOutStructure::footerPos[footerNum]={ 12,  8,  0};
// const ubit16 MatrixReadOutStructure::footerLen[footerNum]={  4,  4,  8};
// const ubit16 MatrixReadOutStructure::footerVal=4;

//----------------------------------------------------------------------------//
MatrixReadOutStructure::MatrixReadOutStructure()
{
  //
  // Constructor used by the simulation program
  //
  setInit();
}//end-of-MatrixReadOutStructure::MatrixReadOutStructure()
//----------------------------------------------------------------------------//
MatrixReadOutStructure::MatrixReadOutStructure(ubit16 inputData)
{
  decodeFragment(inputData, m_field);  
}//end-of-MatrixReadOutStructure::MatrixReadOutStructure(ubit16 inputData)
//----------------------------------------------------------------------------//
void MatrixReadOutStructure::setInit(){
  m_field     = 'U';
  m_word      =0xffff;
  m_cmid      =0xffff;
  m_fel1id    =0xffff;
  m_febcid    =0xffff;
  m_bcid      =0xffff;
  m_time      =0xffff;
  m_ijk       =0xffff;
  m_channel   =0xffff;
  m_strip     =0xffff;
  m_threshold =0xffff;
  m_overlap   =0xffff;
  m_status    =0xffff;
  m_crc       =0xffff;
}//end-of-MatrixReadOutStructure::MatrixReadOutStructure
//----------------------------------------------------------------------------//
MatrixReadOutStructure::~MatrixReadOutStructure() {
}//end-of-MatrixReadOutStructure::~MatrixReadOutStructure
//----------------------------------------------------------------------------//
ubit16 MatrixReadOutStructure::makeHeader(ubit16 *inputData) {
  const ubit16 theHeader[headerNum]={headerVal,*(inputData+1),*(inputData+2)};
  m_cmid = *(inputData+1);
  m_fel1id = *(inputData+2);
  m_word = set16Bits(headerNum, headerPos, theHeader);
  return m_word;
}//end-of-MatrixReadOutStructure::makeHeader
//----------------------------------------------------------------------------//
ubit16 MatrixReadOutStructure::makeSubHeader() {
  m_febcid =1 ;
  const ubit16 theSubHeader[subHeaderNum]={subHeaderVal,1};
  m_word= set16Bits(subHeaderNum,subHeaderPos, theSubHeader);
  return m_word;
}//end-of-MatrixReadOutStructure::makeSubHeader
//----------------------------------------------------------------------------//
ubit16 MatrixReadOutStructure::makeBody(ubit16 *inputData) {
  m_bcid=     *(inputData);
  m_time=   *(inputData+1);
  m_ijk=    *(inputData+2);
  if(m_ijk<7) {
    m_channel=*(inputData+3);
  } else {
    ubit16 temp=*(inputData+3);
    m_overlap      = temp>>2;
    m_threshold    = temp&3;
  }
  ubit16 theCMABody[CMABodyNum];
  for(ubit16 i=0; i<CMABodyNum; i++) {theCMABody[i]=*(inputData+i);}
  m_word=  set16Bits(CMABodyNum,CMABodyPos, theCMABody);
  return m_word;
}//end-of-MatrixReadOutStructure::makeBodyHit
//----------------------------------------------------------------------------//
ubit16 MatrixReadOutStructure::makeFooter(ubit16 inputData) {
  m_status=1;
  m_crc= inputData;
  const ubit16 theFooter[footerNum]={footerVal, m_status, m_crc};
  m_word =  set16Bits(footerNum,footerPos,theFooter);
  return m_word;
}//end-of-MatrixReadOutStructure::makeFooter
//----------------------------------------------------------------------------//
ubit16 MatrixReadOutStructure::makeFooter(ubit16 *inputData) {
  
  return (*inputData)&0x0;
}//end-of-MatrixReadOutStructure::makeFooter


//----------------------------------------------------------------------------//
ubit16 MatrixReadOutStructure::decodeFragment(ubit16 inputWord,
							  char &field){
                                                   
  //
  //  method to decode the words present in the CM event fragment
  //
  //  inputWord     16-bit input word;
  //
  //  field         character that returns the type of the word "inputWord">
  //                H  header;
  //                S  subheader;
  //                B  body;
  //                F  footer;
  //                U  unkown: none of the types above listed.
  //
  //  The value 0 is returned if the word is recognized; otherwise 1 is returned.
  //
  //  The results of the decoding are stored in the class members.

  setInit();
  m_word      = inputWord;
  ubit16  temp;
  ubit16 outputFlag=0;
  if(isBody()) 
    {
      m_field = 'B';
      m_bcid  = get16Bits(inputWord,CMABodyPos[1],CMABodyLen[1]);
      m_time  = get16Bits(inputWord,CMABodyPos[2],CMABodyLen[2]);
      m_ijk   = get16Bits(inputWord,CMABodyPos[3],CMABodyLen[3]);
      if(m_ijk<7) {
	m_channel  = get16Bits(inputWord,CMABodyPos[4],CMABodyLen[4]);
	m_strip    = global_channel();
      } else {
	temp = get16Bits(inputWord,CMABodyPos[4],CMABodyLen[4]);
	m_overlap      = temp>>2;
	m_threshold    = temp&3;
      }
    } else if(isHeader()) {
      m_field = 'H';
      m_cmid      = get16Bits(inputWord,headerPos[1],headerLen[1]);
      m_fel1id    = get16Bits(inputWord,headerPos[2],headerLen[2]);
    } else  if(isSubHeader()) {
      m_field = 'S';
      m_febcid    = get16Bits(inputWord,subHeaderPos[1],subHeaderLen[1]);
    } else if(isFooter()) {
      m_field = 'F';
      m_status    = get16Bits(inputWord,footerPos[1],footerLen[1]);
      m_crc       = get16Bits(inputWord,footerPos[2],footerLen[2]);
    } else {
      m_field = 'U';
      outputFlag=1;
    }
  
  field=m_field;
  return outputFlag;
}//end-of-decodeFragment
//----------------------------------------------------------------------------//

bool MatrixReadOutStructure::isHeader()
{
  bool status= false;
  ubit16 theHeader[headerNum]={headerVal};
  if( (m_word&last4bitsON)== set16Bits(1,headerPos,theHeader)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::isSubHeader()
{
  bool status= false;
  ubit16 theSubHeader[subHeaderNum]={subHeaderVal};
  if( (m_word&last4bitsON)== set16Bits(1,subHeaderPos,theSubHeader)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::isBody()
{
  bool status= false;
  ubit16 theCMABody[CMABodyNum]={CMABodyVal};
  if( (m_word&last2bitsON)== set16Bits(1,CMABodyPos,theCMABody)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::isFooter()
{
  bool status= false;
  ubit16 theFooter[footerNum]={footerVal};
  //
  // old field mapping
  //
  //if( (m_word&last4bitsON)== set16Bits(1,footerPos,theFooter)) status=true;
  if( (m_word&last2bitsON)== set16Bits(1,footerPos,theFooter)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
ubit16 MatrixReadOutStructure::global_channel() 
{
  ubit16 channel=m_channel;
  if(m_ijk==3 || m_ijk ==5) channel=channel+32;
  return channel;
}

ubit16
MatrixReadOutStructure::isPivot()
{
  if(m_ijk==0||m_ijk==1) return 1;
  else return 0;
}

bool
MatrixReadOutStructure::isTrigger()
{
  if(m_ijk==6) return true;
  else return false;
}
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::isLowPt() {
bool output;
m_cmid & 0x0004 ? output=false : output=true;
return output;
}//end-of-MatrixReadOutStructure::lowPt()
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::isHigPt() {
return !isLowPt();
}//end-of-MatrixReadOutStructure::higPt()
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::isEtaProj() {
bool output;
m_cmid & 0x0002 ? output=false : output=true;
return output;
}//end-of-MatrixReadOutStructure::etaProj()
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::isPhiProj() {
return !isEtaProj();
}//end-of-MatrixReadOutStructure::phiProj()
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::is0LocAdd() {
bool output;
m_cmid & 0x0001 ? output=false : output=true;
return output;
}//end-of-MatrixReadOutStructure::0LocAdd()
//----------------------------------------------------------------------------//
bool MatrixReadOutStructure::is1LocAdd() {
return !is0LocAdd();
}//end-of-MatrixReadOutStructure::1LocAdd()
//----------------------------------------------------------------------------//

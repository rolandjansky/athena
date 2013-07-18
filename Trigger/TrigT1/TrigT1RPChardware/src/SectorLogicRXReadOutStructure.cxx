/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<string>
#include "TrigT1RPChardware/SectorLogicRXReadOutStructure.h"

//  header structure
const ubit16 SectorLogicRXReadOutStructure::headerPos[headerNum] ={ 12, 9 ,0};
const ubit16 SectorLogicRXReadOutStructure::headerLen[headerNum] ={  4, 3, 9};
const ubit16 SectorLogicRXReadOutStructure::headerVal=0x000d;  
//  subheader structure
const ubit16 SectorLogicRXReadOutStructure::subHeaderPos[subHeaderNum] ={ 12, 0};
const ubit16 SectorLogicRXReadOutStructure::subHeaderLen[subHeaderNum] ={  4, 12};
const ubit16 SectorLogicRXReadOutStructure::subHeaderVal=0x0006;  
//  input header structure
const ubit16 SectorLogicRXReadOutStructure::inputHeaderPos[inputHeaderNum] ={ 12, 8, 0};
const ubit16 SectorLogicRXReadOutStructure::inputHeaderLen[inputHeaderNum] ={  4, 4, 3};
const ubit16 SectorLogicRXReadOutStructure::inputHeaderVal=0x0001;  
//  output header structure
const ubit16 SectorLogicRXReadOutStructure::outputHeaderPos[outputHeaderNum] ={ 12, 8, 0};
const ubit16 SectorLogicRXReadOutStructure::outputHeaderLen[outputHeaderNum] ={  4, 4, 8};
const ubit16 SectorLogicRXReadOutStructure::outputHeaderVal=0x0002;  
//  input body structure
const ubit16 SectorLogicRXReadOutStructure::inputBodyPos[inputBodyNum] ={ 12, 9, 7, 6, 5, 2, 0};
const ubit16 SectorLogicRXReadOutStructure::inputBodyLen[inputBodyNum] ={  4, 3, 1, 1, 1, 3, 2};
const ubit16 SectorLogicRXReadOutStructure::inputBodyVal=0x0000;  
//  output body structure
const ubit16 SectorLogicRXReadOutStructure::outputBodyPos[outputBodyNum] ={ 12,  0};
const ubit16 SectorLogicRXReadOutStructure::outputBodyLen[outputBodyNum] ={  4, 12};
const ubit16 SectorLogicRXReadOutStructure::outputBodyVal=0x0003;  
//  footer structure
const ubit16 SectorLogicRXReadOutStructure::footerPos[footerNum]={ 12, 8, 0};
const ubit16 SectorLogicRXReadOutStructure::footerLen[footerNum]={  4, 4, 8};
const ubit16 SectorLogicRXReadOutStructure::footerVal=0x000f;

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
  decodeFragment(inputData,m_field);
}
//----------------------------------------------------------------------------//
void SectorLogicRXReadOutStructure::setInit() {
  m_field     = 'U';
}
//----------------------------------------------------------------------------//

ubit16 SectorLogicRXReadOutStructure::makeHeader(ubit16 *inputData) {
  m_word   = set16Bits(headerNum, headerPos, inputData);//theHeader);
  return m_word;
}
//----------------------------------------------------------------------------//
ubit16 SectorLogicRXReadOutStructure::makeFooter(ubit16 *inputData) {
  m_word =  set16Bits(footerNum,footerPos,inputData);
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
  if(isHeader()) {
    m_field = 'H';
    m_fel1id     = get16Bits(inputWord,headerPos[2],headerLen[2]);
    m_slid       = get16Bits(inputWord,headerPos[1],headerLen[1]);
  } else  if(isFooter()) {
    m_field = 'F';
    m_status = get16Bits(inputWord,footerPos[1],footerLen[1]);
    m_crc    = get16Bits(inputWord,footerPos[2],footerLen[2]);
  } else if(isSubHeader()) {
    m_field = 'S';
    m_slbcid = get16Bits(inputWord,subHeaderPos[1],subHeaderLen[1]);
  } else if (isInputHeader()) {
    m_field = 'I';
    m_inputPadId = get16Bits(inputWord,inputHeaderPos[2],inputHeaderLen[2]);
    m_inputRowinBcid = get16Bits(inputWord,inputHeaderPos[1],inputHeaderLen[1]);
  } else if (isInputBody()) {
    m_field = 'B';
    m_inputRoi         = get16Bits(inputWord,inputBodyPos[6],inputBodyLen[6]);
    m_inputThreshold   = get16Bits(inputWord,inputBodyPos[5],inputBodyLen[5]);
    m_inputOuterPlane  = get16Bits(inputWord,inputBodyPos[4],inputBodyLen[4]);
    m_inputOverlapPhi  = get16Bits(inputWord,inputBodyPos[3],inputBodyLen[3]);
    m_inputOverlapEta  = get16Bits(inputWord,inputBodyPos[2],inputBodyLen[2]);   
    m_inputTriggerBcid = get16Bits(inputWord,inputBodyPos[1],inputBodyLen[1]);

  } else if (isOutputHeader()) {
    m_field = 'O';
    m_outputRowinBcid = get16Bits(inputWord,outputHeaderPos[1],outputHeaderLen[1]);
    ubit16 outTrigger = get16Bits(inputWord,outputHeaderPos[2],outputHeaderLen[2]);
    m_outTriggerWord.clear();
    m_outTriggerWord.push_back(outTrigger);
  } else if (isOutputBody() ) {
    m_field = 'M';
    ubit16 outTrigger = get16Bits(inputWord,outputBodyPos[1],outputBodyLen[1]);
    m_outTriggerWord.push_back(outTrigger);
    if(m_outTriggerWord.size() == 3 ){
      uint32_t muctpiWord = 
          ((m_outTriggerWord[0] & 0x00ff) << 24) |
          ((m_outTriggerWord[1] & 0x0fff)<< 12)  |
          ((m_outTriggerWord[2] & 0x0fff) );

      m_moreThan2TriggerCand  = (muctpiWord & 0x1) ;
      m_outputTriggerBcid  = (muctpiWord>>27) & 0x7 ;
      m_nTriggerCand = 0;
      int iTriggerCand = 0;
      if (((muctpiWord>>19) & 0x7) != 0x7)
      {
          m_nTriggerCand++ ;
          iTriggerCand = m_nTriggerCand - 1;
          m_outputThreshold[iTriggerCand] = (muctpiWord>>19) & 0x7 ;
          m_outputRoi[iTriggerCand]       = (muctpiWord>>1 ) & 0x1f;
          m_outputOverlap[iTriggerCand]   = (muctpiWord>>8 ) & 0x3;
      }
      if (((muctpiWord>>22) & 0x7) != 0x7)
      {
          m_nTriggerCand++ ;
          iTriggerCand = m_nTriggerCand - 1;
          m_outputThreshold[iTriggerCand] = (muctpiWord>>22) & 0x7;
          m_outputRoi[iTriggerCand]       = (muctpiWord>>10) & 0x1f;
          m_outputOverlap[iTriggerCand]   = (muctpiWord>>17) & 0x3;	
      }
    }   
  }

  

  field = m_field;
  return 0; // temporary: force return to 0
}

//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isHeader()
{
  bool status= false;
  ubit16 theHeader[headerNum]={headerVal};
  if( (m_word&last4bitsON)== set16Bits(1,headerPos,theHeader)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isSubHeader()
{
  bool status= false;
  ubit16 theSubHeader[subHeaderNum]={subHeaderVal};
  if( (m_word&last4bitsON)== set16Bits(1,subHeaderPos,theSubHeader)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isInputHeader()
{
  bool status= false;
  ubit16 theInputHeader[inputHeaderNum]={inputHeaderVal};
  if( (m_word&last4bitsON)== set16Bits(1,inputHeaderPos,theInputHeader)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isOutputHeader()
{
  bool status= false;
  ubit16 theOutputHeader[outputHeaderNum]={outputHeaderVal};
  if( (m_word&last4bitsON)== set16Bits(1,outputHeaderPos,theOutputHeader)) status=true;
  return status;
}
//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isInputBody()
{
  bool status= false;
  ubit16 theInputBody[inputBodyNum]={inputBodyVal};
  if( (m_word&last4bitsON)== set16Bits(1,inputBodyPos,theInputBody)) status=true;
  return status;
}

//----------------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isOutputBody()
{
  bool status= false;
  ubit16 theOutputBody[outputBodyNum]={outputBodyVal};
  if( (m_word&last4bitsON)== set16Bits(1,outputBodyPos,theOutputBody)) status=true;
  return status;
}

//-----------------------------------------------------------------------//
bool SectorLogicRXReadOutStructure::isFooter()
{
  bool status= false;
  ubit16 theFooter[footerNum]={footerVal};
  if( (m_word&last4bitsON)== set16Bits(1,footerPos,theFooter)) status=true;
  return status;
}

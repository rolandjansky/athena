/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iomanip>
#include<cstring>
#include "TrigT1RPChardware/RPCROD.h"

using namespace std;

//----------------------------------------------------------------------------//
RPCROD::RPCROD(ubit16 subsystem, ubit16 RODID, 
                 RODword LVL1ID, ubit16 ECRID,
		 ubit16 BCID, ubit16 LVL1Type,
		 RODword EvType,  
                 RXReadOut *RX1, RXReadOut *RX2) 
		   : BaseObject(Hardware,"RPCRODReadOut") {

m_subsystem=subsystem;         // identify eta<0 (0) and eta>0 (1) subsystem
m_RODID             = RODID;
m_LVL1ID            = LVL1ID; 
m_ECRID             = ECRID;
m_BunchCrossingID2    = BCID;
m_Level1TriggerType2   = LVL1Type;
m_EventType           = EvType;
//
m_RXlist[0] = RX1;
m_RXlist[1] = RX2;
if(!m_RXlist[0]&&m_RXlist[1]) {
 m_RXlist[0]=RX2;
 m_RXlist[1]=RX1;
}
reset();
DISP<<" WELCOME in RPCROD "<<endl;
DISP_DEBUG;
reset();
makeFragment();
topRODBody();
DISP<<" ROD Fragement done; now try to read it back "<<endl;
DISP_DEBUG;
for(ubit16 i=0; i<m_numberOfWordsInFragment; i++) {
 DISP<<" data word is "<<hex<<readRODWord()<<dec<<endl;
 DISP_DEBUG;
}
bytestream(cout);
}//end-of-RPCROD
//----------------------------------------------------------------------------//
RPCROD::~RPCROD() {
}//end-of-~RPCROD
//----------------------------------------------------------------------------//
void RPCROD::reset() {
m_HeaderMarker         =0;
m_HeaderSize           =0;
m_FormatVersionNumber  =0;
m_SourceIdentifier     =0;
m_Level1ID=0;
m_BunchCrossingID=0;
m_Level1TriggerType=0;
m_DetectorEventType=0;
m_RXFragment[0]= 0;
m_RXFragment[1]= 0;
m_NumberOfStatusElements=0;
m_NumberOfDataElements=0;
m_StatusBlockPosition=0;
for(ubit16 i=0; i<s_maxNumberOfWordsInHeader; i++) m_HeaderArray[i]=0;
for(ubit16 i=0; i<s_maxNumberOfWordsInTrailer; i++) m_FooterArray[i]=0;
//
m_numberOfRXFragments=0;
m_numberOfWordsInFragment=0;
m_numberOf16bitWords=0;
m_numberOfDataElements=0;
//
topRODBody();
}//end-of-reset
//----------------------------------------------------------------------------//
void RPCROD::makeFragment() {
makeHeader();
makeBody();
makeFooter();
}//end-of-makeFragment
//----------------------------------------------------------------------------//
void RPCROD::makeHeader() {
RPCRODStructure ROD;
m_HeaderMarker         =ROD.getHeaderMarker(); m_numberOfWordsInFragment++;
m_HeaderSize           =ROD.getHeaderSize(); m_numberOfWordsInFragment++;
m_FormatVersionNumber  =ROD.getFormatVersion(); m_numberOfWordsInFragment++;
m_SourceIdentifier     =ROD.getSourceID(m_subsystem, m_RODID);
			                         m_numberOfWordsInFragment++;
//
m_Level1ID= (m_LVL1ID<<8) | (m_ECRID); m_numberOfWordsInFragment++;
m_BunchCrossingID=m_BunchCrossingID2; m_numberOfWordsInFragment++;
m_Level1TriggerType=m_Level1TriggerType2; m_numberOfWordsInFragment++;
m_DetectorEventType=m_EventType; m_numberOfWordsInFragment++;
if(m_numberOfWordsInFragment!=m_HeaderSize) {
 DISP<<" problems of Header word counting: counted="<<m_numberOfWordsInFragment
     <<" expected "<<m_HeaderSize<<endl;
 DISP_DEBUG;
}//end-of-if
m_HeaderArray[0]=m_HeaderMarker;
m_HeaderArray[1]=m_HeaderSize;
m_HeaderArray[2]=m_FormatVersionNumber;
m_HeaderArray[3]=m_SourceIdentifier;
m_HeaderArray[4]=m_Level1ID;
m_HeaderArray[5]=m_BunchCrossingID;
m_HeaderArray[6]=m_Level1TriggerType;
m_HeaderArray[7]=m_DetectorEventType;
//DISP<<" Header Marker: "<<hex<<m_HeaderMarker<<dec<<endl
//    <<" Header Size: "<<hex<<m_HeaderSize<<dec<<endl
//    <<" Header Version: "<<hex<<m_FormatVersionNumber<<dec<<endl
//    <<" Header SourceID: "<<hex<<m_SourceIdentifier<<dec<<endl
//    <<" Header LVL1ID: "<<hex<<m_Level1ID<<dec<<endl
//    <<" Header BCID: "<<hex<<m_BunchCrossingID<<dec<<endl
//    <<" Header Level1TriggerType: "<<hex<<m_Level1TriggerType<<dec<<endl
//    <<" Header DetectorType: "<<hex<<m_DetectorEventType<<dec<<endl;
//DISP_DEBUG;
}//end-of-makeHeader
//----------------------------------------------------------------------------//
void RPCROD::makeFooter() {
m_NumberOfStatusElements=s_numberOfStatusElements; m_numberOfWordsInFragment++;
m_NumberOfDataElements=m_numberOfDataElements;     m_numberOfWordsInFragment++;
m_StatusBlockPosition=s_statusBlockPosition;       m_numberOfWordsInFragment++;
m_FooterArray[0]=m_NumberOfStatusElements;
m_FooterArray[1]=m_NumberOfDataElements;
m_FooterArray[2]=m_StatusBlockPosition;
}//end-of-makeFooter
//----------------------------------------------------------------------------//
void RPCROD::makeBody() {
makeStatus();
makeData();
}//end-of-makeBody
//----------------------------------------------------------------------------//
void RPCROD::makeData() {
RXReadOutStructure RXOS;
m_RXFragment[0]= 0;
m_RXFragment[1]= 0;
ubit16 RXID[2]={0,0};
for(ubit16 i=0; i<s_numberOfSectors; i++) {
 if(m_RXlist[i]) {
  RXOS = m_RXlist[i]->getHeader(); 
  RXID[i] = RXOS.RXid();
  DISP<<" This RX has a pointer "<<endl
      <<" RXid= "<<RXID[i]<<endl;
  DISP_DEBUG;
 }//end-of-if
}//end-of-for
if(m_RXlist[0]&&m_RXlist[1]) {
//
// case 1 both pointers are active;
// sort the data block with increasing ID
//
 if(RXID[0]<RXID[1]) {
  m_RXFragment[0]=m_RXlist[0]; m_RXFragment[1]=m_RXlist[1];
 } else {
  m_RXFragment[0]=m_RXlist[1]; m_RXFragment[1]=m_RXlist[0];
 }
}else {
//
// case 2 only one pointer is active
//
 m_RXFragment[0]=m_RXlist[0];
}
//
for(ubit16 i=0; i<s_numberOfSectors; i++) {
 if(m_RXFragment[i]) {
  m_numberOfRXFragments++;
  m_numberOf16bitWords+=m_RXFragment[i]->numberOfFragmentWords ();
 }
}
RODword num=m_numberOf16bitWords;
if(m_numberOf16bitWords&1) num++;
m_numberOfDataElements+=num/2;
m_numberOfWordsInFragment+=num/2;
}//end-of-makeData
//----------------------------------------------------------------------------//
void RPCROD::makeStatus() {
for(ubit16 i=0; i<s_numberOfStatusElements; i++) {
 m_StatusElements[i]=0xdeadcafe;
 m_numberOfWordsInFragment+=1;
}
}//end-of-makeStatus
//----------------------------------------------------------------------------//
void RPCROD::topRODBody() {
m_addressOfWordScanned=0;
m_newRXIndex         =0;
m_newRXRO            =1;
m_numberOfWordsInRXRO=0;
m_numberOfWordsRead  =0;
m_endOfRXFragments   =0;
m_currentRXRO        =0;
m_numberOfTrailerWordsScanned=0;
m_numberOfStatusWordsScanned=0;
m_numberOfDataWordsScanned=0;
}//end-of-topRODBody
//----------------------------------------------------------------------------//
RODword RPCROD::readRODWord() {
RPCRODStructure ROD;
RODword output=0xffffffff;
     if(m_addressOfWordScanned==0)                      output=readHeaderMarker();
else if(m_addressOfWordScanned==1)                      output=readHeaderSize();
else if(m_addressOfWordScanned>1&&m_addressOfWordScanned<readHeaderSize())
                                                      output=readHeaderword();
else if(m_addressOfWordScanned>=(m_numberOfWordsInFragment-ROD.getFooterSize()))
                                                      output=readFooter();
else output = readBody();
m_addressOfWordScanned++;
return output;
}//end-of-readRODWord
//----------------------------------------------------------------------------//
RODword RPCROD::readHeaderMarker() {
return m_HeaderMarker;
}//end-of-readHeaderMarker
//----------------------------------------------------------------------------//
RODword RPCROD::readHeaderSize() {
return m_HeaderSize;
}//end-of-readHeaderSize
//----------------------------------------------------------------------------//
RODword RPCROD::readHeaderword() {
return m_HeaderArray[m_addressOfWordScanned];
}//end-of-readHeaderWord
//----------------------------------------------------------------------------//
RODword RPCROD::readFormatVersion() {
return m_FormatVersionNumber;
}//end-of-readFormatVersion
//----------------------------------------------------------------------------//
RODword RPCROD::readSourceID() {
return m_SourceIdentifier;
}//end-of-readSourceID
//----------------------------------------------------------------------------//
RODword RPCROD::readLVL1ID() {
return m_Level1ID;
}//end-of-readLVL1ID
//----------------------------------------------------------------------------//
RODword RPCROD::readBunchID() {
return m_BunchCrossingID;
}//end-of-readBunchID
//----------------------------------------------------------------------------//
RODword RPCROD::readLVL1Type() {
return m_Level1TriggerType;
}//end-of-readLVL1Type
//----------------------------------------------------------------------------//
RODword RPCROD::readDetectorType() {
return m_DetectorEventType;
}//end-of-readDetectorType
//----------------------------------------------------------------------------//
RODword RPCROD::readBody() {
if(s_statusBlockPosition) {
 if(m_numberOfDataWordsScanned<m_numberOfDataElements) return readData();
 else                                              return readStatus();
} else {
 if(m_numberOfStatusWordsScanned<s_numberOfStatusElements) return readStatus();
 else                                                  return readData();
}
}//end-of-readBody
//----------------------------------------------------------------------------//
RODword RPCROD::readStatus() {
m_numberOfStatusWordsScanned++;
m_numberOfWordsRead++;
return m_StatusElements[m_numberOfStatusWordsScanned-1];
}//end-of-readStatus
//----------------------------------------------------------------------------//
RODword RPCROD::readData() {
RODword output=0x0;
for(ubit16 iword=0; iword<2; iword++) {

 if(m_newRXRO) {
  if(m_newRXIndex<m_numberOfRXFragments) {
   if(m_RXFragment[m_newRXIndex]) {
    m_currentRXRO=m_RXFragment[m_newRXIndex];
    m_currentRXRO->topRXBody();
    m_numberOfWordsInRXRO=m_currentRXRO->numberOfFragmentWords();
    m_numberOfWordsRead=0;
    m_newRXRO=0;
   }//end-of-if(m_RXFragment[m_newRXIndex]
   m_newRXIndex++;
  } else {
   m_endOfRXFragments=1;
  }//end
 
 }//end-of-if(newPDRO
//
 if(!m_endOfRXFragments) {
  output = output | (m_currentRXRO->readRXWord())<<(16*iword);
  m_numberOfWordsRead++;
  if(m_numberOfWordsRead>=m_numberOfWordsInRXRO) m_newRXRO=1;
 } else {
  RPCRODStructure RS;
  output = RS.getEmptyDataWord()<<16 | output;
 }//end-of-if(!m_endOfRXFragments
 
}//end-of-for(iword
 m_numberOfDataWordsScanned++;
 return output;
}//end-of-readData
//----------------------------------------------------------------------------//
RODword RPCROD::readFooter() {
m_numberOfTrailerWordsScanned++;
return m_FooterArray[m_numberOfTrailerWordsScanned-1];
}//end-of-readFooter
//----------------------------------------------------------------------------//
void RPCROD::bytestream(ostream &stream) {
topRODBody();
for(ubit16 i=0; i<m_numberOfWordsInFragment; i++)
 stream<<hex<<readRODWord()<<dec<<endl;
}//end-of-bytestream
//----------------------------------------------------------------------------//

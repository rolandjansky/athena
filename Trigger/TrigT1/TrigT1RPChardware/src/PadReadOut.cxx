/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<iomanip>
#include<cstring>
#include "TrigT1RPChardware/PadReadOut.h"

using namespace std;

//----------------------------------------------------------------------------//
PadReadOut::PadReadOut(ubit16 padID, MatrixReadOut* MROlist[8]) 
                      : BaseObject(Hardware,"PadReadOut") {
m_padID=padID;
//
// copy MROlist pointers to m_MROlist
//
for(ubit16 i=0; i<8; i++) {m_MROlist[i]=MROlist[i];}
reset();
makeFragment();
//cout<<" After makeFragment number of Words in PadID" << padID << " body =" << m_numberOfWordsInFragment<<endl;
topPADBody();
decodeBytestream();
//
}//end-of-PadReadOut::PadReadOut
//----------------------------------------------------------------------------//
PadReadOut::~PadReadOut() {

}//end-of-PadReadOut::PadReadOut
//----------------------------------------------------------------------------//
void PadReadOut::reset() {
m_Header=0x0000;
m_Footer=0x0000;
for(ubit16 i=0; i<8; i++) { m_CMAFragment[i]=0;}
m_numberOfCMFragments=0;
m_numberOfWordsInFragment=0;
//
topPADBody();
}//end-of-PadReadOut::reset
//----------------------------------------------------------------------------//
void PadReadOut::topPADBody() {
m_addressOfWordScanned=0;
m_newCMIndex         =0;
m_newCMRO            =1;
m_numberOfWordsInCMRO=0;
m_numberOfWordsRead  =0;
m_endOfCMFragments   =0;
m_currentCMRO        =0;
}//end-of-PadReadOut::topPADBody
//----------------------------------------------------------------------------//
void PadReadOut::makeFragment() {
 makeHeader();
 makeBody();
 makeFooter();
}//end-of-PadReadOut::makeFragment
//----------------------------------------------------------------------------//
void PadReadOut::makeHeader() {
ubit16 headerval[3];
headerval[0] = 0;
headerval[1] = m_padID;
headerval[2] = 0; // Status bits (to be defined yet)
m_Header = m_PROS.makeHeader(headerval);
m_numberOfWordsInFragment+=1;
//cout<<" Header: "<<hex<<m_Header<<dec<<endl;
}//end-of-PadReadOut::makeHeader()
//----------------------------------------------------------------------------//
void PadReadOut::makeFooter() {
ubit16 errorCodes[6]={0,0,0,0,0,0};
//m_Footer = m_PROS.makeFooter(0);
m_Footer = m_PROS.makeFooter(errorCodes);
m_numberOfWordsInFragment+=1;
}//end-of-PadReadOut::makeFooter()
//----------------------------------------------------------------------------//
void PadReadOut::makeBody(bool debugPrint) {
MatrixReadOut *CMAFragment[8]={0};
MatrixReadOutStructure MROS;
ubit16 numberOfCMBodyWords=0;
ubit16 i;
if (debugPrint) cout <<" makeBody "<<endl;

for(i=0; i<8; i++) {
 if(m_MROlist[i]) {
  if (debugPrint) cout<<m_MROlist[i]<<endl;
  MROS = m_MROlist[i]->getHeader();
  numberOfCMBodyWords=m_MROlist[i]->numberOfBodyWords();
  ubit16 cmid = MROS.cmid();
  if (debugPrint) cout<<" Identificatore di questa Matrice e': "<<cmid<<endl
      <<"                    il numnero di Body words = "<<numberOfCMBodyWords<<endl;
  if(cmid<8 ) {
   if(!CMAFragment[cmid]) {
    CMAFragment[cmid] = m_MROlist[i];
   } else {
    if (debugPrint) cout<<" PadReadOut::makeBody: more than one CMA with address "<<cmid<<endl;
   }
   if (debugPrint) cout<<" Matrix: "<<hex<<m_MROlist[i]->readHeader()<<dec<<endl;
  } else {
   if (debugPrint) cout<<" PadReadOut::makeBody: cmid address "<<cmid<<" is bad "<<endl;
  }//end-of-if(cmid<8  
 }//end-of-if(m_MROlist
}//end-of-while
//
// copy CMAFragment to the array data member m_CMAFragment
//
ubit16 j=0;
for(i=0; i<8; i++) {
 if(CMAFragment[i]) {
  m_CMAFragment[j]=CMAFragment[i];
  if (debugPrint) cout<<" makeBody; number of CM WOrds="
      <<m_CMAFragment[j]->numberOfFragmentWords()<<endl;
  m_numberOfWordsInFragment+=m_CMAFragment[j]->numberOfFragmentWords();
  j++;
 }//end-of-if(
}//end-of-for(i
m_numberOfCMFragments=j;
if (debugPrint) cout<<" Number of CMA Fragments= "<<m_numberOfCMFragments<<endl;
}//end-of-PadReadOut::makeBody
//----------------------------------------------------------------------------//
ubit16 PadReadOut::readHeader() {
 return m_Header;
}//end-of-PadReadOut::readHeader()
//----------------------------------------------------------------------------//
ubit16 PadReadOut::readFooter() {
 return m_Footer;
}//end-of-PadReadOut::readFooter()
//----------------------------------------------------------------------------//
ubit16 PadReadOut::readBody() {
ubit16 output=0xffff;

//cout<<" m_newCMIndex "<<m_newCMIndex
//    <<" m_newCMRO "<<m_newCMRO
//    <<" m_numberOfWordsInCMRO "<<m_numberOfWordsInCMRO
//    <<" m_numberOfWordsRead "<<m_numberOfWordsRead
//    <<" m_endOfCMFragments "<<m_endOfCMFragments<<endl;

 if(m_newCMRO) {
 
  if(m_newCMIndex<8) {
   if(m_CMAFragment[m_newCMIndex]) {
    m_currentCMRO=m_CMAFragment[m_newCMIndex];
    m_currentCMRO->topCMABody();
    m_numberOfWordsInCMRO=m_currentCMRO->numberOfFragmentWords();
    m_numberOfWordsRead=0;
    m_newCMRO=0;
   }//end
   m_newCMIndex++;
  } else {
   m_endOfCMFragments=1;
  }//end
 
 }//end-of-if(m_newCMRO
//
 if(!m_endOfCMFragments) {
  output = m_currentCMRO->readCMAWord();
  m_numberOfWordsRead++;
  if(m_numberOfWordsRead>=m_numberOfWordsInCMRO) m_newCMRO=1;
 }//end-of-if(!m_endOfCMFragments
 return output;
}//end-of-PadReadOut::readBody()
//----------------------------------------------------------------------------//
ubit16 PadReadOut::readPADWord() {
ubit16 output=0xffff;
     if(m_addressOfWordScanned==0)                           output=readHeader();
else if(m_addressOfWordScanned==(m_numberOfWordsInFragment-1)) output=readFooter();
else output = readBody();
m_addressOfWordScanned++;
return output;
}//end-of-PadReadOut::readPADWord
//----------------------------------------------------------------------------//
void PadReadOut::bytestream(ostream &stream) {
stream<<hex<<m_Header<<dec<<endl;               // header
for(ubit16 i=0; i<m_numberOfCMFragments; i++) {
 m_CMAFragment[i]->bytestream(stream);          // body
}//end-of-for(ubit16 i
stream<<hex<<m_Footer<<dec<<endl;               // footer
}//end-of-MatrixReadOut::bytestream
//----------------------------------------------------------------------------//
void PadReadOut::decodeBytestream () {
PadReadOutStructure PROS; 
char field;
ubit16 inputData;
//
ubit16 padHeaderfound=0;
//
topPADBody();
for(ubit16 i=0; i<m_numberOfWordsInFragment; i++) {
 inputData = readPADWord();
 //cout<<" bytestream: current word is "<<hex<<inputData<<dec<<endl;

 if(!padHeaderfound) {
  PROS.decodeFragment(inputData,field);
  if(PROS.isHeader()) {
   padHeaderfound+=1;
   //cout<<" decodeBytestream: PAD Header Found"<<endl;
  }
 } else {
 }
 
}//end-of-for(i
}//end-of-decodeBytestream
//----------------------------------------------------------------------------//
PadReadOutStructure PadReadOut::getHeader() {
PadReadOutStructure theStruct(m_Header);
return theStruct;
}//end-of-PadReadOut::getHeader
//----------------------------------------------------------------------------//
PadReadOutStructure PadReadOut::getFooter() {
PadReadOutStructure theStruct(m_Footer);
return theStruct;
}//end-of-PadReadOut::getFooter

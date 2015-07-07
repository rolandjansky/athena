/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
DISP<<" WELCOME in PadReadOut: PadID= " << m_padID;
DISP_DEBUG;
//
// copy MROlist pointers to m_MROlist
//
for(ubit16 i=0; i<8; i++) {m_MROlist[i]=MROlist[i];}
reset();
makeFragment();
DISP<<" After makeFragment number of Words in PAD body ="
    <<numberOfWordsInFragment;
DISP_DEBUG;
topPADBody();
for(ubit16 i=0; i<numberOfWordsInFragment; i++) {
 DISP<<" output is "<<hex<<readPADWord()<<dec;
 DISP_DEBUG;
}
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
numberOfCMFragments=0;
numberOfWordsInFragment=0;
//
topPADBody();
}//end-of-PadReadOut::reset
//----------------------------------------------------------------------------//
void PadReadOut::topPADBody() {
addressOfWordScanned=0;
newCMIndex         =0;
newCMRO            =1;
numberOfWordsInCMRO=0;
numberOfWordsRead  =0;
endOfCMFragments   =0;
currentCMRO        =0;
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
m_Header = PROS.makeHeader(headerval);
numberOfWordsInFragment+=1;
DISP<<" Header: "<<hex<<m_Header<<dec<<endl;
DISP_DEBUG;
}//end-of-PadReadOut::makeHeader()
//----------------------------------------------------------------------------//
void PadReadOut::makeFooter() {
ubit16 errorCodes[6]={0,0,0,0,0,0};
//m_Footer = PROS.makeFooter(0);
m_Footer = PROS.makeFooter(errorCodes);
numberOfWordsInFragment+=1;
}//end-of-PadReadOut::makeFooter()
//----------------------------------------------------------------------------//
void PadReadOut::makeBody() {
MatrixReadOut *CMAFragment[8]={0};
MatrixReadOutStructure MROS;
ubit16 numberOfCMBodyWords=0;
ubit16 i;
DISP<<" makeBody "<<endl;
DISP_DEBUG;

for(i=0; i<8; i++) {
 if(m_MROlist[i]) {
  DISP<<m_MROlist[i]<<endl;
  DISP_DEBUG;
  MROS = m_MROlist[i]->getHeader();
  numberOfCMBodyWords=m_MROlist[i]->numberOfBodyWords();
  ubit16 cmid = MROS.cmid();
  DISP<<" Identificatore di questa Matrice e': "<<cmid<<endl
      <<"                    il numnero di Body words = "<<numberOfCMBodyWords;
  DISP_DEBUG;
  if(cmid<8 ) {
   if(!CMAFragment[cmid]) {
    CMAFragment[cmid] = m_MROlist[i];
   } else {
    DISP<<" PadReadOut::makeBody: more than one CMA with address "
        <<cmid;
    DISP_DEBUG;
   }
   DISP<<" Matrix: "<<hex<<m_MROlist[i]->readHeader()<<dec<<endl;
   DISP_DEBUG;
  } else {
   DISP<<" PadReadOut::makeBody: cmid address "<<cmid<<" is bad ";
   DISP_DEBUG;
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
  DISP<<" makeBody; number of CM WOrds="
      <<m_CMAFragment[j]->numberOfFragmentWords();
  DISP_DEBUG;
  numberOfWordsInFragment+=m_CMAFragment[j]->numberOfFragmentWords();
  j++;
 }//end-of-if(
}//end-of-for(i
numberOfCMFragments=j;
DISP<<" Number of CMA Fragments= "<<numberOfCMFragments;
DISP_DEBUG;
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

//DISP<<" newCMIndex "<<newCMIndex
//    <<" newCMRO "<<newCMRO
//    <<" numberOfWordsInCMRO "<<numberOfWordsInCMRO
//    <<" numberOfWordsRead "<<numberOfWordsRead
//    <<" endOfCMFragments "<<endOfCMFragments<<endl;
//DISP_DEBUG;

 if(newCMRO) {
 
  if(newCMIndex<8) {
   if(m_CMAFragment[newCMIndex]) {
    currentCMRO=m_CMAFragment[newCMIndex];
    currentCMRO->topCMABody();
    numberOfWordsInCMRO=currentCMRO->numberOfFragmentWords();
    numberOfWordsRead=0;
    newCMRO=0;
   }//end
   newCMIndex++;
  } else {
   endOfCMFragments=1;
  }//end
 
 }//end-of-if(newCMRO
//
 if(!endOfCMFragments) {
  output = currentCMRO->readCMAWord();
  numberOfWordsRead++;
  if(numberOfWordsRead>=numberOfWordsInCMRO) newCMRO=1;
 }//end-of-if(!endOfCMFragments
 return output;
}//end-of-PadReadOut::readBody()
//----------------------------------------------------------------------------//
ubit16 PadReadOut::readPADWord() {
ubit16 output=0xffff;
     if(addressOfWordScanned==0)                           output=readHeader();
else if(addressOfWordScanned==(numberOfWordsInFragment-1)) output=readFooter();
else output = readBody();
addressOfWordScanned++;
return output;
}//end-of-PadReadOut::readPADWord
//----------------------------------------------------------------------------//
void PadReadOut::bytestream(ostream &stream) {
stream<<hex<<m_Header<<dec<<endl;               // header
for(ubit16 i=0; i<numberOfCMFragments; i++) {
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
for(ubit16 i=0; i<numberOfWordsInFragment; i++) {
 inputData = readPADWord();
 DISP<<" bytestream: current word is "<<hex<<inputData<<dec;
 DISP_DEBUG;
 
 if(!padHeaderfound) {
  PROS.decodeFragment(inputData,field);
  if(PROS.isHeader()) {
   padHeaderfound+=1;
   DISP<<" decodeBytestream: PAD Header Found ";
   DISP_DEBUG;
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

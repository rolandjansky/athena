/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<iomanip>
#include<cstring>
#include<stdexcept>
#include "TrigT1RPChardware/SectorLogicReadOut.h"
#include "MuonCablingTools/BaseObject.h"

using namespace std;

//----------------------------------------------------------------------------//
SectorLogicReadOut::SectorLogicReadOut() 
                             : BaseObject(Hardware,"SectorLogicReadOut") {
initialize();
}//end-of-SectorLogicReadOut
//----------------------------------------------------------------------------//
SectorLogicReadOut::SectorLogicReadOut(const SectorLogicReadOut &SLROOrig)
                   : BaseObject(Hardware,"SectorLogicReadOut") {
//
// copy constructor
//
m_Header = SLROOrig.m_Header;
m_Footer = SLROOrig.m_Footer;
m_Body        = 0;
m_BodyCounter = 0;
m_BodyLast    = 0;
m_BodyCurr    = 0;
m_numberOfWordsInFrag = SLROOrig.m_numberOfWordsInFrag;
m_numberOfWordsInBody = SLROOrig.m_numberOfWordsInBody;
m_numberOfWordsInCounters = SLROOrig.m_numberOfWordsInCounters;
m_numberOfWordsInSLHits = SLROOrig.m_numberOfWordsInSLHits;
m_counter32ok = SLROOrig.m_counter32ok;
m_hitok = SLROOrig.m_hitok;
for( ubit16 i=0; i<s_numberOfDataCounters; i++) {
  m_counter16[i] = SLROOrig.m_counter16[i];
}
for( ubit16 i=0; i<s_numberOfDecodedCounters; i++) {
  m_counter32[i] = SLROOrig.m_counter32[i];
}
//
// Copy Dynamic structure
//
SLROData *q = 0;
SLROData *r = 0;
SLROData *p = SLROOrig.m_Body;
ubit16 cnter=0;
while(p) {
 q = new SLROData;
 q->hit = p->hit;
 q->next = 0;
 if(!cnter) {
  m_Body = q;
//  if(cnter==(nGates*nLinks)) m_BodyCounter = q;
 } 
  else r->next=q;
 cnter++;
 r = q;
 p=p->next;
}//end-of-while
m_BodyLast=r;
}//end-of-SectorLogicReadOut
//----------------------------------------------------------------------------//
SectorLogicReadOut::~SectorLogicReadOut() {
//
// delete the SLROData dynamic structure used to describe Body part
// of the event fragment.
//
deleteSLBody();
}
//----------------------------------------------------------------------------//
void SectorLogicReadOut::deleteSLBody() {
  SLROData *p, *q;
  p=m_Body;
  while(p) {
    q=p;
    p=p->next;
    delete q;
  }//end-of-while
  p=0;
  q=0;
  m_Body           = 0;
  m_BodyLast       = 0;
  m_BodyCurr       = 0;
  m_BodyCounter    = 0;
  m_numberOfWordsInBody= 0;
}//end-of-deleteSLBody
//----------------------------------------------------------------------------//
void SectorLogicReadOut::initialize() {
  //
  // initialize data members
  //
  m_Header=0;
  m_Footer=0;
  //
  // initialize pointers
  //
  m_Body         =0;
  m_BodyLast     =0;
  m_BodyCurr     =0;
  m_BodyCounter  =0;
  //
  // initialize check flags
  //
  m_numberOfWordsInFrag      =0;
  m_numberOfWordsInBody      =0;
  m_numberOfWordsInCounters  =s_numberOfDataCounters;
  m_numberOfWordsInSLHits    =0;
  m_counter32ok=false;
  m_hitok      =false;
  for(ubit16 i=0; i<m_numberOfWordsInCounters; i++) {m_counter16[i]=0;}
  for(ubit16 i=0; i<s_numberOfDecodedCounters; i++) {m_counter32[i]=0;}
}//end-of-initialize
//----------------------------------------------------------------------------//
void SectorLogicReadOut::reset() {
//
// reset the data structure to host a new fragment;
// first delete the dinamyc structure...
deleteSLBody();
// then initialize the data members.
initialize();
}//end-of-SectorLogicReadOut::reset()
//----------------------------------------------------------------------------//
void SectorLogicReadOut::writeRecord(ubit16 thisRecord, bool last) {

  if(m_numberOfWordsInFrag==0) {
    m_Header = thisRecord;
  } else if(m_numberOfWordsInFrag && !last) {
    makeNewHit(thisRecord);
  } else {
    m_Footer = thisRecord;
  }
  m_numberOfWordsInFrag++;  
}//end-of-void SectorLogicReadOut
//----------------------------------------------------------------------------//  
void SectorLogicReadOut::makeNewHit(ubit16 newHit) {  
  SLROData *p;
  p = new SLROData;
  p->hit=newHit;
  p->next=0;
  if(!m_Body) {
    m_Body = p;
  } else {
    m_BodyLast->next = p;
  }//end-of-if
  m_BodyLast=p;
  if(!m_numberOfWordsInBody) topSLBody();
  m_numberOfWordsInBody++;
  if(m_numberOfWordsInBody > m_numberOfWordsInCounters)
    m_numberOfWordsInSLHits = (m_numberOfWordsInBody-m_numberOfWordsInCounters);
}//end-of-SectorLogicReadOut::makeNewHit
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::readSLHitCurrent(){
  if(m_BodyCurr) {
    ubit16 hit = m_BodyCurr->hit;
    m_BodyCurr=m_BodyCurr->next;
    return hit;
  } else {
    return 0xefac;
  }
}//end-of-SectorLogicReadOut::readSLHitCurrent
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::readSLCounterCurrent(){
  if(!m_BodyCounter) {
    SLROData *temp=m_Body;
    for(ubit16 i=0; i<numberOfHitWords(); i++) {
      if(temp->next) temp = temp->next;
    }
    if(temp) m_BodyCounter=temp;
    m_BodyCurr = m_BodyCounter;
  }
// 
  if(m_BodyCurr) {
    ubit16 hit = m_BodyCurr->hit;
    m_BodyCurr=m_BodyCurr->next;
    return hit;
  } else {
    return 0xcafe;
  }
}//end-of-SectorLogicReadOut::readSLCounterCurrent
//----------------------------------------------------------------------------//
void SectorLogicReadOut::doCounter32(){
  topSLBodyCounters();
  for(ubit16 i=0; i<m_numberOfWordsInCounters; i++) {
   m_counter16[i]=readSLCounterCurrent()&0x1fff;
  }
//
  ubit16 j=0;
  for(ubit16 i=0;i<s_numberOfDecodedCounters;i++){
    m_counter32[i]=(m_counter16[j]|(m_counter16[j+1]<<13)|(m_counter16[j+2] <<26));
    j+=3;
  }
}//end-of-SectorLogicReadOut::doCounter32()
//----------------------------------------------------------------------------//
RODword SectorLogicReadOut::getCounter32(ubit16 index){
  if(!m_counter32ok) {
   doCounter32();
   m_counter32ok=true;
  }
  if(index<s_numberOfDecodedCounters) {
    return m_counter32[index];
  } else {
    throw std::out_of_range("SectorLogicReadout::getCounter32: index=" + std::to_string(index) +
                            " is larger than " + std::to_string(s_numberOfDecodedCounters));
  }
}
//----------------------------------------------------------------------------//
float SectorLogicReadOut::padTriggerRate(ubit16 padAddress){
  if(!m_counter32ok) doCounter32();
  //
  // units are kHz
  //
  static const float convertToTriggerRate=160314.74/4.0;// units are kHz
  if(padAddress < ((s_numberOfDecodedCounters-3)/2)) {
   return convertToTriggerRate
          * (float (m_counter32[padAddress*2+1])/float (m_counter32[padAddress*2+0]));
  } else {
    throw std::out_of_range("SectorLogicReadout::padTrigger: input padAddress=" +
                            std::to_string(padAddress) + " is not possible");
  }
}
//----------------------------------------------------------------------------//
void SectorLogicReadOut::doHit(){
  topSLBody();
  for(ubit16 j=0; j<s_nGates; j++) {
    for(ubit16 i=0; i<s_nLinks; i++) {
     m_hit[i][j]=readSLHitCurrent();
    }//end-of-for(ubit16 i
  }//end-of-for(ubit16 j
}//end-of-SectorLogicReadOut::doHit()
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::cmadd(ubit16 indexLink, ubit16 indexGate) {
  if(!m_hitok) doHit();
  if(indexLink<s_nLinks && indexGate<s_nGates) {
   return (m_hit[indexLink][indexGate]   ) & 0x3;
  } else {
    throw std::out_of_range("SectorLogicReadout::cmid: indexLink or indexGate out of range");
  }
}//end-of-SectorLogicReadOut::cmid(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::ptid(ubit16 indexLink, ubit16 indexGate) {
  if(!m_hitok) doHit();
  if(indexLink<s_nLinks && indexGate<s_nGates) {
   return (m_hit[indexLink][indexGate]>>2) & 0x7;
  } else {
    throw std::out_of_range("SectorLogicReadout::ptid: indexLink or indexGate out of range");
  }
}//end-of-SectorLogicReadOut::ptid(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::opl(ubit16 indexLink, ubit16 indexGate) {
  if(!m_hitok) doHit();
  if(indexLink<s_nLinks && indexGate<s_nGates) {
   return (m_hit[indexLink][indexGate]>>5) & 0x1;
  } else {
    throw std::out_of_range("SectorLogicReadout::opl: indexLink or indexGate out of range");
  }
}//end-of-SectorLogicReadOut::opl(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::ovphi(ubit16 indexLink, ubit16 indexGate) {
  if(!m_hitok) doHit();
  if(indexLink<s_nLinks && indexGate<s_nGates) {
   return (m_hit[indexLink][indexGate]>>6) & 0x1;
  } else {
    throw std::out_of_range("SectorLogicReadout::ovphi: indexLink or indexGate out of range");
  }
}//end-of-SectorLogicReadOut::ovphi(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::oveta(ubit16 indexLink, ubit16 indexGate) {
  if(!m_hitok) doHit();
  if(indexLink<s_nLinks && indexGate<s_nGates) {
   return (m_hit[indexLink][indexGate]>>7) & 0x1;
  } else {
    throw std::out_of_range("SectorLogicReadout::oveta: indexLink or indexGate out of range");
  }
}//end-of-SectorLogicReadOut::oveta(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::res(ubit16 indexLink, ubit16 indexGate) {
  if(!m_hitok) doHit();
  if(indexLink<s_nLinks && indexGate<s_nGates) {
   return (m_hit[indexLink][indexGate]>>8) & 0x1;
  } else {
    throw std::out_of_range("SectorLogicReadout::res : indexLink or indexGate out of range");
  }
}//end-of-SectorLogicReadOut::res(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::bcid(ubit16 indexLink, ubit16 indexGate) {
  if(!m_hitok) doHit();
  if(indexLink<s_nLinks && indexGate<s_nGates) {
   return (m_hit[indexLink][indexGate]>>9) & 0x7;
  } else {
    throw std::out_of_range("SectorLogicReadout::bcid: indexLink or indexGate out of range");
  }
}//end-of-SectorLogicReadOut::bcid(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
void SectorLogicReadOut::display(std::ostream &stream) {
  stream<<" **** Sector Logic ReadOut Fragment ****"<<std::endl;
  stream<<" SectorLogic: number of Hits    :"<<m_numberOfWordsInSLHits<<std::endl;
  stream<<" SectorLogic: number of Counters:"<<m_numberOfWordsInCounters<<std::endl;
  stream<<" SectorLogic: Header "<<std::hex<<m_Header<<std::dec<<std::endl;
  for(ubit16 i=0; i<m_numberOfWordsInSLHits; i++) {
    stream<<" SectorLogic: hit       "<<(i+1)<<"  ==> "
          <<std::hex<<readSLHitCurrent()<<std::dec<<std::endl;
  }
  stream<<" SectorLogic: Footer "<<std::hex<<m_Footer<<std::dec<<std::endl;
  for(ubit16 i=0; i<m_numberOfWordsInCounters; i++) {
    stream<<" SectorLogic: counter   "<<(i+1)<<"  ==> "
          <<std::hex<<readSLCounterCurrent()<<std::dec<<std::endl;
  }
  for(int i=0; i<s_numberOfDecodedCounters; i++) {
    stream<<" SectorLogic: Counter32 "<<(i+1)<<"  ==> "
          <<" = "<<std::hex<<getCounter32(i)<<std::dec<<std::endl;
  }
  stream<<" SectorLogic: Pad 0 trigger rate: "<<padTriggerRate(0)<<" kHz"<<std::endl;
  stream<<" SectorLogic: Pad 1 trigger rate: "<<padTriggerRate(1)<<" kHz"<<std::endl;
}//end-of-void SectorLogicReadOut::display
//----------------------------------------------------------------------------//
bool SectorLogicReadOut::checkFragment() {
  return (m_numberOfWordsInBody==(s_nGates*s_nLinks+s_numberOfDataCounters));
}//end-of-void SectorLogicReadOut::checkFragment()

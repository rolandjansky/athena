/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<iomanip>
#include<cstring>
#include "TrigT1RPChardware/SectorLogicRXReadOut.h"
#include "MuonCablingTools/BaseObject.h"

using namespace std;

//----------------------------------------------------------------------------//
SectorLogicRXReadOut::SectorLogicRXReadOut() 
                             : BaseObject(Hardware,"SectorLogicRXReadOut") {
  initialize();

}//end-of-SectorLogicRXReadOut

//----------------------------------------------------------------------------//
SectorLogicRXReadOut::~SectorLogicRXReadOut() {
//
// delete the SLRXROData dynamic structure used to describe Body part
// of the event fragment.
//
  deleteSLBody();
}
//----------------------------------------------------------------------------//
void SectorLogicRXReadOut::deleteSLBody() {
  SLRXROData *p, *q;
  p=m_BodyFirst;
  while(p) {
    q=p;
    p=p->next;
    delete q;
  }//end-of-while
  p= 0 ;
  q= 0 ;
  m_BodyFirst      = 0;
  m_BodyLast       = 0;
  m_BodyCurr       = 0;
  m_numberOfInputWords = 0;
}//end-of-deleteSLBody

//----------------------------------------------------------------------------//
void SectorLogicRXReadOut::initialize() {
  //
  // initialize data members
  //
  m_Header=0;
  m_SubHeader=0;
  m_Footer=0;
  //
  // initialize pointers
  //
  m_BodyFirst         = 0;
  m_BodyLast     = 0;
  m_BodyCurr     = 0;
  //
  // initialize check flags
  //
  m_numberOfWordsInFrag     =0;
  m_numberOfInputWords      =0;
  }//end-of-initialize
//----------------------------------------------------------------------------//

void SectorLogicRXReadOut::reset() {
//
// reset the data structure to host a new fragment;
// first delete the dinamyc structure...
  deleteSLBody();
  // then initialize the data members.
  initialize();
}//end-of-SectorLogicRXReadOut::reset()

//----------------------------------------------------------------------------//
void SectorLogicRXReadOut::writeRecord(ubit16 thisRecord, bool last) {

  if(m_numberOfWordsInFrag==0) {
    m_Header = thisRecord;
  } else if (m_numberOfWordsInFrag==1) {
    m_SubHeader=thisRecord;
  } else if(m_numberOfWordsInFrag && !last) {
    makeNewHit(thisRecord);
  } else {
    m_Footer = thisRecord;
  }
  m_numberOfWordsInFrag++;  
}//end-of-void SectorLogicRXReadOut

//----------------------------------------------------------------------------//  
void SectorLogicRXReadOut::makeNewHit(ubit16 newHit) {  
  SLRXROData *p;
  p = new SLRXROData;
  p->hit=newHit;
  p->next= 0 ;
  if(!m_BodyFirst) {
    m_BodyFirst = p;
  } else {
    m_BodyLast->next = p;
  }//end-of-if
  m_BodyLast=p;

  if(!m_numberOfInputWords) topSLBody();
  m_numberOfInputWords++;

}//end-of-SectorLogicRXReadOut::makeNewHit
//----------------------------------------------------------------------------//
ubit16 SectorLogicRXReadOut::readSLHitCurrent(){
  if(m_BodyCurr) {
    ubit16 hit = m_BodyCurr->hit;
    m_BodyCurr=m_BodyCurr->next;
    return hit;
  } else {
    return 0xefac;
  }
}//end-of-SectorLogicRXReadOut::readSLHitCurrent

//----------------------------------------------------------------------------//
void SectorLogicRXReadOut::display(std::ostream &stream) {
  stream<<" **** Sector Logic ReadOut Fragment ****"<<std::endl;
  stream<<" SectorLogic: Header "<<std::hex<<m_Header<<std::dec<<std::endl;
  stream<<" SectorLogic: SubHeader "<<std::hex<<m_SubHeader<<std::dec<<std::endl;
  for(ubit16 i=0; i<m_numberOfInputWords; i++) {
    stream<<" SectorLogic: hit       "<<(i+1)<<"  ==> "
          <<std::hex<<readSLHitCurrent()<<std::dec<<std::endl;
  }
  stream<<" SectorLogic: Footer "<<std::hex<<m_Footer<<std::dec<<std::endl;
}//end-of-void SectorLogicRXReadOut::display

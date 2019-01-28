/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSlaveBoard.hh"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

namespace LVL1TGCTrigger {

TGCSlaveBoard::TGCSlaveBoard()
  :m_lengthOfCoincidenceOut(0),m_coincidenceOut(0), 
   m_slaveBoardOut(0), 
   m_id(0), m_bid(-1),m_idHighPtBoard(0),
   m_type(0), m_region(FORWARD),
   m_patchPanel(0), m_patchPanelOut(0)
{
}

TGCSlaveBoard::~TGCSlaveBoard()
{
  if( m_coincidenceOut!=0) delete m_coincidenceOut;
  if( m_slaveBoardOut!=0) delete m_slaveBoardOut;

  m_coincidenceOut=0;
  m_slaveBoardOut=0;
  m_patchPanel=0;

  deletePatchPanelOut();
}

void TGCSlaveBoard::clockIn(int bidIn)
{
  m_bid = bidIn;
  collectInput();

  m_slbin.reset();

#ifdef TGCDEBUG
  int i;
  std::cout << "#SB: BID= " << m_bid;
  std::cout << " Rgn= " << m_region;
  std::cout << " Typ= " << getTypeName(m_type);
  std::cout << " ID= "  << m_id <<  std::endl;
  if(m_patchPanelOut!=0){
    for( i=0; i<NumberOfConnectorPerPPOut; i+=1)
      if(m_patchPanelOut->getHitPattern(i)!=0){
	std::cout<<"#SB I: BID= "<<m_bid;
	if(m_patchPanelOut->getOrigin()!=0){
	  std::cout << " Typ= " <<m_patchPanelOut->getOrigin()->getType();
	  std::cout << " Rgn= " <<m_patchPanelOut->getOrigin()->getRegion();
	  std::cout << " PPID= "<<m_patchPanelOut->getOrigin()->getId()<<" ";
	  std::cout << " port= "<<i;
          std::cout << "" <<m_id;
        }
        std::cout << ":";
        m_patchPanelOut->getHitPattern(i)->print();
      }
  } else {
    std::cout << "NO PP IN" << std::endl;
  }
#endif
  
  // delete m_slaveBoardOut and m_coincidenceOut 
  if( m_coincidenceOut!=0) delete m_coincidenceOut;
  if( m_slaveBoardOut!=0) delete m_slaveBoardOut;
  m_coincidenceOut = 0;
  m_slaveBoardOut  = 0;

  if(m_patchPanelOut!=0){
    doMaskOperation();

    storeSlbIn();

    doCoincidence();
    if(m_coincidenceOut!=0) {
#ifdef TGCDEBUG_VISUAL
      std::cout << "TGCSlaveBoard::clockIn: after doCoincidence()\n";
      m_coincidenceOut->visual(m_coincidenceOut->getLength(), 1, 0);
#endif 
#ifdef TGCDEBUG
      std::cout << "#SB C: BID= " << m_bid << ":";
      m_coincidenceOut->print();
#endif 
    }
    doDecluster();
#ifdef TGCDEBUG_VISUAL
    if(m_coincidenceOut!=0) {
      std::cout << "TGCSlaveBoard::clockIn: after doDecluster()\n";
      m_coincidenceOut->visual(m_coincidenceOut->getLength(), 1, 0);
    }
#endif 
    createSlaveBoardOut();

#ifdef TGCDEBUG
    showResult();
#endif

    deletePatchPanelOut();
    if(m_coincidenceOut!=0){
      delete m_coincidenceOut;
      m_coincidenceOut=0;
    }
  }
}

void TGCSlaveBoard::storeSlbIn()
{
  // store SlaveBoard input for readout
  TGCHitPattern* inner = m_patchPanelOut->getHitPattern(0);
  TGCHitPattern* pivot = m_patchPanelOut->getHitPattern(1);
  int i;
  if(pivot!=0){
    for(i=0; i<pLength[m_type]; i++){
      m_slbin.set(40+i+pOffset[m_type],pivot->getChannel(i+pLength[m_type]));   
      m_slbin.set(40+36+i+pOffset[m_type],pivot->getChannel(i));   
    }
  }
  if(inner!=0){
    for(i=0; i<iLength[m_type]; i++){
      if(inner->getLength()>iLength[m_type])//WTSB
        m_slbin.set(40+36+36+i+iOffset[m_type],inner->getChannel(i+iLength[m_type]));   
      m_slbin.set(40+36+36+44+i+iOffset[m_type],inner->getChannel(i));   
    }
  }
}

void TGCSlaveBoard::showResult() const 
{

  int i;
  if(m_patchPanelOut!=0){
    for( i=0; i<NumberOfConnectorPerPPOut; i+=1)
      if(m_patchPanelOut->getHitPattern(i)!=0){
        std::cout<<"#SB I: BID= "<<m_bid;
        if(m_patchPanelOut->getOrigin()!=0){
          std::cout << " Typ= " <<m_patchPanelOut->getOrigin()->getType();
          std::cout << " Rgn= " <<m_patchPanelOut->getOrigin()->getRegion();
          std::cout << " PPID= "<<m_patchPanelOut->getOrigin()->getId()<<" ";
          std::cout << " port= "<<i;
          std::cout << "" <<m_id;
        }
        std::cout << ":";
        m_patchPanelOut->getHitPattern(i)->print();
      }
  }

  if(m_slaveBoardOut!=0){
    if(m_coincidenceOut!=0){
      std::cout << "#SB O: BID= " << m_bid;
      std::cout << " Rgn= " << m_region;
      std::cout << " Typ= " << getTypeName(m_type);
      std::cout << " ID= "  << m_id << ":";
      for( i=0; i<m_slaveBoardOut->getNumberOfData(); i++){
        if(m_slaveBoardOut->getHit(i)){
          std::cout << " i= "  << i;
          std::cout << " x= " << m_slaveBoardOut->getPos(i);
          if((m_type==1)||(m_type==3))// doublet
            std::cout << " d= " << m_slaveBoardOut->getDev(i);
        }
      }
      std::cout<<std::endl;      
      std::cout<<"Destination HPBID= "<<m_idHighPtBoard<<std::endl;
    }
  }
}

std::string TGCSlaveBoard::getTypeName(int /*typeIn*/) const 
{
  if(m_type==WTSB) return "WT";//0
  if(m_type==WDSB) return "WD";//1
  if(m_type==STSB) return "ST";//2
  if(m_type==SDSB) return "SD";//3
  if(m_type==WISB) return "WI";//4
  if(m_type==SISB) return "SI";//5
  return "";
}

void TGCSlaveBoard::deletePatchPanelOut()
{
  if( m_patchPanelOut!=0 ) delete m_patchPanelOut;
  m_patchPanelOut=0;
}

void TGCSlaveBoard::doDecluster()
{
  int i;
  bool A, B, C, D;

  if(m_coincidenceOut!=0){
    TGCHitPattern* out = new TGCHitPattern (m_coincidenceOut->getLength());
    int length = out->getLength();
    for( i=2; i<length-1; i+=1) {
      A = m_coincidenceOut->getChannel(i-2);
      B = m_coincidenceOut->getChannel(i-1);
      C = m_coincidenceOut->getChannel(i);
      D = m_coincidenceOut->getChannel(i+1);
      out->setChannel(i,( !B & C & !D )|( !A & B & C ));
    }

    A = false;
    B = false;
    C = m_coincidenceOut->getChannel(0);
    D = m_coincidenceOut->getChannel(1);
    out->setChannel(0,( !B & C & !D )|( !A & B & C ));

    A = false;
    B = m_coincidenceOut->getChannel(0);
    C = m_coincidenceOut->getChannel(1);
    D = m_coincidenceOut->getChannel(2);
    out->setChannel(1,( !B & C & !D )|( !A & B & C ));

    A = m_coincidenceOut->getChannel(length-3);
    B = m_coincidenceOut->getChannel(length-2);
    C = m_coincidenceOut->getChannel(length-1);
    D = false;
    out->setChannel(length-1,( !B & C & !D )|( !A & B & C ));

    delete m_coincidenceOut;
    m_coincidenceOut=out;
    out=0;
  }
}

TGCSlaveBoardOut*  TGCSlaveBoard::getOutput() const
{
  return m_slaveBoardOut;
}

void  TGCSlaveBoard::eraseOutput()
{
  m_slaveBoardOut=0;
}
   
void TGCSlaveBoard::setPatchPanel(TGCPatchPanel* PPIn)
{
  m_patchPanel = PPIn;
}

void TGCSlaveBoard::collectInput()
{
#ifdef TGCDEBUG    
  std::cout << "#SB: BID= " << m_bid;
  std::cout << " Rgn= " << m_region;
  std::cout << " Typ= " << getTypeName(m_type);
  std::cout << " ID= "  << m_id <<  std::endl;
  if (m_patchPanel!=0) m_patchPanel->showProperty();
#endif

  if(m_patchPanel!=0){
    m_patchPanelOut = m_patchPanel->getOutput(m_id);
#ifdef TGCDEBUG    
    if(m_patchPanelOut!=0) m_patchPanelOut->print();
#endif
    m_patchPanel->eraseOutput(m_id);

  }
}

int TGCSlaveBoard::getIdHighPtBoard() const
{
  return m_idHighPtBoard;
}

void TGCSlaveBoard::setIdHighPtBoard(int idIn) 
{
  m_idHighPtBoard = idIn;
}


TGCSlaveBoard::TGCSlaveBoard(const TGCSlaveBoard& right)
{
  m_coincidenceOut = 0;
  m_slaveBoardOut = 0;
  m_patchPanel = 0;
  m_patchPanelOut = 0;
  *this = right;
}

TGCSlaveBoard& TGCSlaveBoard::operator=(const TGCSlaveBoard& right)
{
  if(this!=&right){
    m_id = right.m_id;
    m_bid = right.m_bid;
    m_idHighPtBoard = right.m_idHighPtBoard;
    m_type = right.m_type;
    m_region = right.m_region;
    m_patchPanel = right.m_patchPanel;
  
    if(m_patchPanelOut!=0) delete m_patchPanelOut;
    m_patchPanelOut=0;
    if(right.m_patchPanelOut) m_patchPanelOut = new TGCPatchPanelOut(*right.m_patchPanelOut);

    m_lengthOfCoincidenceOut = right.m_lengthOfCoincidenceOut;
    if(m_coincidenceOut!=0) delete m_coincidenceOut;
    m_coincidenceOut=0;
    if(right.m_coincidenceOut) m_coincidenceOut = new TGCHitPattern(*right.m_coincidenceOut);

    if(m_slaveBoardOut!=0) delete m_slaveBoardOut;
    m_slaveBoardOut=0;
    if(right.m_slaveBoardOut) m_slaveBoardOut = new TGCSlaveBoardOut(*right.m_slaveBoardOut);
  }
  return *this;
}

} //end of namespace bracket

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSlaveBoard.hh"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

namespace LVL1TGCTrigger {

TGCSlaveBoard::TGCSlaveBoard()
  :lengthOfCoincidenceOut(0),coincidenceOut(0), 
   slaveBoardOut(0), 
   id(0), bid(-1),idHighPtBoard(0),
   type(0), region(FORWARD),
   patchPanel(0), patchPanelOut(0)
{
}

TGCSlaveBoard::~TGCSlaveBoard()
{
  if( coincidenceOut!=0) delete coincidenceOut;
  if( slaveBoardOut!=0) delete slaveBoardOut;

  coincidenceOut=0;
  slaveBoardOut=0;
  patchPanel=0;

  deletePatchPanelOut();
}

void TGCSlaveBoard::clockIn(int bidIn)
{
  bid = bidIn;
  collectInput();

  slbin.reset();

#ifdef TGCDEBUG
  int i;
  std::cout << "#SB: BID= " << bid;
  std::cout << " Rgn= " << region;
  std::cout << " Typ= " << getTypeName(type);
  std::cout << " ID= "  << id <<  std::endl;
  if(patchPanelOut!=0){
    for( i=0; i<NumberOfConnectorPerPPOut; i+=1)
      if(patchPanelOut->getHitPattern(i)!=0){
	std::cout<<"#SB I: BID= "<<bid;
	if(patchPanelOut->getOrigin()!=0){
	  std::cout << " Typ= " <<patchPanelOut->getOrigin()->getType();
	  std::cout << " Rgn= " <<patchPanelOut->getOrigin()->getRegion();
	  std::cout << " PPID= "<<patchPanelOut->getOrigin()->getId()<<" ";
	  std::cout << " port= "<<i;
          std::cout << "" <<id;
        }
        std::cout << ":";
        patchPanelOut->getHitPattern(i)->print();
      }
  } else {
    std::cout << "NO PP IN" << std::endl;
  }
#endif
  
  // delete slaveBoardOut and coincidenceOut 
  if( coincidenceOut!=0) delete coincidenceOut;
  if( slaveBoardOut!=0) delete slaveBoardOut;
  coincidenceOut = 0;
  slaveBoardOut  = 0;

  if(patchPanelOut!=0){
    doMaskOperation();

    storeSlbIn();

    doCoincidence();
    if(coincidenceOut!=0) {
#ifdef TGCDEBUG_VISUAL
      std::cout << "TGCSlaveBoard::clockIn: after doCoincidence()\n";
      coincidenceOut->visual(coincidenceOut->getLength(), 1, 0);
#endif 
#ifdef TGCDEBUG
      std::cout << "#SB C: BID= " << bid << ":";
      coincidenceOut->print();
#endif 
    }
    doDecluster();
#ifdef TGCDEBUG_VISUAL
    if(coincidenceOut!=0) {
      std::cout << "TGCSlaveBoard::clockIn: after doDecluster()\n";
      coincidenceOut->visual(coincidenceOut->getLength(), 1, 0);
    }
#endif 
    createSlaveBoardOut();

#ifdef TGCDEBUG
    showResult();
#endif

    deletePatchPanelOut();
    if(coincidenceOut!=0){
      delete coincidenceOut;
      coincidenceOut=0;
    }
  }
}

void TGCSlaveBoard::storeSlbIn()
{
  // store SlaveBoard input for readout
  TGCHitPattern* inner = patchPanelOut->getHitPattern(0);
  TGCHitPattern* pivot = patchPanelOut->getHitPattern(1);
  int i;
  if(pivot!=0){
    for(i=0; i<pLength[type]; i++){
        slbin.set(40+i+pOffset[type],pivot->getChannel(i+pLength[type]));   
        slbin.set(40+36+i+pOffset[type],pivot->getChannel(i));   
    }
  }
  if(inner!=0){
    for(i=0; i<iLength[type]; i++){
        if(inner->getLength()>iLength[type])//WTSB
          slbin.set(40+36+36+i+iOffset[type],inner->getChannel(i+iLength[type]));   
        slbin.set(40+36+36+44+i+iOffset[type],inner->getChannel(i));   
    }
  }
}

void TGCSlaveBoard::showResult() const 
{

  int i;
  if(patchPanelOut!=0){
    for( i=0; i<NumberOfConnectorPerPPOut; i+=1)
      if(patchPanelOut->getHitPattern(i)!=0){
        std::cout<<"#SB I: BID= "<<bid;
        if(patchPanelOut->getOrigin()!=0){
          std::cout << " Typ= " <<patchPanelOut->getOrigin()->getType();
          std::cout << " Rgn= " <<patchPanelOut->getOrigin()->getRegion();
          std::cout << " PPID= "<<patchPanelOut->getOrigin()->getId()<<" ";
          std::cout << " port= "<<i;
          std::cout << "" <<id;
        }
        std::cout << ":";
        patchPanelOut->getHitPattern(i)->print();
      }
  }

  if(slaveBoardOut!=0){
    if(coincidenceOut!=0){
      std::cout << "#SB O: BID= " << bid;
      std::cout << " Rgn= " << region;
      std::cout << " Typ= " << getTypeName(type);
      std::cout << " ID= "  << id << ":";
      for( i=0; i<slaveBoardOut->getNumberOfData(); i++){
        if(slaveBoardOut->getHit(i)){
          std::cout << " i= "  << i;
          std::cout << " x= " << slaveBoardOut->getPos(i);
          if((type==1)||(type==3))// doublet
            std::cout << " d= " << slaveBoardOut->getDev(i);
        }
      }
      std::cout<<std::endl;      
      std::cout<<"Destination HPBID= "<<idHighPtBoard<<std::endl;
    }
  }
}

std::string TGCSlaveBoard::getTypeName(int /*typeIn*/) const 
{
  if(type==WTSB) return "WT";//0
  if(type==WDSB) return "WD";//1
  if(type==STSB) return "ST";//2
  if(type==SDSB) return "SD";//3
  if(type==WISB) return "WI";//4
  if(type==SISB) return "SI";//5
  return "";
}

void TGCSlaveBoard::deletePatchPanelOut()
{
  if( patchPanelOut!=0 ) delete patchPanelOut;
  patchPanelOut=0;
}

void TGCSlaveBoard::doDecluster()
{
  int i;
  bool A, B, C, D;

  if(coincidenceOut!=0){
    TGCHitPattern* out = new TGCHitPattern (coincidenceOut->getLength());
    int length = out->getLength();
    for( i=2; i<length-1; i+=1) {
      A = coincidenceOut->getChannel(i-2);
      B = coincidenceOut->getChannel(i-1);
      C = coincidenceOut->getChannel(i);
      D = coincidenceOut->getChannel(i+1);
      out->setChannel(i,( !B & C & !D )|( !A & B & C ));
    }

    A = false;
    B = false;
    C = coincidenceOut->getChannel(0);
    D = coincidenceOut->getChannel(1);
    out->setChannel(0,( !B & C & !D )|( !A & B & C ));

    A = false;
    B = coincidenceOut->getChannel(0);
    C = coincidenceOut->getChannel(1);
    D = coincidenceOut->getChannel(2);
    out->setChannel(1,( !B & C & !D )|( !A & B & C ));

    A = coincidenceOut->getChannel(length-3);
    B = coincidenceOut->getChannel(length-2);
    C = coincidenceOut->getChannel(length-1);
    D = false;
    out->setChannel(length-1,( !B & C & !D )|( !A & B & C ));

    delete coincidenceOut;
    coincidenceOut=out;
    out=0;
  }
}

TGCSlaveBoardOut*  TGCSlaveBoard::getOutput() const
{
  return slaveBoardOut;
}

void  TGCSlaveBoard::eraseOutput()
{
  slaveBoardOut=0;
}
   
void TGCSlaveBoard::setPatchPanel(TGCPatchPanel* PPIn)
{
  patchPanel = PPIn;
}

void TGCSlaveBoard::collectInput()
{
#ifdef TGCDEBUG    
  std::cout << "#SB: BID= " << bid;
  std::cout << " Rgn= " << region;
  std::cout << " Typ= " << getTypeName(type);
  std::cout << " ID= "  << id <<  std::endl;
  if (patchPanel!=0) patchPanel->showProperty();
#endif

  if(patchPanel!=0){
    patchPanelOut = patchPanel->getOutput(id);
#ifdef TGCDEBUG    
    if(patchPanelOut!=0) patchPanelOut->print();
#endif
    patchPanel->eraseOutput(id);

  }
}

int TGCSlaveBoard::getIdHighPtBoard() const
{
  return idHighPtBoard;
}

void TGCSlaveBoard::setIdHighPtBoard(int idIn) 
{
  idHighPtBoard = idIn;
}


TGCSlaveBoard::TGCSlaveBoard(const TGCSlaveBoard& right)
{
  coincidenceOut = 0;
  slaveBoardOut = 0;
  patchPanel = 0;
  patchPanelOut = 0;
  *this = right;
}

TGCSlaveBoard& TGCSlaveBoard::operator=(const TGCSlaveBoard& right)
{
  if(this!=&right){
    id = right.id;
    bid = right.bid;
    idHighPtBoard = right.idHighPtBoard;
    type = right.type;
    region = right.region;
    patchPanel = right.patchPanel;
  
    if(patchPanelOut!=0) delete patchPanelOut;
    patchPanelOut=0;
    if(right.patchPanelOut) patchPanelOut = new TGCPatchPanelOut(*right.patchPanelOut);

    lengthOfCoincidenceOut = right.lengthOfCoincidenceOut;;
    if(coincidenceOut!=0) delete coincidenceOut;
    coincidenceOut=0;
    if(right.coincidenceOut) coincidenceOut = new TGCHitPattern(*right.coincidenceOut);

    if(slaveBoardOut!=0) delete slaveBoardOut;
    slaveBoardOut=0;
    if(right.slaveBoardOut) slaveBoardOut = new TGCSlaveBoardOut(*right.slaveBoardOut);
  }
  return *this;
}

} //end of namespace bracket

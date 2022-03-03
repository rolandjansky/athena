/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCableInSLB.h"

#include "TGCcablingInterface/TGCIdBase.h"
#include "MuonTGC_Cabling/TGCChannelSLBIn.h"
#include "MuonTGC_Cabling/TGCChannelSLBOut.h"

namespace MuonTGC_Cabling {

TGCChannelId* TGCCableInSLB::getChannel(const TGCChannelId* channelId,
					bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCIdBase::SLBIn)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCIdBase::SLBOut)
      return getChannelIn(channelId,orChannel);
  }
  return nullptr;
}
  
TGCChannelId* TGCCableInSLB::getChannelIn(const TGCChannelId* slbout, 
					  bool orChannel) const {
  if(slbout->isValid()==false) return nullptr;

  int channel = slbout->getChannel();
  int block = slbout->getBlock();
  TGCIdBase::ModuleType moduleType = slbout->getModuleType();

  int channelInBlock = TGCChannelSLBOut::getChannelInBlock(moduleType);
  int numberOfLayer  = TGCChannelSLBOut::getNumberOfLayer(moduleType);

  int adjacentOfSLB = -1;
  int channelInSLB = -1;
  TGCChannelSLBIn::CellType cellType=TGCChannelSLBIn::NoCellType;
  switch(moduleType){ 
  case TGCIdBase::WD:
    //  channel        SLBIn
    //   2*n           CellB    n+block*16  
    //   2*n+1         CellA    n+block*16
    if(orChannel==false){
      if(channel%numberOfLayer==1){
	cellType = TGCChannelSLBIn::CellA;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      } 
      if(channel%numberOfLayer==0){ 
	cellType = TGCChannelSLBIn::CellB;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      }
    } 
    if(orChannel==true){
      if(channel%numberOfLayer==1){
	cellType = TGCChannelSLBIn::CellC;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      } 
      if(channel%numberOfLayer==0){
	cellType = TGCChannelSLBIn::CellD;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      }
    } 
    break;
  case TGCIdBase::SD:
    //  channel        SLBIn
    //   2*n           CellB    n+block*16  
    //   2*n+1         CellA    n+block*16
    if(orChannel==false){
      if(channel%numberOfLayer==1){
	cellType = TGCChannelSLBIn::CellA;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      } 
      if(channel%numberOfLayer==0){
	cellType = TGCChannelSLBIn::CellB;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      }
    } 
    if(orChannel==true){
      if(channel%numberOfLayer==1){
	cellType = TGCChannelSLBIn::CellC;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      } 
      if(channel%numberOfLayer==0){
	cellType = TGCChannelSLBIn::CellD;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      } 
    }
    break;
  case TGCIdBase::WT:
    //  channel        SLBIn
    //   3*n           CellC    n+block*16  
    //   3*n+1         CellB    n+block*16
    //   3*n+2         CellA    n+block*16
    if((channel+block*channelInBlock)%numberOfLayer==2){ 
      cellType = TGCChannelSLBIn::CellA;
      adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
      channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	+adjacentOfSLB;
    }
    if((channel+block*channelInBlock)%numberOfLayer==1){ 
     cellType = TGCChannelSLBIn::CellB;
     adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
     channelInSLB = (channel+block*channelInBlock)/numberOfLayer
       +adjacentOfSLB;
    } 
    if((channel+block*channelInBlock)%numberOfLayer==0){
      cellType = TGCChannelSLBIn::CellC;
      adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
      channelInSLB = (channel+block*channelInBlock)/numberOfLayer 
	+adjacentOfSLB;
    } 
    break;
  case TGCIdBase::ST:
  case TGCIdBase::WI:
  case TGCIdBase::SI:
    //  channel        SLBIn
    //   2*n           CellB    n+block*16  
    //   2*n+1         CellA    n+block*16
    if(orChannel==false){
      if(channel%numberOfLayer==1){
	cellType = TGCChannelSLBIn::CellA;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      }
      if(channel%numberOfLayer==0){
	cellType = TGCChannelSLBIn::CellB;
	adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	  +adjacentOfSLB;
      }
    }
    if(orChannel==true){
	if(channel%numberOfLayer==1){
	  cellType = TGCChannelSLBIn::CellC;
	  adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	  channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	    +adjacentOfSLB;
	}
	if(channel%numberOfLayer==0){
	  cellType = TGCChannelSLBIn::CellD;
	  adjacentOfSLB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);
	  channelInSLB = (channel+block*channelInBlock)/numberOfLayer
	    +adjacentOfSLB;
	}
      }
      break;

  default:
    break;
  }
  
  TGCChannelSLBIn* slbin = nullptr;
  if(channelInSLB!=-1){
    int channelOfSLB = TGCChannelSLBIn::convertChannel(slbout->getModuleType(),
						       cellType,channelInSLB);
    slbin = new TGCChannelSLBIn(slbout->getSideType(),
				slbout->getModuleType(),
				slbout->getRegionType(),
				slbout->getSector(),
				slbout->getId(),
				channelOfSLB);

  }
  return slbin;
}

TGCChannelId* TGCCableInSLB::getChannelOut(const TGCChannelId* slbin, 
					   bool orChannel) const {
  if(slbin->isValid()==false) return nullptr;

  TGCChannelSLBIn::CellType cellType =
    TGCChannelSLBIn::convertCellType(slbin->getChannel());
  if(cellType==TGCChannelSLBIn::NoCellType) return nullptr;

  TGCIdBase::ModuleType moduleType = slbin->getModuleType();
  int channelInCell = 
    TGCChannelSLBIn::convertChannelInCell(slbin->getChannel());
  int channelInSLB = 
    TGCChannelSLBIn::convertChannelInSLB(moduleType,cellType,channelInCell);

  int channelInBlock = TGCChannelSLBOut::getChannelInBlock(moduleType);
  int numberOfLayer  = TGCChannelSLBOut::getNumberOfLayer(moduleType);
  int adjacentOfSLB  = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,cellType);

  int channel = -1;
  int block = -1;
  switch(moduleType){
  case TGCIdBase::WD:
    if(orChannel==false){
      if(cellType==TGCChannelSLBIn::CellA)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+1;
      if(cellType==TGCChannelSLBIn::CellB)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer;
      block = channel/channelInBlock;
      channel = channel%channelInBlock;
    }
    if(orChannel==true){
      if(cellType==TGCChannelSLBIn::CellC)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+1;
      if(cellType==TGCChannelSLBIn::CellD)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer;
      block = channel/channelInBlock;
      channel = channel%channelInBlock;
    }
    break;
  case TGCIdBase::SD:
    if(orChannel==false){
      if(cellType==TGCChannelSLBIn::CellA)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+1;
      if(cellType==TGCChannelSLBIn::CellB)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer;
      block = channel/channelInBlock;
      channel = channel%channelInBlock;
    }
    if(orChannel==true){
      if(cellType==TGCChannelSLBIn::CellC)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+1;
      if(cellType==TGCChannelSLBIn::CellD)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer;
      block = channel/channelInBlock;
      channel = channel%channelInBlock;
    }
    break;
  case TGCIdBase::WT:
    if(cellType==TGCChannelSLBIn::CellA)
      channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+2;
    if(cellType==TGCChannelSLBIn::CellB)
      channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+1;
    if(cellType==TGCChannelSLBIn::CellC)
      channel = (channelInSLB-adjacentOfSLB)*numberOfLayer;
    block = channel/channelInBlock;
    channel = channel%channelInBlock;
    break;
  case TGCIdBase::ST:
  case TGCIdBase::WI:
  case TGCIdBase::SI:
    if(cellType== TGCChannelSLBIn::CellA||cellType== TGCChannelSLBIn::CellB){
      if(cellType==TGCChannelSLBIn::CellA)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+1;
      if(cellType==TGCChannelSLBIn::CellB)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer;
      block = channel/channelInBlock;
      channel = channel%channelInBlock;
    }
    if(cellType== TGCChannelSLBIn::CellC||cellType== TGCChannelSLBIn::CellD){
      if(cellType==TGCChannelSLBIn::CellC)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer+1;
      if(cellType==TGCChannelSLBIn::CellD)
	channel = (channelInSLB-adjacentOfSLB)*numberOfLayer;
      block = channel/channelInBlock;
      channel = channel%channelInBlock;
    }
    break;
  default:
    break;
  }

  TGCChannelSLBOut* slbout = nullptr;
  if(block!=-1&&channel!=-1)
    slbout = new TGCChannelSLBOut(slbin->getSideType(),
				  slbin->getModuleType(),
				  slbin->getRegionType(),
				  slbin->getSector(),
				  slbin->getId(),
				  block,
				  channel);

  return slbout;
}  

} //end of namespace

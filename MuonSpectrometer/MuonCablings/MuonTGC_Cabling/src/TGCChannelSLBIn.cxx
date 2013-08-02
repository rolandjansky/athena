/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCChannelSLBIn.h"

#include "MuonTGC_Cabling/TGCModuleSLB.h"

namespace MuonTGC_Cabling
{

// Constructor
TGCChannelSLBIn::TGCChannelSLBIn(TGCIdBase::SideType vside,
				 TGCIdBase::ModuleType vmodule,
				 TGCIdBase::RegionType vregion,
				 int vsector,
				 int vid,
				 int vchannel)
  : TGCChannelId(TGCIdBase::SLBIn)
{
  setSideType(vside);
  setModuleType(vmodule);
  setRegionType(vregion);
  setSector(vsector);
  setId(vid);
  setChannel(vchannel);
}

TGCModuleId* TGCChannelSLBIn::getModule(void) const 
{
  return (new TGCModuleSLB(getSideType(),
			   getModuleType(),
			   getRegionType(),
			   getSector(),
			   getId()));
}

bool TGCChannelSLBIn::isValid(void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getModuleType()>TGCIdBase::NoModuleType) &&
     (getModuleType()<TGCIdBase::MaxModuleType)&&
     (getRegionType()>TGCIdBase::NoRegionType) &&
     (getRegionType()<TGCIdBase::MaxRegionType)&&
     (getOctant()    >=0)                      &&
     (getOctant()    <8)                       &&
     (getId()        >=0)                      &&
     (getChannel()   >=0)                      &&
     (channelInSLB   <getLengthOfSLB(getModuleType(), getCellType())) &&
     (channelInSLB   >=0)                           )
    return true;
  return false;
}


//                                         Trig  A   B   C   D
const int TGCChannelSLBIn::lengthCell[]  = {40, 36, 36, 44, 44};
const int TGCChannelSLBIn::offsetCell[]  = { 0, 40, 76,112,156};
const int TGCChannelSLBIn::lengthWD[]    = {40, 36, 36, 44, 44};
const int TGCChannelSLBIn::lengthSD[]    = {40, 32, 32, 32, 32};
const int TGCChannelSLBIn::lengthWT[]    = {40, 36, 36, 36, 36};
const int TGCChannelSLBIn::lengthST[]    = {40, 32, 32, 32, 32};
const int TGCChannelSLBIn::adjacentCell[]= { 0,  2,  2,  6,  6};
const int TGCChannelSLBIn::adjacentWD[]  = { 0,  2,  2,  6,  6};
const int TGCChannelSLBIn::adjacentSD[]  = { 0,  0,  0,  0,  0};
const int TGCChannelSLBIn::adjacentWT[]  = { 0,  2,  2,  2,  2};
const int TGCChannelSLBIn::adjacentST[]  = { 0,  0,  0,  0,  0};


int TGCChannelSLBIn::getLengthOfCell(CellType cellType) {
  return lengthCell[cellType];
}
  
int TGCChannelSLBIn::getOffsetOfCell(CellType cellType) {
  return offsetCell[cellType];
}

int TGCChannelSLBIn::getLengthOfSLB(TGCIdBase::ModuleType moduleType, 
				    CellType cellType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return lengthWD[cellType];
  case TGCIdBase::SD:
    return lengthSD[cellType];
  case TGCIdBase::WT:
    return lengthWT[cellType];
  case TGCIdBase::ST:
    return lengthST[cellType];
  case TGCIdBase::WI:
    return lengthST[cellType];
  case TGCIdBase::SI:
    return lengthST[cellType];
  default:
    break;
  }
  return -1;
}

int TGCChannelSLBIn::getAdjacentOfCell(CellType cellType) {
  return adjacentCell[cellType];
}

int TGCChannelSLBIn::getAdjacentOfSLB(TGCIdBase::ModuleType moduleType,
				      CellType cellType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return adjacentWD[cellType];
  case TGCIdBase::SD:
    return adjacentSD[cellType];
  case TGCIdBase::WT:
    return adjacentWT[cellType];
  case TGCIdBase::ST:
    return adjacentST[cellType];
  case TGCIdBase::WI:
    return adjacentST[cellType];
  case TGCIdBase::SI:
    return adjacentST[cellType];
  default:
    break;
  }
  return -1;

}

int TGCChannelSLBIn::convertChannelInCell(int channel) {
  if(channel>=getOffsetOfCell(CellTrig)&&
     channel<getOffsetOfCell(CellTrig)+getLengthOfCell(CellTrig)){
    return channel-getOffsetOfCell(CellTrig);
  }
  if(channel>=getOffsetOfCell(CellA)&&
     channel<getOffsetOfCell(CellA)+getLengthOfCell(CellA)){
    return channel-getOffsetOfCell(CellA);
  }
  if(channel>=getOffsetOfCell(CellB)&&
     channel<getOffsetOfCell(CellB)+getLengthOfCell(CellB)){
    return channel-getOffsetOfCell(CellB);
  }
  if(channel>=getOffsetOfCell(CellC)&&
     channel<getOffsetOfCell(CellC)+getLengthOfCell(CellC)){
    return channel-getOffsetOfCell(CellC);
  }
  if(channel>=getOffsetOfCell(CellD)&&
     channel<getOffsetOfCell(CellD)+getLengthOfCell(CellD)){
    return channel-getOffsetOfCell(CellD);
  }
  return -1;
}
  
TGCChannelSLBIn::CellType TGCChannelSLBIn::convertCellType(int channel) {
  if(channel>=getOffsetOfCell(CellTrig)&&
     channel<getOffsetOfCell(CellTrig)+getLengthOfCell(CellTrig)){
    return CellTrig;
  }
  if(channel>=getOffsetOfCell(CellA)&&
     channel<getOffsetOfCell(CellA)+getLengthOfCell(CellA)){
    return CellA;
  }
  if(channel>=getOffsetOfCell(CellB)&&
     channel<getOffsetOfCell(CellB)+getLengthOfCell(CellB)){
    return CellB;
  }
  if(channel>=getOffsetOfCell(CellC)&&
     channel<getOffsetOfCell(CellC)+getLengthOfCell(CellC)){
    return CellC;
  }
  if(channel>=getOffsetOfCell(CellD)&&
     channel<getOffsetOfCell(CellD)+getLengthOfCell(CellD)){
    return CellD;
  }
  return NoCellType;
}

int TGCChannelSLBIn::convertChannelInSLB(TGCIdBase::ModuleType moduleType,
					 CellType cellType, int channel) {
  int offset = getAdjacentOfCell(cellType)
    -getAdjacentOfSLB(moduleType, cellType);
  return channel-offset;
}

int TGCChannelSLBIn::convertChannel(TGCIdBase::ModuleType moduleType,
				    CellType cellType, int channelInSLB) {
  int offset = getAdjacentOfCell(cellType)
    -getAdjacentOfSLB(moduleType, cellType);
  return getOffsetOfCell(cellType)+offset+channelInSLB;
}

int TGCChannelSLBIn::getChannelInCell(void) const {
  return channelInCell;
}

int TGCChannelSLBIn::getChannelInSLB(void) const {
  return channelInSLB;
}

void TGCChannelSLBIn::setChannel(int vchannel) {
  TGCId::setChannel(vchannel);
  cellType = convertCellType(vchannel);
  channelInCell = convertChannelInCell(vchannel);
  channelInSLB = convertChannelInSLB(getModuleType(), getCellType(),
				     getChannelInCell());
}  


} // end of namespace

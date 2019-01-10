/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCChannelSLBIn.h"

namespace LVL1TGCCabling8
{

// Constructor
TGCChannelSLBIn::TGCChannelSLBIn (TGCIdBase::SideType side,
				  TGCIdBase::ModuleType module,
				  TGCIdBase::RegionType region,
				  int sector,
				  int id,
				  int channel)
  : TGCChannelId(TGCChannelId::SLBIn)
{
  setSideType(side);
  setModuleType(module);
  setRegionType(region);
  setSector(sector);
  setId(id);
  setChannel(channel);
}

TGCModuleId* TGCChannelSLBIn::getModule (void) const 
{
  return (new TGCModuleSLB(getSideType(),
			   getModuleType(),
			   getRegionType(),
			   getSector(),
			   getId()));
}

bool TGCChannelSLBIn::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getModuleType()>TGCIdBase::NoModuleType) &&
     (getModuleType()<TGCIdBase::MaxModuleType)&&
     (getRegionType()>TGCIdBase::NoRegionType) &&
     (getRegionType()<TGCIdBase::MaxRegionType)&&
     (getOctant()    >=0)                  &&
     (getOctant()    <8)                   &&
     (getId()        >=0)                  &&
     (getChannel()   >=0)                  )
    return true;
  return false;
}


//                                           Trig  A   B   C   D
const int TGCChannelSLBIn::s_lengthCell[]  = {40, 36, 36, 44, 44};
const int TGCChannelSLBIn::s_offsetCell[]  = { 0, 40, 76,112,156};
const int TGCChannelSLBIn::s_lengthWD[]    = {40, 36, 36, 44, 44};
const int TGCChannelSLBIn::s_lengthSD[]    = {40, 32, 32, 32, 32};
const int TGCChannelSLBIn::s_lengthWT[]    = {40, 36, 36, 36, 36};
const int TGCChannelSLBIn::s_lengthST[]    = {40, 32, 32, 32, 32};
const int TGCChannelSLBIn::s_adjacentCell[]= { 0,  2,  2,  6,  6};
const int TGCChannelSLBIn::s_adjacentWD[]  = { 0,  2,  2,  6,  6};
const int TGCChannelSLBIn::s_adjacentSD[]  = { 0,  0,  0,  0,  0};
const int TGCChannelSLBIn::s_adjacentWT[]  = { 0,  2,  2,  2,  2};
const int TGCChannelSLBIn::s_adjacentST[]  = { 0,  0,  0,  0,  0};


int TGCChannelSLBIn::getLengthOfCell (CellType cellType) {
  return s_lengthCell[cellType];
}
  
int TGCChannelSLBIn::getOffsetOfCell (CellType cellType) {
  return s_offsetCell[cellType];
}

int TGCChannelSLBIn::getLengthOfSLB (TGCIdBase::ModuleType moduleType, 
				     CellType cellType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return s_lengthWD[cellType];
  case TGCIdBase::SD:
    return s_lengthSD[cellType];
  case TGCIdBase::WT:
    return s_lengthWT[cellType];
  case TGCIdBase::ST:
    return s_lengthST[cellType];
  case TGCIdBase::WI:
    return s_lengthST[cellType];
  case TGCIdBase::SI:
    return s_lengthST[cellType];
  default:
    break;
  }
  return -1;
}

int TGCChannelSLBIn::getAdjacentOfCell (CellType cellType) {
  return s_adjacentCell[cellType];
}

int TGCChannelSLBIn::getAdjacentOfSLB (TGCIdBase::ModuleType moduleType,
				       CellType cellType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return s_adjacentWD[cellType];
  case TGCIdBase::SD:
    return s_adjacentSD[cellType];
  case TGCIdBase::WT:
    return s_adjacentWT[cellType];
  case TGCIdBase::ST:
    return s_adjacentST[cellType];
  case TGCIdBase::WI:
    return s_adjacentST[cellType];
  case TGCIdBase::SI:
    return s_adjacentST[cellType];
  default:
    break;
  }
  return -1;

}

int TGCChannelSLBIn::convertChannelInCell (int channel) {
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
  
TGCChannelSLBIn::CellType TGCChannelSLBIn::convertCellType (int channel) {
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

int TGCChannelSLBIn::convertChannelInSLB (TGCIdBase::ModuleType moduleType,
					  CellType cellType, int channel) {
  int offset = getAdjacentOfCell(cellType)
    -getAdjacentOfSLB(moduleType,cellType);
  return channel-offset;
  
  return -1;
}

int TGCChannelSLBIn::convertChannel (TGCIdBase::ModuleType moduleType,
				     CellType cellType, int channelInSLB) {
  int offset = getAdjacentOfCell(cellType)
    -getAdjacentOfSLB(moduleType,cellType);
  return getOffsetOfCell(cellType)+offset+channelInSLB;
}

int TGCChannelSLBIn::getChannelInCell (void) const {
  return m_channelInCell;
}

int TGCChannelSLBIn::getChannelInSLB (void) const {
  return m_channelInSLB;
}

void TGCChannelSLBIn::setChannel (int channel) {
  TGCIdBase::setChannel(channel);
  m_cellType = convertCellType(channel);
  m_channelInCell = convertChannelInCell(channel);
  m_channelInSLB = convertChannelInSLB(getModuleType(),getCellType(),
	  			       getChannelInCell());
}  


} // end of namespace

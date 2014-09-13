/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibType.h"

//
//_____________________________________________________________________
std::string 
TileCalibType::getClassName(TileCalibType::TYPE type)
{
  std::string strType;
  switch(type){
  case TileCalibType::BASE:
    strType = "TileCalibDrawerBase";
    break;
  case TileCalibType::CMT:
    strType = "TileCalibDrawerCmt";
    break;
  case TileCalibType::OFC:
    strType = "TileCalibDrawerOfc";
    break;
  case TileCalibType::FLT:
    strType = "TileCalibDrawerFlt";
    break;
  case TileCalibType::BCH:
    strType = "TileCalibDrawerBch";
    break;
  case TileCalibType::INT:
    strType = "TileCalibDrawerInt";
    break;
  default:
    strType = "UNKNOWN TYPE";
  }
  return strType;
}

//
//_____________________________________________________________________
std::string 
TileCalibType::getClassName(uint16_t type)
{
  return getClassName(static_cast<TileCalibType::TYPE>(type));
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondBlobObjs/CaloCondType.h"

//
//_____________________________________________________________________
std::string 
CaloCondType::getClassName(CaloCondType::TYPE type)
{
  std::string strType;
  switch(type){
  case CaloCondType::BASE:
    strType = "CaloCondBlobBase";
    break;
  case CaloCondType::FLT:
    strType = "CaloCondBlobFlt";
    break;
  default:
    strType = "UNKNOWN TYPE";
  }
  return strType;
}

//
//_____________________________________________________________________
std::string 
CaloCondType::getClassName(uint16_t type)
{
  return getClassName(static_cast<CaloCondType::TYPE>(type));
}
